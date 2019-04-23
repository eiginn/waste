/*
    WASTE - m_lcaps.h (Local capabilities message builder/parser class)
    Copyright (C) 2003 Nullsoft, Inc.

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
/*
  Copyright (c) 1998-2003 VIA Technologies, Inc.

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTIES OR REPRESENTATIONS; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/

#ifndef _C_MLCAP_H_
#define _C_MLCAP_H_

#include "shbuf.h"
#include "util.h"


#define CMLC_MAX_CAPS 16

#define MLC_SATURATION 0x0100 // the value defines whether or not the host should be saturated (low bit anyway so far)
#define MLC_BANDWIDTH  0x0200 // the value defines the buffer size that the remote would like you to use for sending to it (clamped to 64b->32kb)

/* Modified by LoraYin 2003.11.25
   Add capabilitied for our PadLockSL including c5p AES encryption.
 */
#define MLC_PADLOCKSL_AES_KEYSIZE	0x010000	// the value defines we are PadLockSL and the keysize we are prefered to use.
#define MLC_PADLOCKSL_SWITCH_TO_AES	0x020000	// the value defines switch to AES encryption now.


class C_MessageLocalCaps
{
  public:
    C_MessageLocalCaps();
    C_MessageLocalCaps(C_SHBuf *in);
    ~C_MessageLocalCaps();

    int get_numcaps() { return m_numcaps; }
    void clear_caps() { m_numcaps=0; }
    void get_cap(int idx, int *name, int *value);
    void add_cap(int name, int value);

    C_SHBuf *Make(void); // makes a copy of m_data

  protected:
    int m_data[CMLC_MAX_CAPS*2];
    int m_numcaps;
};

#endif//_C_MLCAP_H_
