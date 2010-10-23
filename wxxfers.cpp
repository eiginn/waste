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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wxxfers.h"
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

#include "wxxfers.h"
#include "wxappresources.h"
#include "main.hpp"

typedef wxWindow * HWND;

////@begin XPM images
////@end XPM images

/*!
* WxXfers type definition
*/

IMPLEMENT_DYNAMIC_CLASS( WxXfers, wxDialog )

/*!
* WxXfers event table definition
*/

BEGIN_EVENT_TABLE( WxXfers, wxDialog )

////@begin WxXfers event table entries
	EVT_CLOSE( WxXfers::OnCloseWindow )

	EVT_MENU( XRCID("IDM_CLEANUP_DLS"), WxXfers::OnMCleanupDlsClick )

	EVT_MENU( XRCID("ID_CLEANUP_SEL"), WxXfers::OnCleanupSelClick )

	EVT_MENU( XRCID("ID_ABORT_DL"), WxXfers::OnAbortDlClick )

	EVT_MENU( XRCID("ID_RESUME_DL"), WxXfers::OnResumeDlClick )

	EVT_MENU( XRCID("IDM_DLNOW"), WxXfers::OnMDlnowClick )

	EVT_MENU( XRCID("IDM_TO_TOP"), WxXfers::OnMToTopClick )

	EVT_MENU( XRCID("IDM_TO_BOT"), WxXfers::OnMToBotClick )

	EVT_MENU( XRCID("IDM_REMOVE"), WxXfers::OnMRemoveClick )

	EVT_LIST_ITEM_ACTIVATED( XRCID("IDD_XFERS"), WxXfers::OnDXfersItemActivated )
	EVT_LIST_ITEM_RIGHT_CLICK( XRCID("IDD_XFERS"), WxXfers::OnDXfersRightClick )

	EVT_CHOICE( XRCID("IDC_AUTOCLEAR"), WxXfers::OnCAutoclearSelected )

	EVT_CHOICE( XRCID("IDC_AUTORESUME"), WxXfers::OnCAutoresumeSelected )

	EVT_BUTTON( XRCID("IDC_RESUME"), WxXfers::OnCResumeClick )

	EVT_BUTTON( XRCID("IDC_ABORTSEL"), WxXfers::OnCAbortselClick )

	EVT_BUTTON( XRCID("IDC_CLEARCOMP"), WxXfers::OnCClearcompClick )

	EVT_BUTTON( XRCID("IDC_CLEARCOMP3"), WxXfers::OnCClearcomp3Click )

	EVT_BUTTON( XRCID("IDC_CLEARALL"), WxXfers::OnCClearallClick )

	EVT_TEXT( XRCID("IDC_MAXSIMULDL"), WxXfers::OnCMaxsimuldlUpdated )
	EVT_TEXT_ENTER( XRCID("IDC_MAXSIMULDL"), WxXfers::OnCMaxsimuldlEnter )

	EVT_LIST_ITEM_ACTIVATED( XRCID("ID_RECVLISTQ"), WxXfers::OnRecvlistqItemActivated )
	EVT_LIST_ITEM_RIGHT_CLICK( XRCID("ID_RECVLISTQ"), WxXfers::OnRecvlistqRightClick )

	EVT_BUTTON( XRCID("IDC_DLNOW"), WxXfers::OnCDlnowClick )

	EVT_BUTTON( XRCID("IDC_MOVETOTOP"), WxXfers::OnCMovetotopClick )

	EVT_BUTTON( XRCID("IDC_MOVETOBOTTOM"), WxXfers::OnCMovetobottomClick )

	EVT_BUTTON( XRCID("IDC_REMOVEQUEUE"), WxXfers::OnCRemovequeueClick )

	EVT_BUTTON( XRCID("IDC_CLEARQUEUE"), WxXfers::OnCClearqueueClick )

////@end WxXfers event table entries

END_EVENT_TABLE()

/*!
* WxXfers constructors
*/

WxXfers::WxXfers( )
{
}

WxXfers::WxXfers( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, caption, pos, size, style);
}

/*!
* WxXfers creator
*/

bool WxXfers::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WxXfers member initialisation
	m_notebook = NULL;
	m_dl_panel = NULL;
	m_xfers = NULL;
	m_autoclear = NULL;
	m_autoresume = NULL;
	m_resume = NULL;
	m_abortsel = NULL;
	m_clearcomp = NULL;
	m_clearcomp3 = NULL;
	m_clearall = NULL;
	m_maxsimuldl = NULL;
	m_dlqueue_panel = NULL;
	m_recvlistq = NULL;
	m_dlnow = NULL;
	m_movetotop = NULL;
	m_movetobottom = NULL;
	m_removequeue = NULL;
	m_clearqueue = NULL;
	m_queueitems = NULL;
////@end WxXfers member initialisation

////@begin WxXfers creation
	SetParent(parent);
	CreateControls();
	SetIcon(GetIconResource(wxT("res/transfers.ico.gif")));
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Centre();
////@end WxXfers creation

	SetSize(-1, -1, 420, 250, wxSIZE_USE_EXISTING);

	return TRUE;
}

/*!
* Control creation for WxXfers
*/

void WxXfers::CreateControls()
{    
////@begin WxXfers content construction

	wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_XFERS"));
	m_notebook = XRCCTRL(*this, "ID_NOTEBOOK", wxNotebook);
	m_dl_panel = XRCCTRL(*this, "ID_DL_PANEL", wxPanel);
	m_xfers = XRCCTRL(*this, "IDD_XFERS", wxListCtrl);
	m_autoclear = XRCCTRL(*this, "IDC_AUTOCLEAR", wxChoice);
	m_autoresume = XRCCTRL(*this, "IDC_AUTORESUME", wxChoice);
	m_resume = XRCCTRL(*this, "IDC_RESUME", wxBitmapButton);
	m_abortsel = XRCCTRL(*this, "IDC_ABORTSEL", wxBitmapButton);
	m_clearcomp = XRCCTRL(*this, "IDC_CLEARCOMP", wxBitmapButton);
	m_clearcomp3 = XRCCTRL(*this, "IDC_CLEARCOMP3", wxBitmapButton);
	m_clearall = XRCCTRL(*this, "IDC_CLEARALL", wxBitmapButton);
	m_maxsimuldl = XRCCTRL(*this, "IDC_MAXSIMULDL", wxTextCtrl);
	m_dlqueue_panel = XRCCTRL(*this, "ID_DLQUEUE_PANEL", wxPanel);
	m_recvlistq = XRCCTRL(*this, "ID_RECVLISTQ", wxListCtrl);
	m_dlnow = XRCCTRL(*this, "IDC_DLNOW", wxBitmapButton);
	m_movetotop = XRCCTRL(*this, "IDC_MOVETOTOP", wxBitmapButton);
	m_movetobottom = XRCCTRL(*this, "IDC_MOVETOBOTTOM", wxBitmapButton);
	m_removequeue = XRCCTRL(*this, "IDC_REMOVEQUEUE", wxBitmapButton);
	m_clearqueue = XRCCTRL(*this, "IDC_CLEARQUEUE", wxBitmapButton);
	m_queueitems = XRCCTRL(*this, "IDC_QUEUEITEMS", wxStaticText);
////@end WxXfers content construction

	// Create custom windows not generated automatically here.

////@begin WxXfers content initialisation

////@end WxXfers content initialisation
}

/*!
* Should we show tooltips?
*/

bool WxXfers::ShowToolTips()
{
	return TRUE;
}

/*!
* Get bitmap resources
*/

wxBitmap WxXfers::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin WxXfers bitmap retrieval
	if (name == wxT("res/dlnow.ico.gif"))
	{
		wxBitmap bitmap(_T("res/dlnow.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/stopdl.ico.gif"))
	{
		wxBitmap bitmap(_T("res/stopdl.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/clear-completed2.ico.gif"))
	{
		wxBitmap bitmap(_T("res/clear-completed2.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/clear-inactive2.ico.gif"))
	{
		wxBitmap bitmap(_T("res/clear-inactive2.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/clear-all2.ico.gif"))
	{
		wxBitmap bitmap(_T("res/clear-all2.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/move2top.ico.gif"))
	{
		wxBitmap bitmap(_T("res/move2top.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/move2end.ico.gif"))
	{
		wxBitmap bitmap(_T("res/move2end.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	return wxNullBitmap;
////@end WxXfers bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon WxXfers::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin WxXfers icon retrieval
	if (name == wxT("res/transfers.ico.gif"))
	{
		wxIcon icon(_T("res/transfers.ico.gif"), wxBITMAP_TYPE_GIF);
		return icon;
	}
	return wxNullIcon;
////@end WxXfers icon retrieval
}


/*!
* wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for IDD_XFERS
*/

void WxXfers::OnDXfersItemActivated( wxListEvent& event )
{
	extern int xferwnd_xfers_RECVLIST_DBLCLK(HWND hwndDlg);
	xferwnd_xfers_RECVLIST_DBLCLK(this);
	event.Skip();
}

/*!
* wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK event handler for IDD_XFERS
*/

void WxXfers::OnDXfersRightClick( wxListEvent& event )
{
	m_xfers->SetItemState(event.GetIndex(),
				wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

	wxMenu *menu = AppResources::CreateDownloadPopupMenu();

#if (wxMAJOR_VERSION >= 2 && wxMINOR_VERSION >= 5)
	PopupMenu(menu);    // no loc = current mouse pos
#else
	PopupMenu(menu, (int) event.GetPoint().x, (int) event.GetPoint().y);
#endif
}

/*!
* wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for ID_RECVLISTQ
*/

void WxXfers::OnRecvlistqItemActivated( wxListEvent& event )
{
	m_recvlistq->SetItemState(event.GetIndex(),
				wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	extern int xferwnd_xfers_IDC_DLNOW(HWND hwndDlg);
	xferwnd_xfers_IDC_DLNOW(this);
	event.Skip();
}

/*!
* wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK event handler for ID_RECVLISTQ
*/

void WxXfers::OnRecvlistqRightClick( wxListEvent& event )
{
	m_recvlistq->SetItemState(event.GetIndex(),
				wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

	wxMenu *menu = AppResources::CreateDownloadQueuePopupMenu();

#if (wxMAJOR_VERSION >= 2 && wxMINOR_VERSION >= 5)
	PopupMenu(menu);    // no loc = current mouse pos
#else
	PopupMenu(menu, (int) event.GetPoint().x, (int) event.GetPoint().y);
#endif
}

void WxXfers::OnDestroy( )
{
	extern void xferwnd_WM_DESTROY(HWND hwndDlg);
	xferwnd_WM_DESTROY(this);
}


/*!
* wxEVT_COMMAND_CHOICE_SELECTED event handler for IDC_AUTOCLEAR
*/

void WxXfers::OnCAutoclearSelected( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_AUTOCLEAR(HWND hwndDlg);
	xferwnd_xfers_IDC_AUTOCLEAR(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_CHOICE_SELECTED event handler for IDC_AUTORESUME
*/

void WxXfers::OnCAutoresumeSelected( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_AUTORESUME(HWND hwndDlg);
	xferwnd_xfers_IDC_AUTORESUME(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_RESUME
*/

void WxXfers::OnCResumeClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_RESUME(HWND hwndDlg);
	xferwnd_xfers_IDC_RESUME(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_ABORTSEL
*/

void WxXfers::OnCAbortselClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_ABORTSEL(HWND, bool);
	xferwnd_xfers_IDC_ABORTSEL(this, false);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARCOMP
*/

void WxXfers::OnCClearcomp3Click( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_CLEARCOMP(HWND, bool);
	xferwnd_xfers_IDC_CLEARCOMP(this, true);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARALL
*/

void WxXfers::OnCClearallClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_CLEARCOMP(HWND, bool);
	xferwnd_xfers_IDC_CLEARCOMP(this, true);
	event.Skip();
}


/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_MAXSIMULDL
*/

void WxXfers::OnCMaxsimuldlUpdated( wxCommandEvent& event )
{
	extern int xfer_IDC_MAXSIMULDL(HWND hwndDlg);
	xfer_IDC_MAXSIMULDL(this);
	event.Skip();
}

/*!
* wxEVT_COMMAND_TEXT_ENTER event handler for IDC_MAXSIMULDL
*/

void WxXfers::OnCMaxsimuldlEnter( wxCommandEvent& event )
{
	extern int xfer_IDC_MAXSIMULDL(HWND hwndDlg);
	xfer_IDC_MAXSIMULDL(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_DLNOW
*/

void WxXfers::OnCDlnowClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_DLNOW(HWND hwndDlg);
	xferwnd_xfers_IDC_DLNOW(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_MOVETOTOP
*/

void WxXfers::OnCMovetotopClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_MOVETOTOP(HWND hwndDlg);
	xferwnd_xfers_IDC_MOVETOTOP(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_MOVETOBOTTOM
*/

void WxXfers::OnCMovetobottomClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_MOVETOBOTTOM(HWND hwndDlg);
	xferwnd_xfers_IDC_MOVETOBOTTOM(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_REMOVEQUEUE
*/

void WxXfers::OnCRemovequeueClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_REMOVEQUEUE(HWND, bool);
	xferwnd_xfers_IDC_REMOVEQUEUE(this, false);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARQUEUE
*/

void WxXfers::OnCClearqueueClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_CLEARQUEUE(HWND hwndDlg);
	xferwnd_xfers_IDC_CLEARQUEUE(this);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_CLEARCOMP
*/

void WxXfers::OnCClearcompClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_CLEARCOMP(HWND, bool);
	xferwnd_xfers_IDC_CLEARCOMP(this, false);
	event.Skip();
}


/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_CLEANUP_DLS
*/

void WxXfers::OnMCleanupDlsClick( wxCommandEvent& event )
{
	extern void CleanupDLFolderEnter(HWND);
	CleanupDLFolderEnter(this);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_CLEANUP_SEL
*/

void WxXfers::OnCleanupSelClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_WM_USER_CLEANUPSELECTED(HWND);
	xferwnd_xfers_WM_USER_CLEANUPSELECTED(this);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_ABORT_DL
*/

void WxXfers::OnAbortDlClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_ABORTSEL(HWND, bool);
	xferwnd_xfers_IDC_ABORTSEL(this, false);
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_RESUME_DL
*/

void WxXfers::OnResumeDlClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_RESUME(HWND hwndDlg);
	xferwnd_xfers_IDC_RESUME(this);
}


/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_DLNOW
*/

void WxXfers::OnMDlnowClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_DLNOW(HWND hwndDlg);
	xferwnd_xfers_IDC_DLNOW(this);
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_TO_TOP
*/

void WxXfers::OnMToTopClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_MOVETOTOP(HWND hwndDlg);
	xferwnd_xfers_IDC_MOVETOTOP(this);
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_TO_BOT
*/

void WxXfers::OnMToBotClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_MOVETOBOTTOM(HWND hwndDlg);
	xferwnd_xfers_IDC_MOVETOBOTTOM(this);
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_REMOVE
*/

void WxXfers::OnMRemoveClick( wxCommandEvent& event )
{
	extern int xferwnd_xfers_IDC_REMOVEQUEUE(HWND, bool);
	xferwnd_xfers_IDC_REMOVEQUEUE(this, false);
}


/*!
* wxEVT_CLOSE_WINDOW event handler for ID_XFERS
*/

void WxXfers::OnCloseWindow( wxCloseEvent& event )
{
	g_config->WriteInt(CONFIG_xfers_vis,0);
	event.Skip();
}


