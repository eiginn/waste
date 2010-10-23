/*
WASTE - wxWidgets main application window
Copyright (C) 2005 WASTE Development Team

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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wxmainframe.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include <wx/window.h>
#include "wxmainframe.h"
#include "wxappresources.h"
#include "wxchat.h"
#include "main.hpp"


// globals

////@begin XPM images
////@end XPM images

/*!
* MainFrame type definition
*/

IMPLEMENT_CLASS( MainFrame, wxFrame )

/*!
* MainFrame event table definition
*/

BEGIN_EVENT_TABLE( MainFrame, wxFrame )

////@begin MainFrame event table entries
	EVT_CLOSE( MainFrame::OnCloseWindow )

	EVT_MENU( XRCID("IDM_CHAT_USER"), MainFrame::OnMChatUserClick )

	EVT_MENU( XRCID("IDM_WHOIS_USER"), MainFrame::OnMWhoisUserClick )

	EVT_MENU( XRCID("IDM_BROWSE_USER"), MainFrame::OnMBrowseUserClick )

	EVT_MENU( XRCID("IDM_SENDFILE_USER"), MainFrame::OnMSendfileUserClick )

	EVT_MENU( XRCID("ID_MENU_PREFS"), MainFrame::OnMenuPrefsClick )

	EVT_MENU( XRCID("ID_MENU_QUIT"), MainFrame::OnMenuQuitClick )

	EVT_MENU( XRCID("ID_MENU_NETSTAT"), MainFrame::OnMenuNetstatClick )

	EVT_MENU( XRCID("ID_MENU_BROWSER"), MainFrame::OnMenuBrowserClick )

	EVT_MENU( XRCID("ID_MENU_XFERS"), MainFrame::OnMenuXfersClick )

	EVT_MENU( XRCID("ID_MENU_UPLOADS"), MainFrame::OnMenuUploadClick )

	EVT_MENU( XRCID("ID_MENU_JOINCHAT"), MainFrame::OnMenuJoinchatClick )

	EVT_BUTTON( XRCID("ID_BITMAPBUTTON_NETSTAT"), MainFrame::OnBitmapbuttonNetstatClick )

	EVT_BUTTON( XRCID("ID_BUTTON_BROWSER"), MainFrame::OnButtonBrowserClick )

	EVT_BUTTON( XRCID("ID_JOINCHAT"), MainFrame::OnJoinchatClick )

	EVT_TREE_SEL_CHANGED( XRCID("IDC_USERS"), MainFrame::OnCUsersSelChanged )
	EVT_TREE_SEL_CHANGING( XRCID("IDC_USERS"), MainFrame::OnCUsersSelChanging )
	EVT_TREE_BEGIN_RDRAG( XRCID("IDC_USERS"), MainFrame::OnCUsersBeginRDrag )
	EVT_TREE_ITEM_ACTIVATED( XRCID("IDC_USERS"), MainFrame::OnCUsersItemActivated )

	EVT_TREE_ITEM_ACTIVATED( XRCID("IDC_CHATROOMS"), MainFrame::OnCChatroomsItemActivated )

////@end MainFrame event table entries

END_EVENT_TABLE()

/*!
* MainFrame constructors
*/

MainFrame::MainFrame( )
{
}

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Create( parent, id, caption, pos, size, style );
}

/*!
* MainFrame creator
*/

bool MainFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	m_exit_requested = false;

////@begin MainFrame member initialisation
	m_netstatus = NULL;
	m_users = NULL;
	m_chatrooms = NULL;
////@end MainFrame member initialisation

	m_netstat_dlg = new WxNetStatDialog(NULL, ID_NETSTAT, _("Network Status"));
	m_browser_dlg = new WxBrowser(NULL, ID_BROWSER, _("Browser"));
	m_xfers_dlg = new WxXfers(NULL, ID_BROWSER, _("Downloads"));
	m_uploads_dlg = new WxUploads(NULL, ID_BROWSER, _("Uploads"));
	m_prefs_dlg = new WxPrefs(NULL, ID_BROWSER, _("Preferences"));

	extern void xferwnd_find_wxwidgets(WxXfers *dlg, WxUploads *uploaddlg);
	xferwnd_find_wxwidgets(m_xfers_dlg, m_uploads_dlg);

////@begin MainFrame creation
	SetParent(parent);
	CreateControls();
	SetIcon(GetIconResource(wxT("res/icon1.ico.gif")));
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Centre();
////@end MainFrame creation

	wasteInitPostwindowcreation();

	m_global_timer = new MainTimer();
	m_global_timer->m_mainframe = this;
	m_global_timer->Start(500);

	return TRUE;
}

void MainFrame::wasteInitPostwindowcreation () {
	extern void main_find_wxwidgets(MainFrame *dlg);
	extern void netkern_find_wxwidgets(WxNetStatDialog *dlg);
	extern void srchwnd_find_wxwidgets(WxBrowser *dlg);
	extern void LoadNetQ();

	main_find_wxwidgets(this);
	srchwnd_find_wxwidgets(m_browser_dlg);
	netkern_find_wxwidgets(m_netstat_dlg);
	LoadNetQ();

	if (g_config->ReadInt(CONFIG_net_vis,CONFIG_net_vis_DEFAULT))
		FlipShownStatus(m_netstat_dlg);

	if (g_config->ReadInt(CONFIG_xfers_vis,CONFIG_xfers_vis_DEFAULT)) {
		FlipShownStatus(m_xfers_dlg);
		FlipShownStatus(m_uploads_dlg);
	}

	if (g_config->ReadInt(CONFIG_search_vis,CONFIG_search_vis_DEFAULT))
		FlipShownStatus(m_browser_dlg);

}

/*!
* Control creation for MainFrame
*/

void MainFrame::CreateControls()
{    
////@begin MainFrame content construction

	wxXmlResource::Get()->LoadFrame(this, GetParent(), _T("ID_MAIN_FRAME"));
	m_netstatus = XRCCTRL(*this, "IDC_NETSTATUS", wxStaticText);
	m_users = XRCCTRL(*this, "IDC_USERS", wxTreeCtrl);
	m_chatrooms = XRCCTRL(*this, "IDC_CHATROOMS", wxTreeCtrl);
////@end MainFrame content construction

	// Create custom windows not generated automatically here.

////@begin MainFrame content initialisation

////@end MainFrame content initialisation
}

/*!
* Should we show tooltips?
*/

bool MainFrame::ShowToolTips()
{
	return TRUE;
}

MainFrame::~MainFrame ()
{
	m_netstat_dlg->Destroy(); m_netstat_dlg = NULL;
	m_browser_dlg->Destroy(); m_browser_dlg = NULL;
	m_xfers_dlg->Destroy(); m_xfers_dlg = NULL;
	m_uploads_dlg->Destroy(); m_uploads_dlg = NULL;
	m_prefs_dlg->Destroy(); m_prefs_dlg = NULL;

	//delete m_netstat_dlg; m_netstat_dlg = NULL;
	//delete m_browser_dlg; m_browser_dlg = NULL;
	//delete m_xfers_dlg; m_xfers_dlg = NULL;
	//delete m_uploads_dlg; m_uploads_dlg = NULL;
	//delete m_prefs_dlg; m_prefs_dlg = NULL;
}

/*!
* Get bitmap resources
*/

wxBitmap MainFrame::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin MainFrame bitmap retrieval
	if (name == wxT("res/connect.ico.gif"))
	{
		wxBitmap bitmap(_T("res/connect.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/search.ico.gif"))
	{
		wxBitmap bitmap(_T("res/search.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/chat.ico.gif"))
	{
		wxBitmap bitmap(_T("res/chat.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	return wxNullBitmap;
////@end MainFrame bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon MainFrame::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin MainFrame icon retrieval
	if (name == wxT("res/icon1.ico.gif"))
	{
		wxIcon icon(_T("res/icon1.ico.gif"), wxBITMAP_TYPE_GIF);
		return icon;
	}
	return wxNullIcon;
////@end MainFrame icon retrieval
}

void MainFrame::WriteConfigVis (wxWindow *win, int vis) {
	int menuid;
	if (win == m_netstat_dlg) {
		g_config->WriteInt(CONFIG_net_vis,vis);
		menuid = GetMenuBar()->FindMenuItem(_T("View"), _T("Network Status..."));
	}
	else if (win == m_browser_dlg) {
		g_config->WriteInt(CONFIG_search_vis,vis);
		menuid = GetMenuBar()->FindMenuItem(_T("View"), _T("Browser..."));
	}
	else if (win == m_xfers_dlg) {
		g_config->WriteInt(CONFIG_xfers_vis,vis);
		menuid = GetMenuBar()->FindMenuItem(_T("View"), _T("Transfers..."));
	}
	else if (win == m_uploads_dlg) {
		g_config->WriteInt(CONFIG_xfers_vis,vis);
		menuid = GetMenuBar()->FindMenuItem(_T("View"), _T("Uploads..."));
	}
	else {
		return; /* prefs, or similar */
	}
	GetMenuBar()->FindItem(menuid)->Check(vis != 0);
}

void MainFrame::FlipShownStatus (wxWindow *win) {
	int vis = 0;
	if (win->IsShown()) {
		win->Show(false);
	} else {
		win->Show(true);
		win->Raise();
		vis = 1;
	}
	WriteConfigVis(win, vis);
}

void MainFrame::ViewWindow (wxWindow *win, bool raise) {
	int vis = 0;
	if (win->IsShown()) {
		if (raise) { win->Raise(); }
	} else {
		win->Show(true);
		if (raise) { win->Raise(); }
		vis = 1;
	}
	WriteConfigVis(win, vis);
}

/*!
* wxEVT_CLOSE_WINDOW event handler for ID_MAIN_FRAME
*/

void MainFrame::OnCloseWindow( wxCloseEvent& event )
{
	exitRequested();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_NETSTAT
*/

void MainFrame::OnBitmapbuttonNetstatClick( wxCommandEvent& event )
{
	FlipShownStatus(m_netstat_dlg);
}


/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_QUIT
*/

void MainFrame::OnMenuQuitClick( wxCommandEvent& event )
{
	this->Close();
}

void MainFrame::exitRequested ()
{
	m_netstat_dlg->Show(false);
	m_browser_dlg->Show(false);
	m_xfers_dlg->Show(false);
	m_uploads_dlg->Show(false);
	m_prefs_dlg->Show(false);
	this->Show(false);

	delete m_global_timer;
	m_exit_requested = true;

	extern void CloseAllChatWindows();
	CloseAllChatWindows();

	extern void main_WM_DESTROY();
	main_WM_DESTROY();

	waste_main_exit();

	this->Destroy();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_NETSTAT
*/

void MainFrame::OnMenuNetstatClick( wxCommandEvent& event )
{
	FlipShownStatus(m_netstat_dlg);
}


void MainTimer::Notify ()
{
	if (m_mainframe->m_exit_requested) {
		return;
	}

        main_onGotNick(NULL,0);
        main_onGotChannel(NULL);

	if (waste_main_poll()) {
		m_mainframe->exitRequested();
		return;
	}
	if (m_mainframe->m_netstat_dlg->IsShown()) {
		extern int netkern_WM_TIMER();
		netkern_WM_TIMER();
	}
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BROWSER
*/

void MainFrame::OnButtonBrowserClick( wxCommandEvent& event )
{
	FlipShownStatus(m_browser_dlg);
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_JOINCHAT
*/

void MainFrame::OnJoinchatClick( wxCommandEvent& event )
{
	extern void CreateChatDialog (const char *chatname);
	CreateChatDialog(NULL);
	event.Skip();
}


/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU
*/

void MainFrame::OnMenuBrowserClick( wxCommandEvent& event )
{
	FlipShownStatus(m_browser_dlg);
}


/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENUITEM
*/

void MainFrame::OnMenuXfersClick( wxCommandEvent& event )
{
	FlipShownStatus(m_xfers_dlg);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENUITEM1
*/

void MainFrame::OnMenuUploadClick( wxCommandEvent& event )
{
	FlipShownStatus(m_uploads_dlg);
	event.Skip();
}


/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_PREFS
*/

void MainFrame::OnMenuPrefsClick( wxCommandEvent& event )
{
	FlipShownStatus(m_prefs_dlg);
	event.Skip();
}


/*!
* wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for IDC_USERS
*/

void MainFrame::OnCUsersItemActivated( wxTreeEvent& event )
{
	extern int main_USERS_clicked(bool);
	main_USERS_clicked(true);
	event.Skip();
}

/*!
* wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for IDC_CHATS
*/

void MainFrame::OnCChatroomsItemActivated( wxTreeEvent& event )
{
	extern int main_USERS_clicked(bool);
	main_USERS_clicked(false);
	event.Skip();
}

/*!
* wxEVT_COMMAND_TREE_BEGIN_RDRAG event handler for IDC_USERS
*/

void MainFrame::OnCUsersBeginRDrag( wxTreeEvent& event )
{
	// this is crazy -- we have to get the mouse pos and adjust from
	// that!  wxTreeCtrls need a proper context-menu event
	wxPoint where = wxGetMousePosition();

	wxMenu *menu = AppResources::CreateMenuUserlistPopup();
	extern bool showTreeCtrlContextMenu (wxTreeCtrl *tree, wxMenu *menu, wxPoint, bool);
	showTreeCtrlContextMenu(m_users, menu, where, true);

	event.Skip();
}


/*!
* wxEVT_COMMAND_TREE_SEL_CHANGED event handler for IDC_USERS
*/

void MainFrame::OnCUsersSelChanged( wxTreeEvent& event )
{
	event.Skip();
}

/*!
* wxEVT_COMMAND_TREE_SEL_CHANGING event handler for IDC_USERS
*/

void MainFrame::OnCUsersSelChanging( wxTreeEvent& event )
{
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_CHAT_USER
*/

void MainFrame::OnMChatUserClick( wxCommandEvent& event )
{
	char buf[1024];
	m_users_hit_id = m_users->GetSelection();
	wxString text = m_users->GetItemText(m_users_hit_id);
	wxstr_to_cstr(buf, text, 1023);

	extern chatroom_item *chat_ShowRoom(const char *channel, int activate);
	chat_ShowRoom(buf, 1);

	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WHOIS_USER
*/

void MainFrame::OnMWhoisUserClick( wxCommandEvent& event )
{
	char buf[1024];
	m_users_hit_id = m_users->GetSelection();
	wxString text = m_users->GetItemText(m_users_hit_id);
	wxstr_to_cstr(buf, text, 1023);
	extern void main_ID_WHOISUSER(char *);
	main_ID_WHOISUSER(buf);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_BROWSE_USER
*/

void MainFrame::OnMBrowseUserClick( wxCommandEvent& event )
{
	m_users_hit_id = m_users->GetSelection();
	BrowseUser(m_users->GetItemText(m_users_hit_id));
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_SENDFILE_USER
*/

void MainFrame::OnMSendfileUserClick( wxCommandEvent& event )
{
	// XXXX todo
	event.Skip();
}

void MainFrame::CreateChatWindow (chatroom_item *cli) {
	WxChat *dlg = new WxChat(NULL, ID_CHAT_DLG, _("Chat"));
	dlg->set_chatroom_item (cli);
	dlg->m_mainframe = this;
	dlg->Show(true);
	dlg->Raise();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_JOINCHAT
*/

void MainFrame::OnMenuJoinchatClick( wxCommandEvent& event )
{
	extern void CreateChatDialog (const char *chatname);
	CreateChatDialog(NULL);
	event.Skip();
}

void MainFrame::BrowseUser (wxString who)
{
	m_browser_dlg->Show(true);
	m_browser_dlg->Raise();

	char buf[1024];
	wxString text = _T("/") + who;
	wxstr_to_cstr(buf, text, 1023);

	extern void Search_Search(char *);
	Search_Search(buf);
}

