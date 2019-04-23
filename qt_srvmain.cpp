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
/*
    PadLockSL - qt_srvmain.cpp (QT-GUI client main entry point and a lot of code :)
    Author: Stanley San
    Date Created: 2003.12.1
*/

/*
 * This file implement exchange additional AES capability message.
 */

#include "main.h"
#include "rsa/md5.h"

#include "m_upload.h"
#include "m_chat.h"
#include "m_search.h"
#include "m_ping.h"
#include "m_file.h"
#include "m_lcaps.h"
#include "m_keydist.h"
#include "netkern.h"
#include "xfers.h"
#include "xferwnd.h"

#include "netq.h"
#include "srchwnd.h"
#include "vaste_mainwindow.h"
#include "vq_chat.h"
#include "qapplication.h"
#include "tag.h"

#include "vaste_idd_xfers.h"

#define VERSION "v1.0"
//#define VERSION "server 1.0a0"

//stanleysan

#ifdef _WIN32
HINSTANCE g_hInst;
#endif


QWidget* g_mainwnd;

int g_timeout; // timeout in seconds for update user list and chatroom list.

int g_aes_keysize;

unsigned char g_networkhash[SHA_OUTSIZE];
int g_use_networkhash;

char *g_nameverstr=APP_NAME " " VERSION;
char *g_def_extlist="ppt;doc;xls;txt;zip;";

C_FileDB *g_database, *g_newdatabase;
C_AsyncDNS *g_dns;
C_Listen *g_listen;
C_MessageQueueList *g_mql;
C_Config *g_config;

char g_config_prefix[1024];

char g_config_mainini[1024];

char g_profile_name[128];
int g_extrainf;
int g_keepup;
int g_conspeed,g_route_traffic;
int g_do_log;
int g_max_simul_dl;
unsigned int g_max_simul_dl_host;
int g_forceip, g_forceip_addr;
int g_use_accesslist;
int g_appendprofiletitles;
int g_do_autorefresh;
int g_accept_downloads;
int g_port;
int g_chat_timestamp;
int g_keydist_flags;

R_RSA_PRIVATE_KEY g_key;
unsigned char g_pubkeyhash[SHA_OUTSIZE];

char g_regnick[32];

char g_filedlg_ulpath[256];

int g_throttle_flag, g_throttle_send, g_throttle_recv;

int g_search_showfull;

int g_search_showfullbytes;


int g_scanloadhack;
char g_scan_status_buf[128];
time_t g_next_refreshtime;

time_t g_last_pingtime,g_last_bcastkeytime;
T_GUID g_last_scanid;
int g_last_scanid_used;
T_GUID g_search_id;
unsigned int g_search_id_time;
T_GUID g_last_pingid;
int g_last_pingid_used;
T_GUID g_client_id;
char g_client_id_str[33];


C_ItemList<C_UploadRequest> uploadPrompts;
C_ItemList<C_KeydistRequest> keydistPrompts;


void main_BroadcastPublicKey(T_Message *src)
{
  C_KeydistRequest rep;
  rep.set_nick(g_regnick);
  rep.set_flags((g_port && g_listen && !g_listen->is_error())?M_KEYDIST_FLAG_LISTEN:0);
  R_RSA_PUBLIC_KEY k;
  k.bits=g_key.bits;
  memcpy(k.exponent,g_key.publicExponent,MAX_RSA_MODULUS_LEN);
  memcpy(k.modulus,g_key.modulus,MAX_RSA_MODULUS_LEN);
  rep.set_key(&k);

  T_Message msg={0,};
	msg.data=rep.Make();
  if (msg.data)
  {
    if (src)
    {
      msg.message_guid=src->message_guid;
      msg.message_type=MESSAGE_KEYDIST_REPLY;
    }
    else
    {
	    msg.message_type=MESSAGE_KEYDIST;
    }
		msg.message_length=msg.data->GetLength();
    g_mql->send(&msg);
  }
}

static void main_handleKeyDist(C_KeydistRequest *kdr, int pending)
{ 
  if (!kdr->get_key()->bits) return;
  if (findPublicKeyFromKey(kdr->get_key())) return;

  PKitem *p=(PKitem *)malloc(sizeof(PKitem));

  SHAify m;
  safe_strncpy(p->name,kdr->get_nick(),sizeof(p->name));
  p->pk = *kdr->get_key();
  m.add((unsigned char *)p->pk.modulus,MAX_RSA_MODULUS_LEN);
  m.add((unsigned char *)p->pk.exponent,MAX_RSA_MODULUS_LEN);
  m.final(p->hash);

  if (pending) 
    g_pklist_pending.Add(p);
  else 
    g_pklist.Add(p);
  
  savePKList();
}

void main_handleUpload(char *guidstr, char *fnstr, C_UploadRequest *t)
//void main_handleUpload(char *guidstr, char *fnstr)
{
//Modified by Raymond 2003.12.26
TAG tag("main_handleUpload()");

#if defined(QTUI)
  int willq=Xfer_WillQ(fnstr,guidstr);

//stanleysan begin
  int p=g_lvrecvq.InsertItem(g_lvrecvq.GetCount(),fnstr,0);
  char sizebuf[64];
  strcpy(sizebuf,"?");
  int fs_l,fs_h;
  t->get_fsize(&fs_l,&fs_h);
  if (fs_l != -1 || fs_h != -1)
  {
    FormatSizeStr64(sizebuf,fs_l,fs_h);
  }

  g_lvrecvq.SetItemText(p,1,sizebuf);
  g_lvrecvq.SetItemText(p,2,guidstr);
  g_files_in_download_queue++;

  if (g_config->ReadInt("aorecv",1))
  {
/*
    HWND h=GetForegroundWindow();
*/
    //SendMessage(g_mainwnd,WM_COMMAND,ID_VIEW_TRANSFERS,0);
    ((QVaste_MainWin*)g_mainwnd)->editTransfer();

    if (g_config->ReadInt("aorecv_btf",0)) //SetForegroundWindow(g_xferwnd);
    {
	g_xferwnd->setActiveWindow();
	g_xferwnd->raise();
	g_xferwnd->setFocus();
    }
/*
    else SetForegroundWindow(h);
*/

    XferDlg_SetSel(willq);

  }
  RecvQ_UpdateStatusText();
//stanleysan end

#endif
}



void main_MsgCallback(T_Message *message, C_MessageQueueList *_this, C_Connection *cn)
{
	switch(message->message_type)
	{
    case MESSAGE_LOCAL_CAPS:
      {
        C_MessageLocalCaps mlc(message->data);
        int x;
        for (x = 0; x < mlc.get_numcaps(); x ++)
        {
          int n,v;
          mlc.get_cap(x,&n,&v);
          switch (n)
          {
            case MLC_SATURATION:
              debug_printf("got request that saturation be %d on this link\n",v);
              if (cn) cn->set_saturatemode(v); 
// if ((get_saturatemode()&1) && (g_throttle_flag&32)) do outbound saturation
            break;
            case MLC_BANDWIDTH:
              debug_printf("got request that max sendbuf size be %d on this link\n",v);
              if (cn) cn->set_max_sendsize(v);
            break;

	    /* Added by LoraYin 2003/12/5 begin.
	       Get the PADLOCKSL cap.
	    */
	    case MLC_PADLOCKSL_AES_KEYSIZE:
              debug_printf("got indication that the peer site is PADLOCKSL with the aes keysize %d\n",v);
	      if (cn && cn->get_peer_padlocksl_flag() != FLAG_PADLOCKSL) 
	      {
		// set the PADLOCKSL parameters.
		cn->set_aes_keysize(v);
		cn->set_peer_padlocksl_flag(FLAG_PADLOCKSL);

		// send a switch to AES cap then switch to AES.
		for (int i=_this->GetNumQueues()-1; i >= 0; i --)
		{
                  C_MessageQueue *q=_this->GetQueue(i);
                  if (cn == q->get_con())
		  {
		    SendSwitchToAESCaps(q);
                    /* Do not set the send encryption to AES 
                       until the switch message is sent out.
                    */
  	  	    //cn->set_send_encryption(PADLOCKSL_ENCRYPTION_AES);
		    break;
		  }
		}
	      } //end of if
	      break;
	    case MLC_PADLOCKSL_SWITCH_TO_AES:
	      debug_printf("got indication that the peer site is switch to AES with the aes keysize %d\n",v);
	      if (cn) 
	      {
		/* set recv_encryption in case of 
                   getting a switch to AES message from the peer site.
                */
		cn->set_recv_encryption(PADLOCKSL_ENCRYPTION_AES);
	      }
	      break;
	    /* Added by LoraYin 2003/12/5 end.*/
          }
        }
      }

    break;
    case MESSAGE_KEYDIST_REPLY:
    case MESSAGE_KEYDIST:
      
      {
        C_KeydistRequest *r=new C_KeydistRequest(message->data);
        if ((r->get_flags() & M_KEYDIST_FLAG_LISTEN) || (g_port && g_listen && !g_listen->is_error()) && r->get_key()->bits)
        {
          if (g_keydist_flags&1) // add without prompt
          {
            main_handleKeyDist(r,0);
            delete r;
          }
          else if (g_keydist_flags&2) // add with prompt
          {
            keydistPrompts.Add(r);
          }
          else // add to pending
          {
            main_handleKeyDist(r,1);
            delete r;
          }

          if (message->message_type != MESSAGE_KEYDIST_REPLY)
          {
            main_BroadcastPublicKey(message);
          }
        }
        else delete r;
      }
    break;
    case MESSAGE_PING:
    {
      MYSRANDUPDATE((unsigned char *)&message->message_guid,16);
      C_MessagePing rep(message->data);
	     
      if (rep.m_nick[0] && rep.m_nick[0] != '#' && rep.m_nick[0] != '&' &&
          rep.m_nick[0] != '.' && strlen(rep.m_nick)<24)
          main_onGotNick(rep.m_nick,0);
    }
    break;
    case MESSAGE_SEARCH_USERLIST:
      if (g_regnick[0] && g_regnick[0] != '.' && strlen(g_regnick)<24)
      {
	C_MessageSearchReply repl;
	repl.set_conspeed(g_conspeed);
	repl.set_guid(&g_client_id);
        repl.add_item(-1,g_regnick,"Node",g_database->GetNumFiles(),g_database->GetNumKB(),g_database->GetLatestTime());
        T_Message msg={0,};
        msg.message_guid=message->message_guid;
        msg.data=repl.Make();
	if (msg.data)
	{
	  msg.message_type=MESSAGE_SEARCH_REPLY;
	  msg.message_length=msg.data->GetLength();
  	  _this->send(&msg);
	}
      }
    break;
    case MESSAGE_SEARCH:
      if ((g_accept_downloads&1) && g_database->GetNumFiles()>0)
      {
        C_MessageSearchRequest req(message->data);
        char *ss=req.get_searchstring();
        if (
           (ss[0] == '/' && (g_accept_downloads&4)) || (ss[0] && ss[0] != '/' && (g_accept_downloads&2))
           )
        {
	  C_MessageSearchReply repl;
	  repl.set_conspeed(g_conspeed);
	  repl.set_guid(&g_client_id);
          g_database->Search(ss,&repl,_this,message,main_MsgCallback);
        }
      }
    break;
    case MESSAGE_SEARCH_REPLY:
    {
      if (g_last_scanid_used && !memcmp(&g_last_scanid,&message->message_guid,16))
      {
        C_MessageSearchReply repl(message->data);
        if (repl.get_numitems()==1)
        {
	  char name[1024],metadata[256];
          if (!repl.get_item(0,NULL,name,metadata,NULL,NULL,NULL))
            main_onGotNick(name,0);
        }
      }
      else Search_AddReply(message);
      }
    break;
    case MESSAGE_FILE_REQUEST:
    {
      debug_printf("got file request message\n");
      C_FileSendRequest *r = new C_FileSendRequest(message->data);

      if (!memcmp(r->get_guid(),&g_client_id,sizeof(g_client_id)))
      {
        int n=g_sends.GetSize();
        int x;
          for (x = 0; x < n; x ++)
          {
            if (!memcmp(r->get_prev_guid(),g_sends.Get(x)->get_guid(),16))
            {
              if (r->is_abort()==2)
              {
                int a=g_sends.Get(x)->GetIdx()-UPLOAD_BASE_IDX;
                if (a >= 0 && a < g_uploads.GetSize())
                {
                  char *p=g_uploads.Get(a);
                  if (p)
                  {
                    int lvidx;
                    while ((lvidx=g_lvsend.FindItemByParam((int)p)) >= 0)
                    {
                      g_lvsend.DeleteItem(lvidx);
                    }
                    free(p);
                    g_uploads.Set(a,NULL);
                  }
                }
              }
              
              g_sends.Get(x)->set_guid(&message->message_guid);
              g_sends.Get(x)->onGotMsg(r);
              break;
            }
          }
          if (x == n && !r->is_abort()) // new file request
          {
            if (g_config->ReadInt("limit_uls",1) && n < g_config->ReadInt("ul_limit",160))
            {
              char fn[2048];
              fn[0]=0;
              int idx=r->get_idx();
              if (idx >= 0)
              {
                if (idx < UPLOAD_BASE_IDX)
                {
                  if (!(g_accept_downloads&1)) fn[0]=0;
                  else g_database->GetFile(idx,fn,NULL,NULL,NULL);
                }
                else
                {
                  idx-=UPLOAD_BASE_IDX;
                  if (idx<g_uploads.GetSize() && g_uploads.Get(idx)) 
                  {
                    safe_strncpy(fn,g_uploads.Get(idx),sizeof(fn));
                    int lvidx;
                    if ((lvidx=g_lvsend.FindItemByParam((int)g_uploads.Get(idx))) >= 0)
                    {
                      g_lvsend.DeleteItem(lvidx);
                    }
                  }
                }
              }
              if (fn[0])
              {
                XferSend *a=new XferSend(_this,&message->message_guid,r,fn);

                // remove any timed out files of a->GetName() from r->get_nick()
                n=g_lvsend.GetCount();
                for (x = 0; x < n; x ++)
                {
                  if (g_lvsend.GetParam(x)) continue;
                  char buf[1024];
                  g_lvsend.GetText(x,3,buf,sizeof(buf));
                  if (strncmp(buf,"Timed out",9)) continue;
                  g_lvsend.GetText(x,0,buf,sizeof(buf));
                  if (strcmp(buf,a->GetName())) continue;
                  g_lvsend.GetText(x,1,buf,sizeof(buf));
                  if (strcmp(buf,r->get_nick())) continue;
                  g_lvsend.DeleteItem(x);
                  x--;
                  n--;
                }

                char *err=a->GetError();
                if (err)
                {
                  if (!g_config->ReadInt("send_autoclear",0))
                  {
                    g_lvsend.InsertItem(0,a->GetName(),0);
                    char buf[32];
                    int fs_l,fs_h;
                    a->GetSize((unsigned int *)&fs_l,(unsigned int *)&fs_h);
                    FormatSizeStr64(buf,fs_l,fs_h);
                    g_lvsend.SetItemText(0,1,r->get_nick());
                    g_lvsend.SetItemText(0,2,buf);
                    g_lvsend.SetItemText(0,3,err);
                  }
                  delete a;
                }
                else
                {
                  g_sends.Add(a);
                  g_lvsend.InsertItem(0,a->GetName(),(int)a);
                  char buf[32];
                  int fs_l,fs_h;
                  a->GetSize((unsigned int *)&fs_l,(unsigned int *)&fs_h);
                  FormatSizeStr64(buf,fs_l,fs_h);
                  g_lvsend.SetItemText(0,1,r->get_nick());
                  g_lvsend.SetItemText(0,2,buf);
                  g_lvsend.SetItemText(0,3,"Sending");
                }
                //PostMessage(g_xferwnd,WM_USER_TITLEUPDATE,0,0);
		((Vaste_Idd_Xfers*)g_xferwnd)->UpdateTitle();
              }
              else
              {
                T_Message msg={0,};
                C_FileSendReply reply;
                reply.set_error(1);
                msg.data=reply.Make();
                if (msg.data)
                {
                  msg.message_type=MESSAGE_FILE_REQUEST_REPLY;
                  msg.message_length=msg.data->GetLength();
                  msg.message_guid=message->message_guid;
                  g_mql->send(&msg);
                }
              }
            }
          }
        }
        delete r;
      }
      break;
    case MESSAGE_FILE_REQUEST_REPLY:
      {
        TAG tag("MESSAGE_FILE_REQUEST_REPLY");
        int n=g_recvs.GetSize();
        int x;
        for (x = 0; x < n; x ++)
        {
          if (!memcmp(g_recvs.Get(x)->get_guid(),&message->message_guid,16))
          {
            g_recvs.Get(x)->onGotMsg(new C_FileSendReply(message->data));
            break;
          }
        }
      }
    break;
    case MESSAGE_CHAT_REPLY:
      chat_HandleMsg(message);
    break;
    case MESSAGE_CHAT:
      debug_printf("got chat message\n");
      if (chat_HandleMsg(message) && g_regnick[0])
      { // send reply
       	C_MessageChatReply rep;          
       	rep.setnick(g_regnick);

        T_Message msg={0,};
        msg.message_guid=message->message_guid;
        msg.data=rep.Make();
        if (msg.data)
        {
          msg.message_type=MESSAGE_CHAT_REPLY;
          msg.message_length=msg.data->GetLength();
          _this->send(&msg);
        }
      }
    break;
    case MESSAGE_UPLOAD:
      {
        int upflag=g_config->ReadInt("accept_uploads",1);
        if (upflag&1) {
          C_UploadRequest *r = new C_UploadRequest(message->data);
          if (!stricmp(r->get_dest(),g_client_id_str) || (g_regnick[0] && !stricmp(r->get_dest(),g_regnick)))
          {
            char *fn=r->get_fn();
            int weirdfn=!strncmp(fn,"..",2) || strstr(fn,":") || strstr(fn,"\\..") || strstr(fn,"/..") || strstr(fn,"..\\") || strstr(fn,"../") || fn[0]=='\\' || fn[0] == '/';
            if (!(upflag & 4) || weirdfn)
            {
              char *p=fn;
              while (*p) p++;
              while (p >= fn && *p != '/' && *p != '\\') p--;
              p++;
              if (p != fn || !weirdfn) fn=p;
              else fn="";
            }

            if (fn[0])
            {              
              if (!(upflag & 2)) 
              {
                char str[64];
                MakeID128Str(r->get_guid(),str);
                sprintf(str+strlen(str),":%d",r->get_idx());
                main_handleUpload(str,fn,r);
              }
              else 
              {
                char *t=strdup(fn);
                r->set_fn(t);
                free(t);
                uploadPrompts.Add(r);
                break;
              }
            }
            else
            {
              debug_printf("got upload request that was invalid\n");
            }
          }
          delete r;
        }
      }
    break;
    default:
      //debug_printf("unknown message received : %d\n",message->message_type);
    break;
  }
}

void update_set_port()
{
  delete g_listen;
  g_listen=NULL;

  g_port=(g_route_traffic && g_config->ReadInt("listen",1)) ? 
            g_config->ReadInt("port",1337) : 0;
  if (g_port) 
  {
    debug_printf("[main] creating listen object on %d\n",g_port);
    g_listen = new C_Listen((short)g_port);
  }
}


void doDatabaseRescan()
{
  debug_printf("DB: reinitializing database\n");
  if (g_database && g_database != g_newdatabase && g_newdatabase) 
  {
    debug_printf("DB: deleting temp db\n");
    delete g_newdatabase;
  }             
  g_newdatabase=new C_FileDB();
  if (g_config->ReadInt("use_extlist",0))
    g_newdatabase->UpdateExtList(g_config->ReadString("extlist",g_def_extlist));
  else 
    g_newdatabase->UpdateExtList("*");

  g_newdatabase->Scan(g_config->ReadString("databasepath",""));
  if (!g_database || !g_database->GetNumFiles())
  {
    if (g_database) 
    {
      debug_printf("DB: making scanning db live\n");
      delete g_database;
    }
    g_database=g_newdatabase;
  }
}


void main_onGotChannel(char *cnl)
{
	GetQVMainWin->main_onGotChannel(cnl);
}



void main_onGotNick(char *nick, int del)
{
  GetQVMainWin->main_onGotNick(nick,del);
}


int g_exit=0;

#if defined(_LINUX)
void sighandler(int sig)
{
  if (sig == SIGHUP)
  {
    g_do_log=0;
  }
  if (sig == SIGPIPE) debug_printf("got SIGPIPE!\n");
  if (sig == SIGINT)
  {
    g_exit=1;
    debug_printf("got SIGINT, setting global exit flag!\n");
  }
}
#endif

extern void load_config(char * argv);

int main(int argc, char **argv)
{
#ifdef _LINUX
  signal(SIGHUP,sighandler);
  signal(SIGPIPE,sighandler);
  signal(SIGINT,sighandler);
#endif
 
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(1, 1), &wsaData))
  {
    memset(&g_key,0,sizeof(g_key));
    MessageBox(NULL,"Error initializing Winsock\n",APP_NAME " Error",0);
    return 1;
  }
#endif

  QApplication app(argc,argv);
  load_config(argv[0]);


//QT init 
//begin

  QVaste_MainWin *mainWin = new QVaste_MainWin;
  app.setMainWidget(mainWin);
  mainWin->show();
  app.exec();

  delete mainWin;
//end

  SaveNetQ();

  if (g_config->ReadInt("db_save",1) && g_database && g_database != g_newdatabase) 
  {
    //printf("flushing db\n");
    char dbfile[1024+8];
    sprintf(dbfile,"%s.pr2",g_config_prefix);
    g_database->writeOut(dbfile);
  }


  delete g_listen;
  delete g_dns;
  if (g_newdatabase != g_database) delete g_newdatabase;
  delete g_database;

  int x;
  for (x = 0; x < SEARCHCACHE_NUMITEMS; x ++) delete g_searchcache[x];

  for (x = 0; x < g_recvs.GetSize(); x ++) delete g_recvs.Get(x);
  for (x = 0; x < g_sends.GetSize(); x ++) delete g_sends.Get(x);
  for (x = 0; x < g_new_net.GetSize(); x ++) delete g_new_net.Get(x);
  for (x = 0; x < g_uploads.GetSize(); x ++) free(g_uploads.Get(x));

  delete g_mql;

  delete g_config;

  memset(&g_key, 0, sizeof(g_key));
 

  return 0;

}

