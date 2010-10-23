/*
WASTE - wxWidgets key-generation dialog
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

#ifndef _WXKGRAND_H_
#define _WXKGRAND_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxkgrand.cpp"
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
#define ID_KEYGEN_DLG 10081
#define SYMBOL_WXKEYGENRANDOMNESSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
#define SYMBOL_WXKEYGENRANDOMNESSDIALOG_TITLE _("WASTE Key Generator")
#define SYMBOL_WXKEYGENRANDOMNESSDIALOG_IDNAME ID_KEYGEN_DLG
#define SYMBOL_WXKEYGENRANDOMNESSDIALOG_SIZE wxSize(400, 500)
#define SYMBOL_WXKEYGENRANDOMNESSDIALOG_POSITION wxDefaultPosition
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
* WxKeyGenRandomnessDialog class declaration
*/

class WxKeyGenRandomnessDialog: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( WxKeyGenRandomnessDialog )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	WxKeyGenRandomnessDialog( );
	WxKeyGenRandomnessDialog( wxWindow* parent, wxWindowID id = SYMBOL_WXKEYGENRANDOMNESSDIALOG_IDNAME, const wxString& caption = SYMBOL_WXKEYGENRANDOMNESSDIALOG_TITLE, const wxPoint& pos = SYMBOL_WXKEYGENRANDOMNESSDIALOG_POSITION, const wxSize& size = SYMBOL_WXKEYGENRANDOMNESSDIALOG_SIZE, long style = SYMBOL_WXKEYGENRANDOMNESSDIALOG_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXKEYGENRANDOMNESSDIALOG_IDNAME, const wxString& caption = SYMBOL_WXKEYGENRANDOMNESSDIALOG_TITLE, const wxPoint& pos = SYMBOL_WXKEYGENRANDOMNESSDIALOG_POSITION, const wxSize& size = SYMBOL_WXKEYGENRANDOMNESSDIALOG_SIZE, long style = SYMBOL_WXKEYGENRANDOMNESSDIALOG_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

////@begin WxKeyGenRandomnessDialog event handler declarations

	/// wxEVT_CLOSE_WINDOW event handler for ID_KEYGEN_DLG
	void OnCloseWindow( wxCloseEvent& event );

	/// wxEVT_MOTION event handler for ID_KEYGEN_DLG
	void OnMotion( wxMouseEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CANCEL
	void OnCancelClick( wxCommandEvent& event );

////@end WxKeyGenRandomnessDialog event handler declarations

////@begin WxKeyGenRandomnessDialog member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end WxKeyGenRandomnessDialog member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin WxKeyGenRandomnessDialog member variables
	wxStaticText* m_dialog_text;
	wxGauge* m_kg_progress;
////@end WxKeyGenRandomnessDialog member variables

	bool m_cancel_pressed;
};

#endif
	// _WXKGRAND_H_
