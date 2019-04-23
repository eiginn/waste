/*
    WASTE - netkern.cpp (Common network runtime code)
    Copyright (C) 2003 Nullsoft, Inc.

    WASTE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    WASTE  is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WASTE; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
/*
  Copyright (c) 1998-2003 VIA Technologies, Inc.

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTIES OR REPRESENTATIONS; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/

#include "main.h"
#include "netq.h"
#include "netkern.h"
#if defined(_WIN32) && !defined(QTUI)
#include "resource.h"
#include "childwnd.h"
#endif
#include "m_search.h"
#include "m_ping.h"
#include "m_lcaps.h"

#include "srchwnd.h"
//Added by Raymond
#include "tag.h"
#ifdef QTUI
#include "vaste_idd_net.h"
//#include <strings.h>
#endif

C_ItemList<C_Connection> g_new_net;

#if defined(_WIN32) && !defined(QTUI)
W_ListView g_lvnetcons;
HWND g_netstatus_wnd;
#define SAVE_CB_HOSTS // turn this off for more privacy
#define MAX_CBHOSTS 32
#elif defined(QTUI)
W_ListView g_lvnetcons;
QWidget* g_netstatus_wnd;
#define SAVE_CB_HOSTS // turn this off for more privacy
#define MAX_CBHOSTS 32
#endif

#define SYNC_SIZE 16
static char g_con_str[SYNC_SIZE+1]="MUGWHUMPJISMSYN2";

static void ListenToSocket(void)
{
  if (g_listen)
  {
    C_Connection *c=g_listen->get_connect();
    if (c)
    {
      unsigned int addr=c->get_remote();
      if (!allowIP(addr))
      {
        //debug_printf("denied connection from %08X due to IP list\n",addr);
        delete c;
      }
      else 
      {
        struct in_addr in;
        in.s_addr=addr;
        char *a=inet_ntoa(in);
	//Added by raymond for debug
	//debug_printf("Listen before send SYNC\n");
        c->send_bytes(g_con_str,SYNC_SIZE);
	//debug_printf("Listen after send SYNC\n");
#if defined(_WIN32) || defined(QTUI)
        char str2[512];
        sprintf(str2,"%s (incoming)",a?a:"?");
	//debug_printf("%s is coming\n", str2);
        g_lvnetcons.InsertItem(0,"Authenticating",(int)c);
        g_lvnetcons.SetItemText(0,1,str2);
        g_lvnetcons.SetItemText(0,2,"100");
#endif
        g_new_net.Add(c);
#if defined(_WIN32) && !defined(QTUI)
        PostMessage(g_netstatus_wnd,WM_USER_TITLEUPDATE,0,0);
#elif defined(QTUI)
	((Vaste_Idd_Net *)g_netstatus_wnd)->UpdateWinTitle();
#endif
      }
    }
  }
}

void NetKern_ConnectToHostIfOK(int ip, int port)
{
  struct in_addr in;
  in.s_addr=ip;
  char *t=inet_ntoa(in);
  if (!t || !*t) return;
  char text[512];
  safe_strncpy(text,t,sizeof(text));

#if defined(_WIN32) || defined(QTUI)
  int netqpos=-1;
  int i,l;
  l=g_lvnetcons.GetCount();
  RTRACE("Conns : " << l);
  for (i = 0; i < l; i ++)
  {
    char text2[256];
    g_lvnetcons.GetText(i,1,text2,256);
    RTRACE("Text :" << text);
    RTRACE("Text2 :" << text2);

#if defined(_WIN32) && !defined(QTUI)
    if (!strnicmp(text2,text,strlen(text)) && 
          (text2[strlen(text)] == 0 ||
           text2[strlen(text)] == ':' ||
           text2[strlen(text)] == ' ')
          )
#elif defined(QTUI)
    if (!strncasecmp(text2,text,strlen(text)) && 
          (text2[strlen(text)] == 0 ||
           text2[strlen(text)] == ':' ||
           text2[strlen(text)] == ' ')
          )
#endif
    {
      if (g_lvnetcons.GetParam(i))
        return;
      else if (netqpos<0) netqpos=i;
    }
  }
#endif //WIN32 || QTUI

  if (isaccessable(text) && allowIP(ip))
  {
    C_Connection *newcon=new C_Connection(text,port,g_dns);
    newcon->send_bytes(g_con_str,SYNC_SIZE);
    g_new_net.Add(newcon);

#if	defined(_WIN32) || defined(QTUI)
    if (netqpos<0)
    {
      char buf[1024];
      sprintf(buf,"%s:%d",text,(int)(unsigned short)port);
      g_lvnetcons.InsertItem(0,"Connecting",(int)newcon);
      g_lvnetcons.SetItemText(0,1,buf);
      g_lvnetcons.SetItemText(0,2,"90");
    }
    else
    {
      g_lvnetcons.SetItemParam(netqpos,(int)newcon);
      g_lvnetcons.SetItemText(netqpos,0,"Connecting");
    }
#if defined(_WIN32) && !defined(QTUI)
    PostMessage(g_netstatus_wnd,WM_USER_TITLEUPDATE,0,0);
#elif defined(QTUI)
	((Vaste_Idd_Net *)g_netstatus_wnd)->UpdateWinTitle();
#endif // _WIN32
#endif//WIN32 and QTUI
  }
}

static void SendCaps(C_MessageQueue *mq)
{
  if (mq) // send caps
  {
    C_MessageLocalCaps l;
    int a=MAX_CONNECTION_SENDSIZE;
    if (g_conspeed<64)a=512; //modems only let sendahead be 512 or so.
    else if (g_conspeed<384)a=2048;
    else if (g_conspeed<1600)a=4096;
    else if (g_conspeed<20000)a=8192;
    l.add_cap(MLC_BANDWIDTH,a); // tell it our max bufsize
    l.add_cap(MLC_SATURATION,!!(g_throttle_flag&16));
    
    // send the PadLockSL cap with the prefered aes keysize.
    l.add_cap(MLC_PADLOCKSL_AES_KEYSIZE, g_aes_keysize);
    
  	T_Message msg={0,};
	  msg.data=l.Make();
	  if (msg.data)
	  {
	    msg.message_type=MESSAGE_LOCAL_CAPS;
		  msg.message_length=msg.data->GetLength();
      msg.message_ttl=1;
      CreateID128(&msg.message_guid);

      msg.message_prio=C_MessageQueueList::GetMessagePriority(msg.message_type);
      C_MessageQueue::calc_md5(&msg,msg.message_md5);

      msg.data->Lock();
      mq->send_message(&msg);
      msg.data->Unlock();
    }
  }
}

void RebroadcastCaps(C_MessageQueueList *mql) // sends a local message to every host connected to
{
  int x;
  for (x = 0; x < mql->GetNumQueues(); x ++) SendCaps(mql->GetQueue(x));
}

static void HandleNewOutCons(void)
{
#if defined(_WIN32) || defined(QTUI) //Ray 1.5
  static unsigned int next_runitem;

  unsigned int a=GetTickCount();
  if (a >= next_runitem || a < next_runitem-30000) // the second one is to detect wrap
  {

    if (!g_mql->GetNumQueues()) // invalidate cache if no queues =)
    {
      int ci;
      for (ci = 0; ci < SEARCHCACHE_NUMITEMS; ci ++) 
      {
        if (g_searchcache[ci]->search_id_time) 
          if (g_searchcache[ci]->numcons || !g_searchcache[ci]->searchreplies.GetSize()) g_searchcache[ci]->search_id_time=0;
      }
    }

    if (g_keepup)
    {
	    //debug_printf("Handle g_keepup = %d\n", g_keepup);
      if (g_mql->GetNumQueues()+g_new_net.GetSize() < g_keepup)
      {
        int hn=g_lvnetcons.GetCount();
        int x;
        int max_pos=-1;
        int max_rat=0;

        char text[512];
        int port;

        for (x = 0; x < hn; x ++)
        {
	    //debug_printf("Handle for hn = %d\n", hn);
          if (g_lvnetcons.GetParam(x)) continue;

	    //debug_printf("Handle for No Param = %d\n", x);
          char rat[32];
          g_lvnetcons.GetText(x,2,rat,sizeof(rat));
          int thisrat=rat[0] ? atoi(rat) : -1;
          if (max_pos < 0 || thisrat > max_rat)
          {
            g_lvnetcons.GetText(x,1,text,sizeof(text));
	    //debug_printf("Handle for No Param = %s\n", text);

            char *p=text;
            while (*p != ':' && *p) p++;
            if (*p)
            {
              *p++=0;
              port=atoi(p);
            }
            else port=1337;
            int i;
            for (i = 0; i < hn; i ++)
            {
              if (g_lvnetcons.GetParam(i) && i != x)
              {
                char text2[256];
                g_lvnetcons.GetText(i,1,text2,256);
#if defined(_WIN32) && !defined(QTUI)
                if (!strnicmp(text2,text,strlen(text)) && 
                      (text2[strlen(text)] == 0 ||
                       text2[strlen(text)] == ':' ||
                       text2[strlen(text)] == ' ')
                      )
#elif defined(QTUI)
                if (!strncasecmp(text2,text,strlen(text)) && 
                      (text2[strlen(text)] == 0 ||
                       text2[strlen(text)] == ':' ||
                       text2[strlen(text)] == ' ')
                      )
#endif
                  break;
              }
            }
            unsigned int ip;
            if (i == hn && isaccessable(text) && (ip=inet_addr(text)) != INADDR_NONE && allowIP(ip))
            {
              max_rat=thisrat;
              max_pos=x;
            }
          }
        } // for
        if (max_pos >= 0)
        {
          char rat[32];
          g_lvnetcons.GetText(max_pos,2,rat,sizeof(rat));          
	  //debug_printf("DeleteItem near max_pos = %d\n", max_pos);
          g_lvnetcons.DeleteItem(max_pos);
	  
	    //debug_printf("Handle for del item = %d\n", max_pos);
          AddConnection(text,port,atoi(rat));
        }
      }
    }
    next_runitem=GetTickCount()+3000;
  }
#endif//_WIN32 && QTUI
  int x;
  //debug_printf("g_new_net.GetSize() = %d\n", g_new_net.GetSize());
  for (x = 0; x < g_new_net.GetSize(); x ++)
  {
	  //debug_printf("In Handle for run net.Getsize() = %d\n", g_new_net.GetSize());
    int s=g_new_net.Get(x)->run(-1,-1);
	  //debug_printf("In Handle after run net status = %d\n", s);
    if (s == C_Connection::STATE_CLOSED || s == C_Connection::STATE_ERROR)
    {
	    //debug_printf("before get_remote_port\n");
      int port=g_new_net.Get(x)->get_remote_port();
	    //debug_printf("after get_remote_port = %d\n", port);
      int irat=0;

	//debug_printf("before QTUI\n");
#if defined(_WIN32) || defined(QTUI) //Ray 1.5
      int a=g_lvnetcons.FindItemByParam((int)g_new_net.Get(x));
	//debug_printf("after QTUI\n");
      if (a >= 0)
      {
        char rat[32];
        g_lvnetcons.GetText(a,2,rat,sizeof(rat));
        if (rat[0]) irat=atoi(rat);
	//debug_printf("DeleteItem by a\n");
        g_lvnetcons.DeleteItem(a);   
        if (port) 
        {
          int maxrat=0;
          if (g_mql->GetNumQueues()) irat-=8;
          else
          {
            int i,n=g_lvnetcons.GetCount();
	    //debug_printf("befter for %d\n", n);
            for (i = 0; i < n; i ++)
            {
              if (i != a)
              {
                char t[32];
                g_lvnetcons.GetText(i,2,t,sizeof(t));
                int thisrat=atoi(t);
                if (maxrat < thisrat) maxrat=thisrat;
              }
            }
	    //debug_printf("affter for %d\n", n);
            irat=maxrat-1;
          }

          if (irat < 0) irat=0;
	  //debug_printf("before add_to_netq %d\n", g_new_net.Get(x)->get_remote());
          add_to_netq(g_new_net.Get(x)->get_remote(),port,irat,1);
        }
      }
#endif // _WIN32 && QTUI

      //debug_printf("Could not connect to host: failed connect!\n");
      delete g_new_net.Get(x);
      g_new_net.Del(x--);
#if defined(_WIN32) && !defined(QTUI)
      PostMessage(g_netstatus_wnd,WM_USER_TITLEUPDATE,0,0);
#elif defined(QTUI)
	((Vaste_Idd_Net *)g_netstatus_wnd)->UpdateWinTitle();
#endif // _WIN32
    }
    else if (g_new_net.Get(x)->recv_bytes_available()>=SYNC_SIZE)
    {
	    //debug_printf("SYNC\n");
      char b[SYNC_SIZE];
      g_new_net.Get(x)->recv_bytes(b,SYNC_SIZE);
      if (memcmp(b,g_con_str,SYNC_SIZE))
      {
#if defined(_WIN32) || defined(QTUI) //Ray 1.5
        int a=g_lvnetcons.FindItemByParam((int)g_new_net.Get(x));
        if (a >= 0)
        {
	 //debug_printf("Delet SYNC\n"); 
          g_lvnetcons.DeleteItem(a);
        }
#endif// _WIN32 && QTUI
        //debug_printf("Could not connect to host: failed auth!\n");
        delete g_new_net.Get(x);
        g_new_net.Del(x--);
#if defined(_WIN32) && !defined(QTUI)
        PostMessage(g_netstatus_wnd,WM_USER_TITLEUPDATE,0,0);
#elif defined(QTUI)
	((Vaste_Idd_Net *)g_netstatus_wnd)->UpdateWinTitle();
#endif
      }
      else if (!allowIP(g_new_net.Get(x)->get_remote()))
      {
	    //debug_printf("IP is denied\n");
#if defined(_WIN32) || defined(QTUI) //Ray 1.5
        int a=g_lvnetcons.FindItemByParam((int)g_new_net.Get(x));
        if (a >= 0)
        {
          g_lvnetcons.DeleteItem(a);
        }
#endif
        //debug_printf("Could not connect to host: host not in access list!\n");
        delete g_new_net.Get(x);
        g_new_net.Del(x--);
#if defined(_WIN32) && !defined(QTUI)
        PostMessage(g_netstatus_wnd,WM_USER_TITLEUPDATE,0,0);
#elif defined(QTUI)
	((Vaste_Idd_Net *)g_netstatus_wnd)->UpdateWinTitle();
#endif
      }
      else
      {
	    //debug_printf("else IP denied\n");
#if defined(_WIN32) || defined(QTUI) //Ray 1.5
        int a=g_lvnetcons.FindItemByParam((int)g_new_net.Get(x));
        if (a >= 0)
        {
          char newconstr[512];
          struct in_addr in;
          in.s_addr=g_new_net.Get(x)->get_remote();
          char *ad=inet_ntoa(in);
          if (!ad) ad="?";
          int port=g_new_net.Get(x)->get_remote_port();
          if (!port)
            sprintf(newconstr,"%s (incoming)",ad);
          else sprintf(newconstr,"%s:%d",ad,(int)(unsigned short)port);
          g_lvnetcons.SetItemText(a,0,"OK");
          g_lvnetcons.SetItemText(a,1,newconstr);
          char descstr[16+SHA_OUTSIZE*2+32];
          unsigned char *hash=g_new_net.Get(x)->get_remote_pkey_hash();
          char *t;
          int nouser=0;
          if ((t=findPublicKey(hash,NULL)) && *t) safe_strncpy(descstr,t,16);
          else { descstr[0]=0; nouser=1; }

          if (!nouser) strcat(descstr," (");
          int x;
          for (x = 0; x < 3; x ++) sprintf(descstr+strlen(descstr),"%02X",hash[x]);
            strcat(descstr,"...");
          if (!nouser) 
            strcat(descstr,")");

          if (port)
            g_lvnetcons.SetItemText(a,2,"100");
          g_lvnetcons.SetItemText(a,3,descstr);

        }
#endif//WIN32
        //debug_printf("Connected to remote host\n");
        C_MessageQueue *newq=new C_MessageQueue(g_new_net.Get(x));
        SendCaps(newq);
        g_mql->AddMessageQueue(newq);
        DoPing(newq);
        g_new_net.Del(x--);
#if defined(_WIN32) && !defined(QTUI)
        PostMessage(g_netstatus_wnd,WM_USER_TITLEUPDATE,0,0);
#elif defined(QTUI)
	((Vaste_Idd_Net *)g_netstatus_wnd)->UpdateWinTitle();
#endif//WIN32
      }
    }
  }//end for x=0; x<g_new
}// end HandleNewOutcons

void NetKern_Run()
{
  ListenToSocket();
  HandleNewOutCons();
}

void AddConnection(char *str, int port, int rating)
{
  C_Connection *newcon;
  newcon=new C_Connection(str,port,g_dns);
  newcon->send_bytes(g_con_str,SYNC_SIZE);
  g_new_net.Add(newcon);
#if defined(_WIN32) || defined(QTUI)
  char str2[512];
  sprintf(str2,"%s:%d",str,(int)(unsigned short)port);
  //debug_printf("(int)new = %d\n", (int)newcon);
  g_lvnetcons.InsertItem(0,"Connecting",(int)newcon);
  //debug_printf("AddConnection IP:port = %s\n", str2);
  g_lvnetcons.SetItemText(0,1,str2);
  sprintf(str2,"%d",rating);
  g_lvnetcons.SetItemText(0,2,str2);
#endif

}


void DoPing(C_MessageQueue *mq)
{
#ifdef _DEBUG
#ifdef QTUI
	qDebug("just in DoPing,mq=%p,g_mql->GetNumQueues()=%d\n",mq,g_mql->GetNumQueues());
#endif
  //debug_printf("just in DoPing,mq=%p,g_mql->GetNumQueues()=%d,mq,g_mql->GetNumQueues()\n");
#endif
  if (!mq && !g_mql->GetNumQueues()) return;

  int sendping=0;

	C_MessagePing rep;
	if (g_route_traffic && g_port && g_listen && !g_listen->is_error() && g_config->ReadInt("advertise_listen",1))
  {
    rep.m_ip=(g_forceip&&g_forceip_addr!=INADDR_NONE)?g_forceip_addr:  
      (mq ? mq->get_con()->get_interface() : 
            g_mql->GetQueue(0)->get_con()->get_interface());

		rep.m_port=g_listen->port();
    sendping++;
  }

  if (g_regnick[0])
  {
    if (g_regnick[0] != '.') sendping++;
    safe_strncpy(rep.m_nick,g_regnick,sizeof(rep.m_nick));
  }

#ifdef _DEBUG
#ifdef QTUI
	qDebug("sendping=%d",sendping);
#endif
  //debug_printf("just in DoPing,mq=%p,g_mql->GetNumQueues()=%d,mq,g_mql->GetNumQueues()\n");
#endif
  if (sendping)
  {
  	T_Message msg={0,};
	  msg.data=rep.Make();
	  if (msg.data)
	  {
	    msg.message_type=MESSAGE_PING;
		  msg.message_length=msg.data->GetLength();
		  if (g_last_pingid_used && mq) 
      {
        msg.message_guid=g_last_pingid;
        msg.message_ttl=G_DEFAULT_TTL;
        msg.data->Lock();
        msg.message_prio=C_MessageQueueList::GetMessagePriority(msg.message_type);
        C_MessageQueue::calc_md5(&msg,msg.message_md5);
        mq->send_message(&msg);
        msg.data->Unlock();
      }     
      else 
      {
        g_mql->send(&msg);
        g_last_pingid=msg.message_guid;
        g_last_pingid_used=1;
      }
	  }
  }
  if (mq) // if we're bringin up a new connection, do a "search"
  {
  	T_Message msg={0,};
    msg.data=new C_SHBuf(0);
    msg.message_type=MESSAGE_SEARCH_USERLIST;
    msg.message_length=0;
    if (time(NULL)-g_last_scanid_used > 120)
    {
      g_mql->send(&msg);
      g_last_scanid_used=time(NULL);
      g_last_scanid=msg.message_guid;
    }
    else if (mq)
    {
      msg.message_guid=g_last_scanid;
      msg.message_ttl=G_DEFAULT_TTL;
      msg.data->Lock();
      msg.message_prio=C_MessageQueueList::GetMessagePriority(msg.message_type);
      C_MessageQueue::calc_md5(&msg,msg.message_md5);
      mq->send_message(&msg);
      msg.data->Unlock();
    }
  }
}

void SendSwitchToAESCaps(C_MessageQueue *mq)
{
  if (mq) // send caps
  {
    C_MessageLocalCaps l;
    
    // send the switch to AES cap with the prefered aes keysize.
    l.add_cap(MLC_PADLOCKSL_SWITCH_TO_AES, g_aes_keysize);
    
    T_Message msg={0,};
    msg.data=l.Make();
    if (msg.data)
    {
      msg.message_type=MESSAGE_LOCAL_CAPS;
      msg.message_length=msg.data->GetLength();
      msg.message_ttl=1;
      CreateID128(&msg.message_guid);

      msg.message_prio=C_MessageQueueList::GetMessagePriority(msg.message_type);
      msg.switch_flag = 1; // indicate this message is the switch message.
      C_MessageQueue::calc_md5(&msg,msg.message_md5);

      msg.data->Lock();
      mq->send_message(&msg);
      msg.data->Unlock();
    }
  }
}

#if defined(_WIN32) && !defined(QTUI)
BOOL WINAPI Net_DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static dlgSizeInfo sizeinf={"net",90,140,440,330};
  static ChildWndResizeItem rlist[]={
    {IDC_ADDCONNECTION,0x1010},
    {IDC_NETCONS,0x0011},
    {IDC_NEWCONNECTION,0x0010},
  };
  switch (uMsg)
  {
    case WM_GETMINMAXINFO:
      {
        LPMINMAXINFO m=(LPMINMAXINFO)lParam;
        if (m)
        {
          m->ptMinTrackSize.x=245;
          m->ptMinTrackSize.y=180;
        }
      }
    return 0;
    case WM_CLOSE:
      CheckMenuItem(GetMenu(g_mainwnd),
        ID_VIEW_NETWORKCONNECTIONS,MF_UNCHECKED|MF_BYCOMMAND);
      ShowWindow(hwndDlg,SW_HIDE);
      g_config->WriteInt("net_vis",0);
    return 0;
    case WM_SHOWWINDOW:
      {
        static int m_hack;
        if (!m_hack)
        {
          m_hack=1;
          int m=g_config->ReadInt("net_maximized",0);
          handleDialogSizeMsgs(hwndDlg,WM_INITDIALOG,0,0,&sizeinf);
          if (m) ShowWindow(hwndDlg,SW_SHOWMAXIMIZED);
        }
      }
    return 0;
    case WM_MOVE:
      handleDialogSizeMsgs(hwndDlg,uMsg,wParam,lParam,&sizeinf);
    return 0;
    case WM_SIZE:
      if (wParam != SIZE_MINIMIZED)
      {
        childresize_resize(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
        handleDialogSizeMsgs(hwndDlg,uMsg,wParam,lParam,&sizeinf);
      }
    return 0;
    case WM_TIMER:
      if (IsWindowVisible(hwndDlg))
      {
        int l=g_lvnetcons.GetCount();
        int x;
        for (x = 0; x < l; x ++)
        {
          C_Connection *thiscon = (C_Connection *)g_lvnetcons.GetParam(x);
          if (thiscon && thiscon->was_ever_connected())
          {
            char str[128];
            int sbps, rbps;
            thiscon->get_last_bps(&sbps,&rbps);

            sprintf(str,"%d.%dk/s out, %d.%dk/s in",
              sbps/1024,((sbps*10)/1024)%10,
              rbps/1024,((rbps*10)/1024)%10);
            if (g_extrainf)
            {
              for (int n=g_mql->GetNumQueues()-1; n >= 0; n --)
              {
                C_MessageQueue *q=g_mql->GetQueue(n);
                if (thiscon == q->get_con())
                {
                  sprintf(str+strlen(str), " (%d,%d,%d)",
                    q->get_stat_send(),
                    q->get_stat_recv(),
                    q->get_stat_drop());
                  break;
                }
              }
            }

            g_lvnetcons.SetItemText(x,0,str);
          }
        }
      }

    return 0;
    case WM_INITDIALOG:

      CreateTooltip(GetDlgItem(hwndDlg,IDC_REMOVECATCH),"Disconnect and remove selected host(s)");
      CreateTooltip(GetDlgItem(hwndDlg,IDC_CONNECTCATCH),"Connect to selected host(s)");
      CreateTooltip(GetDlgItem(hwndDlg,IDC_REMOVECONNECTION),"Disconnect from selected host(s)");
      CreateTooltip(GetDlgItem(hwndDlg,IDC_ADDCONNECTION),"Connect to host specified in text box");

      SendDlgItemMessage(hwndDlg,IDC_CLEAR,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_CLEARALL),IMAGE_ICON,16,16,0));
      SendDlgItemMessage(hwndDlg,IDC_REMOVECATCH,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_DISCONNECT),IMAGE_ICON,16,16,0));
      SendDlgItemMessage(hwndDlg,IDC_CONNECTCATCH,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_CONNECT),IMAGE_ICON,16,16,0));


      SendDlgItemMessage(hwndDlg,IDC_REMOVECONNECTION,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_STOPDL),IMAGE_ICON,16,16,0));
      SendDlgItemMessage(hwndDlg,IDC_ADDCONNECTION,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_CONNECT),IMAGE_ICON,16,16,0));


      SetTimer(hwndDlg,1,250,NULL);
      g_lvnetcons.setwnd(GetDlgItem(hwndDlg,IDC_NETCONS));
      g_lvnetcons.AddCol("Status",g_config->ReadInt("net_col3",133));
      g_lvnetcons.AddCol("Host",g_config->ReadInt("net_col1",126));
      g_lvnetcons.AddCol("Rating",g_config->ReadInt("net_col2",45));
      g_lvnetcons.AddCol("User (key)",g_config->ReadInt("net_col4",108));
      g_mql->SetStatusListView(&g_lvnetcons);
      SetDlgItemInt(hwndDlg,IDC_NUMCONUP,g_keepup,0);
      childresize_init(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
#ifdef SAVE_CB_HOSTS
      {
        int x;
        int p=g_config->ReadInt("concb_pos",0);
        for (x = 0 ; x < MAX_CBHOSTS; x ++)
        {
          char buf[123];
          sprintf(buf,"concb_%d",(p+x)%MAX_CBHOSTS);
          char *o=g_config->ReadString(buf,"");
          if (*o) SendDlgItemMessage(hwndDlg,IDC_NEWCONNECTION,CB_INSERTSTRING,0,(LPARAM)o);
        }
      }
#endif
    case WM_USER_TITLEUPDATE:

      {
        char buf[1024];
        strcpy(buf,"Network Status");
        int numcons=g_mql->GetNumQueues();
        int numneg=g_new_net.GetSize();
        int a=0;

        if (numcons)
        {
          sprintf(buf+strlen(buf),"%s%d connection%s up",a++?", ":" (",numcons,numcons==1?"":"s");
        }
        if (numneg)
        {
          sprintf(buf+strlen(buf),"%s%d connecting",a++?", ":" (",numneg);
        }

        if (a) strcat(buf,")");
        else strcat(buf," (network down)");
       
        strcat(buf," - " APP_NAME);
        SetWndTitle(hwndDlg,buf);
      }

    return 0;
    case WM_DESTROY:
      g_config->WriteInt("net_col3",g_lvnetcons.GetColumnWidth(0));
      g_config->WriteInt("net_col1",g_lvnetcons.GetColumnWidth(1));
      g_config->WriteInt("net_col2",g_lvnetcons.GetColumnWidth(2));
      g_config->WriteInt("net_col4",g_lvnetcons.GetColumnWidth(3));
    return 0;
    case WM_NOTIFY:
      {
        LPNMHDR l=(LPNMHDR)lParam;
        if (l->idFrom==IDC_NETCONS)
        {
          if (l->code == NM_DBLCLK)
          {
            SendMessage(hwndDlg,WM_COMMAND,IDC_CONNECTCATCH,0);
          }
          int isSel=!!ListView_GetSelectedCount(l->hwndFrom);
          int isCon=0;
          int isDiscon=0;
          if (isSel)
          {
            int a,n=g_lvnetcons.GetCount();
            for (a = 0; a < n; a ++)
            {
              if (g_lvnetcons.GetSelected(a))
              {
                if (g_lvnetcons.GetParam(a)) isCon++;
                else isDiscon++;
              }
              if (isCon && isDiscon) break;
            }
          }
          EnableWindow(GetDlgItem(hwndDlg,IDC_REMOVECONNECTION),isCon);
          EnableWindow(GetDlgItem(hwndDlg,IDC_CONNECTCATCH),isDiscon);
          EnableWindow(GetDlgItem(hwndDlg,IDC_REMOVECATCH),isSel);
        }
      }
    return 0;
    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
        case IDC_NEWCONNECTION:
          if (HIWORD(wParam) == CBN_SELCHANGE)
          {
            EnableWindow(GetDlgItem(hwndDlg,IDC_ADDCONNECTION),1);
          }
          else
          {
            char buf[32];
            GetDlgItemText(hwndDlg,IDC_NEWCONNECTION,buf,sizeof(buf));
            EnableWindow(GetDlgItem(hwndDlg,IDC_ADDCONNECTION),!!buf[0]);
          }
        return 0;
        case IDC_NUMCONUP:
          {
            int r;
            BOOL b;
            r=GetDlgItemInt(hwndDlg,IDC_NUMCONUP,&b,0);
            if (b)
            {
              g_keepup=r;
              g_config->WriteInt("keepupnet",r);
            }
          }
        return 0;
        case IDC_ADDCONNECTION:
          {
            char text[512];
            int port=1337;
            char *p=text;
            GetDlgItemText(hwndDlg,IDC_NEWCONNECTION,text,511);
            if (strlen(text)>0)
            {
              if (SendDlgItemMessage(hwndDlg,IDC_NEWCONNECTION,CB_FINDSTRINGEXACT,-1,(LPARAM)text) == CB_ERR)
              {
                if (SendDlgItemMessage(hwndDlg,IDC_NEWCONNECTION,CB_GETCOUNT,0,0)>MAX_CBHOSTS)
                {
                  SendDlgItemMessage(hwndDlg,IDC_NEWCONNECTION,CB_DELETESTRING,SendDlgItemMessage(hwndDlg,IDC_NEWCONNECTION,CB_GETCOUNT,0,0)-1,0);
                }
                SendDlgItemMessage(hwndDlg,IDC_NEWCONNECTION,CB_INSERTSTRING,0,(LPARAM)text);
#ifdef SAVE_CB_HOSTS
                int p=g_config->ReadInt("concb_pos",0);
                char buf[123];
                sprintf(buf,"concb_%d",p);
                g_config->WriteString(buf,text);
                p++;
                g_config->WriteInt("concb_pos",p%MAX_CBHOSTS);
#endif
              }
              while (*p && *p != ':') p++;
              if (*p==':') 
              {
                *p++=0;
                port=atoi(p);
              }
              if (port)
              {
                AddConnection(text,port,1);
              }
            }
          }
        return 0;

        case IDC_REMOVECONNECTION:
          {
            int n=g_lvnetcons.GetCount();
            int x;
            for (x = 0; x < n; x++)
            {
              if (g_lvnetcons.GetSelected(x))
              {
                C_Connection *t= (C_Connection*)g_lvnetcons.GetParam(x);
                if (t)
                  t->close(1);
                //g_lvnetcons.DeleteItem(x--);
                //n--;
              }
            }
          }
        break;
        case IDC_REMOVECATCH:
          {
            int n=g_lvnetcons.GetCount();
            int x;
            for (x = 0; x < n; x++)
            {
              if (g_lvnetcons.GetSelected(x))
              {
                C_Connection *t= (C_Connection*)g_lvnetcons.GetParam(x);
                if (t)
                  t->close(1);
                g_lvnetcons.DeleteItem(x--);
                n--;
              }
            }
          }
        break;
        case IDC_CONNECTCATCH:
          {
            int n=g_lvnetcons.GetCount();
            int x;
            for (x = 0; x < n; x++)
            {
              if (g_lvnetcons.GetSelected(x))
              {
                if (!g_lvnetcons.GetParam(x))
                {
                  char text[512];
                  int port;
                  char rat[32];
                  g_lvnetcons.GetText(x,1,text,sizeof(text));
                  g_lvnetcons.GetText(x,2,rat,sizeof(rat));
                  
                  char *p=text;
                  while (*p != ':' && *p) p++;
                  if (*p)
                  {
                    *p++=0;
                    port=atoi(p);
                  }
                  else port=1337;

                  g_lvnetcons.DeleteItem(x--);
                  AddConnection(text,port,atoi(rat));
                }
              }
            }
          }
        break;
      }
    return 0;
  }
  return 0;
}

#endif//WIN32
