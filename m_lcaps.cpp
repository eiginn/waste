/*
    WASTE - m_lcaps.cpp (Local capabilities message builder/parser class)
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

#include "main.h"
#include "m_lcaps.h"


C_MessageLocalCaps::C_MessageLocalCaps()
{
  m_numcaps=0;
}

C_MessageLocalCaps::~C_MessageLocalCaps()
{
}

C_MessageLocalCaps::C_MessageLocalCaps(C_SHBuf *in)
{
  unsigned char *data=(unsigned char *)in->Get();
  m_numcaps=in->GetLength()/8;
  if (m_numcaps < 0) m_numcaps=0;
  if (m_numcaps > CMLC_MAX_CAPS) m_numcaps=CMLC_MAX_CAPS;
  int x;
  for (x = 0; x < m_numcaps; x ++)
  {
    m_data[x*2]=data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24);  data+=4;
    m_data[x*2+1]=data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24);  data+=4;
  }
}

void C_MessageLocalCaps::get_cap(int idx, int *name, int *value)
{
  if (idx < 0 || idx >= m_numcaps) return;

  *name=m_data[idx*2];
  *value=m_data[idx*2+1];
}


void C_MessageLocalCaps::add_cap(int name, int value)
{
  if (m_numcaps >= CMLC_MAX_CAPS) return;
  m_data[m_numcaps*2]=name;
  m_data[m_numcaps*2+1]=value;
  m_numcaps++;
}

C_SHBuf *C_MessageLocalCaps::Make(void) // makes a copy of m_data
{
  C_SHBuf *p=new C_SHBuf(m_numcaps*8);
  unsigned char *data=(unsigned char *)p->Get();
  int x;
  for (x = 0; x < m_numcaps; x ++)
  {
    data[0]=m_data[x*2]&0xff; 
    data[1]=(m_data[x*2]>>8)&0xff; 
    data[2]=(m_data[x*2]>>16)&0xff; 
    data[3]=(m_data[x*2]>>24)&0xff; 
    data+=4;
    data[0]=m_data[x*2+1]&0xff; 
    data[1]=(m_data[x*2+1]>>8)&0xff; 
    data[2]=(m_data[x*2+1]>>16)&0xff; 
    data[3]=(m_data[x*2+1]>>24)&0xff; 
    data+=4;
  }
  return p;
}
