/*
WASTE - wxWidgets preferences dialog
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

#ifndef _WXPREFS_H_
#define _WXPREFS_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxprefs.cpp"
#endif

/*!
* Includes
*/

////@begin includes
#include "wx/xrc/xmlres.h"
////@end includes

/*!
* Forward declarations
*/

////@begin forward declarations
////@end forward declarations

/*!
* Control identifiers
*/

////@begin control identifiers
#define ID_PREFS_DIALOG 10057
#define SYMBOL_WXPREFS_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
#define SYMBOL_WXPREFS_TITLE _("Preferences")
#define SYMBOL_WXPREFS_IDNAME ID_PREFS_DIALOG
#define SYMBOL_WXPREFS_SIZE wxSize(400, 300)
#define SYMBOL_WXPREFS_POSITION wxDefaultPosition
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
* WxPrefs class declaration
*/

class WxPrefs: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( WxPrefs )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	WxPrefs( );
	WxPrefs( wxWindow* parent, wxWindowID id = SYMBOL_WXPREFS_IDNAME, const wxString& caption = SYMBOL_WXPREFS_TITLE, const wxPoint& pos = SYMBOL_WXPREFS_POSITION, const wxSize& size = SYMBOL_WXPREFS_SIZE, long style = SYMBOL_WXPREFS_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXPREFS_IDNAME, const wxString& caption = SYMBOL_WXPREFS_TITLE, const wxPoint& pos = SYMBOL_WXPREFS_POSITION, const wxSize& size = SYMBOL_WXPREFS_SIZE, long style = SYMBOL_WXPREFS_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

////@begin WxPrefs event handler declarations

	/// wxEVT_CLOSE_WINDOW event handler for ID_PREFS_DIALOG
	void OnCloseWindow( wxCloseEvent& event );

	/// wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_CONN_SPEED
	void OnConnSpeedSelected( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_NICKNAME
	void OnNicknameUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_ENTER event handler for ID_NICKNAME
	void OnNicknameEnter( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_UPD_NICK
	void OnUpdNickClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_USER_INFO
	void OnUserInfoUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_ENTER event handler for ID_USER_INFO
	void OnUserInfoEnter( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_NET_NAME
	void OnNetNameUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_ENTER event handler for ID_NET_NAME
	void OnNetNameEnter( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_GEN_PRIVKEY
	void OnGenPrivkeyClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COPY_PUBKEY
	void OnCopyPubkeyClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_VIEW_PUBKEYS
	void OnViewPubkeysClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADD_PUB_KEYS
	void OnAddPubKeysClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BROADCAST_PUBKEY
	void OnBroadcastPubkeyClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_DOWNLOAD_PATH
	void OnDownloadPathUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BROWSE_DL_PATH
	void OnBrowseDlPathClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SHARE_PATH
	void OnSharePathUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADD_SHARE_PATH
	void OnAddSharePathClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLOSE_PREFS
	void OnClosePrefsClick( wxCommandEvent& event );

////@end WxPrefs event handler declarations

////@begin WxPrefs member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end WxPrefs member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin WxPrefs member variables
	wxChoice* m_conn_speed;
	wxTextCtrl* m_nickname;
	wxTextCtrl* m_user_info;
	wxTextCtrl* m_net_name;
	wxTextCtrl* m_download_path;
	wxTextCtrl* m_share_path;
////@end WxPrefs member variables
};


#endif
// _WXPREFS_H_
