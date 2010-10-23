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

#ifndef _WXMAINFRAME_H_
#define _WXMAINFRAME_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxmainframe.cpp"
#endif

/*!
* Includes
*/

////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/frame.h"
#include "wx/treectrl.h"
#include "wx/statline.h"
////@end includes

#include "wxnetstat.h"
#include "wxbrowser.h"
#include "wxuploads.h"
#include "wxxfers.h"
#include "wxprefs.h"

struct ChatroomItem;
typedef struct ChatroomItem chatroom_item;

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
#define ID_MAIN_FRAME 10003
#define SYMBOL_MAINFRAME_STYLE wxDEFAULT_FRAME_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxTAB_TRAVERSAL
#define SYMBOL_MAINFRAME_TITLE _("WxWASTE")
#define SYMBOL_MAINFRAME_IDNAME ID_MAIN_FRAME
#define SYMBOL_MAINFRAME_SIZE wxSize(400, 300)
#define SYMBOL_MAINFRAME_POSITION wxDefaultPosition
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

#include <wx/timer.h>
class MainFrame;
class MainTimer : public wxTimer
{
public:
class MainFrame *m_mainframe;
virtual void Notify ();
};

/*!
* MainFrame class declaration
*/

class MainFrame: public wxFrame
{    
	DECLARE_CLASS( MainFrame )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	MainFrame( );
	MainFrame( wxWindow* parent, wxWindowID id = SYMBOL_MAINFRAME_IDNAME, const wxString& caption = SYMBOL_MAINFRAME_TITLE, const wxPoint& pos = SYMBOL_MAINFRAME_POSITION, const wxSize& size = SYMBOL_MAINFRAME_SIZE, long style = SYMBOL_MAINFRAME_STYLE );

	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MAINFRAME_IDNAME, const wxString& caption = SYMBOL_MAINFRAME_TITLE, const wxPoint& pos = SYMBOL_MAINFRAME_POSITION, const wxSize& size = SYMBOL_MAINFRAME_SIZE, long style = SYMBOL_MAINFRAME_STYLE );

	virtual ~MainFrame( );

	/// Creates the controls and sizers
	void CreateControls();

////@begin MainFrame event handler declarations

	/// wxEVT_CLOSE_WINDOW event handler for ID_MAIN_FRAME
	void OnCloseWindow( wxCloseEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_CHAT_USER
	void OnMChatUserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WHOIS_USER
	void OnMWhoisUserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_BROWSE_USER
	void OnMBrowseUserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_SENDFILE_USER
	void OnMSendfileUserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_PREFS
	void OnMenuPrefsClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_QUIT
	void OnMenuQuitClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_NETSTAT
	void OnMenuNetstatClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_BROWSER
	void OnMenuBrowserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_XFERS
	void OnMenuXfersClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_UPLOADS
	void OnMenuUploadClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_JOINCHAT
	void OnMenuJoinchatClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_NETSTAT
	void OnBitmapbuttonNetstatClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BROWSER
	void OnButtonBrowserClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_JOINCHAT
	void OnJoinchatClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_TREE_SEL_CHANGED event handler for IDC_USERS
	void OnCUsersSelChanged( wxTreeEvent& event );

	/// wxEVT_COMMAND_TREE_SEL_CHANGING event handler for IDC_USERS
	void OnCUsersSelChanging( wxTreeEvent& event );

	/// wxEVT_COMMAND_TREE_BEGIN_RDRAG event handler for IDC_USERS
	void OnCUsersBeginRDrag( wxTreeEvent& event );

	/// wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for IDC_USERS
	void OnCUsersItemActivated( wxTreeEvent& event );

	/// wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for IDC_CHATROOMS
	void OnCChatroomsItemActivated( wxTreeEvent& event );

////@end MainFrame event handler declarations

	void BrowseUser (wxString who);

////@begin MainFrame member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end MainFrame member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();
	
	void exitRequested();
	void wasteInitPostwindowcreation();

	void CreateChatWindow (chatroom_item *cli);
	void ViewWindow (wxWindow *win, bool raise);
	void WriteConfigVis (wxWindow *win, int vis);
	void FlipShownStatus (wxWindow *win);

////@begin MainFrame member variables
	wxStaticText* m_netstatus;
	wxTreeCtrl* m_users;
	wxTreeCtrl* m_chatrooms;
////@end MainFrame member variables

	WxNetStatDialog *m_netstat_dlg;
	WxBrowser *m_browser_dlg;
	WxXfers *m_xfers_dlg;
	WxUploads *m_uploads_dlg;
	WxPrefs *m_prefs_dlg;

	wxTreeItemId m_users_hit_id;
	wxTreeItemId m_chats_hit_id;

	MainTimer *m_global_timer;
	bool m_exit_requested;
};

#endif // _WXMAINFRAME_H_
