/*
WASTE - wxWidgets downloads dialog
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

#ifndef _WXXFERS_H_
#define _WXXFERS_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxxfers.cpp"
#endif

/*!
* Includes
*/

////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/notebook.h"
#include "wx/listctrl.h"
////@end includes

/*!
* Forward declarations
*/

////@begin forward declarations
class wxNotebook;
class wxListCtrl;
////@end forward declarations

/*!
* Control identifiers
*/

////@begin control identifiers
#define ID_XFERS 10002
#define SYMBOL_WXXFERS_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
#define SYMBOL_WXXFERS_TITLE _("Transfers")
#define SYMBOL_WXXFERS_IDNAME ID_XFERS
#define SYMBOL_WXXFERS_SIZE wxSize(550, 300)
#define SYMBOL_WXXFERS_POSITION wxDefaultPosition
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
* WxXfers class declaration
*/

class WxXfers: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( WxXfers )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	WxXfers( );
	WxXfers( wxWindow* parent, wxWindowID id = SYMBOL_WXXFERS_IDNAME, const wxString& caption = SYMBOL_WXXFERS_TITLE, const wxPoint& pos = SYMBOL_WXXFERS_POSITION, const wxSize& size = SYMBOL_WXXFERS_SIZE, long style = SYMBOL_WXXFERS_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXXFERS_IDNAME, const wxString& caption = SYMBOL_WXXFERS_TITLE, const wxPoint& pos = SYMBOL_WXXFERS_POSITION, const wxSize& size = SYMBOL_WXXFERS_SIZE, long style = SYMBOL_WXXFERS_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

////@begin WxXfers event handler declarations

	/// wxEVT_CLOSE_WINDOW event handler for ID_XFERS
	void OnCloseWindow( wxCloseEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_CLEANUP_DLS
	void OnMCleanupDlsClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_CLEANUP_SEL
	void OnCleanupSelClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_ABORT_DL
	void OnAbortDlClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_RESUME_DL
	void OnResumeDlClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_DLNOW
	void OnMDlnowClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_TO_TOP
	void OnMToTopClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_TO_BOT
	void OnMToBotClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_REMOVE
	void OnMRemoveClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for IDD_XFERS
	void OnDXfersItemActivated( wxListEvent& event );

	/// wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK event handler for IDD_XFERS
	void OnDXfersRightClick( wxListEvent& event );

	/// wxEVT_COMMAND_CHOICE_SELECTED event handler for IDC_AUTOCLEAR
	void OnCAutoclearSelected( wxCommandEvent& event );

	/// wxEVT_COMMAND_CHOICE_SELECTED event handler for IDC_AUTORESUME
	void OnCAutoresumeSelected( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_RESUME
	void OnCResumeClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_ABORTSEL
	void OnCAbortselClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARCOMP
	void OnCClearcompClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARCOMP3
	void OnCClearcomp3Click( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARALL
	void OnCClearallClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_MAXSIMULDL
	void OnCMaxsimuldlUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_ENTER event handler for IDC_MAXSIMULDL
	void OnCMaxsimuldlEnter( wxCommandEvent& event );

	/// wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for ID_RECVLISTQ
	void OnRecvlistqItemActivated( wxListEvent& event );

	/// wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK event handler for ID_RECVLISTQ
	void OnRecvlistqRightClick( wxListEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_DLNOW
	void OnCDlnowClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_MOVETOTOP
	void OnCMovetotopClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_MOVETOBOTTOM
	void OnCMovetobottomClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_REMOVEQUEUE
	void OnCRemovequeueClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARQUEUE
	void OnCClearqueueClick( wxCommandEvent& event );

////@end WxXfers event handler declarations

////@begin WxXfers member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end WxXfers member function declarations

	void OnDestroy();

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin WxXfers member variables
	wxNotebook* m_notebook;
	wxPanel* m_dl_panel;
	wxListCtrl* m_xfers;
	wxChoice* m_autoclear;
	wxChoice* m_autoresume;
	wxBitmapButton* m_resume;
	wxBitmapButton* m_abortsel;
	wxBitmapButton* m_clearcomp;
	wxBitmapButton* m_clearcomp3;
	wxBitmapButton* m_clearall;
	wxTextCtrl* m_maxsimuldl;
	wxPanel* m_dlqueue_panel;
	wxListCtrl* m_recvlistq;
	wxBitmapButton* m_dlnow;
	wxBitmapButton* m_movetotop;
	wxBitmapButton* m_movetobottom;
	wxBitmapButton* m_removequeue;
	wxBitmapButton* m_clearqueue;
	wxStaticText* m_queueitems;
////@end WxXfers member variables
};

#endif
	// _WXXFERS_H_
