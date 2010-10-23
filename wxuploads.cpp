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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wxuploads.h"
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

#include "wxuploads.h"
#include "wxappresources.h"
#include "main.hpp"

typedef wxWindow * HWND;

////@begin XPM images
////@end XPM images

/*!
* WxUploads type definition
*/

IMPLEMENT_DYNAMIC_CLASS( WxUploads, wxDialog )

/*!
* WxUploads event table definition
*/

BEGIN_EVENT_TABLE( WxUploads, wxDialog )

////@begin WxUploads event table entries
	EVT_CLOSE( WxUploads::OnCloseWindow )

	EVT_MENU( XRCID("IDM_ABORT_UPLOAD"), WxUploads::OnMAbortUploadClick )

	EVT_LIST_ITEM_ACTIVATED( XRCID("IDD_UPLOADS"), WxUploads::OnDUploadsItemActivated )
	EVT_LIST_ITEM_RIGHT_CLICK( XRCID("IDD_UPLOADS"), WxUploads::OnDUploadsRightClick )

	EVT_BUTTON( XRCID("IDC_ABORTSEL"), WxUploads::OnCAbortselClick )

	EVT_BUTTON( XRCID("IDC_CLEARCOMP"), WxUploads::OnCClearcompClick )

	EVT_BUTTON( XRCID("IDC_CLEARCOMP2"), WxUploads::OnCClearcomp2Click )

	EVT_CHOICE( XRCID("IDC_AUTOCLEAR"), WxUploads::OnCAutoclearSelected )

////@end WxUploads event table entries

END_EVENT_TABLE()

/*!
* WxUploads constructors
*/

WxUploads::WxUploads( )
{
}

WxUploads::WxUploads( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, caption, pos, size, style);
}

/*!
* WxUploads creator
*/

bool WxUploads::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WxUploads member initialisation
	m_uploads = NULL;
	m_abortsel = NULL;
	m_clearcomp = NULL;
	m_clearcomp2 = NULL;
	m_autoclear = NULL;
////@end WxUploads member initialisation

////@begin WxUploads creation
	SetParent(parent);
	CreateControls();
	SetIcon(GetIconResource(wxT("res/transfers.ico.gif")));
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Centre();
////@end WxUploads creation

	SetSize(-1, -1, 420, 250, wxSIZE_USE_EXISTING);

	return TRUE;
}

/*!
* Control creation for WxUploads
*/

void WxUploads::CreateControls()
{    
////@begin WxUploads content construction

	wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_UPLOADS"));
	m_uploads = XRCCTRL(*this, "IDD_UPLOADS", wxListCtrl);
	m_abortsel = XRCCTRL(*this, "IDC_ABORTSEL", wxBitmapButton);
	m_clearcomp = XRCCTRL(*this, "IDC_CLEARCOMP", wxBitmapButton);
	m_clearcomp2 = XRCCTRL(*this, "IDC_CLEARCOMP2", wxBitmapButton);
	m_autoclear = XRCCTRL(*this, "IDC_AUTOCLEAR", wxChoice);
////@end WxUploads content construction

	// Create custom windows not generated automatically here.

////@begin WxUploads content initialisation

////@end WxUploads content initialisation
}

/*!
* Should we show tooltips?
*/

bool WxUploads::ShowToolTips()
{
	return TRUE;
}

/*!
* Get bitmap resources
*/

wxBitmap WxUploads::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin WxUploads bitmap retrieval
	if (name == wxT("res/stopdl.ico.gif"))
	{
		wxBitmap bitmap(_T("res/stopdl.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/clear-inactive2.ico.gif"))
	{
		wxBitmap bitmap(_T("res/clear-inactive2.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/clear-completed2.ico.gif"))
	{
		wxBitmap bitmap(_T("res/clear-completed2.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	return wxNullBitmap;
////@end WxUploads bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon WxUploads::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin WxUploads icon retrieval
	if (name == wxT("res/transfers.ico.gif"))
	{
		wxIcon icon(_T("res/transfers.ico.gif"), wxBITMAP_TYPE_GIF);
		return icon;
	}
	return wxNullIcon;
////@end WxUploads icon retrieval
}

/*!
* wxEVT_COMMAND_CHOICE_SELECTED event handler for IDC_AUTOCLEAR
*/

void WxUploads::OnCAutoclearSelected( wxCommandEvent& event )
{
	extern int xferwnd_uploads_IDC_AUTOCLEAR(HWND hwndDlg);
	xferwnd_uploads_IDC_AUTOCLEAR(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for IDD_UPLOADS
*/

void WxUploads::OnDUploadsItemActivated( wxListEvent& event )
{
	event.Skip();
}

/*!
* wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK event handler for IDD_UPLOADS
*/

void WxUploads::OnDUploadsRightClick( wxListEvent& event )
{
	m_uploads->SetItemState(event.GetIndex(),
				wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

	void xferwnd_uploads_IDC_SENDLIST_NM_RCLICK(HWND, int , int );
	xferwnd_uploads_IDC_SENDLIST_NM_RCLICK(this, (int) event.GetPoint().x,
				(int) event.GetPoint().y);
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_ABORTSEL
*/

void WxUploads::OnCAbortselClick( wxCommandEvent& event )
{
	extern int xferwnd_uploads_IDC_ABORTSEL(HWND, bool);
	xferwnd_uploads_IDC_ABORTSEL(this, false);
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARCOMP
*/

void WxUploads::OnCClearcompClick( wxCommandEvent& event )
{
	extern int xferwnd_uploads_IDC_CLEARCOMP(HWND hwndDlg);
	xferwnd_uploads_IDC_CLEARCOMP(this);
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARCOMP2
*/

void WxUploads::OnCClearcomp2Click( wxCommandEvent& event )
{
	extern int xferwnd_uploads_IDC_CLEARCOMP2(HWND hwndDlg);
	xferwnd_uploads_IDC_CLEARCOMP2(this);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_ABORT_UPLOAD
*/

void WxUploads::OnMAbortUploadClick( wxCommandEvent& event )
{
	extern int xferwnd_uploads_IDC_ABORTSEL(HWND, bool);
	xferwnd_uploads_IDC_ABORTSEL(this, false);
	event.Skip();
}
/*!
* wxEVT_CLOSE_WINDOW event handler for ID_UPLOADS
*/

void WxUploads::OnCloseWindow( wxCloseEvent& event )
{
	g_config->WriteInt(CONFIG_xfers_vis,0);
	event.Skip();
}


