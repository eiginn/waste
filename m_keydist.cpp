/*
    WASTE - m_keydist.cpp (Key distribution message builder/parser class)
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

#include "m_keydist.h"

C_KeydistRequest::~C_KeydistRequest()
{
}

C_KeydistRequest::C_KeydistRequest()
{
  memset(m_nick,0,32);
  memset(&m_key,0,sizeof(m_key));
  m_flags=0;

}

/*
** my nick (32 bytes)
** flags 1 byte
** keysize bits 2 bytes
** modulus size bytes 2 bytes
** exponent size bytes 2 bytes
** modulus bytes
** exponent bytes
*/

C_KeydistRequest::C_KeydistRequest(C_SHBuf *in)
{
  memset(m_nick,0,32);
  memset(&m_key,0,sizeof(m_key));
  m_flags=0;

  unsigned char *data=(unsigned char *)in->Get();
  int datalen=in->GetLength();

  if (datalen < 31+1+2+2+2 || datalen > 31+1+2+2+2 + MAX_RSA_MODULUS_LEN*2) 
  {
    debug_printf("keydistrequest: length out of range (%d)\n",datalen);
    return;
  }
  memcpy(m_nick,data,31);
  data+=31;
  datalen-=31;

  m_flags=*data++;
  datalen--;

  m_key.bits = (data[0]) | (data[1]<<8); data+=2; datalen-=2;
  int modlen=data[0] | (data[1]<<8); data+=2; datalen-=2;
  int explen=data[0] | (data[1]<<8); data+=2; datalen-=2;
  if (m_key.bits < MIN_RSA_MODULUS_BITS || m_key.bits > MAX_RSA_MODULUS_BITS ||
      modlen > MAX_RSA_MODULUS_LEN || explen > MAX_RSA_MODULUS_LEN || datalen < explen+modlen)
  {
    debug_printf("keydistrequest: key bits out of range (%d,%d,%d,%d)\n",m_key.bits,modlen,explen,datalen);
    m_key.bits=0;
    return;
  }
  memcpy(m_key.modulus+MAX_RSA_MODULUS_LEN-modlen,data,modlen); data+=modlen; datalen-=modlen;
  memcpy(m_key.exponent+MAX_RSA_MODULUS_LEN-explen,data,explen); data+=explen; datalen-=explen;
}

C_SHBuf *C_KeydistRequest::Make(void)
{
  if (!m_key.bits)
  {
    debug_printf("keydistrequest::make(): bits=0\n");
    return new C_SHBuf(0);
  }
  int explen=MAX_RSA_MODULUS_LEN;
  int modlen=MAX_RSA_MODULUS_LEN;
  unsigned char *expptr=m_key.exponent;
  unsigned char *modptr=m_key.modulus;
  while (*expptr == 0 && explen>0)
  {
    explen--;
    expptr++;
  }
  while (*modptr == 0 && modlen>0)
  {
    modlen--;
    modptr++;
  }
  
  debug_printf("keydistrequest: modlen=%d,explen=%d\n",modlen,explen);

  C_SHBuf *p=new C_SHBuf(31+1+2+2+2+explen+modlen);
  unsigned char *data=(unsigned char *)p->Get();

  memcpy(data,m_nick,31); data+=31;
  *data++=m_flags;
  *data++=m_key.bits&0xff;
  *data++=(m_key.bits>>8)&0xff;
  *data++=modlen&0xff;
  *data++=(modlen>>8)&0xff;
  *data++=explen&0xff;
  *data++=(explen>>8)&0xff;
  memcpy(data,modptr,modlen); data+=modlen;
  memcpy(data,expptr,explen); data+=explen;
  return p;
}