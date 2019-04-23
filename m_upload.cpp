/*
    WASTE - m_upload.cpp (Upload request message builder/parser class)
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

#include "platform.h"

#include "m_upload.h"

C_UploadRequest::~C_UploadRequest()
{
  ::free(m_fn);
}

C_UploadRequest::C_UploadRequest()
{
  m_fn=0;
  m_idx=-1;
  memset(&m_guid,0,sizeof(m_guid));
  m_dest[0]=0;
  m_size_low=m_size_high=-1;
  memset(m_nick,0,32);
}

C_UploadRequest::C_UploadRequest(C_SHBuf *in)
{
  m_fn=0;
  m_idx=-1;
  memset(&m_guid,0,sizeof(m_guid));
  m_dest[0]=0;
  memset(m_nick,0,31);

  unsigned char *data=(unsigned char *)in->Get();
  int datalen=in->GetLength();

  if (datalen < 16+8+32+4+31 + 1 || datalen > 16+8+32+4+31 + 2048) 
  {
    debug_printf("uploadrequest: length out of range (%d)\n",datalen);
    return;
  }
  memcpy(&m_guid,data,16); data+=16; datalen-=16;

  m_size_low = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
  data+=4;
  datalen-=4;

  m_size_high = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
  data+=4;
  datalen-=4;

  memcpy(m_dest,data,32);
  m_dest[32]=0;
  data+=32;
  datalen-=32;

  m_idx = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
  data+=4;
  datalen-=4;

  memcpy(m_nick,data,31);
  data+=31;
  datalen-=31;

  m_fn=(char*)::malloc(datalen+1);
  ::memcpy(m_fn,data,datalen);
  m_fn[datalen]=0;
}

C_SHBuf *C_UploadRequest::Make(void)
{
  if (!m_fn || !m_fn[0] || strlen(m_fn) > 2048)
  {
    debug_printf("uploadrequest::make(): filename length %d out of range\n",m_fn?strlen(m_fn):0);
    return new C_SHBuf(0);
  }
  C_SHBuf *p=new C_SHBuf(16+8+32+4+31+::strlen(m_fn));
  unsigned char *data=(unsigned char *)p->Get();

  memcpy(data,&m_guid,16); data+=16;

  data[0]=m_size_low&0xff; 
  data[1]=(m_size_low>>8)&0xff; 
  data[2]=(m_size_low>>16)&0xff; 
  data[3]=(m_size_low>>24)&0xff; 
  data+=4;
  data[0]=m_size_high&0xff; 
  data[1]=(m_size_high>>8)&0xff; 
  data[2]=(m_size_high>>16)&0xff; 
  data[3]=(m_size_high>>24)&0xff; 
  data+=4;

  memcpy(data,m_dest,32);
  data+=32;

  data[0]=m_idx&0xff; 
  data[1]=(m_idx>>8)&0xff; 
  data[2]=(m_idx>>16)&0xff; 
  data[3]=(m_idx>>24)&0xff; 
  data+=4;

  memcpy(data,m_nick,31); data+=31;

  ::memcpy(data,m_fn,strlen(m_fn));

  return p;
}