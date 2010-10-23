/*
WASTE - wxWidgets chat dialogs
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
#pragma implementation "wxchat.h"
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

#include "stdafx.hpp"
#include "main.hpp"
#include "d_chat.hpp"
#include "m_chat.hpp"
#include "wxappresources.h"
#include "wxchat.h"

////@begin XPM images
////@end XPM images

/*!
* WxChat type definition
*/

IMPLEMENT_DYNAMIC_CLASS( WxChat, wxDialog )

/*!
* WxChat event table definition
*/

BEGIN_EVENT_TABLE( WxChat, wxDialog )

////@begin WxChat event table entries
	EVT_CLOSE( WxChat::OnCloseWindow )

	EVT_MENU( XRCID("IDM_CHAT_USER"), WxChat::OnMChatUserClick )

	EVT_MENU( XRCID("IDM_WHOIS_USER"), WxChat::OnMWhoisUserClick )

	EVT_MENU( XRCID("IDM_BROWSE_USER"), WxChat::OnMBrowseUserClick )

	EVT_MENU( XRCID("IDM_SENDFILE_USER"), WxChat::OnMSendfileUserClick )

	EVT_TREE_BEGIN_RDRAG( XRCID("IDC_TREE_CHATROOM"), WxChat::OnCTreeChatroomBeginRDrag )
	EVT_TREE_ITEM_ACTIVATED( XRCID("IDC_TREE_CHATROOM"), WxChat::OnCTreeChatroomItemActivated )

	EVT_TEXT_ENTER( XRCID("IDC_CHATEDIT"), WxChat::OnCChateditEnter )

////@end WxChat event table entries

END_EVENT_TABLE()

/*!
* WxChat constructors
*/

WxChat::WxChat( )
{
}

WxChat::WxChat( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, caption, pos, size, style);
}

/*!
* WxChat creator
*/

bool WxChat::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WxChat member initialisation
	m_chattext = NULL;
	m_tree_chatroom = NULL;
	m_chatedit = NULL;
	m_status = NULL;
////@end WxChat member initialisation

////@begin WxChat creation
	SetParent(parent);
	CreateControls();
	SetIcon(GetIconResource(wxT("res/chat_icon.gif")));
	Centre();
////@end WxChat creation

	SetSize(-1, -1, 420, 270, wxSIZE_USE_EXISTING);

	return TRUE;
}

/*!
* Control creation for WxChat
*/

void WxChat::CreateControls()
{    
////@begin WxChat content construction

	wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_CHAT_DLG"));
	m_chattext = XRCCTRL(*this, "IDC_CHATTEXT", wxTextCtrl);
	m_tree_chatroom = XRCCTRL(*this, "IDC_TREE_CHATROOM", wxTreeCtrl);
	m_chatedit = XRCCTRL(*this, "IDC_CHATEDIT", wxTextCtrl);
	m_status = XRCCTRL(*this, "IDC_STATUS", wxStaticText);
////@end WxChat content construction

////@begin WxChat content initialisation

////@end WxChat content initialisation
}

void WxChat::set_chatroom_item(chatroom_item *item)
{
	extern void wx_setup_chat_window(WxChat *, chatroom_item *);
	wx_setup_chat_window(this, item);
	m_userdata = (long) item;
}

/*!
* Should we show tooltips?
*/

bool WxChat::ShowToolTips()
{
	return TRUE;
}

/*!
* Get bitmap resources
*/

wxBitmap WxChat::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin WxChat bitmap retrieval
	return wxNullBitmap;
////@end WxChat bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon WxChat::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin WxChat icon retrieval
	if (name == wxT("res/chat_icon.gif"))
	{
		wxIcon icon(_T("res/chat_icon.gif"), wxBITMAP_TYPE_GIF);
		return icon;
	}
	return wxNullIcon;
////@end WxChat icon retrieval
}
/*!
* wxEVT_COMMAND_TREE_BEGIN_RDRAG event handler for IDC_TREE_CHATROOM
*/

void WxChat::OnCTreeChatroomBeginRDrag( wxTreeEvent& event )
{
	wxMenu *menu = AppResources::CreateMenuUserlistPopup();
	extern bool showTreeCtrlContextMenu (wxTreeCtrl *tree, wxMenu *menu, wxPoint, bool);
	showTreeCtrlContextMenu(m_tree_chatroom, menu, wxGetMousePosition(), true);
}

/*!
* wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for IDC_TREE_CHATROOM
*/

void WxChat::OnCTreeChatroomItemActivated( wxTreeEvent& event )
{
	extern chatroom_item *chat_ShowRoom(const char *channel, int activate);
	wxString name = m_tree_chatroom->GetItemText(event.GetItem());
	if (!name.IsEmpty()) {
		char buf[256];
		wxstr_to_cstr(buf, name, 255);
		chat_ShowRoom(buf, 2);
	}
	event.Skip();
}


/*!
* wxEVT_COMMAND_TEXT_ENTER event handler for IDC_CHATEDIT
*/

void WxChat::OnCChateditEnter( wxCommandEvent& event )
{
	extern int chat_WM_COMMAND_IDC_CHAT(wxWindow *hwndDlg);
	chat_WM_COMMAND_IDC_CHAT(this);
	event.Skip();
}


/*!
* wxEVT_CLOSE_WINDOW event handler for ID_CHAT_DLG
*/

void WxChat::OnCloseWindow( wxCloseEvent& event )
{
	extern int chat_WM_CLOSE(wxWindow *hwndDlg);
	chat_WM_CLOSE(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_CHAT_USER
*/

void WxChat::OnMChatUserClick( wxCommandEvent& event )
{
	char buf[1024];
	wxString text = m_tree_chatroom->GetItemText(m_users_hit_id);
	wxstr_to_cstr(buf, text, 1023);

	extern chatroom_item *chat_ShowRoom(const char *channel, int activate);
	chat_ShowRoom(buf, 1);

	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WHOIS_USER
*/

void WxChat::OnMWhoisUserClick( wxCommandEvent& event )
{
	char buf[1024];
	wxString text = m_tree_chatroom->GetItemText(m_users_hit_id);
	wxstr_to_cstr(buf, text, 1023);
	extern void main_ID_WHOISUSER(char *);
	main_ID_WHOISUSER(buf);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_BROWSE_USER
*/

void WxChat::OnMBrowseUserClick( wxCommandEvent& event )
{
	m_mainframe->BrowseUser(m_tree_chatroom->GetItemText(m_users_hit_id));
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_SENDFILE_USER
*/

void WxChat::OnMSendfileUserClick( wxCommandEvent& event )
{
	// XXXX todo
	event.Skip();
}


