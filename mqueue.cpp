/*
    WASTE - mqueue.cpp (Class that handles sending messages over a connection)
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
/* 
 * This file implement functions with 128-bits boundary to support AES encryption.
 */

#include "platform.h"
#include "main.h"
#include "rsa/global.h"
#include "rsa/md5.h"
extern "C" {
#include "rsa/r_random.h"
};
#include "mqueue.h"


#define PAD8(x) (((x)+7)&(~7))

/* 128-bit boundary. */
#define PAD16(x) (((x)+15)&(~15))

C_MessageQueue::C_MessageQueue(C_Connection *con, int maxmsg, int maxroute)
{
  m_stat_send=0;
  m_stat_recv=0;
  m_stat_drop=0;
  m_newmsg_pos=-1;
  memset(&m_newmsg,0,sizeof(m_newmsg));
  m_con=con;
  m_msg=(T_Message*)::malloc(sizeof(T_Message)*maxmsg); 
  m_msg_len=maxmsg; m_msg_used=0; 
  
  int y;
  for (y = 0; y < ROUTETAB_NUM; y ++)
  {
    m_num_routes[y]=maxroute;
    m_routes[y]=(T_GUID *)::malloc(sizeof(T_GUID)*m_num_routes[y]*2);
    m_routes_order[y]=m_routes[y]+m_num_routes[y];
    int x;
    for (x = 0; x < m_num_routes[y]; x++)
    {
      memset(&m_routes[y][x],0,16);
      memset(&m_routes_order[y][x],0,16);
      m_routes[y][x].idc[3]=m_routes_order[y][x].idc[3]=x&0xff;
      m_routes[y][x].idc[2]=m_routes_order[y][x].idc[2]=(x>>8)&0xff;
      m_routes[y][x].idc[1]=m_routes_order[y][x].idc[1]=(x>>16)&0xff;
      m_routes[y][x].idc[0]=m_routes_order[y][x].idc[0]=(x>>24)&0xff;
    }
    m_route_opos[y]=0;
  }
  m_msg_bsent=-1;
}

C_MessageQueue::~C_MessageQueue()
{
  if (m_msg) {
    int x;
    for (x = 0; x < m_msg_used; x ++)
    {
      if (m_msg[x].data) m_msg[x].data->Unlock();
    }
    ::free(m_msg);
    m_msg=0;
  }
  int y;
  for (y = 0; y < ROUTETAB_NUM; y ++)
  {
    ::free(m_routes[y]);
  }
  if (m_newmsg.data)
  {
    m_newmsg.data->Unlock();
  }
  if (m_con) 
  {
    delete m_con;
  }
}

void C_MessageQueue::calc_md5(T_Message *msg, unsigned char buf[16])
{
  MD5_CTX ctx;
  MD5Init(&ctx);
  unsigned char t[7];
  t[0]=msg->message_type&0xff;
  t[1]=(msg->message_type>>8)&0xff;
  t[2]=(msg->message_type>>16)&0xff;
  t[3]=(msg->message_type>>24)&0xff;
  t[4]=msg->message_prio;
  t[5]=msg->message_length&0xff;
  t[6]=(msg->message_length>>8)&0xff;
  MD5Update(&ctx,t,7);
  MD5Update(&ctx,(unsigned char *)&msg->message_guid,16);
  MD5Update(&ctx,(unsigned char *)(msg->data?msg->data->Get():0),msg->message_length);
  MD5Final(buf,&ctx);
}

int C_MessageQueue::send_message(T_Message *msg)
{
  // if m_msg_bsent<0 then its safe to modify the first message
  int x;

  for (x = (m_msg_bsent>=0); x < m_msg_used; x ++)
  {
    if (msg->message_prio<m_msg[x].message_prio) break;
  }

  if (m_msg_used==m_msg_len)
  {
    m_stat_drop++;
  }

  if (x < m_msg_len) 
  {
    insert(x,msg);
    m_stat_send++;
  }

  return 1;
}

int C_MessageQueue::recv_message(T_Message *msg)
{
  /* 128-bit buffer boundary. */
  int pos = PAD16(m_newmsg.message_length);
  if (m_newmsg_pos>=pos)
  {
    ::memcpy(msg,&m_newmsg,sizeof(m_newmsg));
    ::memset(&m_newmsg,0,sizeof(m_newmsg));
    m_stat_recv++;
    m_newmsg_pos=-1;
    return 0;
  }
  return 1;
}

void C_MessageQueue::saturate(int satsize)
{
//  debug_printf("sending a %d byte saturation message\n",40+satsize);
  // add a message, saturation, baby
  T_Message satmsg={0,};
  satmsg.data=new C_SHBuf(satsize);
  if (satmsg.data)
  {
    R_GenerateBytes((unsigned char *)satmsg.data->Get(),satsize,&g_random);

          satmsg.message_type=MESSAGE_LOCAL_SATURATE;
        	satmsg.message_length=satmsg.data->GetLength();
    satmsg.message_ttl=1;
    CreateID128(&satmsg.message_guid);

    satmsg.message_prio=C_MessageQueueList::GetMessagePriority(satmsg.message_type);
    C_MessageQueue::calc_md5(&satmsg,satmsg.message_md5);

    satmsg.data->Lock();
    send_message(&satmsg);
    satmsg.data->Unlock();
  }
}


void C_MessageQueue::run(int isrecv, int maxbytesend)
{
  // recieve message
  if (isrecv)
  {
    if (m_newmsg_pos==-1)
    {
      if (m_con->recv_bytes_available() >= 40)
      {
        /*  pad to 128-bits boundary for AES encryption.   */
	int tsize = 16;
        unsigned char *t = new unsigned char[tsize];

        m_con->recv_bytes(&m_newmsg.message_md5,16);
        
        m_con->recv_bytes(t,tsize);
        
        
        m_newmsg.message_type=t[0]|(t[1]<<8)|(t[2]<<16)|(t[3]<<24);
        m_newmsg.message_prio=t[4];
        m_newmsg.message_length=t[5]|(t[6]<<8);
        m_newmsg.message_ttl=t[7];
        m_con->recv_bytes(&m_newmsg.message_guid,16);

        if (!m_newmsg.message_type ||
            m_newmsg.message_ttl > G_MAX_TTL ||
            m_newmsg.message_length < 0 || 
            m_newmsg.message_length > MESSAGE_MAX_PAYLOAD_ROUTE ||            
              (MESSAGE_TYPE_BCAST(m_newmsg.message_type) &&
               m_newmsg.message_length > MESSAGE_MAX_PAYLOAD_BCAST)
            )
        {       
          debug_printf("queue::run() got bad message type=%d, prio=%d, ttl=%d, len=%d\n", m_newmsg.message_type,m_newmsg.message_prio,m_newmsg.message_ttl,m_newmsg.message_length);        
          m_newmsg.message_length=0;
          m_con->close(1);
          return;
        }
        m_newmsg.data=new C_SHBuf(m_newmsg.message_length);
        if (!m_newmsg.data->Get())
        {
          delete m_newmsg.data;
          m_newmsg.message_length=0;
          m_con->close(1);
          return;
        }
        m_newmsg.data->Lock();
        m_newmsg_pos=0;
      }
    }

    /* pad to 128-bits boundary for AES encryption.   */
    int padlen = PAD16(m_newmsg.message_length);
    
    if (m_newmsg_pos >= 0 && m_newmsg_pos < padlen)
    {
      int len=padlen-m_newmsg_pos;
      int len2=m_con->recv_bytes_available();
      if (len > len2) len=len2;
      if (len > 0)
      {
        //debug_printf("queue::run() recv msg data with msglen %d\n", m_newmsg.message_length);
        m_con->recv_bytes((char*)m_newmsg.data->Get()+m_newmsg_pos,len);
        m_newmsg_pos+=len;
      }
      if (m_newmsg_pos >= padlen) // finish crc calculation
      {
        unsigned char buf[16];
        calc_md5(&m_newmsg,buf);
        if (memcmp(buf,m_newmsg.message_md5,16))
        {
          debug_printf("queue::run() got bad message (MD5 differs) type=%d, prio=%d, ttl=%d, len=%d\n", m_newmsg.message_type,m_newmsg.message_prio,m_newmsg.message_ttl,m_newmsg.message_length);        
          m_newmsg.message_length=0;
          m_newmsg_pos=-1;
          m_con->close(1);
          return;
        }
      }
    }
  }
  else
  {
    int do_saturate=(g_throttle_flag&32) && (m_con->get_saturatemode()&1);
    int satsize=min(m_con->getMaxSendSize()/64,216);
    if (satsize < 4) satsize=4;
    if (!m_msg_used && do_saturate && m_con->send_bytes_in_queue() < 40+satsize*2 && (maxbytesend < 0 || m_con->send_bytes_in_queue()+40+satsize < maxbytesend)) saturate(satsize);

    while (m_msg_used>0 && (maxbytesend<0 || m_con->send_bytes_in_queue() < maxbytesend))
    {
      if (m_msg_bsent<0)
      {
        if (m_con->send_bytes_available() >= 40)
        {
          /*  
             Pad to 128-bits boundary for AES encryption.
             Do not change to 128-bit boundary if the message is a switch to AES message 
             with the switch_flag = 1.
           */
          int tsize = 16;
          unsigned char *t = new unsigned char[tsize];
                            
          if (m_msg->switch_flag !=0)
            debug_printf("m_msg->switch_flag = %d\n",m_msg->switch_flag);
          //if (m_con->get_send_encryption() == PADLOCKSL_ENCRYPTION_AES)
            //debug_printf("m_con (%x )->get_send_encryption() == PADLOCKSL_ENCRYPTION_AES\n",m_con);

          m_con->send_bytes(&m_msg->message_md5,16,m_msg->switch_flag);
          //m_con->send_bytes(&m_msg->message_md5,16);
          
          t[0]=m_msg->message_type&0xff;
          t[1]=(m_msg->message_type>>8)&0xff;
          t[2]=(m_msg->message_type>>16)&0xff;
          t[3]=(m_msg->message_type>>24)&0xff;
          t[4]=m_msg->message_prio;
          t[5]=m_msg->message_length&0xff;
          t[6]=(m_msg->message_length>>8)&0xff;
          t[7]=m_msg->message_ttl;
          
          m_con->send_bytes(t,tsize,m_msg->switch_flag);

          m_con->send_bytes(&m_msg->message_guid,16,m_msg->switch_flag);
          //m_con->send_bytes(&m_msg->message_guid,16);
          
          m_msg_bsent=0;
          if (MESSAGE_TYPE_BCAST(m_msg->message_type) &&
              m_msg->message_length > MESSAGE_MAX_PAYLOAD_BCAST)
          {
            debug_printf("queue::run() send bcast payload length of %d too large\n",m_msg->message_length);
          }
          else if (m_msg->message_length > MESSAGE_MAX_PAYLOAD_ROUTE)
          {
            debug_printf("queue::run() send route/local payload length of %d too large\n",m_msg->message_length);
          }
        }
        else break;
      }
      if (m_msg_bsent>=0)
      {
        /* Pad to 128-bits boundary for AES encryption. */
        //debug_printf("queue::run() send msg data \n");
        int len = (PAD16(m_msg->message_length)-m_msg_bsent);
        
        int len2=m_con->send_bytes_available();
        if (len > len2) len=len2;
        int pad = 16;
        if (len >= pad)
        {
          //debug_printf("queue::run() send msg data with msglen %d\n", m_msg->message_length);
          
          m_con->send_bytes((char*)m_msg->data->Get()+m_msg_bsent,len, m_msg->switch_flag);
          //m_con->send_bytes((char*)m_msg->data->Get()+m_msg_bsent,len);
          m_msg_bsent += len;
        }
        if (m_msg_bsent >= m_msg->message_length)
        {
          m_msg_bsent=-1;
          removefirst();
          if (!m_msg_used && do_saturate && m_con->send_bytes_available() >= 40+satsize && m_con->send_bytes_in_queue() < 40+satsize*2 && (maxbytesend < 0 || m_con->send_bytes_in_queue()+40+satsize < maxbytesend)) saturate(satsize);
        }

        if (len2 < 8) break;
      }
    }
  }
  //debug_printf("C_MessageQueue::run() return \n");
}

#define __idcmp(x,y) memcmp((x),(y),16)

void C_MessageQueue::add_route(T_GUID *id, unsigned char msgtype)
{
  int r;
  int whichtab=msg2tab(msgtype);
  int maxi=find_route(&m_routes_order[whichtab][m_route_opos[whichtab]],whichtab);
  int offs=find_route(id,whichtab);

  if (offs < maxi) // shift everything to the right one
  {
    for (r = maxi; r > offs; r --)
    {
      m_routes[whichtab][r]=m_routes[whichtab][r-1];
    }
  }
  else if (maxi < offs) // shift everything to the left
  {
    offs--;
    for (r = maxi; r < offs; r ++)
    {
      m_routes[whichtab][r]=m_routes[whichtab][r+1];
    }
  }

  m_routes[whichtab][offs]=*id;
  m_routes_order[whichtab][m_route_opos[whichtab]]=*id;

  if (++m_route_opos[whichtab] >= m_num_routes[whichtab]) 
  {
//    debug_printf("Mqueue: m_route_opos[%d] wrapped around\n",whichtab);
    m_route_opos[whichtab]=0;
  }
}


// if idcmp is < 0 then the first parm is to the left of the secon parm

int C_MessageQueue::find_route(T_GUID *id, int whichtab)
{
  int pos=m_num_routes[whichtab]/2;
  int psize=pos/2;
  while (1)
  {
    int dir=__idcmp(id,&m_routes[whichtab][pos]);
    if (!dir||psize<2) 
    {
      if (dir < 0)
      {
        while (pos > 0)
        {
          int r=__idcmp(id,&m_routes[whichtab][pos-1]);
          if (!r) return pos-1;
          if (r > 0) break;
          pos--;
        }
      }
      if (dir > 0)
      {
        while (pos < m_num_routes[whichtab]-1)
        {
          int r=__idcmp(id,&m_routes[whichtab][pos+1]);
          if (r <= 0) return pos+1;
          pos++;
          if (pos == m_num_routes[whichtab]-1) return m_num_routes[whichtab];
        }
      }
      return pos;
    }
    if (dir > 0) pos+=psize;
    if (dir < 0) pos-=psize;
    psize/=2;
    if (pos < 0) return 0;
    if (pos >= m_num_routes[whichtab]) return m_num_routes[whichtab];
  }
}

int C_MessageQueue::is_route(T_GUID *id, unsigned char msgtype)
{
  int whichtab=msg2tab(msgtype);
  int r=find_route(id,whichtab);
  return (r != m_num_routes[whichtab] && !__idcmp(id,&m_routes[whichtab][r]));
} 

