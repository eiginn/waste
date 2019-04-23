/*
    WASTE - m_file.cpp (File transfer message builder/parser class)
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
#include "m_file.h"

// constructor for building
C_FileSendRequest::C_FileSendRequest()
{
  m_port=m_ip=0;
  m_need_chunks_used=0;
  m_need_chunks_indexused=0;
  memset(&m_prev_guid,0,sizeof(m_prev_guid));
  memset(&m_guid,0,sizeof(m_guid));
  memset(m_fnhash,0,sizeof(m_fnhash));
  memset(m_nick,0,32);
  m_idx=-1;
  m_abort=0;
}


C_FileSendRequest::C_FileSendRequest(C_SHBuf *in)
{
  m_port=m_ip=0;
  m_need_chunks_used=0;
  memset(&m_prev_guid,0,sizeof(m_prev_guid));
  memset(&m_guid,0,sizeof(m_guid));
  memset(m_fnhash,0,sizeof(m_fnhash));
  memset(m_nick,0,32);
  m_idx=-1;
  m_abort=0;

  unsigned char *data=(unsigned char *)in->Get();
  int datalen=in->GetLength();

  if (datalen < 16+16) 
  {
    return;
  }
  memcpy(&m_guid,data,16); data+=16; datalen-=16;
  memcpy(&m_prev_guid,data,16); data+=16; datalen-=16;

  if (datalen < 4+SHA_OUTSIZE+31+6)
  {
    if (datalen) m_abort=2;
    else m_abort=1;
    return;
  }


  m_idx = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
  data+=4;
  datalen-=4;

  memcpy(m_fnhash,data,SHA_OUTSIZE);
  data+=SHA_OUTSIZE;
  datalen-=SHA_OUTSIZE;

  memcpy(m_nick,data,31);
  data+=31;
  datalen-=31;

  m_ip= data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
  data+=4;
  datalen-=4;
  m_port= data[0]|(data[1]<<8); 
  data+=2;
  datalen-=2;

  while (datalen>4 && m_need_chunks_used < FILE_MAX_CHUNKS_PER_REQ)
  {
    int offs=data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24);
    int len=(int)data[4]+1;
    int x;
    data+=5;
    datalen-=5;

    for (x = 0; x < len && m_need_chunks_used < FILE_MAX_CHUNKS_PER_REQ; x ++)
    {
      m_need_chunks_offs[m_need_chunks_used++]=offs++; // rle decompress
    }
  }
}

C_SHBuf *C_FileSendRequest::Make(void)
{
  // send guid (16 bytes)
  // send old guid (16 bytes)
  // if not abort=1:
    // one random byte
    // if not abort=2
    // send file index (4 bytes)
    // send filename hash (SHA_OUTSIZE bytes)
    // send nick (31 bytes, can be zeros)
    // send dc ip/port (6 bytes, can be 0)
      // if request table
         // send request table
  int x;
  int size=16+16;
  if (!m_abort) size+=4+SHA_OUTSIZE+31+6+m_need_chunks_indexused*5;
  if (m_abort == 2) size++;

  C_SHBuf *p=new C_SHBuf(size);
  unsigned char *data=(unsigned char *)p->Get();
  memcpy(data,&m_guid,16); data+=16;
  memcpy(data,&m_prev_guid,16); data+=16;

  if (m_abort) return p;

  data[0]=m_idx&0xff; 
  data[1]=(m_idx>>8)&0xff; 
  data[2]=(m_idx>>16)&0xff; 
  data[3]=(m_idx>>24)&0xff; 
  data+=4;
  memcpy(data,m_fnhash,SHA_OUTSIZE); data+=SHA_OUTSIZE;

  memcpy(data,m_nick,31); data+=31;

  data[0]=m_ip&0xff; 
  data[1]=(m_ip>>8)&0xff; 
  data[2]=(m_ip>>16)&0xff; 
  data[3]=(m_ip>>24)&0xff; 
  data+=4;

  data[0]=m_port&0xff; 
  data[1]=(m_port>>8)&0xff; 
  data+=2;


  debug_printf("sending a request with %d pairs, for %d items\n",m_need_chunks_indexused,m_need_chunks_used);
  for (x = 0; x < m_need_chunks_indexused; x ++)
  {
    data[0]=m_need_chunks_offs[x]&0xff; 
    data[1]=(m_need_chunks_offs[x]>>8)&0xff; 
    data[2]=(m_need_chunks_offs[x]>>16)&0xff; 
    data[3]=(m_need_chunks_offs[x]>>24)&0xff; 
    data[4]=m_need_chunks_len[x];
    data+=5;
  }
  return p;
}

void C_FileSendRequest::add_need_chunk(unsigned int idx)
{
  if (m_need_chunks_used >= FILE_MAX_CHUNKS_PER_REQ) return;

  // see if this one can be added on to the old run
  if (m_need_chunks_indexused > 0 && m_need_chunks_offs[m_need_chunks_indexused-1] + m_need_chunks_len[m_need_chunks_indexused-1] + 1 == idx)
  {
    m_need_chunks_len[m_need_chunks_indexused-1]++;
    //debug_printf("continuing run with %d\n",idx);
  }
  else
  {
    //debug_printf("new run at %d\n",idx);
    m_need_chunks_len[m_need_chunks_indexused]=0;
    m_need_chunks_offs[m_need_chunks_indexused++]=idx;
  }

  m_need_chunks_used++;
}



C_FileSendRequest::~C_FileSendRequest()
{
}


C_FileSendReply::C_FileSendReply()
{
  m_error=0;

  m_index=0;

  m_file_len_low=m_file_len_high=0;
  m_create_date=m_mod_date=0;
  m_chunkcnt=0;
  m_port=m_ip=0;

  m_data_len=0;

}


C_FileSendReply::C_FileSendReply(C_SHBuf *in)
{
  m_error=0;

  m_index=0;
  m_port=m_ip=0;

  m_file_len_low=m_file_len_high=0;
  m_create_date=m_mod_date=0;
  m_chunkcnt=0;

  m_data_len=0;

 
  unsigned char *data=(unsigned char *)in->Get();
  int datalen=in->GetLength();

  if (datalen < 4) { m_error=datalen; return; }

  m_index = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
  data+=4;
  datalen-=4;

  if (m_index != 0xFFFFFFFF) // woohoo we get data now
  {
    m_data_len=datalen;
    if (m_data_len < 0 || m_data_len > FILE_CHUNKSIZE)
    {
      debug_printf("filesendreply: data length out of range, %d\n",m_data_len);
      m_data_len=0;
    }
    if (m_data_len) 
    {
      memcpy(m_data,data,m_data_len);
    }
  }
  else // header parsing
  {
    if (datalen < 4+4+4+4+4+SHA_OUTSIZE+4+2) 
    {
      debug_printf("filesendreply: got packet that is a header, with length of %d\n",datalen);
      return;
    }
    m_file_len_low = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
    data+=4;
    datalen-=4;
    m_file_len_high = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
    data+=4;
    datalen-=4;
    m_create_date = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
    data+=4;
    datalen-=4;
    m_mod_date = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
    data+=4;
    datalen-=4;
    m_chunkcnt = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
    data+=4;
    datalen-=4;    
    memcpy(m_hash,data,SHA_OUTSIZE);
    data+=SHA_OUTSIZE;
    datalen-=SHA_OUTSIZE;

    m_ip= data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24); 
    data+=4;
    datalen-=4;
    m_port= data[0]|(data[1]<<8); 
    data+=2;
    datalen-=2;
  }
}

C_FileSendReply::~C_FileSendReply()
{
}

C_SHBuf *C_FileSendReply::Make(void)
{
  // chunk index (-1 is header info)
  // if header:
  //   file length 8 bytes
  //   dates 8 bytes
  //   count of max chunks coming 4 bytes
  //   file md5 16 bytes
  // else
  //   FILE_CHUNKSIZE bytes of data :)
  C_SHBuf *p;

  if (m_error) return new C_SHBuf(m_error>3?3:m_error);

  if (m_index!=0xFFFFFFFF)
  {
    if (m_data_len > FILE_CHUNKSIZE)
    {
      debug_printf("filesendreply::make() data length = %d\n",m_data_len);
      return new C_SHBuf(0);
    }

    p=new C_SHBuf(4+m_data_len);
    unsigned char *data=(unsigned char *)p->Get();
    data[0]=m_index&0xff; 
    data[1]=(m_index>>8)&0xff; 
    data[2]=(m_index>>16)&0xff; 
    data[3]=(m_index>>24)&0xff; 
    data+=4;
    memcpy(data,m_data,m_data_len);
  }
  else
  {
    p=new C_SHBuf(4+8+8+4+SHA_OUTSIZE+6);
    unsigned char *data=(unsigned char *)p->Get();
    data[0]=m_index&0xff; 
    data[1]=(m_index>>8)&0xff; 
    data[2]=(m_index>>16)&0xff; 
    data[3]=(m_index>>24)&0xff; 
    data+=4;
    data[0]=m_file_len_low&0xff; 
    data[1]=(m_file_len_low>>8)&0xff; 
    data[2]=(m_file_len_low>>16)&0xff; 
    data[3]=(m_file_len_low>>24)&0xff; 
    data+=4;
    data[0]=m_file_len_high&0xff; 
    data[1]=(m_file_len_high>>8)&0xff; 
    data[2]=(m_file_len_high>>16)&0xff; 
    data[3]=(m_file_len_high>>24)&0xff; 
    data+=4;
    data[0]=m_create_date&0xff; 
    data[1]=(m_create_date>>8)&0xff; 
    data[2]=(m_create_date>>16)&0xff; 
    data[3]=(m_create_date>>24)&0xff; 
    data+=4;
    data[0]=m_mod_date&0xff; 
    data[1]=(m_mod_date>>8)&0xff; 
    data[2]=(m_mod_date>>16)&0xff; 
    data[3]=(m_mod_date>>24)&0xff; 
    data+=4;
    data[0]=m_chunkcnt&0xff; 
    data[1]=(m_chunkcnt>>8)&0xff; 
    data[2]=(m_chunkcnt>>16)&0xff; 
    data[3]=(m_chunkcnt>>24)&0xff; 
    data+=4;    
    memcpy(data,m_hash,SHA_OUTSIZE);
    data+=SHA_OUTSIZE;
    data[0]=m_ip&0xff; 
    data[1]=(m_ip>>8)&0xff; 
    data[2]=(m_ip>>16)&0xff; 
    data[3]=(m_ip>>24)&0xff; 
    data+=4;

    data[0]=m_port&0xff; 
    data[1]=(m_port>>8)&0xff; 
    data+=2;
  }
  return p;
}

void C_FileSendReply::set_file_len(unsigned int file_len_low, unsigned int file_len_high)
{
  m_file_len_low=file_len_low;
  m_file_len_high=file_len_high;
}


void C_FileSendReply::set_data(unsigned char *buf, int len) 
{ 
  if (len < 0 || len > FILE_CHUNKSIZE)
  {
    debug_printf("filesendreply::set_data() data length out of range, %d\n",len);
    return;
  }
  m_data_len=len;
  memcpy(m_data,buf,len);
}
