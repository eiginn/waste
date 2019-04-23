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
 * This file implements the protocol of chat
 * Author: Stanley San
 * Date Created: 2003.12.1
*/

#include "vq_chat.h"
#include "vaste_chat.h"

#include <qtextbrowser.h>
#include <qlabel.h>
#include <qdir.h>

#include "main.h"
#include "ui_treeview.h"

#define MAX_CHATTEXT_SIZE 30000

#include "main.h"

#include "m_chat.h"

extern C_Config* g_config;

chatroom_item *L_Chatroom;

inline char* __safe_strncpy(char* dst,const char* src,int max)
{
	if(src)
		return strncpy(dst,src,max);
	else
	{
		debug_printf("error,src==NULL\n");
		return NULL;
	}
}
#define strncpy __safe_strncpy

void add_chatline(QTextBrowser* hwndChat,const char *line);

int chat_HandleMsg(T_Message *message)
{
	qDebug("chat_HandleMsg");
	chatroom_item *p=L_Chatroom;
	
	if (message->message_type == MESSAGE_CHAT_REPLY)
	{
		qDebug("reply");

		C_MessageChatReply repl(message->data);
		char *n=repl.getnick();
		if (strlen(n) < 24 && n[0] && n[0] != '.') main_onGotNick(n,0);
		while(p!=NULL)
		{
			if(!memcmp(&p->lastMsgGuid,&message->message_guid,16))
			{
				if (n[0])
				{
					// see if nick is already in list
					if (p->channel[0] == '#' || p->channel[0] == '&')
					{
/////
						//((Vaste_ChatChan*)p->hwnd)->gotNick(n,TVIS_BOLD,TVIS_BOLD,0);
						((Vaste_ChatChan*)p->hwnd)->gotNick(n,0,0,0);
					}
					else
					{
						char buf[128];
						buf[0]='\0';
						char *tmp="Message sent, delivery confirmed";
///
						strncpy(buf,((Vaste_ChatPriv*)p->hwnd)->Label_IDC_STATUS->text().ascii(),sizeof(buf));
						if (strncmp(buf,tmp,strlen(tmp)))
						{
							sprintf(buf,"%s - (%dms roundtrip)",tmp,GetTickCount()-p->lastmsgguid_time);
						}
						else
						{
							int a=1;
							char *q=buf+strlen(tmp)+1;
							if (*q == '(') a=atoi(q+1);
							sprintf(buf,"%s (%d recipients) - (%dms max roundtrip).",tmp,a+1,GetTickCount()-p->lastmsgguid_time);
						}
////
						((Vaste_ChatPriv*)p->hwnd)->Label_IDC_STATUS->setText(buf);
					}
				}
				break;
			}
			p=p->next;
		}
		return !!p;
	}
	int retval=0;
	C_MessageChat chat(message->data);
	char *cnl=chat.get_dest();
	{
		char *n=chat.get_src();
		if (n[0] && strlen(n)<24 && n[0] != '.')
			main_onGotNick(n,0);
	}
	if (cnl && *cnl == '#')
	{
		main_onGotChannel(cnl);
	}
	
	
	if (!strcmp(chat.get_chatstring(),"/whois"))
	{
		if (!strcasecmp(chat.get_dest(),g_regnick) && g_regnick[0])
		{
			char buf[256+512+32];
			sprintf(buf,"/iam/%s (%s",g_config->ReadString("userinfo",APP_NAME " User"),g_nameverstr);
			int length=0;
			if (g_database) length=g_database->GetNumKB();
			else if (g_newdatabase) length=g_newdatabase->GetNumKB();
			
			if (length)
			{
				if (length < 1024)
					sprintf(buf+strlen(buf),", %u KB public",length);
				else if (length < (1 << 20))
					sprintf(buf+strlen(buf),", %u.%u MB public",length>>10,((length%1024)*10)/1024);
				else sprintf(buf+strlen(buf),", %u.%u GB public",length>>20,(((length>>10)%1024)*10)/1024);
			}
			strcat(buf,")");
			C_MessageChat req;
			req.set_chatstring(buf);
			req.set_dest(chat.get_src());
			req.set_src(g_regnick);
			T_Message msg={0,};
			msg.data=req.Make();
			msg.message_type=MESSAGE_CHAT;
			if (msg.data)
			{
				msg.message_length=msg.data->GetLength();
				g_mql->send(&msg);
			}
		}
		return 0;
	}
	
	if (!strncmp(chat.get_chatstring(),"/nick/",6) && chat.get_chatstring()[6] && !strcmp(chat.get_dest(),"&"))
	{
		main_onGotNick(chat.get_chatstring()+6,1);
		
		if (chat.get_chatstring()[6] != '#' && chat.get_chatstring()[6] != '&') while(p!=NULL)
		{
			int isgood=0;
			if (p->channel[0] == '#' || p->channel[0] == '&')
			{
////
				QListView* hwndTree=((Vaste_ChatChan*)p->hwnd)->ListView_IDC_TREE1;

				VUser_TreeViewItem *h= TreeView_GetRootChild(hwndTree);
				while (h)
				{
					if (!strcasecmp(h->text(0).ascii(),chat.get_chatstring()+6))
					{
						isgood=1;
						//((Vaste_ChatChan*)p->hwnd)->gotNick(chat.get_src(),TVIS_BOLD,TVIS_BOLD,0);
						((Vaste_ChatChan*)p->hwnd)->gotNick(chat.get_src(),0,0,0);
						break;
					}
					h=TreeView_GetNextSibling(hwndTree,h);
				}        
			}
			
			if (isgood || !strcasecmp(p->channel,chat.get_chatstring()+6))
			{
				qDebug("before ready add chatline\n");
////
				char txt[4096];
				txt[0] = '\0';
				if (p->channel[0] == '#' || p->channel[0] == '&')
				{
					Vaste_ChatChan* wnd = (Vaste_ChatChan*)(p->hwnd);
					wnd->formatChatString(txt,chat.get_src(),chat.get_chatstring());
					if (txt[0]) add_chatline(wnd->IDC_CHATTEXT,txt);
				}
				else
				{
					Vaste_ChatPriv* wnd = (Vaste_ChatPriv*)(p->hwnd);
					wnd->formatChatString(txt,chat.get_src(),chat.get_chatstring());
					if (txt[0]) add_chatline(wnd->IDC_CHATTEXT,txt);
				}
				retval=1;
			}
			p=p->next;
		}
		return 0;
	}
	
#if 0
	while(p!=NULL)
	{
		if (!strcmp(p->channel,"/sniff/"))
		{
			char txt[4096];
			sprintf(txt,"<%s/%s>: %s",chat.get_src(),chat.get_dest(),chat.get_chatstring());
			add_chatline(p->hwnd,txt);
			break;
		}
		p=p->next;
	}
	p=L_Chatroom;
#endif
	
	//Replies from people in private chats come here
	if (g_regnick[0] && !strcasecmp(cnl,g_regnick)) // privmsg
	{
		qDebug("private msg 1");
		while(p!=NULL)
		{
			qDebug("while: p->channel=%s,chat.get_src()=%s",p->channel,chat.get_src());

			if(!strcasecmp(p->channel,chat.get_src()))
			{
				Vaste_ChatPriv* wnd = (Vaste_ChatPriv*)(p->hwnd);
				char txt[4096];
				txt[0] = '\0';
////
				wnd->formatChatString(txt,chat.get_src(),chat.get_chatstring());
				if (txt[0]) 
				{
					add_chatline(wnd->IDC_CHATTEXT,txt); 
					int f=g_config->ReadInt("gayflashp",65);
					if (f&1) DoFlashWindow(p->hwnd,(f&2)?f/4:0);
					retval=1;
				}
				break;
			}
			p=p->next;
		}

		qDebug("private msg 2");

		if (!p && g_config->ReadInt("allowpriv",1)) // time to create a privchat
		{
			char txt[4096];
			txt[0] = '\0';
			p=chat_ShowRoom(chat.get_src(),1);
	
			((Vaste_ChatPriv*)(p->hwnd))->formatChatString(txt,chat.get_src(),chat.get_chatstring());
			if (txt[0])
			{
				add_chatline(((Vaste_ChatPriv*)(p->hwnd))->IDC_CHATTEXT,txt);
				int f=g_config->ReadInt("gayflashp",65);
				if (f&1) DoFlashWindow(p->hwnd,(f&2)?f/4:0);
				retval=1;
			}
		}
	}
	else if (cnl[0] == '#' || cnl[0] == '&')
	{
		while(p!=NULL)
		{
			if(!stricmp(p->channel,cnl))
			{
				Vaste_ChatChan* wnd = (Vaste_ChatChan*)(p->hwnd);

				char txt[4096];
				txt[0] = '\0';
				//wnd->gotNick(chat.get_src(),TVIS_BOLD,TVIS_BOLD,!strcmp(chat.get_chatstring(),"/leave"));
				wnd->gotNick(chat.get_src(),0,0,!strcmp(chat.get_chatstring(),"/leave"));
				wnd->formatChatString(txt,chat.get_src(),chat.get_chatstring());
				if (txt[0]) 
				{
					add_chatline(wnd->IDC_CHATTEXT,txt);
					int f=g_config->ReadInt("gayflash",64);
					if (f&1) DoFlashWindow(p->hwnd,(f&2)?f/4:0);
					retval=1;
				}
				break;
			}
			p=p->next;
		}
	}
	return retval;
}

void add_chatline(QTextBrowser* hwndChat,const char *line)
{
	qDebug("add_chatline: line=%s",line);
	//SCROLLINFO si={sizeof(si),SIF_RANGE|SIF_POS|SIF_TRACKPOS,};
	//GetScrollInfo(m_hwnd,SB_VERT,&si);
	if(!hwndChat)
	{
		qDebug("Error: add_chatline hwndChat==NULL");
		return;
	}
	
	if (line&&hwndChat)
	{
		int oldsels,oldsele;
		//	SendMessage(m_hwnd,EM_GETSEL,(WPARAM)&oldsels,(LPARAM)&oldsele);
		int pF,iF,pT,iT,sN;
		hwndChat->getSelection(&pF,&iF,&pT,&iT);
		
		{
		for(int i = 0; i<pF-1 ;i++)
			oldsels += hwndChat->paragraphLength(i);
		oldsels += iF;
		}
		{
		for(int i = 0; i<pT-1 ;i++)
			oldsels += hwndChat->paragraphLength(i);
		oldsels += iT;
		}
		
		if(strlen(line)>MAX_CHATTEXT_SIZE-1) return;
		
		qDebug("txt=%s",hwndChat->text().ascii());
		char txt[MAX_CHATTEXT_SIZE];
		txt[0]='\0';
		//GetWindowText(m_hwnd,txt,MAX_CHATTEXT_SIZE-1);
		strncpy(txt,hwndChat->text().utf8(),MAX_CHATTEXT_SIZE-1);
		txt[MAX_CHATTEXT_SIZE-1]=0;
		
		while(strlen(txt)+strlen(line)+4>MAX_CHATTEXT_SIZE)
		{
			char *p=txt;
			while(*p!=0 && *p!='\n') p++;
			if(*p==0) return;
			while (*p=='\n') p++;
			strcpy(txt,p);
			oldsels -= p-txt;
			oldsele -= p-txt;
		}
		if (oldsels < 0) oldsels=0;
		if (oldsele < 0) oldsele=0;
		
		if(txt[0]!=0) strcat(txt,"\n");
		strcat(txt,line);
		
		/*
		CHARFORMAT2 cf2;
		cf2.cbSize=sizeof(cf2);
		cf2.dwMask=CFM_LINK;
		cf2.dwEffects=0;
		SendMessage(m_hwnd,EM_SETCHARFORMAT,SCF_ALL,(LPARAM)&cf2);
		SetWindowText(m_hwnd,txt);
		*/
		qDebug("before setText : txt=%s",txt);
		//hwndChat->setText(tr(txt));
		//hwndChat->setText(QString::fromLocal8Bit(txt));
		hwndChat->setText(QString::fromUtf8(txt));
		
		/////////////////////
		txt[0]='\0';
		QCString cs = hwndChat->text().local8Bit();
		strncpy(txt,(const char*)cs,MAX_CHATTEXT_SIZE-1);
		txt[MAX_CHATTEXT_SIZE-1]=0;
		
		//BSC BEGIN : Color nicks other than ours blue
		char *buffer=txt;
		char testchar=0;
		int subcounter=0;
		char nickbuff[40] = "";
		chatroom_item *chatpointer = L_Chatroom;
		//////////////////////
		
		//Find the right chatroom, if it exists
		while (chatpointer != NULL) {
			if (chatpointer->hwnd == hwndChat->parentWidget()) break;
			chatpointer = chatpointer->next;
		}
		/////////////////////
		if (chatpointer) {
			strcat(nickbuff, chatpointer->channel);
			//strcat(nickbuff, othernick);
			strcat(nickbuff, ">");
			//strcpy(nickbuff, "Scytale>"); //For testing
			while (*buffer)
			{
				if ( (testchar == '<') || (testchar == 0) )
				{
					int isurl=0;
					if (!strncasecmp(buffer,nickbuff,strlen(nickbuff))) isurl=strlen(nickbuff);
					
					if ( (isurl) )
					{
						int spos=buffer - txt - subcounter;
						buffer += isurl;
						/*
						SendMessage(m_hwnd,EM_SETSEL,spos-1,spos + isurl);
						*/
						int tmp = hwndChat->paragraphs();
						int sp=0,si=0;
						int ep=0,ei=0;
						for(int loop=0;loop<tmp;loop++)
						{
							if(sp+hwndChat->paragraphLength(loop)>spos)
								break;
							sp+=hwndChat->paragraphLength(loop);
						}
						si=spos-sp;
						
						//hwndChat->setSelect();
						
						
						/*
						CHARFORMAT2 cf2;
						cf2.cbSize=sizeof(cf2);
						cf2.dwMask = CFM_BOLD | CFM_COLOR;
						cf2.dwEffects = 0;
						cf2.crTextColor = RGB(0,0,255);
						SendMessage(m_hwnd,EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf2);
						*/
						hwndChat->setColor(QColor(0,0,255));
					}
				}
				if (*buffer == '\n') subcounter++;
				if (*buffer) testchar=*buffer++;
			}
			//SendMessage(m_hwnd,EM_SETSEL,oldsels,oldsele);
		}

	hwndChat->scrollToBottom();
		
		
		/////////////
///following maybe useful later ,keep it anyway

#if 0
		//BSC END : Color nicks other than ours blue
		
		//Detect if a link is present in the chat line
		char *t=txt;
		char lt=' ';
		int sub=0;
		while (*t)
		{
			if (lt == ' ' || lt == '\n' || lt == '\r')
			{
				int isurl=0;
				if (*t == '#' || *t == '&') isurl=1;
				else if (!strnicmp(t,"padlocksl:",6)) isurl=6;
				else if (!strnicmp(t,"http:",5)) isurl=5;
				else if (!strnicmp(t,"ftp:",4)) isurl=4;
				else if (!strnicmp(t,"www.",4)) isurl=4;
				
				if (isurl && t[isurl] != ' ' && t[isurl] != '\n' && t[isurl] != '\r' && t[isurl])
				{
					int spos=t-txt-sub;
					t+=isurl;
					while (*t && *t != ' ' && *t != '\n' && *t != '\r') { t++; }
					SendMessage(m_hwnd,EM_SETSEL,spos,(t-txt)-sub);
					CHARFORMAT2 cf2;
					cf2.cbSize=sizeof(cf2);
					cf2.dwMask=CFM_LINK;
					cf2.dwEffects=CFE_LINK;
					SendMessage(m_hwnd,EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf2);
				}
			}
			if (*t == '\n') sub++;
			if (*t) lt=*t++;
		}
		SendMessage(m_hwnd,EM_SETSEL,oldsels,oldsele);
#endif
	}
	//chatroom_item *cli=(chatroom_item *)GetWindowLong(hwndDlg,GWL_USERDATA);
	
	/*
	if (GetFocus() == m_hwnd)
	{
	SendMessage(m_hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION,si.nTrackPos),0);
	}
	else
	{
	GetScrollInfo(m_hwnd,SB_VERT,&si);
	SendMessage(m_hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION,si.nMax),0);
	//SendMessage(m_hwnd,EM_LINESCROLL,0,4096);
	}
	*/
}

chatroom_item *chat_ShowRoom(const char *channel, int activate)
{
  qDebug("chat_ShowRoom channel=%s,activate=%d",channel,activate);

	int n=0;
	chatroom_item *p=L_Chatroom;
	while(p!=NULL)
	{
		if(!strcasecmp(p->channel,channel))
		{
			if (activate)
			{
/*
				if(p->hwnd->isMinimized())
					p->hwnd->showNormal();
				else
*/
					p->hwnd->show();
					p->hwnd->setActiveWindow();
					p->hwnd->raise();
					p->hwnd->setFocus();
/*
				else
				p->hwnd->show();
*/
				if (activate == 2)
				{
					//SetTimer(p->hwnd,3,250,NULL);
				}
			}
			return p;
		}
		p=p->next;
		n++;
	}
	
	if (g_config->ReadInt("limitchat",1) && g_config->ReadInt("limitchatn",64) <= n)
	{
		return NULL;
	}
	
	
	chatroom_item *cli=(chatroom_item *)malloc(sizeof(chatroom_item));
	memset(cli,0,sizeof(chatroom_item));
	
	cli->next=L_Chatroom;
	L_Chatroom=cli;
	strcpy(cli->channel,channel);
	if ('#'==channel[0] || '&'==channel[0])
	{
		if ('#'== channel[0])
			main_onGotChannel(const_cast<char*>(channel));
		cli->hwnd = new Vaste_ChatChan();
	}
	else
	{
		cli->hwnd = new Vaste_ChatPriv();
	}
	
	Logout(cli->channel, "\n**** Chat started ****", true); //BSC
	

	if (activate == 2)
	{
		//SetTimer(cli->hwnd,3,250,NULL);
		cli->hwnd->show();
	}
	else if (activate) cli->hwnd->show();
	else cli->hwnd->show();

	
	cli->hwnd->show();
	
	return cli;
}

void chat_updateTitles()
{
  chatroom_item *p=L_Chatroom;
  while (p)
  {
	if(p->channel[0]=='&'||p->channel[0]=='#')
    		((Vaste_ChatChan*)p->hwnd)->titleUpdate();
	else
    		((Vaste_ChatPriv*)p->hwnd)->titleUpdate();
//    InvalidateRect(p->hwnd,NULL,FALSE);
    p=p->next;
  }
}


static void getProfilePath(char *tmp);
//BSC BEGIN : Function to log chat outputs
void Logout(char *nick, char *chatline, bool timedate) {
	FILE *logfile;
	char filepath[1024];
	filepath[0] = '\0';
	char buffer[200]="";
	time_t timep;
	
#ifdef _WIN32
	//Construct the filename
	getProfilePath(filepath);
	strcat(filepath,"Chatlog-");
	strcat(filepath,nick);
	strcat(filepath,".txt");
	//Open the logfile
	logfile = fopen(filepath, "a+");
	if (logfile) {
    		//Move to the end of file for append
    		SetFilePointer (logfile, 0, NULL, FILE_END);
    		//See if we need to stamp this line
    		if (timedate) {
      		_strdate(buffer);
      		fputs(buffer, logfile); //date
      		fputs(" ", logfile);
      		_strtime(buffer);
      		fputs(buffer, logfile); //time
      		fputs(" ", logfile);
    		}
		//Write out the file info
		fputs(chatline, logfile);
		fputs("\n", logfile);
		fclose(logfile);
	}
#endif
}
//BSC END
static void getProfilePath(char *tmp)
{
#if defined(_LINUX)
	tmp = get_current_dir_name();
	strcat(tmp,"/");
#elif defined(QTUI)
/*
  GetModuleFileName(g_hInst,tmp,1024);
*/
  QString str = QDir::currentDirPath();
/*
  char *p=tmp;
*/
  strncpy(tmp,str.ascii(),1024);
  tmp[1023]='\0';
  char *p = tmp;
  while (*p) p++;
  while (p >= tmp && *p != '\\') p--;
  p[1]=0;
#endif
}

void chat_sendNickChangeFrom(char *oldnick)
{
  qDebug("chat_sendNickChangeFrom %s\n",oldnick);

  char buf[64];
  sprintf(buf,"/nick/%s",oldnick);
  {
    T_Message msg={0,};
    C_MessageChat req;
    req.set_chatstring(buf);
		req.set_dest("&");
		req.set_src(g_regnick);
    msg.data=req.Make();
    msg.message_type=MESSAGE_CHAT;
    if (msg.data)
    {
      msg.message_length=msg.data->GetLength();
      g_mql->send(&msg);
    }
  }
  chatroom_item *p=L_Chatroom;
  while (p)
  {
    T_Message msg={0,};
    C_MessageChat req;
    if (p->channel[0] == '#' || p->channel[0] == '&')
    {
      QListView* hwndTree=((Vaste_ChatChan*)p->hwnd)->ListView_IDC_TREE1;
      VUser_TreeViewItem *h= TreeView_GetRootChild(hwndTree);
      while (h)
      {
	if (!strcasecmp(h->text(0).ascii(),oldnick))
	{
          TreeView_DeleteItem(hwndTree,h);
          break;
	}
	h=TreeView_GetNextSibling(hwndTree,h);
      }
      TreeView_InsertItem(hwndTree,g_regnick,0);
    }
    p=p->next;
  }
}

