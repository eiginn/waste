/*
WASTE - wxWidgets implementation of network status dialog
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

#ifndef _WXNETSTAT_H_
#define _WXNETSTAT_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxnetstat.cpp"
#endif

/*!
* Includes
*/

////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/listctrl.h"
////@end includes

/*!
* Forward declarations
*/

////@begin forward declarations
class wxListCtrl;
////@end forward declarations

/*!
* Control identifiers
*/

////@begin control identifiers
#define ID_NETSTAT 10004
#define SYMBOL_WXNETSTATDIALOG_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
#define SYMBOL_WXNETSTATDIALOG_TITLE _("Network Status")
#define SYMBOL_WXNETSTATDIALOG_IDNAME ID_NETSTAT
#define SYMBOL_WXNETSTATDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_WXNETSTATDIALOG_POSITION wxDefaultPosition
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
* WxNetStatDialog class declaration
*/

class WxNetStatDialog: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( WxNetStatDialog )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	WxNetStatDialog( );
	WxNetStatDialog( wxWindow* parent, wxWindowID id = SYMBOL_WXNETSTATDIALOG_IDNAME, const wxString& caption = SYMBOL_WXNETSTATDIALOG_TITLE, const wxPoint& pos = SYMBOL_WXNETSTATDIALOG_POSITION, const wxSize& size = SYMBOL_WXNETSTATDIALOG_SIZE, long style = SYMBOL_WXNETSTATDIALOG_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXNETSTATDIALOG_IDNAME, const wxString& caption = SYMBOL_WXNETSTATDIALOG_TITLE, const wxPoint& pos = SYMBOL_WXNETSTATDIALOG_POSITION, const wxSize& size = SYMBOL_WXNETSTATDIALOG_SIZE, long style = SYMBOL_WXNETSTATDIALOG_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

////@begin WxNetStatDialog event handler declarations

	/// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for IDC_NEWCONNECTION
	void OnCNewconnectionSelected( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_NEWCONNECTION
	void OnCNewconnectionUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_ADDCONNECTION
	void OnCAddconnectionClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CONNECTCATCH
	void OnCConnectcatchClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_REMOVECONNECTION
	void OnCRemoveconnectionClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_REMOVECATCH
	void OnCRemovecatchClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_NUMCONUP
	void OnCNumconupUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_NS_HOST_LISTCTRL
	void OnNsHostListctrlSelected( wxListEvent& event );

	/// wxEVT_COMMAND_LIST_ITEM_DESELECTED event handler for ID_NS_HOST_LISTCTRL
	void OnNsHostListctrlDeselected( wxListEvent& event );

	/// wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for ID_NS_HOST_LISTCTRL
	void OnNsHostListctrlItemActivated( wxListEvent& event );

////@end WxNetStatDialog event handler declarations

	void OnCNewconnectionActivate( wxCommandEvent& event );

	void OnDestroy();

////@begin WxNetStatDialog member function declarations

	wxString GetHostListSel() const { return m_host_list_sel ; }
	void SetHostListSel(wxString value) { m_host_list_sel = value ; }

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end WxNetStatDialog member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin WxNetStatDialog member variables
	wxComboBox* m_newconnection;
	wxBitmapButton* m_addconnection;
	wxBitmapButton* m_connectcatch;
	wxBitmapButton* m_removeconnection;
	wxBitmapButton* m_removecatch;
	wxTextCtrl* m_num_con_up;
	wxListCtrl* m_hostlist;
	wxString m_host_list_sel;
////@end WxNetStatDialog member variables
};

#endif
	// _WXNETSTAT_H_
