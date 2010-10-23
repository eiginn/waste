/*
WASTE - wxWidgets browser / search dialog
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
#pragma implementation "wxbrowser.h"
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

#include "wxbrowser.h"
#include "main.hpp"

////@begin XPM images
#include "res/file.xpm"
#include "res/dir.xpm"
#include "res/user.xpm"
////@end XPM images

/*!
* WxBrowser type definition
*/

IMPLEMENT_DYNAMIC_CLASS( WxBrowser, wxDialog )

/*!
* WxBrowser event table definition
*/

BEGIN_EVENT_TABLE( WxBrowser, wxDialog )

////@begin WxBrowser event table entries
	EVT_MENU( XRCID("ID_DOWNLOAD"), WxBrowser::OnDownloadClick )

	EVT_MENU( XRCID("ID_DLPARENT"), WxBrowser::OnDlparentClick )

	EVT_MENU( XRCID("ID_BROWSELOC"), WxBrowser::OnBrowselocClick )

	EVT_MENU( XRCID("ID_COPYLOC"), WxBrowser::OnCopylocClick )

	EVT_MENU( XRCID("ID_BROWSENODE"), WxBrowser::OnBrowsenodeClick )

	EVT_MENU( XRCID("ID_PRIVMSGNODE"), WxBrowser::OnPrivmsgnodeClick )

	EVT_MENU( XRCID("IDM_SEARCHRES_USER_WHOIS"), WxBrowser::OnMSearchresUserWhoisClick )

	EVT_MENU( XRCID("ID_SENDFILENODE"), WxBrowser::OnSendfilenodeClick )

	EVT_MENU( XRCID("ID_DOWNLOADFOLDER"), WxBrowser::OnDownloadfolderClick )

	EVT_MENU( XRCID("ID_BROWSEFOLDER"), WxBrowser::OnBrowsefolderClick )

	EVT_BUTTON( XRCID("ID_SRCH_USERLIST"), WxBrowser::OnSrchUserlistClick )

	EVT_BUTTON( XRCID("IDC_GOPARENT"), WxBrowser::OnCGoparentClick )

	EVT_BUTTON( XRCID("IDC_BACK"), WxBrowser::OnCBackClick )

	EVT_BUTTON( XRCID("IDC_BACK2"), WxBrowser::OnCBack2Click )

	EVT_BUTTON( XRCID("IDC_FORWARD"), WxBrowser::OnCForwardClick )

	EVT_BUTTON( XRCID("IDC_FORWARD2"), WxBrowser::OnCForward2Click )

	EVT_COMBOBOX( XRCID("IDC_SEARCHTEXT"), WxBrowser::OnCSearchtextSelected )
	EVT_TEXT( XRCID("IDC_SEARCHTEXT"), WxBrowser::OnCSearchtextUpdated )

	EVT_BUTTON( XRCID("IDC_SEARCH"), WxBrowser::OnCSearchClick )

	EVT_TEXT( XRCID("IDC_EDIT_FILTER"), WxBrowser::OnCEditFilterUpdated )
	EVT_TEXT_ENTER( XRCID("IDC_EDIT_FILTER"), WxBrowser::OnCEditFilterEnter )

////@end WxBrowser event table entries

	EVT_TEXT_ENTER( XRCID("IDC_SEARCHTEXT"), WxBrowser::OnCSearchtextActivate )

END_EVENT_TABLE()

/*!
* WxBrowser constructors
*/

WxBrowser::WxBrowser( )
{
}

WxBrowser::WxBrowser( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, caption, pos, size, style);
}

WxBrowser::~WxBrowser( )
{
	delete m_imagelist;
}

/*!
* WxBrowser creator
*/

bool WxBrowser::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WxBrowser member initialisation
	m_srch_userlist = NULL;
	m_goparent = NULL;
	m_back = NULL;
	m_back2 = NULL;
	m_forward = NULL;
	m_forward2 = NULL;
	m_searchtext = NULL;
	m_search = NULL;
	m_searchres_panel = NULL;
	m_edit_filter = NULL;
	m_searchstats = NULL;
////@end WxBrowser member initialisation

////@begin WxBrowser creation
	SetParent(parent);
	CreateControls();
	SetIcon(GetIconResource(wxT("res/browse.ico.gif")));
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Centre();
////@end WxBrowser creation

#define ID_RESLISTCTRL          13042

	m_ressizer = new wxBoxSizer(wxVERTICAL);

	// create our ListCtrl
	m_searchres = new WxWasteSearchResultsListCtrl((wxWindow*) m_searchres_panel,
			(wxWindowID) ID_RESLISTCTRL, wxDefaultPosition, wxDefaultSize,
			wxLC_REPORT | wxLC_VIRTUAL | wxSUNKEN_BORDER);

	m_searchres->m_browser = this;

	m_ressizer->Add(m_searchres, 1, wxEXPAND | wxALL);
	m_searchres_panel->SetSizer(m_ressizer);
	m_ressizer->SetSizeHints(m_searchres_panel);

	{
		wxImageList *hImageList = new wxImageList(16, 16, true);

		// I implemented these with XPMs instead of GIFs etc. since according
		// to the wxWidgets documentation, XPMs are more portable! go figure.
		hImageList->Add(wxIcon(file_xpm));
		hImageList->Add(wxIcon(dir_xpm));
		hImageList->Add(wxIcon(user_xpm));
		m_imagelist = hImageList;
		m_searchres->SetImageList(hImageList, wxIMAGE_LIST_SMALL);
	}

	return TRUE;
}

/*!
* Control creation for WxBrowser
*/

void WxBrowser::CreateControls()
{    
////@begin WxBrowser content construction

	wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_BROWSER"));
	m_srch_userlist = XRCCTRL(*this, "ID_SRCH_USERLIST", wxBitmapButton);
	m_goparent = XRCCTRL(*this, "IDC_GOPARENT", wxBitmapButton);
	m_back = XRCCTRL(*this, "IDC_BACK", wxBitmapButton);
	m_back2 = XRCCTRL(*this, "IDC_BACK2", wxBitmapButton);
	m_forward = XRCCTRL(*this, "IDC_FORWARD", wxBitmapButton);
	m_forward2 = XRCCTRL(*this, "IDC_FORWARD2", wxBitmapButton);
	m_searchtext = XRCCTRL(*this, "IDC_SEARCHTEXT", wxComboBox);
	m_search = XRCCTRL(*this, "IDC_SEARCH", wxBitmapButton);
	m_searchres_panel = XRCCTRL(*this, "IDC_SEARCHRES_PANEL", wxPanel);
	m_edit_filter = XRCCTRL(*this, "IDC_EDIT_FILTER", wxTextCtrl);
	m_searchstats = XRCCTRL(*this, "ID_SEARCH_DATA", wxStaticText);
////@end WxBrowser content construction

	// Create custom windows not generated automatically here.

////@begin WxBrowser content initialisation

////@end WxBrowser content initialisation
}

/*!
* Should we show tooltips?
*/

bool WxBrowser::ShowToolTips()
{
	return TRUE;
}

/*!
* Get bitmap resources
*/

wxBitmap WxBrowser::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin WxBrowser bitmap retrieval
	if (name == wxT("res/allusers.ico.gif"))
	{
		wxBitmap bitmap(_T("res/allusers.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/updir.ico.gif"))
	{
		wxBitmap bitmap(_T("res/updir.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/backward.ico.gif"))
	{
		wxBitmap bitmap(_T("res/backward.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/more2.ico.gif"))
	{
		wxBitmap bitmap(_T("res/more2.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/forward.ico.gif"))
	{
		wxBitmap bitmap(_T("res/forward.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	else if (name == wxT("res/search.ico.gif"))
	{
		wxBitmap bitmap(_T("res/search.ico.gif"), wxBITMAP_TYPE_GIF);
		return bitmap;
	}
	return wxNullBitmap;
////@end WxBrowser bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon WxBrowser::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin WxBrowser icon retrieval
	if (name == wxT("res/browse.ico.gif"))
	{
		wxIcon icon(_T("res/browse.ico.gif"), wxBITMAP_TYPE_GIF);
		return icon;
	}
	return wxNullIcon;
////@end WxBrowser icon retrieval
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SRCH_USERLIST
*/

void WxBrowser::OnSrchUserlistClick( wxCommandEvent& event )
{
	extern void Search_Search(char *);
	Search_Search("/");
	event.Skip();
}

void WxBrowser::OnDestroy( )
{
	extern void srchwnd_WM_DESTROY();
	srchwnd_WM_DESTROY();
	g_config->WriteInt(CONFIG_search_vis,0);
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_GOPARENT
*/

void WxBrowser::OnCGoparentClick( wxCommandEvent& event )
{
	extern int srch_WM_COMMAND_IDC_GOPARENT();
	srch_WM_COMMAND_IDC_GOPARENT();
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_BACK
*/

void WxBrowser::OnCBackClick( wxCommandEvent& event )
{
	extern int srch_WM_COMMAND_IDC_BACK(int);
	srch_WM_COMMAND_IDC_BACK(0);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_BACK2
*/

void WxBrowser::OnCBack2Click( wxCommandEvent& event )
{
	extern int srch_WM_COMMAND_IDC_BACK(int);
	srch_WM_COMMAND_IDC_BACK(1);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_FORWARD
*/

void WxBrowser::OnCForwardClick( wxCommandEvent& event )
{
	extern int srch_WM_COMMAND_IDC_FORWARD(int);
	srch_WM_COMMAND_IDC_FORWARD(0);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_FORWARD2
*/

void WxBrowser::OnCForward2Click( wxCommandEvent& event )
{
	extern int srch_WM_COMMAND_IDC_FORWARD(int);
	srch_WM_COMMAND_IDC_FORWARD(1);
	event.Skip();
}


/*!
* wxEVT_COMMAND_COMBOBOX_SELECTED event handler for IDC_SEARCHTEXT
*/

void WxBrowser::OnCSearchtextSelected( wxCommandEvent& event )
{
	extern int srch_SEARCHTEXT_SELCHANGE();
	srch_SEARCHTEXT_SELCHANGE();
	event.Skip();
}



/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_SEARCH
*/

void WxBrowser::OnCSearchClick( wxCommandEvent& event )
{
	extern int srch_WM_COMMAND_IDC_SEARCH();
	srch_WM_COMMAND_IDC_SEARCH();
	event.Skip();
}


/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_EDIT_FILTER
*/

void WxBrowser::OnCEditFilterUpdated( wxCommandEvent& event )
{
	extern void Search_Resort();
	Search_Resort();
	event.Skip();
}

/*!
* wxEVT_COMMAND_TEXT_ENTER event handler for IDC_EDIT_FILTER
*/

void WxBrowser::OnCEditFilterEnter( wxCommandEvent& event )
{
	extern void Search_Resort();
	Search_Resort();
	event.Skip();
}


/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_SEARCHTEXT
*/

void WxBrowser::OnCSearchtextUpdated( wxCommandEvent& event )
{
	extern int srch_SEARCHTEXT_SELCHANGE();
	srch_SEARCHTEXT_SELCHANGE();
	event.Skip();
}


BEGIN_EVENT_TABLE(WxWasteSearchResultsListCtrl, wxListCtrl)
	EVT_LIST_ITEM_ACTIVATED(ID_RESLISTCTRL, WxWasteSearchResultsListCtrl::OnActivated)
	EVT_LIST_COL_CLICK(ID_RESLISTCTRL, WxWasteSearchResultsListCtrl::OnColClick)
	EVT_RIGHT_DOWN(WxWasteSearchResultsListCtrl::OnRightDown)
	EVT_MOUSEWHEEL(WxWasteSearchResultsListCtrl::OnMousewheel)
END_EVENT_TABLE()


wxString WxWasteSearchResultsListCtrl::OnGetItemText(long item, long column) const
{
	extern wxString SrchWnd_GetItemText(long, long);
	return SrchWnd_GetItemText(item, column);
}

int WxWasteSearchResultsListCtrl::OnGetItemImage(long item) const
{
	extern int SrchWnd_GetItemImage(long);
	int ret = SrchWnd_GetItemImage(item);
	return ret;
}

wxListItemAttr* WxWasteSearchResultsListCtrl::OnGetItemAttr(long item) const
{
	return NULL;
}

void WxWasteSearchResultsListCtrl::OnActivated(wxListEvent& event)
{
	extern void SrchWnd_Item_Run(int);
	SrchWnd_Item_Run(1);
	event.Skip();
}

void WxWasteSearchResultsListCtrl::OnColClick(wxListEvent& event)
{
	extern void SrchWnd_LVN_COLUMNCLICK(int col);
	int col = event.GetColumn();
	SrchWnd_LVN_COLUMNCLICK(col);
}

void WxBrowser::OnCSearchtextActivate( wxCommandEvent& event )
{
	extern int srch_WM_COMMAND_IDC_SEARCH();
	srch_WM_COMMAND_IDC_SEARCH();
	event.Skip();
}


void WxWasteSearchResultsListCtrl::OnRightDown (wxMouseEvent& event)
{
	wxPoint pt = event.GetPosition();

	int hitflags = wxLIST_HITTEST_ONITEM;
	long item_hit = HitTest(pt, hitflags);

	if (hitflags) {
		long cursel = -1;
		bool in_cursel = false;

		while (1) {
		cursel = GetNextItem(cursel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		if (cursel == -1) {
			break;
		}

		if (cursel == item_hit) {
			in_cursel = true;
		}
		}

		// the item wasn't in the current selection.  clear that out
		// and replace it
		if (!in_cursel) {
			while (1) {
				cursel = GetNextItem(cursel, wxLIST_NEXT_ALL,
											wxLIST_STATE_SELECTED);
				if (cursel == -1) {
					break;
				}
				SetItemState(cursel, 0L, wxLIST_STATE_SELECTED);
			}

			// set the new item as selected
			SetItemState(item_hit, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED,
								wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
		}
	}

	extern void srchwnd_RCLICK(int, int);
	srchwnd_RCLICK(event.GetX(), event.GetY());
	event.Skip();
}

void WxWasteSearchResultsListCtrl::OnMousewheel (wxMouseEvent& event )
{
	event.Skip();
}


/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for IDM_SEARCHRES_USER_WHOIS
*/

void WxBrowser::OnMSearchresUserWhoisClick( wxCommandEvent& event )
{
	extern void srchwnd_ID_WHOISUSER();
	srchwnd_ID_WHOISUSER();
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_COPYLOC
*/

void WxBrowser::OnCopylocClick( wxCommandEvent& event )
{
	extern void srchwnd_ID_BROWSECOPYLOC(int);
	srchwnd_ID_BROWSECOPYLOC(1);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_DOWNLOAD
*/

void WxBrowser::OnDownloadClick( wxCommandEvent& event )
{
	extern void SrchWnd_Item_Run(int);
	SrchWnd_Item_Run(1);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_DLPARENT
*/

void WxBrowser::OnDlparentClick( wxCommandEvent& event )
{
	extern void srchwnd_ID_DLPARENT();
	srchwnd_ID_DLPARENT();
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_BROWSELOC
*/

void WxBrowser::OnBrowselocClick( wxCommandEvent& event )
{
	extern void srchwnd_ID_BROWSECOPYLOC(int);
	srchwnd_ID_BROWSECOPYLOC(0);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_BROWSENODE
*/

void WxBrowser::OnBrowsenodeClick( wxCommandEvent& event )
{
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_PRIVMSGNODE
*/

void WxBrowser::OnPrivmsgnodeClick( wxCommandEvent& event )
{
	extern void srchwnd_ID_PRIVMSGNODE();
	srchwnd_ID_PRIVMSGNODE();
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_SENDFILENODE
*/

void WxBrowser::OnSendfilenodeClick( wxCommandEvent& event )
{
	extern void srchwnd_ID_SENDFILENODE();
	srchwnd_ID_SENDFILENODE();
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_DOWNLOADFOLDER
*/

void WxBrowser::OnDownloadfolderClick( wxCommandEvent& event )
{
	extern void SrchWnd_Item_Run(int);
	SrchWnd_Item_Run(0);
	event.Skip();
}

/*!
* wxEVT_COMMAND_MENU_SELECTED event handler for ID_BROWSEFOLDER
*/

void WxBrowser::OnBrowsefolderClick( wxCommandEvent& event )
{
	extern void SrchWnd_Item_Run(int);
	SrchWnd_Item_Run(1);
	event.Skip();
}


