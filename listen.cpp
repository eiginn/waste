/*
    WASTE - listen.cpp (Port listening class)
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
#include "listen.h"
#include "sockets.h"

C_Listen::C_Listen(short port, char *which_interface)
{
  m_port=port;
  m_socket = ::socket(AF_INET,SOCK_STREAM,0);
  if (m_socket < 0) 
  {
    debug_printf("C_Listen: call to socket() failed: %d.\n",ERRNO);
  }
  else
  {
    struct sockaddr_in sin;
    SET_SOCK_BLOCK(m_socket,0);
#ifndef _WIN32
    int bflag = 1;
    setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, &bflag, sizeof(bflag));
#endif
    ::memset((char *) &sin, 0,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons( (short) port );
    if (which_interface) 
    {
      sin.sin_addr.s_addr = ::inet_addr(which_interface);
    }
    if (!which_interface || !sin.sin_addr.s_addr) 
    {
      sin.sin_addr.s_addr=INADDR_ANY;
    }
    if (::bind(m_socket,(struct sockaddr *)&sin,sizeof(sin))) 
    {
      closesocket(m_socket);
      m_socket=-1;
      debug_printf("C_Listen: call to bind() failed: %d.\n",ERRNO);
    }
    else
    {  
      if (listen(m_socket,8)==-1) 
      {
        closesocket(m_socket);
        m_socket=-1;
        debug_printf("C_Listen: call to listen() failed: %d.\n",ERRNO);
      }
    }
  }
}

C_Listen::~C_Listen()
{
  if (m_socket>=0)
  {
    closesocket(m_socket);
  }
}

C_Connection *C_Listen::get_connect(void)
{
  if (m_socket < 0)
  {
    return NULL;
  }
	struct sockaddr_in saddr;
	int length = sizeof(struct sockaddr_in);
	int s = accept(m_socket, (struct sockaddr *) &saddr, (socklen_t *)&length);
  if (s != -1)
  {
    return new C_Connection(s,&saddr);
  }
  return NULL;
}
