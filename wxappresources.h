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

#ifndef _WXAPPRESOURCES_H_
#define _WXAPPRESOURCES_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxappresources.cpp"
#endif

/*!
* Control identifiers
*/

////@begin control identifiers
////@end control identifiers

class AppResources
{
public:
	AppResources() {}

/*!
* Resource functions
*/

////@begin AppResources resource functions
	/// Menu creation function for IDM_USERLIST_POPUP
	static wxMenu* CreateMenuUserlistPopup();

	/// Menu creation function for IDM_SEARCHRES_POPUP
	static wxMenu* CreateMenuSearchResPopup();

	/// Menu creation function for IDM_SEARCHRES_FILE_POPUP
	static wxMenu* CreateMenuSearchresFilePopup();

	/// Menu creation function for IDM_SEARCHRES_USER_POPUP
	static wxMenu* CreateMenuSearchresUserPopup();

	/// Menu creation function for IDM_UPLOADS_MENU
	static wxMenu* CreateUploadsPopupMenu();

	/// Menu creation function for IDM_DL_POPUP
	static wxMenu* CreateDownloadPopupMenu();

	/// Menu creation function for IDM_DLQ_POPUP
	static wxMenu* CreateDownloadQueuePopupMenu();

	/// Retrieves bitmap resources
	static wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	static wxIcon GetIconResource( const wxString& name );
////@end AppResources resource functions

};

#endif
	// _WXAPPRESOURCES_H_
