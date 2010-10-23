/*
WASTE - wxWidgets main application class
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

#ifndef _WXMAIN_H_
#define _WXMAIN_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxmain.cpp"
#endif

/*!
* Includes
*/

////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/image.h"
#include "wxmainframe.h"
////@end includes

/*!
* Forward declarations
*/

////@begin forward declarations
////@end forward declarations

/*!
* Control identifiers
*/

////@begin control identifiers
////@end control identifiers

/*!
* WxwasteApp class declaration
*/

class WxwasteApp: public wxApp
{    
	DECLARE_CLASS( WxwasteApp )
	DECLARE_EVENT_TABLE()

public:
	/// Constructor
	WxwasteApp();

	/// Initialises the application
	virtual bool OnInit();

	/// Called on exit
	virtual int OnExit();
	
	virtual int MainLoop();

////@begin WxwasteApp event handler declarations

////@end WxwasteApp event handler declarations

////@begin WxwasteApp member function declarations

////@end WxwasteApp member function declarations

////@begin WxwasteApp member variables
////@end WxwasteApp member variables

	MainFrame *m_mainframe;
};

/*!
* Application instance declaration 
*/

////@begin declare app
DECLARE_APP(WxwasteApp)
////@end declare app

#endif
	// _WXMAIN_H_
