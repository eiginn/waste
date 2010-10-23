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

#ifndef _WXBROWSER_H_
#define _WXBROWSER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxbrowser.cpp"
#endif

/*!
* Includes
*/
#include <wx/listctrl.h>

////@begin includes
#include "wx/xrc/xmlres.h"
////@end includes

class WxBrowser;
class WxWasteSearchResultsListCtrl : public wxListCtrl {
public:
	WxWasteSearchResultsListCtrl(wxWindow *parent,
			const wxWindowID id,
			const wxPoint& pos,
			const wxSize& size,
			long style)
		: wxListCtrl(parent, id, pos, size, style)
		{ }

	virtual wxString OnGetItemText(long item, long column) const;
	virtual int OnGetItemImage(long item) const;
	virtual wxListItemAttr * OnGetItemAttr(long item) const;

	virtual void OnActivated(wxListEvent& event);
	virtual void OnColClick(wxListEvent& event);
	virtual void OnRightDown(wxMouseEvent& event);
	virtual void OnMousewheel(wxMouseEvent& event);

	WxBrowser *m_browser;

	DECLARE_NO_COPY_CLASS(WxWasteSearchResultsListCtrl)
	DECLARE_EVENT_TABLE()
};

/*!
* Forward declarations
*/

////@begin forward declarations
////@end forward declarations

/*!
* Control identifiers
*/

////@begin control identifiers
#define ID_BROWSER 10000
#define SYMBOL_WXBROWSER_STYLE wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
#define SYMBOL_WXBROWSER_TITLE _("Browser")
#define SYMBOL_WXBROWSER_IDNAME ID_BROWSER
#define SYMBOL_WXBROWSER_SIZE wxDefaultSize
#define SYMBOL_WXBROWSER_POSITION wxDefaultPosition
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
* WxBrowser class declaration
*/

class WxBrowser: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( WxBrowser )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	WxBrowser( );
	WxBrowser( wxWindow* parent, wxWindowID id = SYMBOL_WXBROWSER_IDNAME, const wxString& caption = SYMBOL_WXBROWSER_TITLE, const wxPoint& pos = SYMBOL_WXBROWSER_POSITION, const wxSize& size = SYMBOL_WXBROWSER_SIZE, long style = SYMBOL_WXBROWSER_STYLE );
	virtual ~WxBrowser( );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXBROWSER_IDNAME, const wxString& caption = SYMBOL_WXBROWSER_TITLE, const wxPoint& pos = SYMBOL_WXBROWSER_POSITION, const wxSize& size = SYMBOL_WXBROWSER_SIZE, long style = SYMBOL_WXBROWSER_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

////@begin WxBrowser event handler declarations

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_DOWNLOAD
	void OnDownloadClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_DLPARENT
	void OnDlparentClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_BROWSELOC
	void OnBrowselocClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_COPYLOC
	void OnCopylocClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_BROWSENODE
	void OnBrowsenodeClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_PRIVMSGNODE
	void OnPrivmsgnodeClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_SEARCHRES_USER_WHOIS
	void OnMSearchresUserWhoisClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_SENDFILENODE
	void OnSendfilenodeClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_DOWNLOADFOLDER
	void OnDownloadfolderClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_BROWSEFOLDER
	void OnBrowsefolderClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SRCH_USERLIST
	void OnSrchUserlistClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_GOPARENT
	void OnCGoparentClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_BACK
	void OnCBackClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_BACK2
	void OnCBack2Click( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_FORWARD
	void OnCForwardClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_FORWARD2
	void OnCForward2Click( wxCommandEvent& event );

	/// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for IDC_SEARCHTEXT
	void OnCSearchtextSelected( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_SEARCHTEXT
	void OnCSearchtextUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for IDC_SEARCH
	void OnCSearchClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for IDC_EDIT_FILTER
	void OnCEditFilterUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_TEXT_ENTER event handler for IDC_EDIT_FILTER
	void OnCEditFilterEnter( wxCommandEvent& event );

////@end WxBrowser event handler declarations

	void OnCSearchtextActivate( wxCommandEvent& event );

////@begin WxBrowser member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end WxBrowser member function declarations

	void OnDestroy();

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin WxBrowser member variables
	wxBitmapButton* m_srch_userlist;
	wxBitmapButton* m_goparent;
	wxBitmapButton* m_back;
	wxBitmapButton* m_back2;
	wxBitmapButton* m_forward;
	wxBitmapButton* m_forward2;
	wxComboBox* m_searchtext;
	wxBitmapButton* m_search;
	wxPanel* m_searchres_panel;
	wxTextCtrl* m_edit_filter;
	wxStaticText* m_searchstats;
////@end WxBrowser member variables

	wxBoxSizer *m_ressizer;
	WxWasteSearchResultsListCtrl* m_searchres;
	wxImageList *m_imagelist;
};

#endif
	// _WXBROWSER_H_
