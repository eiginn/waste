/*
WASTE - wxWidgets application resources (menus etc)
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
#pragma implementation "wxappresources.h"
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

#include "wx/xrc/xmlres.h"
#include "wxappresources.h"

////@begin XPM images
////@end XPM images

/*!
* Resource functions
*/

////@begin AppResources resource functions
/*!
* Menu creation function for IDM_USERLIST_POPUP
*/

wxMenu* AppResources::CreateMenuUserlistPopup()
{
	return wxXmlResource::Get()->LoadMenu(_T("IDM_USERLIST_POPUP"));
}

/*!
* Menu creation function for IDM_SEARCHRES_POPUP
*/

wxMenu* AppResources::CreateMenuSearchResPopup()
{
	return wxXmlResource::Get()->LoadMenu(_T("IDM_SEARCHRES_POPUP"));
}

/*!
* Menu creation function for IDM_SEARCHRES_FILE_POPUP
*/

wxMenu* AppResources::CreateMenuSearchresFilePopup()
{
	return wxXmlResource::Get()->LoadMenu(_T("IDM_SEARCHRES_FILE_POPUP"));
}

/*!
* Menu creation function for IDM_SEARCHRES_USER_POPUP
*/

wxMenu* AppResources::CreateMenuSearchresUserPopup()
{
	return wxXmlResource::Get()->LoadMenu(_T("IDM_SEARCHRES_USER_POPUP"));
}

/*!
* Menu creation function for IDM_UPLOADS_MENU
*/

wxMenu* AppResources::CreateUploadsPopupMenu()
{
	return wxXmlResource::Get()->LoadMenu(_T("IDM_UPLOADS_MENU"));
}

/*!
* Menu creation function for IDM_DL_POPUP
*/

wxMenu* AppResources::CreateDownloadPopupMenu()
{
	return wxXmlResource::Get()->LoadMenu(_T("IDM_DL_POPUP"));
}

/*!
* Menu creation function for IDM_DLQ_POPUP
*/

wxMenu* AppResources::CreateDownloadQueuePopupMenu()
{
	return wxXmlResource::Get()->LoadMenu(_T("IDM_DLQ_POPUP"));
}

////@end AppResources resource functions

/*!
* Get bitmap resources
*/

wxBitmap AppResources::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin AppResources bitmap retrieval
	return wxNullBitmap;
////@end AppResources bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon AppResources::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin AppResources icon retrieval
	return wxNullIcon;
////@end AppResources icon retrieval
}
