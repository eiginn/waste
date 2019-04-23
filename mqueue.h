/*
    WASTE - mqueue.h (Class that handles sending messages over a connection)
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


#ifndef _C_MQUEUE_H_
#define _C_MQUEUE_H_

#include "util.h"
#include "connection.h"
#include "shbuf.h"


#define G_DEFAULT_TTL (96)

#define G_MAX_TTL (G_DEFAULT_TTL)


// message types. 
// lowest bit=routed, second lowest bit = local (unless low bit set)

#define MESSAGE_LOCAL_CAPS          (10)
#define MESSAGE_UPLOAD              (16)
#define MESSAGE_CHAT                (32)
#define MESSAGE_CHAT_REPLY          (32|1)
#define MESSAGE_KEYDIST             (36)
#define MESSAGE_KEYDIST_REPLY       (36|1)
#define MESSAGE_PING                (40)
#define MESSAGE_SEARCH              (64)
#define MESSAGE_SEARCH_REPLY        (64|1)
#define MESSAGE_SEARCH_USERLIST     (68)
#define MESSAGE_FILE_REQUEST        (72)
#define MESSAGE_FILE_REQUEST_REPLY  (72|1)
#define MESSAGE_LOCAL_SATURATE      (254)
// we have room for another few billion types of messages, phew.



#define MESSAGE_TYPE_ROUTED(x) ((x)&1)
#define MESSAGE_TYPE_BCAST(x) (!((x)&3))
#define MESSAGE_TYPE_LOCAL(x) (((x)&3)==2)

#define MESSAGE_MAX_PAYLOAD_ROUTE 32768
#define MESSAGE_MAX_PAYLOAD_BCAST 2048


// priorities for the messages
#define MSGPRIO_LOCAL_CAPS 4 // very high
#define MSGPRIO_UPLOAD 8 // pretty high priority.
#define MSGPRIO_CHAT 24
#define MSGPRIO_CHAT_REPLY 36
#define MSGPRIO_KEYDIST_REPLY 40
#define MSGPRIO_KEYDIST 44
#define MSGPRIO_PING 54
#define MSGPRIO_SEARCH 64
#define MSGPRIO_SEARCH_USERLIST 68
#define MSGPRIO_SEARCH_REPLY 80
#define MSGPRIO_FILE_REQUEST 96
#define MSGPRIO_FILE_REQUEST_REPLY 110
#define MSGPRIO_LOCAL_SATURATE 255 // lowest possible priority



typedef struct // the header is now 40 bytes per packet. this is kinda a lot, heh. oh well.
{
  // data sent over network
  unsigned char message_md5[16];

  // crc applied to the following
  unsigned char message_prio;
  unsigned int message_type;
  int message_length; //actually encoded as two bytes
  unsigned char message_ttl;

  T_GUID message_guid;

  // etc
  C_SHBuf *data;
  
  // Added by LoraYin 2003/11/26 to indicate AES switch.
  int switch_flag;
} T_Message;



enum {
  ROUTETAB_GENERAL,
  ROUTETAB_PING,
  ROUTETAB_FILE,
  ROUTETAB_NUM
};

class C_MessageQueue
{
  public:
    C_MessageQueue(C_Connection *con, int maxmsg=256, int maxroute=8192);
    ~C_MessageQueue();
    static void calc_md5(T_Message *msg, unsigned char buf[16]);
    int send_message(T_Message *msg); // return 1 on error (full)
    int recv_message(T_Message *msg); // return 0 if message filled     
    C_Connection *get_con() { return m_con; }

    void run(int isrecv, int maxbytesend);

    void add_route(T_GUID *id, unsigned char msgtype);
    int is_route(T_GUID *id, unsigned char msgtype);

    int get_stat_recv(void) { return m_stat_recv; }
    int get_stat_send(void) { return m_stat_send; }
    int get_stat_drop(void) { return m_stat_drop; }

    int getlen(void) { return m_msg_used; }
    int getmaxlen(void) {  return m_msg_len; }

  protected:
// list
    T_Message *get(int x) { if (x >= 0 && x < m_msg_used) return m_msg+x; return 0; }

    void saturate(int satsize);

    void insert(int pos, T_Message *msg) 
    {
      if (pos >= 0 && pos <= m_msg_used && pos < m_msg_len) 
      {
        if (m_msg_used<m_msg_len) m_msg_used++;
        else
        {
          if (m_msg[m_msg_len-1].data) m_msg[m_msg_len-1].data->Unlock();
        }
        int x;
        if (m_msg_used-1>pos) for (x = m_msg_used-1; x > pos; x--) m_msg[x]=m_msg[x-1];
        m_msg[pos]=*msg;
        if (m_msg[pos].data) m_msg[pos].data->Lock();
      }
    };
    void removefirst() 
    { 
      if (m_msg_used>0) 
      { 
        if (m_msg->data) m_msg->data->Unlock();
        m_msg_used--; 
        if (m_msg_used) memcpy(m_msg,m_msg+1,sizeof(T_Message)*m_msg_used); 
      } 
    };
    T_Message *m_msg;
    int m_msg_len, m_msg_used;


    int msg2tab(unsigned char msg)
    {
      if (msg == MESSAGE_PING) return ROUTETAB_PING;
      if (msg == MESSAGE_FILE_REQUEST || msg == MESSAGE_FILE_REQUEST_REPLY) return ROUTETAB_FILE;
      return ROUTETAB_GENERAL; // todo: more route tables? :)
    }


    T_GUID *m_routes[ROUTETAB_NUM];
    T_GUID *m_routes_order[ROUTETAB_NUM];
    int m_num_routes[ROUTETAB_NUM], m_route_opos[ROUTETAB_NUM];

    int find_route(T_GUID *id, int whichtab); // returns index of where route should go (or is)

    C_Connection *m_con;
    
    int m_msg_bsent;
    T_Message m_newmsg;
    int m_newmsg_pos;

    int m_stat_recv;
    int m_stat_send,m_stat_drop;


};

#endif //_C_MQUEUE_H_
