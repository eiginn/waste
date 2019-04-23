/*
    WASTE - m_chat.cpp (Chat message builder/parser class)
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
    along with v; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "main.h"
#include "m_chat.h"


C_MessageChatReply::C_MessageChatReply()
{
  nick[0]=0;
}

C_MessageChatReply::~C_MessageChatReply()
{
}

C_MessageChatReply::C_MessageChatReply(C_SHBuf *in)
{
  nick[0]=0;
  if (in->GetLength() > 31)
  {
    debug_printf("chatreply: length of %d too big\n",in->GetLength());
  }
  else 
  {
    nick[in->GetLength()]=0;
    memcpy(nick,in->Get(),in->GetLength());
  }
}

C_SHBuf *C_MessageChatReply::Make()
{
  int l=strlen(nick);
  if (l > 31)
  {
    debug_printf("chatreply::make() length is %d\n",l);
    l=31;
  }
  C_SHBuf *n=new C_SHBuf(l);
  if (l) memcpy(n->Get(),nick,l);
  return n;
}


C_MessageChat::~C_MessageChat()
{
}

C_MessageChat::C_MessageChat()
{
  m_chatstring[0]=0;
	m_src[0]=0;
	m_dest[0]=0;
}

C_MessageChat::C_MessageChat(C_SHBuf *in)
{
  m_chatstring[0]=0;
	m_src[0]=0;
	m_dest[0]=0;
  unsigned char *data=(unsigned char*)in->Get();
  int l=in->GetLength();
  int t;

  for (t=0; t < min(l,32) && data[t]; t ++);
  if (t == min(l,32)) return;
	::memcpy(m_src,data,t+1);
  l-=t+1;
  data+=t+1;

  for (t=0; t < min(l,32) && data[t]; t ++);
  if (t == min(l,32)) return;
	::memcpy(m_dest,data,t+1);
  l-=t+1;
  data+=t+1;

  for (t=0; t < min(l,256) && data[t]; t ++);
  if (t == min(l,256)) return;
  ::memcpy(m_chatstring,data,t+1);
  l-=t+1;
  data+=t+1;
}

C_SHBuf *C_MessageChat::Make(void)
{
  C_SHBuf *buf=new C_SHBuf(::strlen(m_src)+::strlen(m_dest)+::strlen(m_chatstring)+3);
  unsigned char *data=(unsigned char *)buf->Get();
  ::memcpy((char*)data,m_src,strlen(m_src)+1); 
	data+=::strlen(m_src)+1;
  ::memcpy((char*)data,m_dest,strlen(m_dest)+1); 
	data+=::strlen(m_dest)+1;
  ::memcpy((char*)data,m_chatstring,strlen(m_chatstring)+1); 
  return buf;
}
