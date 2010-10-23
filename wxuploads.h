/*
WASTE - wxWidgets uploads dialog
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

#ifndef _WXUPLOADS_H_
#define _WXUPLOADS_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxuploads.cpp"
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
#define ID_UPLOADS 10039
#define SYMBOL_WXUPLOADS_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
#define SYMBOL_WXUPLOADS_TITLE _("Uploads")
#define SYMBOL_WXUPLOADS_IDNAME ID_UPLOADS
#define SYMBOL_WXUPLOADS_SIZE wxSize(550, 300)
#define SYMBOL_WXUPLOADS_POSITION wxDefaultPosition
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
* WxUploads class declaration
*/

class WxUploads: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( WxUploads )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	WxUploads( );
	WxUploads( wxWindow* parent, wxWindowID id = SYMBOL_WXUPLOADS_IDNAME, const wxString& caption = SYMBOL_WXUPLOADS_TITLE, const wxPoint& pos = SYMBOL_WXUPLOADS_POSITION, const wxSize& size = SYMBOL_WXUPLOADS_SIZE, long style = SYMBOL_WXUPLOADS_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXUPLOADS_IDNAME, const wxString& caption = SYMBOL_WXUPLOADS_TITLE, const wxPoint& pos = SYMBOL_WXUPLOADS_POSITION, const wxSize& size = SYMBOL_WXUPLOADS_SIZE, long style = SYMBOL_WXUPLOADS_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

////@begin WxUploads event handler declarations

	/// wxEVT_CLOSE_WINDOW event handler for ID_UPLOADS
	void OnCloseWindow( wxCloseEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_ABORT_UPLOAD
	void OnMAbortUploadClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for IDD_UPLOADS
	void OnDUploadsItemActivated( wxListEvent& event );

	/// wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK event handler for IDD_UPLOADS
	void OnDUploadsRightClick( wxListEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_ABORTSEL
	void OnCAbortselClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARCOMP
	void OnCClearcompClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARCOMP2
	void OnCClearcomp2Click( wxCommandEvent& event );

	/// wxEVT_COMMAND_CHOICE_SELECTED event handler for IDC_AUTOCLEAR
	void OnCAutoclearSelected( wxCommandEvent& event );

////@end WxUploads event handler declarations

////@begin WxUploads member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end WxUploads member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin WxUploads member variables
	wxListCtrl* m_uploads;
	wxBitmapButton* m_abortsel;
	wxBitmapButton* m_clearcomp;
	wxBitmapButton* m_clearcomp2;
	wxChoice* m_autoclear;
////@end WxUploads member variables
};

#endif
	// _WXUPLOADS_H_
