/*
    WASTE - m_chat.h (Chat message builder/parser class)
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

#ifndef _C_MCHAT_H_
#define _C_MCHAT_H_

#include "shbuf.h"
#include "util.h"

class C_MessageChatReply
{
  public:
    C_MessageChatReply();
    C_MessageChatReply(C_SHBuf *in);
    ~C_MessageChatReply();

    void setnick(char *fn) { safe_strncpy(nick,fn,32); }
    char *getnick() { return nick; }

    C_SHBuf *Make(void);

  protected:
    char nick[32];
};


class C_MessageChat
{
  public:
    C_MessageChat();
    C_MessageChat(C_SHBuf *in);
    ~C_MessageChat();

    C_SHBuf *Make(void);

    char *get_chatstring(void) { return m_chatstring; }
    char *get_src(void) { return m_src; }
    char *get_dest(void) { return m_dest; }

    void set_chatstring(char *str) { ::strncpy(m_chatstring,str,255); m_chatstring[255]=0; }
    void set_src(char *str) { ::strncpy(m_src,str,31); m_src[31]=0; }
    void set_dest(char *str) { ::strncpy(m_dest,str,31); m_dest[31]=0; }

  protected:
    char m_chatstring[256];
		char m_src[32];
		char m_dest[32];

};

#endif//_C_MCHAT_H_
