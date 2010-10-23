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

#ifndef _WXCHAT_H_
#define _WXCHAT_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxchat.cpp"
#endif

/*!
* Includes
*/

////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/treectrl.h"
////@end includes
#include "wxmainframe.h"

/*!
* Forward declarations
*/

////@begin forward declarations
class wxTreeCtrl;
////@end forward declarations

/*!
* Control identifiers
*/

////@begin control identifiers
#define ID_CHAT_DLG 10078
#define SYMBOL_WXCHAT_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
#define SYMBOL_WXCHAT_TITLE _("Chat")
#define SYMBOL_WXCHAT_IDNAME ID_CHAT_DLG
#define SYMBOL_WXCHAT_SIZE wxSize(400, 300)
#define SYMBOL_WXCHAT_POSITION wxDefaultPosition
////@end control identifiers

/*!
* Compatibility
*/

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
* WxChat class declaration
*/

class WxChat: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( WxChat )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	WxChat( );
	WxChat( wxWindow* parent, wxWindowID id = SYMBOL_WXCHAT_IDNAME, const wxString& caption = SYMBOL_WXCHAT_TITLE, const wxPoint& pos = SYMBOL_WXCHAT_POSITION, const wxSize& size = SYMBOL_WXCHAT_SIZE, long style = SYMBOL_WXCHAT_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXCHAT_IDNAME, const wxString& caption = SYMBOL_WXCHAT_TITLE, const wxPoint& pos = SYMBOL_WXCHAT_POSITION, const wxSize& size = SYMBOL_WXCHAT_SIZE, long style = SYMBOL_WXCHAT_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

////@begin WxChat event handler declarations

	/// wxEVT_CLOSE_WINDOW event handler for ID_CHAT_DLG
	void OnCloseWindow( wxCloseEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_CHAT_USER
	void OnMChatUserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WHOIS_USER
	void OnMWhoisUserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_BROWSE_USER
	void OnMBrowseUserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_SENDFILE_USER
	void OnMSendfileUserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_TREE_BEGIN_RDRAG event handler for IDC_TREE_CHATROOM
	void OnCTreeChatroomBeginRDrag( wxTreeEvent& event );

	/// wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for IDC_TREE_CHATROOM
	void OnCTreeChatroomItemActivated( wxTreeEvent& event );

	/// wxEVT_COMMAND_TEXT_ENTER event handler for IDC_CHATEDIT
	void OnCChateditEnter( wxCommandEvent& event );

////@end WxChat event handler declarations

////@begin WxChat member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end WxChat member function declarations

	void set_chatroom_item(chatroom_item *item);

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin WxChat member variables
	wxTextCtrl* m_chattext;
	wxTreeCtrl* m_tree_chatroom;
	wxTextCtrl* m_chatedit;
	wxStaticText* m_status;
////@end WxChat member variables

	wxTreeItemId m_users_hit_id;
	MainFrame *m_mainframe;
	long    m_userdata;
};

#endif
	// _WXCHAT_H_
