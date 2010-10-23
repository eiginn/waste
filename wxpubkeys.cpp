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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wxpubkeys.h"
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

#include "platform.hpp"
#include <wx/ffile.h>
#include "wxpubkeys.h"

////@begin XPM images
////@end XPM images

/*!
* WxPubkeyViewer type definition
*/

IMPLEMENT_DYNAMIC_CLASS( WxPubkeyViewer, wxDialog )

/*!
* WxPubkeyViewer event table definition
*/

BEGIN_EVENT_TABLE( WxPubkeyViewer, wxDialog )

////@begin WxPubkeyViewer event table entries
	EVT_BUTTON( XRCID("ID_CLOSE"), WxPubkeyViewer::OnCloseClick )

////@end WxPubkeyViewer event table entries

END_EVENT_TABLE()

/*!
* WxPubkeyViewer constructors
*/

WxPubkeyViewer::WxPubkeyViewer( )
{
}

WxPubkeyViewer::WxPubkeyViewer( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, caption, pos, size, style);
}

/*!
* WxPubkeyViewer creator
*/

bool WxPubkeyViewer::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WxPubkeyViewer member initialisation
	m_keys_text = NULL;
////@end WxPubkeyViewer member initialisation

////@begin WxPubkeyViewer creation
	SetParent(parent);
	CreateControls();
	SetIcon(GetIconResource(wxT("res/icon1.ico.gif")));
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
////@end WxPubkeyViewer creation
	return TRUE;
}

/*!
* Control creation for WxPubkeyViewer
*/

void WxPubkeyViewer::CreateControls()
{    
////@begin WxPubkeyViewer content construction

	wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_PUBKEYVIEWER"));
	m_keys_text = XRCCTRL(*this, "ID_KEYS_TEXT", wxTextCtrl);
////@end WxPubkeyViewer content construction

	// Create custom windows not generated automatically here.

////@begin WxPubkeyViewer content initialisation

////@end WxPubkeyViewer content initialisation
}

/*!
* wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLOSE
*/

void WxPubkeyViewer::OnCloseClick( wxCommandEvent& event )
{
	this->Destroy();
	event.Skip();
}

/*!
* Should we show tooltips?
*/

bool WxPubkeyViewer::ShowToolTips()
{
	return TRUE;
}

/*!
* Get bitmap resources
*/

wxBitmap WxPubkeyViewer::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin WxPubkeyViewer bitmap retrieval
	return wxNullBitmap;
////@end WxPubkeyViewer bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon WxPubkeyViewer::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin WxPubkeyViewer icon retrieval
	if (name == wxT("res/icon1.ico.gif"))
	{
		wxIcon icon(_T("res/icon1.ico.gif"), wxBITMAP_TYPE_GIF);
		return icon;
	}
	return wxNullIcon;
////@end WxPubkeyViewer icon retrieval
}

void WxPubkeyViewer::ShowFile (const char *fpath) {
// do this by hand - wxTextFile is way too slow and LoadFile()
// gives GTK errors
wxString wxpath = cstr_to_wxstr(fpath);
wxFFile file(wxpath.c_str(), _T("r"));

size_t len = file.Length();
char *buf = new char[len+1];
int readsofar = 0;
int toread, ret;
wxString bigbuf;

if (!file.IsOpened()) {
	m_keys_text->SetValue(_T("(Failed to open '") + wxpath + _T("'!)"));
	goto cleanup;
}

for (toread = (int) len; toread > 0; ) {
	ret = file.Read(buf+readsofar, toread);
	if (ret < 0) {
	m_keys_text->SetValue(_T("(Failed to open '") + wxpath + _T("'!)"));
	goto cleanup;
	}
	readsofar += ret;
	toread -= ret;
}

char *ptr;
char *startofline;
for (ptr = startofline = buf; *ptr != '\0'; ptr++) {
	if (*ptr == '\r') {
	*ptr = '\0';      // strip CRs too
	}
	else if (*ptr == '\n') {
	*ptr = '\0';      // terminate
	wxString wxstr = cstr_to_wxstr(startofline);
	bigbuf << wxstr << _T("\n");
	startofline = ptr+1;
	}
}
m_keys_text->SetValue(bigbuf);

cleanup:
delete[] buf;
m_keys_text->SetInsertionPoint(0L);
file.Close();
}


