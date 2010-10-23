/*
WASTE - wxWidgets network status dialog
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
#pragma implementation "wxnetstat.h"
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

#include "wxnetstat.h"
#include "main.hpp"

////@begin XPM images
////@end XPM images

/*!
* WxNetStatDialog type definition
*/

IMPLEMENT_DYNAMIC_CLASS( WxNetStatDialog, wxDialog )

/*!
* WxNetStatDialog event table definition
*/

BEGIN_EVENT_TABLE( WxNetStatDialog, wxDialog )

////@begin WxNetStatDialog event table entries
	EVT_COMBOBOX( XRCID("IDC_NEWCONNECTION"), WxNetStatDialog::OnCNewconnectionSelected )
	EVT_TEXT( XRCID("IDC_NEWCONNECTION"), WxNetStatDialog::OnCNewconnectionUpdated )

	EVT_BUTTON( XRCID("IDC_ADDCONNECTION"), WxNetStatDialog::OnCAddconnectionClick )

	EVT_BUTTON( XRCID("IDC_CONNECTCATCH"), WxNetStatDialog::OnCConnectcatchClick )

	EVT_BUTTON( XRCID("IDC_REMOVECONNECTION"), WxNetStatDialog::OnCRemoveconnectionClick )

	EVT_BUTTON( XRCID("IDC_REMOVECATCH"), WxNetStatDialog::OnCRemovecatchClick )

	EVT_TEXT( XRCID("IDC_NUMCONUP"), WxNetStatDialog::OnCNumconupUpdated )

	EVT_LIST_ITEM_SELECTED( XRCID("ID_NS_HOST_LISTCTRL"), WxNetStatDialog::OnNsHostListctrlSelected )
	EVT_LIST_ITEM_DESELECTED( XRCID("ID_NS_HOST_LISTCTRL"), WxNetStatDialog::OnNsHostListctrlDeselected )
	EVT_LIST_ITEM_ACTIVATED( XRCID("ID_NS_HOST_LISTCTRL"), WxNetStatDialog::OnNsHostListctrlItemActivated )

////@end WxNetStatDialog event table entries

	EVT_TEXT_ENTER( XRCID("IDC_NEWCONNECTION"), WxNetStatDialog::OnCNewconnectionActivate )

END_EVENT_TABLE()

/*!
* WxNetStatDialog constructors
*/

WxNetStatDialog::WxNetStatDialog( )
{
}

WxNetStatDialog::WxNetStatDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, caption, pos, size, style);
}

/*!
* WxNetStatDialog creator
*/

bool WxNetStatDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WxNetStatDialog member initialisation
	m_newconnection = NULL;
	m_addconnection = NULL;
	m_connectcatch = NULL;
	m_removeconnection = NULL;
	m_removecatch = NULL;
	m_num_con_up = NULL;
	m_hostlist = NULL;
////@end WxNetStatDialog member initialisation

////@begin WxNetStatDialog creation
	SetParent(parent);
	CreateControls();
	SetIcon(GetIconResource(wxT("res/connect_icon.gif")));
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Centre();
////@end WxNetStatDialog creation
	return TRUE;
}

/*!
* Control creation for WxNetStatDialog
*/

void WxNetStatDialog::CreateControls()
{    
////@begin WxNetStatDialog content construction

	wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_NETSTAT"));
	m_newconnection = XRCCTRL(*this, "IDC_NEWCONNECTION", wxComboBox);
	m_addconnection = XRCCTRL(*this, "IDC_ADDCONNECTION", wxBitmapButton);
	m_connectcatch = XRCCTRL(*this, "IDC_CONNECTCATCH", wxBitmapButton);
	m_removeconnection = XRCCTRL(*this, "IDC_REMOVECONNECTION", wxBitmapButton);
	m_removecatch = XRCCTRL(*this, "IDC_REMOVECATCH", wxBitmapButton);
	m_num_con_up = XRCCTRL(*this, "IDC_NUMCONUP", wxTextCtrl);
	m_hostlist = XRCCTRL(*this, "ID_NS_HOST_LISTCTRL", wxListCtrl);
////@end WxNetStatDialog content construction

	// Create custom windows not generated automatically here.

////@begin WxNetStatDialog content initialisation

////@end WxNetStatDialog content initialisation
}

/*!
* Should we show tooltips?
*/

bool WxNetStatDialog::ShowToolTips()
{
	return TRUE;
}

/*!
* Get bitmap resources
*/

wxBitmap WxNetStatDialog::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin WxNetStatDialog bitmap retrieval
	if (name == wxT("res/connect.ico.gif"))
	{
		wxBitmap bitmap(_T("res/connect.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/stopdl.ico.gif"))
	{
		wxBitmap bitmap(_T("res/stopdl.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/disconnect.ico.gif"))
	{
		wxBitmap bitmap(_T("res/disconnect.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	return wxNullBitmap;
////@end WxNetStatDialog bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon WxNetStatDialog::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin WxNetStatDialog icon retrieval
	if (name == wxT("res/connect_icon.gif"))
	{
		wxIcon icon(_T("res/connect_icon.gif"), wxBITMAP_TYPE_GIF);
		return icon;
	}
	return wxNullIcon;
////@end WxNetStatDialog icon retrieval
}


/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_NUMCONUP
*/

void WxNetStatDialog::OnCNumconupUpdated( wxCommandEvent& event )
{
	extern int netkern_WM_COMMAND_IDC_NUMCONUP();
	netkern_WM_COMMAND_IDC_NUMCONUP();
	event.Skip();
}

void WxNetStatDialog::OnDestroy( )
{
	extern void netkern_WM_DESTROY();
	netkern_WM_DESTROY();
	g_config->WriteInt(CONFIG_net_vis,0);
}

/*!
* wxEVT_COMMAND_COMBOBOX_SELECTED event handler for IDC_NEWCONNECTION
*/

void WxNetStatDialog::OnCNewconnectionSelected( wxCommandEvent& event )
{
	extern int netkern_WM_COMMAND_IDC_NEWCONNECTION(int);
	netkern_WM_COMMAND_IDC_NEWCONNECTION(0);
	event.Skip();
}



/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_ADDCONNECTION
*/

void WxNetStatDialog::OnCAddconnectionClick( wxCommandEvent& event )
{
	extern int netkern_WM_COMMAND_IDC_ADDCONNECTION();
	netkern_WM_COMMAND_IDC_ADDCONNECTION();
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CONNECTCATCH
*/

void WxNetStatDialog::OnCConnectcatchClick( wxCommandEvent& event )
{
	extern int netkern_WM_COMMAND_IDC_CONNECTCATCH();
	netkern_WM_COMMAND_IDC_CONNECTCATCH();
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_REMOVECONNECTION
*/

void WxNetStatDialog::OnCRemoveconnectionClick( wxCommandEvent& event )
{
	extern int netkern_WM_COMMAND_IDC_REMOVECONNECTION();
	netkern_WM_COMMAND_IDC_REMOVECONNECTION();
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_REMOVECATCH
*/

void WxNetStatDialog::OnCRemovecatchClick( wxCommandEvent& event )
{
	extern int netkern_WM_COMMAND_IDC_REMOVECATCH();
	netkern_WM_COMMAND_IDC_REMOVECATCH();
	event.Skip();
}


/*!
* wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for ID_NS_HOST_LISTCTRL
*/

void WxNetStatDialog::OnNsHostListctrlItemActivated( wxListEvent& event )
{
	extern int netkern_WM_COMMAND_IDC_CONNECTCATCH();
	netkern_WM_COMMAND_IDC_CONNECTCATCH();
	event.Skip();
}



/*!
* wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_NS_HOST_LISTCTRL
*/

void WxNetStatDialog::OnNsHostListctrlSelected( wxListEvent& event )
{
	extern int netkern_WM_NOTIFY_IDC_NETCONS();
	netkern_WM_NOTIFY_IDC_NETCONS();
	event.Skip();
}

/*!
* wxEVT_COMMAND_LIST_ITEM_DESELECTED event handler for ID_NS_HOST_LISTCTRL
*/

void WxNetStatDialog::OnNsHostListctrlDeselected( wxListEvent& event )
{
	extern int netkern_WM_NOTIFY_IDC_NETCONS();
	netkern_WM_NOTIFY_IDC_NETCONS();
	event.Skip();
}


/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_NEWCONNECTION
*/

void WxNetStatDialog::OnCNewconnectionUpdated( wxCommandEvent& event )
{
	extern int netkern_WM_COMMAND_IDC_NEWCONNECTION(int);
	netkern_WM_COMMAND_IDC_NEWCONNECTION(0);
	event.Skip();
}

void WxNetStatDialog::OnCNewconnectionActivate( wxCommandEvent& event )
{
	extern int netkern_WM_COMMAND_IDC_ADDCONNECTION();
	netkern_WM_COMMAND_IDC_ADDCONNECTION();
	event.Skip();
}



