/*
WASTE - wxWidgets preferences dialog
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
#pragma implementation "wxprefs.h"
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

#include "wxprefs.h"
#include "wxpubkeys.h"
#include "main.hpp"

////@begin XPM images
////@end XPM images

/*!
* WxPrefs type definition
*/

IMPLEMENT_DYNAMIC_CLASS( WxPrefs, wxDialog )

/*!
* WxPrefs event table definition
*/

BEGIN_EVENT_TABLE( WxPrefs, wxDialog )

////@begin WxPrefs event table entries
	EVT_CLOSE( WxPrefs::OnCloseWindow )

	EVT_CHOICE( XRCID("ID_CONN_SPEED"), WxPrefs::OnConnSpeedSelected )

	EVT_TEXT( XRCID("ID_NICKNAME"), WxPrefs::OnNicknameUpdated )
	EVT_TEXT_ENTER( XRCID("ID_NICKNAME"), WxPrefs::OnNicknameEnter )

	EVT_BUTTON( XRCID("ID_UPD_NICK"), WxPrefs::OnUpdNickClick )

	EVT_TEXT( XRCID("ID_USER_INFO"), WxPrefs::OnUserInfoUpdated )
	EVT_TEXT_ENTER( XRCID("ID_USER_INFO"), WxPrefs::OnUserInfoEnter )

	EVT_TEXT( XRCID("ID_NET_NAME"), WxPrefs::OnNetNameUpdated )
	EVT_TEXT_ENTER( XRCID("ID_NET_NAME"), WxPrefs::OnNetNameEnter )

	EVT_BUTTON( XRCID("ID_GEN_PRIVKEY"), WxPrefs::OnGenPrivkeyClick )

	EVT_BUTTON( XRCID("ID_COPY_PUBKEY"), WxPrefs::OnCopyPubkeyClick )

	EVT_BUTTON( XRCID("ID_VIEW_PUBKEYS"), WxPrefs::OnViewPubkeysClick )

	EVT_BUTTON( XRCID("ID_ADD_PUB_KEYS"), WxPrefs::OnAddPubKeysClick )

	EVT_BUTTON( XRCID("ID_BROADCAST_PUBKEY"), WxPrefs::OnBroadcastPubkeyClick )

	EVT_TEXT( XRCID("ID_DOWNLOAD_PATH"), WxPrefs::OnDownloadPathUpdated )

	EVT_BUTTON( XRCID("ID_BROWSE_DL_PATH"), WxPrefs::OnBrowseDlPathClick )

	EVT_TEXT( XRCID("ID_SHARE_PATH"), WxPrefs::OnSharePathUpdated )

	EVT_BUTTON( XRCID("ID_ADD_SHARE_PATH"), WxPrefs::OnAddSharePathClick )

	EVT_BUTTON( XRCID("ID_CLOSE_PREFS"), WxPrefs::OnClosePrefsClick )

////@end WxPrefs event table entries

END_EVENT_TABLE()

/*!
* WxPrefs constructors
*/

WxPrefs::WxPrefs( )
{
}

WxPrefs::WxPrefs( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, caption, pos, size, style);
}

/*!
* WxPrefs creator
*/

bool WxPrefs::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WxPrefs member initialisation
	m_conn_speed = NULL;
	m_nickname = NULL;
	m_user_info = NULL;
	m_net_name = NULL;
	m_download_path = NULL;
	m_share_path = NULL;
////@end WxPrefs member initialisation

////@begin WxPrefs creation
	SetParent(parent);
	CreateControls();
	SetIcon(GetIconResource(wxT("res/icon1.ico.gif")));
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Centre();
////@end WxPrefs creation

	extern void prefs_find_wxwidgets(WxPrefs *);
	prefs_find_wxwidgets(this);

	return TRUE;
}

/*!
* Control creation for WxPrefs
*/

void WxPrefs::CreateControls()
{    
////@begin WxPrefs content construction

	wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_PREFS_DIALOG"));
	m_conn_speed = XRCCTRL(*this, "ID_CONN_SPEED", wxChoice);
	m_nickname = XRCCTRL(*this, "ID_NICKNAME", wxTextCtrl);
	m_user_info = XRCCTRL(*this, "ID_USER_INFO", wxTextCtrl);
	m_net_name = XRCCTRL(*this, "ID_NET_NAME", wxTextCtrl);
	m_download_path = XRCCTRL(*this, "ID_DOWNLOAD_PATH", wxTextCtrl);
	m_share_path = XRCCTRL(*this, "ID_SHARE_PATH", wxTextCtrl);
////@end WxPrefs content construction

	// Create custom windows not generated automatically here.

////@begin WxPrefs content initialisation

////@end WxPrefs content initialisation
}

/*!
* Should we show tooltips?
*/

bool WxPrefs::ShowToolTips()
{
	return TRUE;
}

/*!
* Get bitmap resources
*/

wxBitmap WxPrefs::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin WxPrefs bitmap retrieval
	return wxNullBitmap;
////@end WxPrefs bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon WxPrefs::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin WxPrefs icon retrieval
	if (name == wxT("res/icon1.ico.gif"))
	{
		wxIcon icon(_T("res/icon1.ico.gif"), wxBITMAP_TYPE_GIF);
		return icon;
	}
	return wxNullIcon;
////@end WxPrefs icon retrieval
}
/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_UPD_NICK
*/

void WxPrefs::OnUpdNickClick( wxCommandEvent& event )
{
	extern void clicked_IDC_BUTTON_UPDATE_NICK();
	clicked_IDC_BUTTON_UPDATE_NICK();
	event.Skip();
}

/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for ID_NET_NAME
*/

extern void prefs_update_from_textboxes();

void WxPrefs::OnNetNameUpdated( wxCommandEvent& event )
{
	prefs_update_from_textboxes();
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_GEN_PRIVKEY
*/

void WxPrefs::OnGenPrivkeyClick( wxCommandEvent& event )
{
	char buf[2048];
	sprintf(buf,"%s.pr4",g_config_prefix);
	RunKeyGen(this, buf);
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COPY_PUBKEY
*/

void WxPrefs::OnCopyPubkeyClick( wxCommandEvent& event )
{
	extern void copyMyPubKey();
	copyMyPubKey();
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BROADCAST_PUBKEY
*/

void WxPrefs::OnBroadcastPubkeyClick( wxCommandEvent& event )
{
	main_BroadcastPublicKey();
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_VIEW_PUBKEYS
*/

void WxPrefs::OnViewPubkeysClick( wxCommandEvent& event )
{
	WxPubkeyViewer *viewer = new WxPubkeyViewer(this, ID_PUBKEYVIEWER, _("Public Keys"));
	viewer->Show(true);

	char str[2048];
	sprintf(str,"%s.pr3",g_config_prefix);
	viewer->ShowFile(str);
	event.Skip();
}

/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for ID_DOWNLOAD_PATH
*/

void WxPrefs::OnDownloadPathUpdated( wxCommandEvent& event )
{
	prefs_update_from_textboxes();
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BROWSE_DL_PATH
*/

void WxPrefs::OnBrowseDlPathClick( wxCommandEvent& event )
{
	const wxString &dir = ::wxDirSelector(_T("Select a directory:"));
			//m_download_path->GetValue());
	if (!dir.empty()) {
		m_download_path->SetValue(dir);
		prefs_update_from_textboxes();
	}
	event.Skip();
}

/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SHARE_PATH
*/

void WxPrefs::OnSharePathUpdated( wxCommandEvent& event )
{
	prefs_update_from_textboxes();
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADD_SHARE_PATH
*/

void WxPrefs::OnAddSharePathClick( wxCommandEvent& event )
{
	wxString dir = ::wxDirSelector(_T("Select a directory:"));
	if (!dir.empty()) {
		wxString prevdirs = m_share_path->GetValue();
		if (prevdirs.Length() > 0 && prevdirs.Last() != L';') {
			prevdirs += _T(";");
		}
		prevdirs += dir;
		m_share_path->SetValue(prevdirs);
		prefs_update_from_textboxes();
	}
	event.Skip();
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLOSE_PREFS
*/

void WxPrefs::OnClosePrefsClick( wxCommandEvent& event )
{
	this->Show(false);
        doDatabaseRescan();     /* always trigger a rescan */
	event.Skip();
}


/*!
* wxEVT_CLOSE_WINDOW event handler for ID_DIALOG
*/

void WxPrefs::OnCloseWindow( wxCloseEvent& event )
{
	this->Show(false);
	event.Skip();
}


/*!
* wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_CONN_SPEED
*/

void WxPrefs::OnConnSpeedSelected( wxCommandEvent& event )
{
	prefs_update_from_textboxes();
	event.Skip();
}

/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for ID_USER_INFO
*/

void WxPrefs::OnUserInfoUpdated( wxCommandEvent& event )
{
	prefs_update_from_textboxes();
	event.Skip();
}

/*!
* wxEVT_COMMAND_TEXT_ENTER event handler for ID_USER_INFO
*/

void WxPrefs::OnUserInfoEnter( wxCommandEvent& event )
{
	prefs_update_from_textboxes();
	event.Skip();
}


/*!
* wxEVT_COMMAND_TEXT_UPDATED event handler for ID_NICKNAME
*/

void WxPrefs::OnNicknameUpdated( wxCommandEvent& event )
{
	prefs_update_from_textboxes();
	event.Skip();
}

/*!
* wxEVT_COMMAND_TEXT_ENTER event handler for ID_NICKNAME
*/

void WxPrefs::OnNicknameEnter( wxCommandEvent& event )
{
	prefs_update_from_textboxes();
	event.Skip();
}


/*!
* wxEVT_COMMAND_TEXT_ENTER event handler for ID_NET_NAME
*/

void WxPrefs::OnNetNameEnter( wxCommandEvent& event )
{
	prefs_update_from_textboxes();
	event.Skip();
}


/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADD_PUB_KEYS
*/

void WxPrefs::OnAddPubKeysClick( wxCommandEvent& event )
{
	wxFileDialog dialog(this, _T("Import public key file:"),
			_T(""), _T(""),
			_T("Public key files (*.pub, *.txt)|*.pub;*.txt|All files (*.*)|*.*"),
			wxOPEN|wxMULTIPLE);

	if (dialog.ShowModal() == wxID_OK)
	{
		wxArrayString paths, filenames;
		dialog.GetPaths(paths);

		wxString msg, s;
		int count = paths.GetCount();
		int num = 0;
		for (int n = 0; n < count; n++)
		{
			char buf[1024];
			wxstr_to_cstr(buf, paths[n], 1023);
			num+=loadPKList(buf);
		}

		if (num) {
			savePKList();

			wxString str = wxString::Format(_T("Imported %d public keys successfully."), num);
			::wxMessageBox(str, _T("Public Key Import"));
		}
	}

	event.Skip();
}


