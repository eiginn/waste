/*
WASTE - wxWidgets platform-specific header
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

#ifndef _INCLD_WXWASTE_HPP
#define _INCLD_WXWASTE_HPP 1

#ifdef _DEFINE_WXUI

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/string.h>

// wxWindow is the closest type to a HWND.  typedef it here
class wxWindow;
typedef wxWindow* HWND;

extern void wxSetDlgItemText(wxObject *ptr, const char *buf);
extern void wxGetDlgItemText(wxObject *ptr, char *buf, int len);

#define PostMessage(x,y,z,a)        ;
#define InvalidateRect(x,y,z)       ;
#define wxEnableWindow(x,y)         x->Enable(y)

/* our policy: should we consider all strings to be encoded as UTF-8?
* this seems to be a bad idea, as it includes strings downloaded
* from remote peers.  they're more like truncated wchar_t's...
*/
#undef WX_COPY_STRINGS_AS_UTF8

static inline char *wxstr_to_cstr(char *out, const wxString &in, int maxl) {
#ifdef WX_COPY_STRINGS_AS_UTF8

	const char *buf;
#if wxUSE_UNICODE
	// use a temp var to avoid it going out of scope and deleting the buffer!
	wxCharBuffer unibuf = in.mb_str(*wxConvCurrent);
	buf = unibuf.data();
#else
	buf = in.mb_str(*wxConvCurrent);
#endif
	strncpy(out,buf,maxl);

#else

	int i;
	unsigned char *ucout = (unsigned char *) out;
	int len = in.Length();
	for (i = 0; i < len; i++) {
		if (i == maxl-1) {
		ucout[i] = '\0';
		break;
		}
		
		// truncate to 8 bits
		wxChar c = in.GetChar((size_t) i);
		ucout[i] = (unsigned char) (c & 0xff);
	}
	ucout[i]='\0';        // terminate

#endif

	out[maxl-1]='\0';     // paranoia
	return out;
}

/* copy an ISO-8859-1 string to a wxString buffer, doing extremely
* stupid conversion -- namely assume that the string is in ISO-8859-1
* and not UTF-8 etc. */
static inline wxString &cstr_to_wxstr(const char *buf) {
	const char *p;
	wxString *ret = new wxString();
	for (p = buf; *p != '\0'; p++) {
		wxChar wc = (unsigned char) *p;    /* force conversion */
		*ret << wc;
	}
	return *ret;
}

#endif

#endif // _INCLD_WXWASTE_HPP
