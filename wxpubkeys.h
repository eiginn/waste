/*
WASTE - wxWidgets public-keys viewer dialog
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

#ifndef _WXPUBKEYS_H_
#define _WXPUBKEYS_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxpubkeys.cpp"
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
#define ID_PUBKEYVIEWER 10075
#define SYMBOL_WXPUBKEYVIEWER_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
#define SYMBOL_WXPUBKEYVIEWER_TITLE _("Public Keys")
#define SYMBOL_WXPUBKEYVIEWER_IDNAME ID_PUBKEYVIEWER
#define SYMBOL_WXPUBKEYVIEWER_SIZE wxSize(400, 450)
#define SYMBOL_WXPUBKEYVIEWER_POSITION wxDefaultPosition
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
* WxPubkeyViewer class declaration
*/

class WxPubkeyViewer: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( WxPubkeyViewer )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	WxPubkeyViewer( );
	WxPubkeyViewer( wxWindow* parent, wxWindowID id = SYMBOL_WXPUBKEYVIEWER_IDNAME, const wxString& caption = SYMBOL_WXPUBKEYVIEWER_TITLE, const wxPoint& pos = SYMBOL_WXPUBKEYVIEWER_POSITION, const wxSize& size = SYMBOL_WXPUBKEYVIEWER_SIZE, long style = SYMBOL_WXPUBKEYVIEWER_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXPUBKEYVIEWER_IDNAME, const wxString& caption = SYMBOL_WXPUBKEYVIEWER_TITLE, const wxPoint& pos = SYMBOL_WXPUBKEYVIEWER_POSITION, const wxSize& size = SYMBOL_WXPUBKEYVIEWER_SIZE, long style = SYMBOL_WXPUBKEYVIEWER_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

////@begin WxPubkeyViewer event handler declarations

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLOSE
	void OnCloseClick( wxCommandEvent& event );

////@end WxPubkeyViewer event handler declarations

////@begin WxPubkeyViewer member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end WxPubkeyViewer member function declarations

	void ShowFile (const char *path);

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin WxPubkeyViewer member variables
	wxTextCtrl* m_keys_text;
////@end WxPubkeyViewer member variables
};

#endif
	// _WXPUBKEYS_H_
