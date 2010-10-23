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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wxkgrand.h"
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

#include "wxkgrand.h"

////@begin XPM images
////@end XPM images

/*!
* WxKeyGenRandomnessDialog type definition
*/

IMPLEMENT_DYNAMIC_CLASS( WxKeyGenRandomnessDialog, wxDialog )

/*!
* WxKeyGenRandomnessDialog event table definition
*/

BEGIN_EVENT_TABLE( WxKeyGenRandomnessDialog, wxDialog )

////@begin WxKeyGenRandomnessDialog event table entries
	EVT_CLOSE( WxKeyGenRandomnessDialog::OnCloseWindow )
	EVT_MOTION( WxKeyGenRandomnessDialog::OnMotion )

	EVT_BUTTON( XRCID("ID_CANCEL"), WxKeyGenRandomnessDialog::OnCancelClick )

////@end WxKeyGenRandomnessDialog event table entries

END_EVENT_TABLE()

/*!
* WxKeyGenRandomnessDialog constructors
*/

WxKeyGenRandomnessDialog::WxKeyGenRandomnessDialog( )
{
}

WxKeyGenRandomnessDialog::WxKeyGenRandomnessDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, caption, pos, size, style);
}

/*!
* WxKeyGenRandomnessDialog creator
*/

bool WxKeyGenRandomnessDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WxKeyGenRandomnessDialog member initialisation
	m_dialog_text = NULL;
	m_kg_progress = NULL;
////@end WxKeyGenRandomnessDialog member initialisation

////@begin WxKeyGenRandomnessDialog creation
	SetParent(parent);
	CreateControls();
	SetIcon(GetIconResource(wxT("res/icon1.ico.gif")));
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Centre();
////@end WxKeyGenRandomnessDialog creation

	m_cancel_pressed = false;
	return TRUE;
}

/*!
* Control creation for WxKeyGenRandomnessDialog
*/

void WxKeyGenRandomnessDialog::CreateControls()
{    
////@begin WxKeyGenRandomnessDialog content construction

	wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_KEYGEN_DLG"));
	m_dialog_text = XRCCTRL(*this, "wxID_STATIC", wxStaticText);
	m_kg_progress = XRCCTRL(*this, "ID_KG_PROGRESS", wxGauge);
////@end WxKeyGenRandomnessDialog content construction

	// Create custom windows not generated automatically here.

////@begin WxKeyGenRandomnessDialog content initialisation

////@end WxKeyGenRandomnessDialog content initialisation
}

/*!
* Should we show tooltips?
*/

bool WxKeyGenRandomnessDialog::ShowToolTips()
{
	return TRUE;
}

/*!
* Get bitmap resources
*/

wxBitmap WxKeyGenRandomnessDialog::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin WxKeyGenRandomnessDialog bitmap retrieval
	return wxNullBitmap;
////@end WxKeyGenRandomnessDialog bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon WxKeyGenRandomnessDialog::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin WxKeyGenRandomnessDialog icon retrieval
	if (name == wxT("res/icon1.ico.gif"))
	{
		wxIcon icon(_T("res/icon1.ico.gif"), wxBITMAP_TYPE_GIF);
		return icon;
	}
	return wxNullIcon;
////@end WxKeyGenRandomnessDialog icon retrieval
}
/*!
* wxEVT_MOTION event handler for ID_DIALOG
*/

void WxKeyGenRandomnessDialog::OnMotion( wxMouseEvent& event )
{
////@begin wxEVT_MOTION event handler for ID_DIALOG in WxKeyGenRandomnessDialog.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_MOTION event handler for ID_DIALOG in WxKeyGenRandomnessDialog. 
}


/*!
* wxEVT_CLOSE_WINDOW event handler for ID_DIALOG
*/

void WxKeyGenRandomnessDialog::OnCloseWindow( wxCloseEvent& event )
{
	m_cancel_pressed = true;
	this->EndModal(wxID_CANCEL);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CANCEL
*/

void WxKeyGenRandomnessDialog::OnCancelClick( wxCommandEvent& event )
{
	m_cancel_pressed = true;
	this->EndModal(wxID_CANCEL);
	event.Skip();
}

