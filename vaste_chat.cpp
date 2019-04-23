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
 * This file implements the display of chat
 * Author: Stanley San
 * Date Created: 2003.12.1
*/
#include "main.h"
#include "vaste_chat.h"

#include "vq_chat.h"
#include <qlineedit.h>
#include <qevent.h>
#include <qtextbrowser.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qbitmap.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qpopmenu.h>
#include <qapplication.h>
#include <qpushbutton.h>

#include "ui_listview.h"
#include "ui_treeview.h"
#include "vaste_mainwindow.h"
#include "qulf_dialog.h"

#include "mqueue.h"
#include "m_chat.h"
#include "srchwnd.h"
#include "xferwnd.h"


extern void add_chatline(QTextBrowser* hwndChat,const char *line);

Vaste_ChatChan::Vaste_ChatChan( QWidget* parent, const char* name, WFlags fl )
    : IDD_CHATCHAN( parent, name, fl|WDestructiveClose )
{
#include "vaste_widget_palette.xx"
	setPalette( dialog_pal );
	Main_Pane->setPalette( frame_pal );
    IDC_CHATTEXT->setPalette( texteditor_pal );
    ListView_IDC_TREE1->setPalette( listview_pal );
    PushButton_IDC_CHAT->setPalette( pushbutton_pal );
    PushButton_IDC_USERINFO->setPalette( pushbutton_pal );
    PushButton_IDC_BROWSE->setPalette( pushbutton_pal );
    PushButton_IDC_SENDFILE->setPalette( pushbutton_pal );

	QPixmap image0("icons/vaste_chat_userinfo_enable.png");
    QPixmap image1("icons/vaste_mainwindow_browse_enable.png");
    QPixmap image2("icons/vaste_chat_filetransfer_enable.png");

	PushButton_IDC_USERINFO->setPixmap( image0 );
	PushButton_IDC_BROWSE->setPixmap( image1 );
	PushButton_IDC_SENDFILE->setPixmap( image2 );

	// skin settings here.
	QPixmap button_bg("skins/vaste_toolbutton_bg.png");
	
	PushButton_IDC_USERINFO->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_BROWSE->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_SENDFILE->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_CHAT->setPaletteBackgroundPixmap(button_bg);

	QPixmap dlg_bg("skins/vaste_pane_bg.png");
	QPixmap pane_bg("skins/vaste_pane_bg1.png");
	
	setPaletteBackgroundPixmap(dlg_bg);
	Main_Pane->setPaletteBackgroundPixmap(pane_bg);
	// skin settings end.


	LineEdit_IDC_CHATEDIT->setFocus();
	ListView_IDC_TREE1->addColumn("Name",90);
	ListView_IDC_TREE1->header()->hide();

	L_Chatroom->hwnd = this;
	data = L_Chatroom;
        
	if (L_Chatroom->channel[0] == '#' || L_Chatroom->channel[0] == '&')
	{
  		qDebug("TODO: Vaste_ChatPriv - child resize");
//////
		T_Message msg={0,};
		C_MessageChat req;
		req.set_chatstring("/join");
		req.set_dest(L_Chatroom->channel);
		req.set_src(g_regnick);
		msg.data=req.Make();
		msg.message_type=MESSAGE_CHAT;
		if (msg.data)
		{
			msg.message_length=msg.data->GetLength();
			g_mql->send(&msg);
			L_Chatroom->lastMsgGuid=msg.message_guid;
			L_Chatroom->lastmsgguid_time=GetTickCount();
		}
/*
		HWND hwndTree=GetDlgItem(hwndDlg,IDC_TREE1);
		TVINSERTSTRUCT tis;
		tis.hParent=TVI_ROOT;
		tis.hInsertAfter=TVI_SORT;
		tis.item.mask=TVIF_PARAM|TVIF_STATE|TVIF_TEXT;
		tis.item.lParam=0;
		tis.item.pszText=g_regnick;
		tis.item.state=TVIS_BOLD;
		tis.item.stateMask=TVIS_BOLD;
		TreeView_InsertItem(hwndTree,&tis);
*/
		TreeView_InsertItem( ListView_IDC_TREE1,g_regnick,0); 
	}
	titleUpdate();

}

Vaste_ChatChan::~Vaste_ChatChan()
{
}

void Vaste_ChatPriv::showEvent(QShowEvent *e)
{
}

Vaste_ChatPriv::Vaste_ChatPriv( QWidget* parent, const char* name, WFlags fl )
    : IDD_CHATPRIV( parent, name, fl|WDestructiveClose )
{
#include "vaste_widget_palette.xx"
    setPalette( dialog_pal );
	Main_Pane->setPalette( frame_pal );
    PushButton_IDC_USERINFO->setPalette( pushbutton_pal );
    PushButton_IDC_BROWSE->setPalette( pushbutton_pal );
    PushButton_IDC_SENDFILE->setPalette( pushbutton_pal );
    IDC_CHATTEXT->setPalette( texteditor_pal );
    PushButton_IDC_CHAT->setPalette( pushbutton_pal );

	QPixmap image0("icons/vaste_chat_userinfo_enable.png");
    QPixmap image1("icons/vaste_mainwindow_browse_enable.png");
    QPixmap image2("icons/vaste_chat_filetransfer_enable.png");

	PushButton_IDC_USERINFO->setPixmap( image0 );
	PushButton_IDC_BROWSE->setPixmap( image1 );
	PushButton_IDC_SENDFILE->setPixmap( image2 );

	// skin settings here.
	QPixmap button_bg("skins/vaste_toolbutton_bg.png");
	
	PushButton_IDC_USERINFO->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_BROWSE->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_SENDFILE->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_CHAT->setPaletteBackgroundPixmap(button_bg);

	QPixmap dlg_bg("skins/vaste_pane_bg.png");
	QPixmap pane_bg("skins/vaste_pane_bg1.png");
	
	setPaletteBackgroundPixmap(dlg_bg);
	Main_Pane->setPaletteBackgroundPixmap(pane_bg);
	// skin settings end.

	LineEdit_IDC_CHATEDIT1->setFocus();
	L_Chatroom->hwnd = this;
	data = L_Chatroom;
	titleUpdate();
}

Vaste_ChatPriv::~Vaste_ChatPriv()
{
}

void Vaste_ChatChan::showEvent(QShowEvent *e)
{
}
/*
void Vaste_ChatPriv::processChatLine()
{
	QString line = LineEdit_IDC_CHATEDIT1->text();
  	qDebug("Vaste_ChatChan::processChatLine() : %s",line.ascii());

    	QTextBrowser* hwndText = IDC_CHATTEXT;
	if(!g_regnick[0])
	{
		add_chatline(hwndText,"Register your nickname first !");
		LineEdit_IDC_CHATEDIT1->setText("");
		return ;
	}

	{
		int isme=0;
		chatroom_item *cli=data;
		char textb[2048];
		QCString cs = line.utf8();
		//memcpy(textb,(const char*)cs,2047);
		strncpy(textb,(const char*)cs,2047);
		//memcpy(textb,line.ascii(),2047);
		char *text=textb;
		while (*text == ' ') text++;
		if (text[0] == '/' && text[1] != '/')
		{
			if (text[1] == 'J' || text[1] == 'j')
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if(strstr(text,"\r") || strstr(text,"\n") || (*text != '#' && *text != '&'))

*/
void Vaste_ChatPriv::processChatLine()
{
	QString line = LineEdit_IDC_CHATEDIT1->text();
  	qDebug("Vaste_ChatChan::processChatLine() : %s",line.ascii());

    	QTextBrowser* hwndText = IDC_CHATTEXT;
	if(!g_regnick[0])
	{
		add_chatline(hwndText,"Register your nickname first !");
		LineEdit_IDC_CHATEDIT1->setText("");
		return ;
	}

	{
		int isme=0;
		chatroom_item *cli=data;
		char textb[2048];
		QCString cs = line.utf8();
		//memcpy(textb,(const char*)cs,2047);
		strncpy(textb,(const char*)cs,2047);
		//memcpy(textb,line.ascii(),2047);
		char *text=textb;
		while (*text == ' ') text++;
		if (text[0] == '/' && text[1] != '/')
		{
			if (text[1] == 'J' || text[1] == 'j')
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if(strstr(text,"\r") || strstr(text,"\n") || (*text != '#' && *text != '&'))
				{
					add_chatline(hwndText,"Usage: /join <#channel|&channel>");
				}
				else
				{
					chat_ShowRoom(text,1);
					//	SetDlgItemText(hwndDlg,IDC_CHATEDIT,"");
					return;
				}
			}
			else if (!strncasecmp(text+1,"whois",5))
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if(!*text || strstr(text,"\r") || strstr(text,"\n") || strstr(text,"#") || strstr(text,"&"))
				{
					add_chatline(hwndText,"Usage: /whois <username>");
				}
				else
				{
					T_Message msg ={0,};
					// send a message to text that is /whois
					C_MessageChat req;
					req.set_chatstring("/whois");
					req.set_dest(text);
					req.set_src(g_regnick);
					msg.data=req.Make();
					msg.message_type=MESSAGE_CHAT;
					if (msg.data)
					{
						msg.message_length=msg.data->GetLength();
						g_mql->send(&msg);
					}
				}
			}
			else if (!strncasecmp(text+1,"search",6))
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if(!*text || strstr(text,"\r") || strstr(text,"\n"))
				{
					add_chatline(hwndText,"Usage: /search <searchterm>");
				}
				else
				{
					qDebug("TODO:Search_Search() %s",text);
					/*
					SendMessage(g_mainwnd,WM_COMMAND,IDC_SEARCH,0);
					Search_Search(text);
					SetDlgItemText(hwndDlg,IDC_CHATEDIT,"");
					*/
					return;
				}
			}
			else if (!strncasecmp(text+1,"browse",6))
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if(!*text || strstr(text,"\r") || strstr(text,"\n"))
				{
					add_chatline(hwndText,"Usage: /browse <user|path>");
				}
				else
				{
					if (*text != '/')
					{
						text--;
						*text='/';
					}
					qDebug("TODO:Search_Search() %s",text);
					/*
					SendMessage(g_mainwnd,WM_COMMAND,IDC_SEARCH,0);
					Search_Search(text);
					SetDlgItemText(hwndDlg,IDC_CHATEDIT,"");
					*/
					return;
				}
			}
			else if (!strncasecmp(text+1,"leave",5))
			{
				close();
			}
			else if (!strncasecmp(text+1,"clear",5))
			{
    				IDC_CHATTEXT->clear();
			}
			else if (!strncasecmp(text+1,"query",5))
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if (!strstr(text,"\r") && !strstr(text,"\n") && *text && text[0] != '#' && text[0] != '&')
				{
					chat_ShowRoom(text,1);
					/*
					SetDlgItemText(hwndDlg,IDC_CHATEDIT,"");
					*/
					return;
				}
				else
				{
					add_chatline(hwndText,"Usage: /query <nickname>");
				}
			}
			else if (!strncasecmp(text+1,"me",2))
			{
				if (!strstr(text,"\r") && !strstr(text,"\n") && text[3]==' ' && text[4])
				{
					isme=1;
					goto senditanyway;
				}
				add_chatline(hwndText,"Usage: /me <action>");
			}
			else if (!strncasecmp(text+1,"msg",3))
			{
				char *dstr;
				if (!strstr(text,"\r") && !strstr(text,"\n") && text[4] == ' ' && text[5] && text[5] != ' ' &&
				(dstr=strstr(text+5," ")) )
				{
					T_Message msg={0,};
					C_MessageChat req;
					// dstr is the string we are sending
					*dstr=0;
					if (dstr[1] == '/')
					{
						dstr[0]='/';
						req.set_chatstring(dstr);
					}
					else
					{
						req.set_chatstring(dstr+1);
					}
					*dstr=0;

					req.set_dest(text+5);
					req.set_src(g_regnick);
					msg.data=req.Make();
					msg.message_type=MESSAGE_CHAT;
					if (msg.data)
					{
						msg.message_length=msg.data->GetLength();
						g_mql->send(&msg);
						char tmp[4096];
						sprintf(tmp,"-> *%s*: %s",text+5,dstr+1);
						add_chatline(hwndText,tmp);
					}
				}
				else
				add_chatline(hwndText,"Usage: /msg <user> <text>");
			}
			else if (!strnicmp(text+1,"help",4))
			{
				add_chatline(hwndText,"Commands:");
				add_chatline(hwndText,"  /msg <user> <text>");
				add_chatline(hwndText,"  /query <user>");
				add_chatline(hwndText,"  /whois <user>");
				add_chatline(hwndText,"  /join <channel>");
				add_chatline(hwndText,"  /me <action>");
				add_chatline(hwndText,"  /search <searchterm>");
				add_chatline(hwndText,"  /browse <user|path>");
				add_chatline(hwndText,"  /leave");
				add_chatline(hwndText,"  /clear");
			}
			else
			{
				add_chatline(hwndText,"Unknown command! Try /help");
			}
		}
		else if (strlen(text)>0)
		{
			senditanyway:
/*
			if (cli->channel[0] == '#' || cli->channel[0] == '&')
			{
				QListView* htree = 
				VUser_TreeViewItem *h = TreeView_GetRootChild(htree);
				while (h)
				{
					/ *
					TVITEM i;
					i.mask=TVIF_PARAM|TVIF_STATE;
					i.hItem=h;
					i.state=0;
					i.stateMask=TVIS_BOLD;
					TreeView_GetItem(hwndTree,&i);

					if (i.state || !i.lParam)
					{
					i.hItem=h;
					i.mask=TVIF_HANDLE|TVIF_PARAM;
					i.lParam=GetTickCount();
					TreeView_SetItem(hwndTree,&i);
					}
					* /
					h->m_param = GetTickCount();
					h=TreeView_GetNextSibling(htree,h);
				}
			}
			else
			{
*/
    				Label_IDC_STATUS->setText("Sent message, awaiting reply");
/*
			}
*/

			int l=0;
			while (text && *text && l++ < 16)
			{
				while (*text == '\r' || *text == '\n') text++;
				while (*text == ' ') text++;

				char *lasttext=text;
				while (*text && *text != '\r' && *text != '\n') text++;
				if (*text) *text++=0;
				if (!*lasttext) break;

				T_Message msg={0,};
				C_MessageChat req;

				char buf[1024];
				if (lasttext[0] == '/' && lasttext[1] != '/' && !isme)
				{
					safe_strncpy(buf+1,lasttext,sizeof(buf)-1);
					buf[0]='/';
				}
				else
				{
					safe_strncpy(buf,lasttext,sizeof(buf));
				}
				isme=0;
				req.set_chatstring(buf);

				req.set_dest(cli->channel);
				req.set_src(g_regnick);
				msg.data=req.Make();
				msg.message_type=MESSAGE_CHAT;
				if (msg.data)
				{
					msg.message_length=msg.data->GetLength();
					g_mql->send(&msg);
					cli->lastMsgGuid=msg.message_guid;
					cli->lastmsgguid_time=GetTickCount();
					char tmp[4096];
						formatChatString(tmp,req.get_src(),buf);
					if (tmp[0]) add_chatline(hwndText,tmp);
				}
			}
		}
	}

	LineEdit_IDC_CHATEDIT1->setText("");
//	add_chatline(IDC_CHATTEXT , LineEdit_IDC_CHATEDIT1->text().ascii());
}

void Vaste_ChatChan::processChatLine()
{
	QString line = LineEdit_IDC_CHATEDIT->text();
  	qDebug("Vaste_ChatChan::processChatLine() : %s",line.ascii());

    	QTextBrowser* hwndText = IDC_CHATTEXT;
	if(!g_regnick[0])
	{
		add_chatline(hwndText,"Register your nickname first !");
		LineEdit_IDC_CHATEDIT->setText("");
		//SetDlgItemText(hwndDlg,IDC_CHATEDIT,"");
		return ;
	}

	{
		int isme=0;
		chatroom_item *cli=data;
		char textb[2048];
		//QCString cs = line.local8Bit();
		QCString cs = line.utf8();
		//memcpy(textb,(const char*)cs,2047);
		strncpy(textb,(const char*)cs,2047);
		char *text=textb;
		while (*text == ' ') text++;
		if (text[0] == '/' && text[1] != '/')
		{
			if (text[1] == 'J' || text[1] == 'j')
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if(strstr(text,"\r") || strstr(text,"\n") || (*text != '#' && *text != '&'))
				{
					add_chatline(hwndText,"Usage: /join <#channel|&channel>");
				}
				else
				{
					chat_ShowRoom(text,1);
					LineEdit_IDC_CHATEDIT->setText("");
					//	SetDlgItemText(hwndDlg,IDC_CHATEDIT,"");
					return;
				}
			}
			else if (!strncasecmp(text+1,"whois",5))
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if(!*text || strstr(text,"\r") || strstr(text,"\n") || strstr(text,"#") || strstr(text,"&"))
				{
					add_chatline(hwndText,"Usage: /whois <username>");
				}
				else
				{
					T_Message msg = {0,};
					// send a message to text that is /whois
					C_MessageChat req;
					req.set_chatstring("/whois");
					req.set_dest(text);
					req.set_src(g_regnick);
					msg.data=req.Make();
					msg.message_type=MESSAGE_CHAT;
					if (msg.data)
					{
						msg.message_length=msg.data->GetLength();
						g_mql->send(&msg);
					}
				}
			}
			else if (!strncasecmp(text+1,"search",6))
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if(!*text || strstr(text,"\r") || strstr(text,"\n"))
				{
					add_chatline(hwndText,"Usage: /search <searchterm>");
				}
				else
				{
					qDebug("TODO:Search_Search() %s",text);
					LineEdit_IDC_CHATEDIT->setText("");
					/*
					SendMessage(g_mainwnd,WM_COMMAND,IDC_SEARCH,0);
					Search_Search(text);
					SetDlgItemText(hwndDlg,IDC_CHATEDIT,"");
					*/
					return;
				}
			}
			else if (!strncasecmp(text+1,"browse",6))
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if(!*text || strstr(text,"\r") || strstr(text,"\n"))
				{
					add_chatline(hwndText,"Usage: /browse <user|path>");
				}
				else
				{
					if (*text != '/')
					{
						text--;
						*text='/';
					}
					qDebug("TODO:Search_Search() %s",text);
					LineEdit_IDC_CHATEDIT->setText("");
					/*
					SendMessage(g_mainwnd,WM_COMMAND,IDC_SEARCH,0);
					Search_Search(text);
					SetDlgItemText(hwndDlg,IDC_CHATEDIT,"");
					*/
					return;
				}
			}
			else if (!strncasecmp(text+1,"leave",5))
			{
				close();
			}
			else if (!strncasecmp(text+1,"clear",5))
			{
    				IDC_CHATTEXT->clear();
			}
			else if (!strncasecmp(text+1,"query",5))
			{
				while (*text && *text != ' ') text++;
				while (*text == ' ') text++;
				if (!strstr(text,"\r") && !strstr(text,"\n") && *text && text[0] != '#' && text[0] != '&')
				{
					chat_ShowRoom(text,1);
					LineEdit_IDC_CHATEDIT->setText("");
					/*
					SetDlgItemText(hwndDlg,IDC_CHATEDIT,"");
					*/
					return;
				}
				else
				{
					add_chatline(hwndText,"Usage: /query <nickname>");
				}
			}
			else if (!strncasecmp(text+1,"me",2))
			{
				if (!strstr(text,"\r") && !strstr(text,"\n") && text[3]==' ' && text[4])
				{
					isme=1;
					goto senditanyway;
				}
				add_chatline(hwndText,"Usage: /me <action>");
			}
			else if (!strncasecmp(text+1,"msg",3))
			{
				char *dstr;
				if (!strstr(text,"\r") && !strstr(text,"\n") && text[4] == ' ' && text[5] && text[5] != ' ' &&
				(dstr=strstr(text+5," ")) )
				{
					T_Message msg={0,};
					C_MessageChat req;
					// dstr is the string we are sending
					*dstr=0;
					if (dstr[1] == '/')
					{
						dstr[0]='/';
						req.set_chatstring(dstr);
					}
					else
					{
						req.set_chatstring(dstr+1);
					}
					*dstr=0;

					req.set_dest(text+5);
					req.set_src(g_regnick);
					msg.data=req.Make();
					msg.message_type=MESSAGE_CHAT;
					if (msg.data)
					{
						msg.message_length=msg.data->GetLength();
						g_mql->send(&msg);
						char tmp[4096];
						sprintf(tmp,"-> *%s*: %s",text+5,dstr+1);
						add_chatline(hwndText,tmp);
					}
				}
				else
				add_chatline(hwndText,"Usage: /msg <user> <text>");
			}
			else if (!strnicmp(text+1,"help",4))
			{
				add_chatline(hwndText,"Commands:");
				add_chatline(hwndText,"  /msg <user> <text>");
				add_chatline(hwndText,"  /query <user>");
				add_chatline(hwndText,"  /whois <user>");
				add_chatline(hwndText,"  /join <channel>");
				add_chatline(hwndText,"  /me <action>");
				add_chatline(hwndText,"  /search <searchterm>");
				add_chatline(hwndText,"  /browse <user|path>");
				add_chatline(hwndText,"  /leave");
				add_chatline(hwndText,"  /clear");
			}
			else
			{
				add_chatline(hwndText,"Unknown command! Try /help");
			}
		}
		else if (strlen(text)>0)
		{
			senditanyway:
			if (cli->channel[0] == '#' || cli->channel[0] == '&')
			{
				QListView* htree = ListView_IDC_TREE1;
				VUser_TreeViewItem *h = TreeView_GetRootChild(htree);
				while (h)
				{
					/*
					TVITEM i;
					i.mask=TVIF_PARAM|TVIF_STATE;
					i.hItem=h;
					i.state=0;
					i.stateMask=TVIS_BOLD;
					TreeView_GetItem(hwndTree,&i);

					if (i.state || !i.lParam)
					{
					i.hItem=h;
					i.mask=TVIF_HANDLE|TVIF_PARAM;
					i.lParam=GetTickCount();
					TreeView_SetItem(hwndTree,&i);
					}
					*/
					h->m_param = GetTickCount();
					h=TreeView_GetNextSibling(htree,h);
				}
			}
			else
			{
				qDebug("Error : status_Label needed");
    				//Label_IDC_STATUS->setText("Sent message, awaiting reply");
			}

			int l=0;
			while (text && *text && l++ < 16)
			{
				while (*text == '\r' || *text == '\n') text++;
				while (*text == ' ') text++;

				char *lasttext=text;
				while (*text && *text != '\r' && *text != '\n') text++;
				if (*text) *text++=0;
				if (!*lasttext) break;

				T_Message msg={0,};
				C_MessageChat req;

				char buf[1024];
				if (lasttext[0] == '/' && lasttext[1] != '/' && !isme)
				{
					safe_strncpy(buf+1,lasttext,sizeof(buf)-1);
					buf[0]='/';
				}
				else
				{
					safe_strncpy(buf,lasttext,sizeof(buf));
				}
				isme=0;
				req.set_chatstring(buf);

				req.set_dest(cli->channel);
				req.set_src(g_regnick);
				msg.data=req.Make();
				msg.message_type=MESSAGE_CHAT;
				if (msg.data)
				{
					msg.message_length=msg.data->GetLength();
					g_mql->send(&msg);
					cli->lastMsgGuid=msg.message_guid;
					cli->lastmsgguid_time=GetTickCount();
					char tmp[4096];
						formatChatString(tmp,req.get_src(),buf);
					if (tmp[0]) add_chatline(hwndText,tmp);
				}
			}
		}
	}

	LineEdit_IDC_CHATEDIT->setText("");
//	add_chatline(IDC_CHATTEXT , LineEdit_IDC_CHATEDIT1->text().ascii());
}


//??
void Vaste_ChatPriv::formatChatString(char *txt, char *src, char *str)
{
	qDebug("txt=%s,src=%s,str=%s",txt,src,str);

    	chatroom_item* cnl = data;

	if (!cnl)
	{
		debug_printf("formatChatString: got chat message '%s','%s','%s', but cnl=NULL\n",
		txt,src,str);
		return;
	}
	if (g_chat_timestamp&(1 << (int)(cnl->channel[0]=='#' || cnl->channel[0]=='&')))
	{
		time_t t; struct tm *tm;
		t = time(NULL);
		tm = localtime(&t);
		if (tm)
		{
			if (g_chat_timestamp&4)
			txt+=sprintf(txt,"[%d/%d/%02d@%02d:%02d:%02d] ",
			tm->tm_mon+1,tm->tm_mday,tm->tm_year%100,
			tm->tm_hour,tm->tm_min, tm->tm_sec);
			else 
			txt+=sprintf(txt,"[%02d:%02d:%02d] ",tm->tm_hour,tm->tm_min, tm->tm_sec);
		}
	}

	if (!strncmp(str,"/nick/",6) && str[6])
	{
		if (cnl->channel[0] == '#' || cnl->channel[0] == '&')
		{
			sprintf(txt,"*** %s is now known as %s",str+6,src);
			debug_printf("Error : need goNick");
		}
		else
		{
			if (str[6] == '#' || str[6] == '&')
			{
				strcpy(txt,"*** invalid message received");
			}
			else
			{
				sprintf(txt,"*** %s is now known as %s",str+6,src);
				safe_strncpy(cnl->channel,src,sizeof(cnl->channel));
				titleUpdate();
				
			}
		}
	}
	else if (!strncmp(str,"/iam/",5))
	{
		if (strlen(str)>255) str[255]=0;
		sprintf(txt,"*** %s is %s",src,str+5);
	}
	else if (!strcmp(str,"/join"))
	{
		if (cnl->channel[0] == '#' || cnl->channel[0] == '&')
		{
			sprintf(txt,"*** %s has joined %s",src,cnl->channel);
		}
		else strcpy(txt,"*** invalid message received");
	}
	else if (!strcmp(str,"/leave"))
	{
		if (cnl->channel[0] == '#' || cnl->channel[0] == '&')
		{
			sprintf(txt,"*** %s has left %s",src,cnl->channel);
		}
		else strcpy(txt,"*** invalid message received");
	}
	else if (!strncasecmp(str,"/me ",4)) sprintf(txt,"* %s %s",src,str+4);
	else if (!strncmp(str,"//",2))
	{
		sprintf(txt,"<%s> %s", src, str+1);
	}
	else if (str[0] != '/')
	{
		//BSC BEGIN : Play a sound if this line is coming from someone else
		if (strcmp(src, g_regnick) != 0) {
/*
			char tmp[1024+1024];
			tmp[0]='\0';
	//		getProfilePath(tmp);
			strcat(tmp,"IMInbound.wav");
	//		sndPlaySound(tmp, SND_FILENAME | SND_ASYNC);
*/
		}
		//BSC END
		sprintf(txt,"<%s> %s", src, str);
		Logout(cnl->channel, txt, false);
	}
	else txt[0]=0;

}



//??
void Vaste_ChatChan::formatChatString(char *txt, char *src, char *str)
{
    	chatroom_item* cnl = data;

	if (!cnl)
	{
		debug_printf("formatChatString: got chat message '%s','%s','%s', but cnl=NULL\n",
		txt,src,str);
		return;
	}
	if (g_chat_timestamp&(1 << (int)(cnl->channel[0]=='#' || cnl->channel[0]=='&')))
	{
		time_t t; struct tm *tm;
		t = time(NULL);
		tm = localtime(&t);
		if (tm)
		{
			if (g_chat_timestamp&4)
			txt+=sprintf(txt,"[%d/%d/%02d@%02d:%02d:%02d] ",
			tm->tm_mon+1,tm->tm_mday,tm->tm_year%100,
			tm->tm_hour,tm->tm_min, tm->tm_sec);
			else
			txt+=sprintf(txt,"[%02d:%02d:%02d] ",tm->tm_hour,tm->tm_min, tm->tm_sec);
		}
	}

	if (!strncmp(str,"/nick/",6) && str[6])
	{
		if (cnl->channel[0] == '#' || cnl->channel[0] == '&')
		{
			sprintf(txt,"*** %s is now known as %s",str+6,src);
			debug_printf("TODO: goNick");
			gotNick(str+6,0,0,1);
		}
		else
		{
			if (str[6] == '#' || str[6] == '&')
			{
				strcpy(txt,"*** invalid message received");
			}
			else
			{
				sprintf(txt,"*** %s is now known as %s",str+6,src);
				safe_strncpy(cnl->channel,src,sizeof(cnl->channel));
				titleUpdate();
				
			}
		}
	}
	else if (!strncmp(str,"/iam/",5))
	{
		if (strlen(str)>255) str[255]=0;
		sprintf(txt,"*** %s is %s",src,str+5);
	}
	else if (!strcmp(str,"/join"))
	{
		if (cnl->channel[0] == '#' || cnl->channel[0] == '&')
		{
			sprintf(txt,"*** %s has joined %s",src,cnl->channel);
		}
		else strcpy(txt,"*** invalid message received");
	}
	else if (!strcmp(str,"/leave"))
	{
		if (cnl->channel[0] == '#' || cnl->channel[0] == '&')
		{
			sprintf(txt,"*** %s has left %s",src,cnl->channel);
		}
		else strcpy(txt,"*** invalid message received");
	}
	else if (!strncasecmp(str,"/me ",4)) sprintf(txt,"* %s %s",src,str+4);
	else if (!strncmp(str,"//",2))
	{
		sprintf(txt,"<%s> %s", src, str+1);
	}
	else if (str[0] != '/')
	{
		//BSC BEGIN : Play a sound if this line is coming from someone else
		if (strcmp(src, g_regnick) != 0) {
#ifdef _WIN32
/*
			char tmp[1024+1024];
			tmp[0] = '\0';
			getProfilePath(tmp);
			strcat(tmp,"IMInbound.wav");
		sndPlaySound(tmp, SND_FILENAME | SND_ASYNC);
*/
#endif
		}
		//BSC END
		sprintf(txt,"<%s> %s", src, str);
		Logout(cnl->channel, txt, false);
	}
	else txt[0]=0;

}


void Vaste_ChatChan::gotNick(char *nick, int state, int statemask, int delifold)
{
    	killTimer(m_t1kId);
    	m_t1kId = startTimer(1000);

    	QListView* hwndTree = ListView_IDC_TREE1;
	VUser_TreeViewItem *h = TreeView_GetRootChild(hwndTree);
	
	int gotme=0;
	while (h)
	{
		if (delifold && !nick)
		{
			h=TreeView_GetNextSibling(hwndTree,h);
			//TODO
			if (!strcasecmp(h->text(0).ascii(),g_regnick) && !gotme)
			{
				gotme=1;
			}
			else if (h->m_param)
			{
				int age=GetTickCount() - h->m_param;
/*
				if (!(i.state&TVIS_BOLD))
				{
*/
					if (age > 15000)
					{
						TreeView_DeleteItem(hwndTree,h);
						chatroom_item *cli=data;
						if (cli)
						{
							char buf[1024];
							//TODO
							sprintf(buf,"*** %s may have left %s (no response)",h->text(0).ascii(),cli->channel);
							add_chatline(IDC_CHATTEXT,buf);
						}

					}
/*
				}
*/

				else
/*
				{
*/
					if (age > 5000)  // set it to unbold
					{
/*
						i.mask=TVIF_HANDLE|TVIF_STATE;
						i.state=0;
						i.stateMask=TVIS_BOLD;
						TreeView_SetItem(hwndTree,&i);
*/
					}
/*
				}
*/
			}

		}
		else
		{
			//TODO		
			if (!strcasecmp(h->text(0).ascii(),nick))
			{
				if (delifold)
				{
					TreeView_DeleteItem(hwndTree,h);
				}
				else
				{
/*
					i.hItem=h;
					i.mask=TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT;
					i.pszText=nick;
					i.lParam=0;
					i.state=state;
					i.stateMask=statemask;
					TreeView_SetItem(hwndTree,&i);
*/
				}
				return;
			}
			h=TreeView_GetNextSibling(hwndTree,h);
		}
	}

	if (!delifold)
	{
/*
		TVINSERTSTRUCT tis;
		tis.hParent=TVI_ROOT;
		tis.hInsertAfter=TVI_SORT;
		tis.item.mask=TVIF_PARAM|TVIF_STATE|TVIF_TEXT;
		tis.item.lParam=0;
		tis.item.pszText=nick;
		tis.item.state=state;
		tis.item.stateMask=statemask;
*/
		TreeView_InsertItem(hwndTree,nick,0);
	}
	else if (!nick && !gotme)
	{
/*
		TVINSERTSTRUCT tis;
		tis.hParent=TVI_ROOT;
		tis.hInsertAfter=TVI_SORT;
		tis.item.mask=TVIF_PARAM|TVIF_STATE|TVIF_TEXT;
		tis.item.lParam=0;
		tis.item.pszText=g_regnick;
		tis.item.state=TVIS_BOLD;
		tis.item.stateMask=TVIS_BOLD;
*/
		TreeView_InsertItem(hwndTree,g_regnick,0);
	}
}

void Vaste_ChatChan::titleUpdate()
{
	//qDebug("TODO Vaste_ChatChan::titleUpdate()");
        char buf[64];
        chatroom_item *cli=data;
        if (cli->channel[0] == '#' ||
            cli->channel[0] == '&')
          sprintf(buf,"%s - " APP_NAME " Chat",cli->channel);
        else
          sprintf(buf,"%s - " APP_NAME " User Chat",cli->channel);
	setCaption(buf);
}

void Vaste_ChatPriv::titleUpdate()
{
	//qDebug("TODO Vaste_ChatPriv::titleUpdate()");
        char buf[64];
        chatroom_item *cli=data;
        if (cli->channel[0] == '#' ||
            cli->channel[0] == '&')
          sprintf(buf,"%s - " APP_NAME " Chat",cli->channel);
        else
          sprintf(buf,"%s - " APP_NAME " User Chat",cli->channel);
	setCaption(buf);
}

void Vaste_ChatPriv::closeEvent(QCloseEvent* e)
{
	qDebug("Vaste_ChatPriv closeEvent()");

///
	chatroom_item *cli=data;

        int w= width();
        int h= height();

        if (cli && (cli->channel[0] == '#' || cli->channel[0] == '&'))
        {
          T_Message msg={0,};
          C_MessageChat req;
          req.set_chatstring("/leave");
			    req.set_dest(cli->channel);
			    req.set_src(g_regnick);
          msg.data=req.Make();
          msg.message_type=MESSAGE_CHAT;
          if (msg.data)
          {
            msg.message_length=msg.data->GetLength();
            g_mql->send(&msg);
            cli->lastMsgGuid=msg.message_guid;
            cli->lastmsgguid_time=GetTickCount();
			    }
          g_config->WriteInt("chatchan_w",w);
          g_config->WriteInt("chatchan_h",h);
          g_config->WriteInt("chat_divpos",cli->lastdivpos);

        }
        else
        {
          g_config->WriteInt("chatuser_w",w);
          g_config->WriteInt("chatuser_h",h);
        }
        
        if(cli==L_Chatroom) L_Chatroom=cli->next;
	else {
		chatroom_item *p=L_Chatroom;
		while(p->next && p->next!=cli) p=p->next;
		if(p->next) p->next=p->next->next;
	}
	free(cli);
	cli = NULL;
	e->accept();
}

void Vaste_ChatChan::closeEvent(QCloseEvent* e)
{
	qDebug("Vaste_ChatPriv closeEvent()");

///
	chatroom_item *cli=data;

        int w= width();
        int h= height();

        if (cli && (cli->channel[0] == '#' || cli->channel[0] == '&'))
        {
          T_Message msg={0,};
          C_MessageChat req;
          req.set_chatstring("/leave");
			    req.set_dest(cli->channel);
			    req.set_src(g_regnick);
          msg.data=req.Make();
          msg.message_type=MESSAGE_CHAT;
          if (msg.data)
          {
            msg.message_length=msg.data->GetLength();
            g_mql->send(&msg);
            cli->lastMsgGuid=msg.message_guid;
            cli->lastmsgguid_time=GetTickCount();
			    }
          g_config->WriteInt("chatchan_w",w);
          g_config->WriteInt("chatchan_h",h);
          g_config->WriteInt("chat_divpos",cli->lastdivpos);

        }
        else
        {
          g_config->WriteInt("chatuser_w",w);
          g_config->WriteInt("chatuser_h",h);
        }
        
        if(cli==L_Chatroom) L_Chatroom=cli->next;
	else {
		chatroom_item *p=L_Chatroom;
		while(p->next && p->next!=cli) p=p->next;
		if(p->next) p->next=p->next->next;
	}
	free(cli);
	cli = NULL;
	e->accept();
}

void Vaste_ChatChan::onUserListDBClicked(QListViewItem* it)
{
	if(it)
	{
		//TODO
		chat_ShowRoom(it->text(0).ascii(),2);
	}
}

void Vaste_ChatChan::onUserListRMBClicked(QListViewItem* it,const QPoint& pt,int w)
{
	qDebug("Vaste_ChatChan::onUserListRMBClicked");
	if(it) 
	{
		QPopupMenu *contextMenu = new QPopupMenu( this );
		Q_CHECK_PTR( contextMenu );
		int whoisId = contextMenu->insertItem( "Whois user" );
		int browseId = contextMenu->insertItem( "Browse user" );
		int sendId = contextMenu->insertItem( "Send file(s) to user" );

		int ret = contextMenu->exec( pt );
		if(ret == whoisId)
		{
			qDebug("whois selected name=%s",it->text(0).ascii());
        		T_Message msg = {0,};
        		// send a message to text that is /whois
        		C_MessageChat req;
        		req.set_chatstring("/whois");
               		req.set_dest(const_cast<char*>(it->text(0).ascii()));
                	req.set_src(g_regnick);
        		msg.data=req.Make();
        		msg.message_type=MESSAGE_CHAT;
        		if (msg.data)
        		{
	          		msg.message_length=msg.data->GetLength();
       		   		g_mql->send(&msg);
        		}
		}
		else if(ret == browseId)
		{
			qDebug("TODO :browse selected");
			GetQVMainWin->editBrowse();
			char buf[1024];
			sprintf(buf,"/%s",it->text(0).ascii());
			Search_Search(buf);
		}
		else if(ret == sendId)
		{
			qDebug("send selected");
			Vaste_UploadFileDlg dlg;
/*
		QString s = dlg.getOpenFileName(
				"/",
				"*.*",
				this,
				"open file dialog",
				"Choose a file" );
*/
			if(QDialog::Accepted == dlg.exec())
			{
				qDebug("TODO : xfer_UploadFileToUser, selected file=%s,upload path=%s",dlg.selectedFile().ascii(),dlg.getUpLoadPath().ascii());
				//Xfer_UploadFileToUser(NULL, const_cast<char*>(dlg.selectedFile().ascii()), const_cast<char*>(it->text(0).ascii()), const_cast<char*>(dlg.getUpLoadPath().ascii()));

				QStringList fileList = dlg.selectedFiles();
				for( QStringList::iterator f= fileList.begin();f != fileList.end();++f)
				Xfer_UploadFileToUser(NULL, (char*)((*f).ascii()), (char*)(it->text(0).ascii()), (char*)(dlg.getUpLoadPath().ascii()));

			}
		}
		else
		{
			qDebug("Error unknown selected");
		}
	}
}

void Vaste_ChatChan::onUserInfoClicked()
{
	qDebug("Vaste_ChatChan::onUserInfoClicked");
	QListViewItem* it = ListView_IDC_TREE1->selectedItem();
	if(!it)
	{
		qDebug("TODO: QDialog::info");
	}
	else
	{
			qDebug("whois selected name=%s",it->text(0).ascii());
        		T_Message msg = {0,};
        		// send a message to text that is /whois
        		C_MessageChat req;
        		req.set_chatstring("/whois");
               		req.set_dest(const_cast<char*>(it->text(0).ascii()));
                	req.set_src(g_regnick);
        		msg.data=req.Make();
        		msg.message_type=MESSAGE_CHAT;
        		if (msg.data)
        		{
	          		msg.message_length=msg.data->GetLength();
       		   		g_mql->send(&msg);
        		}
	}
		
}

void Vaste_ChatChan::onBrowseClicked()
{
	qDebug("Vaste_ChatChan::onBrowseClicked");
	QListViewItem* it = ListView_IDC_TREE1->selectedItem();
	if(!it)
	{
		qDebug("TODO: QDialog::info");
	}
	else
	{
			qDebug("TODO :browse selected");
			GetQVMainWin->editBrowse();
			char buf[1024];
			sprintf(buf,"/%s",it->text(0).ascii());
			Search_Search(buf);
	}
}

void Vaste_ChatChan::onSendFileClicked()
{
	qDebug("Vaste_ChatChan::onSendFileClicked");
	QListViewItem* it = ListView_IDC_TREE1->selectedItem();
	if(!it)
	{
		qDebug("TODO: QDialog::info");
	}
	else
	{
			qDebug("send selected");
			Vaste_UploadFileDlg dlg;
/*
		QString s = dlg.getOpenFileName(
				"/",
				"*.*",
				this,
				"open file dialog",
				"Choose a file" );
*/
			if(QDialog::Accepted == dlg.exec())
			{
				qDebug("TODO : xfer_UploadFileToUser, selected file=%s,upload path=%s",dlg.selectedFile().ascii(),dlg.getUpLoadPath().ascii());
				//Xfer_UploadFileToUser(NULL, const_cast<char*>(dlg.selectedFile().ascii()), const_cast<char*>(it->text(0).ascii()), const_cast<char*>(dlg.getUpLoadPath().ascii()));

				QStringList fileList = dlg.selectedFiles();
				for( QStringList::iterator f= fileList.begin();f != fileList.end();++f)
				Xfer_UploadFileToUser(NULL, (char*)((*f).ascii()), (char*)(it->text(0).ascii()), (char*)(dlg.getUpLoadPath().ascii()));

			}
	}
}


void Vaste_ChatPriv::onBrowseClicked()
{
	qDebug("Vaste_ChatChan::onBrowseClicked");

	chatroom_item *cli=data;
	GetQVMainWin->editBrowse();
	char buf[1024];
	sprintf(buf,"/%s",cli->channel);
	Search_Search(buf);

}

void Vaste_ChatPriv::onUserInfoClicked()
{
	qDebug("Vaste_ChatChan::onUserInfoClicked");
	chatroom_item *cli=data;
        T_Message msg = {0,};
        // send a message to text that is /whois
        C_MessageChat req;
        req.set_chatstring("/whois");
        req.set_dest(cli->channel);
        req.set_src(g_regnick);
        msg.data=req.Make();
        msg.message_type=MESSAGE_CHAT;
        if (msg.data)
        {
		msg.message_length=msg.data->GetLength();
       		g_mql->send(&msg);
        }

}

void Vaste_ChatPriv::onSendFile()
{
	qDebug("Vaste_ChatChan::onSendFile");
	chatroom_item *cli=data;

	Vaste_UploadFileDlg dlg;
/*
		QString s = dlg.getOpenFileName(
				"/",
				"*.*",
				this,
				"open file dialog",
				"Choose a file" );
*/
	if(QDialog::Accepted == dlg.exec())
	{
		qDebug("TODO : xfer_UploadFileToUser, selected file=%s,upload path=%s",dlg.selectedFile().ascii(),dlg.getUpLoadPath().ascii());
		//Xfer_UploadFileToUser(NULL, const_cast<char*>(dlg.selectedFile().ascii()), const_cast<char*>(it->text(0).ascii()), const_cast<char*>(dlg.getUpLoadPath().ascii()));

		QStringList fileList = dlg.selectedFiles();
		for( QStringList::iterator f= fileList.begin();f != fileList.end();++f)
		Xfer_UploadFileToUser(NULL, (char*)((*f).ascii()), cli->channel, (char*)(dlg.getUpLoadPath().ascii()));

	}
}


void Vaste_ChatPriv::onChatSendClicked()
{
	qDebug("Vaste_ChatChan::onChatSendClicked");
	processChatLine();
}

