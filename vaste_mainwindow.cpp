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
 * This file implements the display of MainGui
 * Author: Stanley San
 * Date Created: 2003.12.1
*/
#include "main.h"
#include "xferwnd.h"
#include "netq.h"

#include "vaste_mainwindow.h"
#include "ui_treeview.h"
#include "ui_listview.h"

#include "vaste_idd_net.h"
#include "vaste_idd_search.h"
#include "vaste_idd_xfers.h"

#include "idd_chatroomcreate.h"
#include "vaste_chat.h"
#include "vq_chat.h"
#include "qulf_dialog.h"
#include "idd_about.h"
#include "idd_help.h"

#include "srchwnd.h"

#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qheader.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qbitmap.h>
#include <qpushbutton.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include "netkern.h"
#include "m_chat.h"


#include "preference.h"

QVaste_MainWin::QVaste_MainWin( QWidget* parent , const char* name , WFlags fl )
    : QVaste( parent, name, fl )
{
    g_mainwnd = this;
    setCaption(APP_NAME);

#include "vaste_widget_palette.xx"

    setPalette( dialog_pal );
    PushButton_IDC_NETSETTINGS->setPalette( pushbutton_pal );
    PushButton_IDC_SEARCH->setPalette( pushbutton_pal );
    PushButton_IDC_CREATECHATROOM->setPalette( pushbutton_pal );
    PushButton_IDC_PREF->setPalette( pushbutton_pal );
    ListView_IDC_USERS->setPalette( listview_pal );
    ListView_IDC_CHATROOMS->setPalette( listview_pal );
	Main_Pane->setPalette( frame_pal );

	// skin settings here.
	QPixmap button_bg("skins/vaste_toolbutton_bg.png");

	PushButton_IDC_NETSETTINGS->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_SEARCH->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_CREATECHATROOM->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_PREF->setPaletteBackgroundPixmap(button_bg);

	QPixmap dlg_bg("skins/vaste_pane_bg.png");
	QPixmap pane_bg("skins/vaste_pane_bg1.png");
	
	setPaletteBackgroundPixmap(dlg_bg);
	Main_Pane->setPaletteBackgroundPixmap(pane_bg);
	// skin settings end.

	QPixmap listview_head("skins/vaste_listview_header.png");
	ListView_IDC_USERS->header()->setPaletteBackgroundPixmap(listview_head);
	ListView_IDC_CHATROOMS->header()->setPaletteBackgroundPixmap(listview_head);

	QPixmap netstat_0("icons/vaste_mainwindow_network_enable.png");
	QPixmap browser_0("icons/vaste_mainwindow_browse_enable.png");
	QPixmap chat_0("icons/vaste_mainwindow_chat_enable.png");
	QPixmap preference_0("icons/vaste_mainwindow_preference_enable.png");
	
	PushButton_IDC_NETSETTINGS->setPixmap( netstat_0 );
	PushButton_IDC_SEARCH->setPixmap( browser_0 );
	PushButton_IDC_CREATECHATROOM->setPixmap( chat_0 );
	PushButton_IDC_PREF->setPixmap( preference_0 );

    m_ChatSubMenu = new QPopupMenu();

    EditMenu->insertItem("Chat Window", m_ChatSubMenu ,-1,5);
    connect( m_ChatSubMenu, SIGNAL( activated(int) ), this, SLOT( processChatSubMenu(int) ));
    ListView_IDC_USERS->addColumn("User Name",180);
    ListView_IDC_CHATROOMS->addColumn("ChannelChat Name",180);
    connect( EditMenu , SIGNAL( aboutToShow() ), this, SLOT( makeChatSubMenu() ) ); 

    init();
}

QVaste_MainWin::~QVaste_MainWin()
{
    if(m_ChatSubMenu)
	delete m_ChatSubMenu;

    killTimer( m_UpdateUserListTimer );

    deinit();
}

void QVaste_MainWin::deinit(void)
{
//following maybe useful later, keep it anyway
/*
	if(g_netstatus_wnd)
	{
		delete g_netstatus_wnd;
		g_netstatus_wnd = NULL;
	}
	if(g_xferwnd)
	{
		delete g_xferwnd;
		g_xferwnd = NULL;
	}


	if(g_search_wnd)
	{
		delete g_search_wnd;
		g_search_wnd = NULL;
	}
*/

/*	if(pref)
		delete pref;
*/
}

void QVaste_MainWin::init(void)
{
        pref = new WndPreference;
        connect( pref, SIGNAL( signalAppendWndTitle(const QString&,bool) ), this, SLOT( slotAppendWndTitle(const QString&,bool) ) );

////	
        safe_strncpy(g_regnick,g_config->ReadString("nick",""),sizeof(g_regnick));
        if (g_regnick[0] == '#' || g_regnick[0] == '&')
          g_regnick[0]=0;

        setCaption(APP_NAME);

  	//qDebug("TODO: create g_xferwnd");
	g_search_wnd= new Vaste_Idd_Search(NULL,"Browse");
////
        g_netstatus_wnd = new Vaste_Idd_Net(NULL,"Netstatus");


        g_xferwnd= new Vaste_Idd_Xfers(NULL,"Xfer");
///
        if (g_config->ReadInt("net_vis",1))
	{
        	g_netstatus_wnd->show();
	}
        if (g_config->ReadInt("xfers_vis",0))
	{
	        g_xferwnd->show();
	}
        if (g_config->ReadInt("search_vis",0))
	{
		g_search_wnd->show();
	}
	
        LoadNetQ();

	m_UpdateUserListTimer = -1;
        m_LoopTimer = startTimer(5);


//
	L_Chatroom = NULL;
}

void QVaste_MainWin::editCreateChatWin()
{
	IDD_CHATROOMCREATE dlg(this);
	dlg.setCaption("Join/Create Chat Room -"APP_NAME);
	if(dlg.exec()==QDialog::Accepted)
	{
  		qDebug("QVaste_MainWin::editCreateChatWin accept %s",dlg.LineEdit_IDC_EDIT1->text().ascii());
		chatroom_item* p=chat_ShowRoom(dlg.LineEdit_IDC_EDIT1->text().ascii(),1);
		if(p)
			p->hwnd->show();
	}
	else
	{
  		qDebug("QVaste_MainWin::editCreateChatWin reject");
	}
	
}

void QVaste_MainWin::main_onGotChannel(char* cnl)
{
  qDebug("main_onGotChannel, %s",cnl);
  QListView* htree = ListView_IDC_CHATROOMS;
  int added=0;
  VUser_TreeViewItem* h = TreeView_GetRootChild(htree);
  while (h)
  {
    qDebug("just in outer while\n");

    const char * text = (const char*)(h->text(0).ascii());
    qDebug("text = %s",text);

    if (cnl)
    {
      if (!strcasecmp(text,cnl))
      {
	TreeView_SetItem(htree,h,cnl,time(NULL));
        added=1;
      }
    }
  
    qDebug("before time\n");
    if (time(NULL)-h->m_param > 4*60)
    {
      VUser_TreeViewItem* oldIt = h;	
      h=TreeView_GetNextSibling(htree,h);

      chatroom_item *p=L_Chatroom;
      while(p!=NULL)
      {
        qDebug("just in inner while p=%x\n",p);
        qDebug("just in inner while p->channel=%s\n",p->channel);
        qDebug("just in inner while oldIt =%x\n",oldIt);
        qDebug("just in inner while text=%s\n",text);
        if (!strcasecmp(text,p->channel)) break;
        p=p->next;
      }
      qDebug("just out inner while\n");
      if (!p) TreeView_DeleteItem(htree,oldIt);
    }
    else h=TreeView_GetNextSibling(htree,h);
  }

  qDebug("just out outer while\n");
  if (!added && cnl)
  {
    //TreeView_InsertItem(htree,cnl,time(NULL));
    TreeView_InsertItem(htree,cnl,time(NULL),"icons/vaste_group_small.png");
  }
}

void QVaste_MainWin::main_onGotNick(char* nick,int del)
{
#ifdef _DEBUG
  time_t t = time(NULL);
  qDebug("-----------------main_onGotNick, %s,%d,time=%s",nick,del,ctime(&t));
#endif
  if (nick)
  {
    if (!strcasecmp(nick,g_regnick)) return;
    killTimer( m_UpdateUserListTimer );
    m_UpdateUserListTimer = startTimer(5000);
  }
  QListView* htree = ListView_IDC_USERS;

  VUser_TreeViewItem* h = TreeView_GetRootChild(htree);

  while (h)
  {
    qDebug("just in while loop");

    if (nick)
    {
      if (!strcasecmp((const char*)(h->text(0).ascii()),nick))
      {
        if (del)
        {
        	TreeView_DeleteItem(htree,h);
                return;
        }
	TreeView_SetItem(htree,h,nick,time(NULL));
        break;
      }
      h=TreeView_GetNextSibling(htree,h);
    }
    else
    {
      VUser_TreeViewItem* oldIt = h;
      h=TreeView_GetNextSibling(htree,h);
#ifdef _DEBUG
      time_t t = time(NULL);
      qDebug("-----------g_timeout=%d,now time=%s",g_timeout,ctime(&t));
#endif

      if (time(NULL)-oldIt->m_param > 240)
      {
      	TreeView_DeleteItem(htree,oldIt);
      }
    }
  }
  if (!h && nick)
  {
    //TreeView_InsertItem(htree,nick,time(NULL));
    TreeView_InsertItem(htree,nick,time(NULL),"icons/vaste_user_small.png");
  }
}

void QVaste_MainWin::timerEvent(QTimerEvent *e)
{
	//qDebug("here\n");
	if(e->timerId()==m_LoopTimer)
	{
		loopHandler();
	}
	else if(e->timerId()==m_UpdateUserListTimer)
	{
		main_onGotNick(NULL,0);
		main_onGotChannel(NULL);
	}
	else
	{
		qDebug("timer event, id %d",e->timerId());
	}
	//qDebug("dehere\n");
}

extern void vasteKernRun(void);

void QVaste_MainWin::loopHandler(void)
{
	vasteKernRun();
}

void QVaste_MainWin::filePreference()
{
        pref->show();
}

void QVaste_MainWin::slotAppendWndTitle(const QString &str, bool b)
{
      qDebug("TODO: xferwnd QVaste_MainWin::slotAppendWndTitle(),str=%s,b=%d",str.ascii(),b);
      SetWndTitle(this, APP_NAME);
      if (g_search_wnd) ((Vaste_Idd_Search*)g_search_wnd)->SetWndTitle();
      if (g_netstatus_wnd) ((Vaste_Idd_Net*)g_netstatus_wnd)->UpdateWinTitle();
      if (g_xferwnd) ((Vaste_Idd_Xfers*)g_xferwnd)->UpdateTitle();
      chat_updateTitles();
}

void QVaste_MainWin::editAlwaysOnTop()
{
    int id = EditMenu->idAt(7);

    if(EditMenu->isItemChecked(id))
    {
    	EditMenu->setItemChecked(id,0);
    }
    else 
    {
    	EditMenu->setItemChecked(id,1);
    }
}

void QVaste_MainWin::onUserListClicked(QListViewItem* it)
{
	qDebug("user Clicked! %s",it->text(0).ascii());
	if(it)
	{
		char text[256];
		text[0]='\0';
		strncpy(text,it->text(0).ascii(),sizeof(text));
		text[255]='\0';
		if(text[0])
			chat_ShowRoom(text,2);
	}
}

void QVaste_MainWin::onChatRoomListClicked(QListViewItem* it)
{
	onUserListClicked(it);
}

void QVaste_MainWin::onUserListRMBClicked(QListViewItem* it,const QPoint& pt,int col)
{
	if(it)
	{
	qDebug("onUserListRMBClicked item=%s",it->text(0).ascii());

	QPopupMenu *contextMenu = new QPopupMenu( this );
	Q_CHECK_PTR( contextMenu );
	int chatId = contextMenu->insertItem( "Chat user" );
	int whoisId = contextMenu->insertItem( "Whois user" );
	int browseId = contextMenu->insertItem( "Browse user" );
	int sendId = contextMenu->insertItem( "Send file(s) to user" );
	int ret = contextMenu->exec( pt );
	if(ret == chatId)
	{
		qDebug("chat selected");
		chat_ShowRoom(it->text(0).ascii(),1);
	}
	else if(ret == whoisId)
	{
		qDebug("whois selected");
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
		editBrowse();
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

void QVaste_MainWin::fileExit()
{
	qDebug("QVaste_MainWin::fileExit()");
	close();
}

void QVaste_MainWin::editStatus()
{
	if(!g_netstatus_wnd)	
        	g_netstatus_wnd = new Vaste_Idd_Net(NULL,"Netstatus");
	g_netstatus_wnd->show();
	g_netstatus_wnd->setActiveWindow();
	g_netstatus_wnd->raise();
	g_netstatus_wnd->setFocus();
        g_config->WriteInt("net_vis",1);
}

void QVaste_MainWin::editBrowse()
{
	if(!g_search_wnd)	
		g_search_wnd= new Vaste_Idd_Search(NULL,"Browse");
	g_search_wnd->show();
        g_config->WriteInt("search_vis",1);
	g_search_wnd->setActiveWindow();
	g_search_wnd->raise();
	g_search_wnd->setFocus();
}

void QVaste_MainWin::editTransfer()
{
	qDebug("editTransfer() isTopLevel=%d",isTopLevel());
	if(!g_xferwnd)	
		g_xferwnd= new Vaste_Idd_Xfers(NULL,"Xfers");
	g_xferwnd->show();
	g_xferwnd->setActiveWindow();
	g_xferwnd->raise();
	g_xferwnd->setFocus();
        g_config->WriteInt("xfers_vis",1);
}

void QVaste_MainWin::editChatWin()
{
	char text[64];
	text[0]=0;
	VUser_TreeViewItem* it = TreeView_GetSelection(ListView_IDC_USERS);
	if(it)
	{
	}
}

void QVaste_MainWin::makeChatSubMenu()
{
	qDebug("makeChatSubMenu()");
	
  	chatroom_item *p=L_Chatroom;
	
    	m_ChatSubMenu->clear();
	if(!p)
	{
    		m_ChatSubMenu->insertItem("No active chats");
	}
	else
	{
    		m_ChatSubMenu->insertItem("Hide all chat windows");
    		m_ChatSubMenu->insertItem("Show all chat windows");
    		m_ChatSubMenu->insertSeparator();
		int num=0;
		while(p&&num<CHATMENU_MAX)
		{
    			m_ChatSubMenu->insertItem(p->channel);
			p = p->next;
		}
	
	}
    	//m_ChatSubMenu->insertItem("help");
}

void QVaste_MainWin::processChatSubMenu(int id)
{
	const char * str = m_ChatSubMenu->text(id).ascii();
	qDebug("select Menu->text()=%s",m_ChatSubMenu->text(id).ascii());
  	chatroom_item *p=L_Chatroom;

	if(id==m_ChatSubMenu->idAt(0))
	{
		while(p)
		{
			p->hwnd->hide();
			p = p->next;
		}
	}
	else if(id==m_ChatSubMenu->idAt(1))
	{
		while(p)
		{
			p->hwnd->show();
			p->hwnd->raise();
			p = p->next;
		}
	}
	else
	{
		chat_ShowRoom(str,1);
	}
}

void QVaste_MainWin::resizeEvent(QResizeEvent* e)
{
	qDebug("QVaste_MainWin::resizeEvent()");
}

void QVaste_MainWin::onHelpHelp()
{
	qDebug("QVaste_MainWin::onHelpHelp()");
	/*
	Form2 hl;
	hl.exec();
	*/

#ifdef _WIN32
	 //UINT ret = WinExec("\"C:\\Program Files\\Internet Explorer\\IEXPLORE.exe\" d:\\index.htm",SW_SHOW);
	UINT ret = WinExec("hh.exe padlocksl.chm",SW_SHOW);
	/*
	if (ret == 0)
		QMessageBox::warning(0, QString(APP_NAME " r"), "The system is out of memory or resources.");
	if (ret == ERROR_BAD_FORMAT)
		QMessageBox::warning(0, QString(APP_NAME " r"), "The .exe file is invalid");
	if (ret == ERROR_FILE_NOT_FOUND)
		QMessageBox::warning(0, QString(APP_NAME " r"), "The specified file was not found.");
	if (ret == ERROR_PATH_NOT_FOUND)
		QMessageBox::warning(0, QString(APP_NAME " r"), "The specified path was not found");
	*/
#else
	QProcess *proc = new QProcess(this);
	proc->addArgument("mozilla");
	QString path = QDir("./docs/index.htm").absPath();
	proc->addArgument(path);
	proc->start();
#endif
}

void QVaste_MainWin::onHelpAbout()
{
	qDebug("QVaste_MainWin::onHelpAbout()");
	About ab;
	ab.exec();
}

