/*
    WASTE - connection.cpp (Secured TCP connection class)
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
#include "main.h"
#include "util.h"
#include "connection.h"
#include "sockets.h"

extern "C" {
#include "rsa/r_random.h"
#include "rsa/rsa.h"
#include "C5P.h"
};

// incoming
C_Connection::C_Connection(int s, struct sockaddr_in *loc)
{
  m_send_encryption = 0;
  m_recv_encryption = 0;
  m_aes_keysize = 0;
  m_peer_padlocksl = FLAG_NON_PADLOCKSL;

  // Allocate 128-bit boundary address aligned.
  m_aes.key = (int*)c5p_malloc(56);
  memset(m_aes.key,0,56);
  m_aes.recv_iv = (int*)c5p_malloc(16);
  memset(m_aes.recv_iv,0,16);
  m_aes.send_iv = (int*)c5p_malloc(16);
  memset(m_aes.send_iv,0,16);
  

  do_init();
  m_socket=s;
  SET_SOCK_BLOCK(m_socket,0);
  m_remote_port=0;
  m_state=STATE_CONNECTED;
  m_dns=NULL;
  m_ever_connected=0;
  if (loc) m_saddr=*loc;
  else memset(&m_saddr,0,NULL);
  /* Added by Raymond 2003/12/12 for debug connection */
  //char *p = NULL;
  //debug_printf("New IP : %s \n", inet_ntoa(m_saddr.sin_addr));
  m_cansend=0;
}

// outgoing
C_Connection::C_Connection(char *hostname, int port, C_AsyncDNS *dns)
{
  m_send_encryption = 0;
  m_recv_encryption = 0;
  m_aes_keysize = 0;
  m_peer_padlocksl = FLAG_NON_PADLOCKSL;

  // Allocate 128-bit boundary address aligned.
  m_aes.key = (int*)c5p_malloc(56);
  memset(m_aes.key,0,56);
  m_aes.recv_iv = (int*)c5p_malloc(16);
  memset(m_aes.recv_iv,0,16);
  m_aes.send_iv = (int*)c5p_malloc(16);
  memset(m_aes.send_iv,0,16);
  

  do_init();
  m_cansend=1;
  m_state=STATE_ERROR;
  m_ever_connected=0;
  m_dns=dns;
  m_remote_port=(short)port;
  m_socket=socket(AF_INET,SOCK_STREAM,0);
  if (m_socket==-1)
  {
    debug_printf("connection: call to socket() failed: %d.\n",ERRNO);
  }
  else
  {
    SET_SOCK_BLOCK(m_socket,0);
    strcpy(m_host,hostname);
    memset(&m_saddr,0,sizeof(m_saddr));
                m_saddr.sin_family=AF_INET;
    m_saddr.sin_port=htons(port);
    m_saddr.sin_addr.s_addr=inet_addr(hostname);
    m_state=STATE_RESOLVING;
  }
}


void C_Connection::init_crypt()
{
  bf_initted=0;

  R_GenerateBytes((unsigned char *)m_send_buffer,CONNECTION_BFPUBKEY,&g_random);


  // according to X9.17, afaik.

  // first 16 bytes of m_send_buffer are randA
  // next 24(32) bytes of m_send_buffer are pubKey_A_HASH
  memcpy(m_send_buffer+CONNECTION_BFPUBKEY,g_pubkeyhash,SHA_OUTSIZE);


  if (g_use_networkhash)
  {
    // encrypt pubKey_A_HASH with network name, if necessary
    unsigned char g_networkhashkey[16];
    memcpy(g_networkhashkey,g_networkhash,16);
    int* buf = (int*)c5p_malloc(CONNECTION_PKEYHASHSIZE);
    AES_Result result = e_AES_Failed;
    if (c5p_aes_available())
    {
      memcpy((char *)buf,m_send_buffer+CONNECTION_BFPUBKEY,CONNECTION_PKEYHASHSIZE);
      result = c5p_aes_encrypt((int *)g_networkhashkey,
                               16,
                               e_aes_ecb,
                               buf,
                               CONNECTION_PKEYHASHSIZE,
                               buf,
                               NULL);
    }
    if (result != e_AES_Successed)
    {
      memcpy((char *)buf,m_send_buffer+CONNECTION_BFPUBKEY,CONNECTION_PKEYHASHSIZE);
      sft_aes_encrypt((int *)g_networkhashkey,
                               16,
                               e_aes_ecb,
                               buf,
                               CONNECTION_PKEYHASHSIZE,
                               buf,
                               NULL);
	  
    }
    memcpy(m_send_buffer+CONNECTION_BFPUBKEY,(char *)buf,CONNECTION_PKEYHASHSIZE);
    c5p_free(buf);
  }

      
  // encrypt pubkey_A_HASH with randA.

  int * tem = (int*)c5p_malloc(CONNECTION_PKEYHASHSIZE);
 
  AES_Result result = e_AES_Failed;
  if (c5p_aes_available())
  {
    memcpy(tem,m_send_buffer+CONNECTION_BFPUBKEY,CONNECTION_PKEYHASHSIZE);
    result = c5p_aes_encrypt((int *)m_send_buffer,
                               CONNECTION_BFPUBKEY,
                               e_aes_ecb,
                               tem,
                               CONNECTION_PKEYHASHSIZE,
                               tem,
                               NULL);
  }
  if (result != e_AES_Successed)
  {
    memcpy((char *)tem,m_send_buffer+CONNECTION_BFPUBKEY,CONNECTION_PKEYHASHSIZE);
    sft_aes_encrypt((int *)m_send_buffer,
                               CONNECTION_BFPUBKEY,
                               e_aes_ecb,
                               tem,
                               CONNECTION_PKEYHASHSIZE,
                               tem,
                               NULL);
	  
  }

  memcpy(m_send_buffer+CONNECTION_BFPUBKEY,(char *)tem, CONNECTION_PKEYHASHSIZE);
  c5p_free(tem);
  m_send_len=CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE+CONNECTION_PADSKEYSIZE; // fill in shit later
  //debug_printf("sending public key portion\n");
}

void C_Connection::init_crypt_gotpkey() // got their public key
{
  m_cansend=1;

  // check to make sure randB != randA
  if (!memcmp(m_recv_buffer,m_send_buffer,CONNECTION_BFPUBKEY))
  {
    debug_printf("connection: got what looks like feedback on bfpubkey.\n");
    close(1);
    return;
  }


  // get pubkey_B_HASH
  memcpy(m_remote_pkey,m_recv_buffer+CONNECTION_BFPUBKEY,CONNECTION_PKEYHASHSIZE);
  
  // decrypt pubkey_B_HASH with randB
  int* buf = (int*)c5p_malloc(CONNECTION_PKEYHASHSIZE);
  AES_Result result = e_AES_Failed;
  if (c5p_aes_available())
  {
    memcpy((char *)buf,m_remote_pkey,CONNECTION_PKEYHASHSIZE);
    result = c5p_aes_decrypt((int *)m_recv_buffer,
                               CONNECTION_BFPUBKEY,
                               e_aes_ecb,
                               buf,
                               CONNECTION_PKEYHASHSIZE,
                               buf,
                               NULL);
  }
  if (result != e_AES_Successed)
  {
    memcpy((char *)buf,m_remote_pkey,CONNECTION_PKEYHASHSIZE);
    sft_aes_decrypt((int *)m_recv_buffer,
                               CONNECTION_BFPUBKEY,
                               e_aes_ecb,
                               buf,
                               CONNECTION_PKEYHASHSIZE,
                               buf,
                               NULL);
	  
  }
  memcpy(m_remote_pkey,(char *)buf,CONNECTION_PKEYHASHSIZE);
  c5p_free(buf);

  // decrypt pubkey_B_HASH with network name. if necessary
  if (g_use_networkhash)
  {
   unsigned char g_networkhashkey[16];
   memcpy(g_networkhashkey,g_networkhash,16);
   int* buffer = (int*)c5p_malloc(CONNECTION_PKEYHASHSIZE);
   AES_Result res = e_AES_Failed;
   if (c5p_aes_available())
   {
     memcpy((char *)buffer,m_remote_pkey,CONNECTION_PKEYHASHSIZE);
     res = c5p_aes_decrypt((int *)m_recv_buffer,
                               16,
                               e_aes_ecb,
                               buffer,
                               CONNECTION_PKEYHASHSIZE,
                               buffer,
                               NULL);
   }
   if (res != e_AES_Successed)
   {
     memcpy((char *)buffer,m_remote_pkey,CONNECTION_PKEYHASHSIZE);
     sft_aes_decrypt((int *)m_recv_buffer,
                               16,
                               e_aes_ecb,
                               buffer,
                               CONNECTION_PKEYHASHSIZE,
                               buffer,
                               NULL);
	  
   }
   memcpy(m_remote_pkey,(char *)buffer,CONNECTION_PKEYHASHSIZE);
   c5p_free(buffer);
 }


  // find pubkey_B_HASH
  if (!findPublicKey(m_remote_pkey,&remote_pubkey))
  {
    close(1);
    return;
  }


  // at this point, generate my session key and IV (sKeyA)
  R_GenerateBytes(m_mykeyinfo, CONNECTION_KEYSIZE+8, &g_random);

  // according to X9.17, afaik.

  // save a copy of the last 8 bytes of sKeyA for the send IV
  memcpy(m_send_cbc,m_mykeyinfo+CONNECTION_KEYSIZE,8);

  // append randB to sKeyA.
  memcpy(m_mykeyinfo+CONNECTION_KEYSIZE+8,m_recv_buffer,CONNECTION_BFPUBKEY);

  // create a working copy of sKeyA+randB
  unsigned char localkeyinfo[CONNECTION_KEYSIZE+8+CONNECTION_BFPUBKEY];
  memcpy(localkeyinfo,m_mykeyinfo,sizeof(localkeyinfo));

  // if using a network name, encrypt the first 56 bytes of the sKeyA portion of localkeyinfo
  if (g_use_networkhash)
  {
    unsigned char g_networkhashkey[16];
    memcpy(g_networkhashkey,g_networkhash,16);
    int* tem = (int*)c5p_malloc(32);
    AES_Result res = e_AES_Failed;
    if (c5p_aes_available())
    {
      memcpy((char *)tem,localkeyinfo,32);
      res = c5p_aes_encrypt((int *)g_networkhashkey,
                               16,
                               e_aes_ecb,
                               tem,
                               32,
                               tem,
                               NULL);
    }
    if (res != e_AES_Successed)
    {
       memcpy((char *)tem,localkeyinfo,32);
       sft_aes_encrypt((int *)g_networkhashkey,
                               16,
                               e_aes_ecb,
                               tem,
                               32,
                               tem,
                               NULL);
	  
    }
    memcpy(localkeyinfo,(char *)tem,32);
    c5p_free(tem);
  }

  // encrypt using pubkey_B, the local key info (sKeyA + randB), (where sKeyA might be encrypted using network name)
  unsigned int l=0;
  if (RSAPublicEncrypt((unsigned char *)m_send_buffer+CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE,&l,localkeyinfo,CONNECTION_KEYSIZE+8+CONNECTION_BFPUBKEY,&remote_pubkey,&g_random))
  {
    debug_printf("connection: error encrypting session key\n");
  }
  memset(localkeyinfo,0,sizeof(localkeyinfo));
}



void C_Connection::init_crypt_decodekey() // got their encrypted session key for me to use
{
  unsigned char m_key[MAX_RSA_MODULUS_LEN];
  unsigned int m_kl=0;
  int err=0;

  // decrypt with my private key to get sKeyB + randA (where sKeyB might be encrypted using network name)
  if ((err=RSAPrivateDecrypt(m_key,&m_kl,(unsigned char *)m_recv_buffer+CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE,
                              (g_key.bits + 7) / 8,&g_key)) || 
        m_kl != CONNECTION_KEYSIZE+8+CONNECTION_BFPUBKEY) //error decrypting
  {
    debug_printf("connection: error decrypting session key with my own private key (%d,%d)\n",err,m_kl);
    close(1);
    return;
  }


  //decrypt to get sKeyB if necessary
  if (g_use_networkhash)
  {
    unsigned char g_networkhashkey[16];
    memcpy(g_networkhashkey,g_networkhash,16);
    int* tem = (int*)c5p_malloc(32);
    AES_Result res = e_AES_Failed;
    if (c5p_aes_available())
    {
      memcpy((char *)tem,m_key,32);
      res = c5p_aes_decrypt((int *)g_networkhashkey,
                               16,
                               e_aes_ecb,
                               tem,
                               32,
                               tem,
                               NULL);
    }
    if (res != e_AES_Successed)
    {
       memcpy((char *)tem,m_key,32);
       sft_aes_decrypt((int *)g_networkhashkey,
                               16,
                               e_aes_ecb,
                               tem,
                               32,
                               tem,
                               NULL);
	  
    }
    memcpy(m_key,(char *)tem,32);
    c5p_free(tem);
  }

  // use the last 16 bytes of sKeyB for aes recv IV
  memcpy((char*)m_aes.send_iv,m_key+CONNECTION_KEYSIZE-8,16);

  
  // combine the first 56 bytes of sKeyA and sKeyB to get our session key
  // check to make sure it is nonzero.
  int zeros=0;
  int x;
  for (x = 0; x < CONNECTION_KEYSIZE; x ++)
  {
    m_key[x]^=m_mykeyinfo[x];
    if (!m_key[x]) zeros++;
  }       

  if (zeros == CONNECTION_KEYSIZE)
  {
    debug_printf("connection: zero session key, aborting.\n");
    close(1);
    return;
  }

  // save a copy of the last 8 bytes of sKeyB for the recv IV.
  memcpy(m_recv_cbc,m_key+CONNECTION_KEYSIZE,8);

  if (!memcmp(m_recv_cbc,m_send_cbc,8))
  {
    debug_printf("connection: CBC IVs are equal, being hacked?\n");
    close(1);
    return;
  }

  // check to see the decrypted randA is the same as the randA we sent
  if (memcmp(m_key+CONNECTION_KEYSIZE+8,m_send_buffer,CONNECTION_BFPUBKEY))
  {
    debug_printf("connection: error decrypting session key (signature is wrong, being hacked?)\n");
    close(1);
    return;
  }

  m_ever_connected=1;

  // use the 56 Bytes of m_key to initiate the aes_key
  memcpy(m_aes.key, m_key,CONNECTION_KEYSIZE);
  // use the last 16 bytes of sKeyA for the send IV
  memcpy(m_aes.recv_iv,m_mykeyinfo+CONNECTION_KEYSIZE-8,16);
  
}


void C_Connection::do_init(void)
{
  int x;
  m_satmode=0;
  m_remote_maxsend=2048;
  for (x = 0 ; x < sizeof(bps_count)/sizeof(bps_count[0]); x ++)
  {
    bps_count[x].recv_bytes=0;
    bps_count[x].send_bytes=0;
    bps_count[x].time_ms=GetTickCount();
  }
  bps_count_pos=0;

  memset(m_recv_buffer,0,sizeof(m_recv_buffer));
  memset(m_send_buffer,0,sizeof(m_send_buffer));
  m_recv_len=m_recv_pos=0;
  m_send_len=m_send_pos=0;
  m_send_bytes_total=0;
  m_recv_bytes_total=0;
  m_start_time=GetTickCount();
  init_crypt();
}

C_Connection::~C_Connection()
{
  if (m_socket >= 0)
  {
    shutdown(m_socket, SHUT_RDWR );
    closesocket(m_socket);
  }

  c5p_free(m_aes.key);
  c5p_free(m_aes.send_iv);
  c5p_free(m_aes.recv_iv);
  
}

void C_Connection::calc_bps(int *send, int *recv)
{
  unsigned int now=GetTickCount();
  unsigned int timediff=now-bps_count[bps_count_pos].time_ms;
  if (timediff < 1) timediff=1;

#ifdef _WIN32
  m_last_sendbps=MulDiv(m_send_bytes_total-bps_count[bps_count_pos].send_bytes,1000,timediff);
  m_last_recvbps=MulDiv(m_recv_bytes_total-bps_count[bps_count_pos].recv_bytes,1000,timediff);
#else
  // fixme: enough prec?
  m_last_sendbps=(((m_send_bytes_total-bps_count[bps_count_pos].send_bytes)*1000)/timediff);
  m_last_recvbps=(((m_recv_bytes_total-bps_count[bps_count_pos].recv_bytes)*1000)/timediff);
#endif

  if (send) *send=m_last_sendbps;
  if (recv) *recv=m_last_recvbps;

  int l=bps_count_pos-1;
  if (l<0) l=sizeof(bps_count)/sizeof(bps_count[0])-1;

  if (now - bps_count[l].time_ms < 0 ||
      now - bps_count[l].time_ms >= 100)
  {
    bps_count[bps_count_pos].send_bytes=m_send_bytes_total;
    bps_count[bps_count_pos].recv_bytes=m_recv_bytes_total;
    bps_count[bps_count_pos].time_ms=now;
    bps_count_pos++;
    if (bps_count_pos>=sizeof(bps_count)/sizeof(bps_count[0])) bps_count_pos=0;
  }
}


C_Connection::state C_Connection::run(int max_send_bytes, int max_recv_bytes)
{
  int bytes_allowed_to_send=(max_send_bytes<0||max_send_bytes>MAX_CONNECTION_SENDSIZE)?MAX_CONNECTION_SENDSIZE:max_send_bytes;
  int bytes_allowed_to_recv=(max_recv_bytes<0||max_recv_bytes>MAX_CONNECTION_RECVSIZE)?MAX_CONNECTION_RECVSIZE:max_recv_bytes;  

  if (GetTickCount() - m_start_time > 10*1000 && bf_initted<12)
  {
    return m_state=STATE_ERROR;
  }

  if (m_socket==-1 || m_state==STATE_ERROR)
  {
    return STATE_ERROR;
  }

  if (m_state == STATE_RESOLVING)
  {
    if (!m_host[0]) return (m_state=STATE_ERROR);
    if (m_saddr.sin_addr.s_addr == INADDR_NONE)
    {
      int a=m_dns?m_dns->resolve(m_host,(unsigned long int *)&m_saddr.sin_addr.s_addr):-1;
      switch (a)
      {
        case 0: m_state=STATE_CONNECTING; break;
        case 1: return STATE_RESOLVING;
        case -1: 
          debug_printf("connection: error resolving hostname\n");
        return (m_state=STATE_ERROR);
      }
    }
    int res=::connect(m_socket,(struct sockaddr *)&m_saddr,16);
    if (!res) 
    {
      m_state=STATE_CONNECTED;
    }
    else if (ERRNO!=EINPROGRESS)
    {
      debug_printf("connection: connect() returned error: %d\n",ERRNO);
      return (m_state=STATE_ERROR);
    }
    else {
      	 debug_printf("connection: connect() returned CONNING: %d\n",ERRNO);
	    m_state=STATE_CONNECTING;
    }
      	 debug_printf("connection: connect() returned in resolving: \n");
    return m_state;
  }
  if (m_state == STATE_CONNECTING)
  {        	
		fd_set f,f2;
		FD_ZERO(&f);
		FD_ZERO(&f2);
		FD_SET(m_socket,&f);
		FD_SET(m_socket,&f2);
    struct timeval tv;
    memset(&tv,0,sizeof(tv));

    if (select(m_socket + 1,NULL,&f,&f2,&tv)==-1) 
    {
      debug_printf("connection::run: select() returned error: %d\n",ERRNO);
      return (m_state=STATE_ERROR);
    }
    if (FD_ISSET(m_socket,&f2))
    {
      debug_printf("connection::run: select() notified of error\n");
      return (m_state=STATE_ERROR);
    }
    if (!FD_ISSET(m_socket,&f))
    {
      //debug_printf("connection::run: before return coning\n");
      return STATE_CONNECTING;
    }
    m_state=STATE_CONNECTED;
  }
  int active=0;
  if (m_state == STATE_CONNECTED || m_state == STATE_CLOSING)
  {
    if (m_send_len>0 && m_cansend)
    {
      int len=MAX_CONNECTION_SENDSIZE-m_send_pos;
      if (len > m_send_len) 
      {
        len=m_send_len;
      }

      if (bf_initted<3)
      {
        int l=CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE-m_send_pos;
        if (len > l) len=l;
        if (len<0) len=0;
      }
      else if (bf_initted < 12) // session key negotiation
      {
        int l=CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE+CONNECTION_PADSKEYSIZE-m_send_pos;
        if (len > l) len=l;
        if (len<0) len=0;
      }

      if (len > bytes_allowed_to_send)
      {
        len=bytes_allowed_to_send;
      }
      if (len > 0)
      {
        int res=::send(m_socket,m_send_buffer+m_send_pos,len,0);
        if (res==-1 && ERRNO != EWOULDBLOCK)
        {
          debug_printf("connection::run: send(%d) returned error: %d\n",len,ERRNO);
          return STATE_CLOSED;
        }
        if (res>0)
        {
          //debug_printf("connection(%x)::run: send(%d) res  %d\n",this,len, res);

          bytes_allowed_to_send-=res;
          m_send_bytes_total+=res;
          m_send_pos+=res;
          m_send_len-=res;
          if (res == len) active=1;
          if (bf_initted < 3)
          {
            if (m_send_pos >= CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE) 
            {
              debug_printf("sent all of my pubkey\n");
              bf_initted|=2;
            }
          }
          if (bf_initted < 12)
          {
            if (m_send_pos >= CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE+CONNECTION_PADSKEYSIZE) 
            {
              debug_printf("sent all of my encrypted session key\n");
              bf_initted|=8;
            }
          }
          if (m_send_pos >= MAX_CONNECTION_SENDSIZE) m_send_pos=0;
        }
      }
    }

    int len=MAX_CONNECTION_RECVSIZE-m_recv_len;
    if (len > 0)
    {
      if (len > bytes_allowed_to_recv) len=bytes_allowed_to_recv;

      char rbuf[MAX_CONNECTION_RECVSIZE];
 
      int res=-1;
      if (len > 0)
      {
        res=::recv(m_socket,rbuf,len,0);
        if (res == 0)
        {        
          debug_printf("connection::run2: recv(%d) returned 0!\n",len);
          return (m_state=STATE_CLOSED);
        }
        if (res < 0 && ERRNO != EWOULDBLOCK) 
        {
          debug_printf("connection::run: recv() returned error %d!\n",ERRNO);
          return (m_state=STATE_CLOSED);
        }
      }
      if (res > 0)
      {
        //debug_printf("connection(%x)::run: recv(%d) res  %d\n",this, len, res);

        if (res == len) active=1;
       
        if (m_recv_pos+res >= MAX_CONNECTION_RECVSIZE) //wrappy
        {
          int l=m_recv_pos+res-MAX_CONNECTION_RECVSIZE;
          memcpy(m_recv_buffer+m_recv_pos,rbuf,res-l);
          if (l) 
          {
            memcpy(m_recv_buffer,rbuf+res-l,l);
          }
          m_recv_pos=l;
          debug_printf("wrapped\n");
        }
        else
        {
          memcpy(m_recv_buffer+m_recv_pos,rbuf,res);
          m_recv_pos+=res;
        }
        bytes_allowed_to_recv-=res;
        m_recv_bytes_total+=res;
        m_recv_len+=res;

        if (bf_initted < 3) 
        {
          if (!(bf_initted&1) && m_recv_pos >= CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE) // get our key
          {
            m_recv_len-=CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE;
            init_crypt_gotpkey();
            bf_initted|=1;
          }
        }
        if (bf_initted < 12 && !(bf_initted&4))
        {
          if (m_recv_pos >= CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE+CONNECTION_PADSKEYSIZE) // get our key
          {
            bf_initted|=4;
            init_crypt_decodekey();
            m_recv_len-=CONNECTION_PADSKEYSIZE;

            int msp=m_send_pos, msl=m_send_len;
            if (msp < CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE+CONNECTION_PADSKEYSIZE) // only encrypt after first bytes
            {
              msl -= (CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE+CONNECTION_PADSKEYSIZE-msp);
              msp = CONNECTION_BFPUBKEY+CONNECTION_PKEYHASHSIZE+CONNECTION_PADSKEYSIZE;
            }

            if (msl>0)
            {
              debug_printf("encrypting %d bytes at %d\n",msl,msp);
              encrypt_buffer(m_send_buffer+msp,msl);
            }
          }
        }
      }
    }
    if (m_state == STATE_CLOSING)
    {
      if (m_send_len < 1)
      {
        return (m_state = STATE_CLOSED);
      }
      return STATE_CLOSING;
    }
    return active?STATE_CONNECTED:STATE_IDLE;
  }
  debug_printf("before last return\n");
  return m_state;
}

void C_Connection::close(int quick)
{
  if (m_state == STATE_RESOLVING || m_state == STATE_CONNECTING ||
      (m_state != STATE_ERROR && quick))
    m_state=STATE_CLOSED;
  else if (m_state != STATE_CLOSED && m_state != STATE_ERROR)
    m_state=STATE_CLOSING;
}

int C_Connection::send_bytes_in_queue(void)
{
  return m_send_len;
}

int C_Connection::getMaxSendSize(void)
{
  int a=MAX_CONNECTION_SENDSIZE;
  if (g_conspeed<64)a=512; //modems only let sendahead be 512 or so.
  else if (g_conspeed<384)a=2048;
  else if (g_conspeed<1600)a=4096;
  else if (g_conspeed<20000)a=8192;
  if (a > m_remote_maxsend) a=m_remote_maxsend;
//  debug_printf("max_size (%d,%d)\n",a,m_remote_maxsend);
  return a;
}

int C_Connection::send_bytes_available(void)
{
  extern int g_conspeed;
  int a=getMaxSendSize();

  a-=m_send_len;
  if (a<0)a=0;
 
  
  /* For 128-bits boundary. */
  //return (m_send_encryption == PADLOCKSL_ENCRYPTION_AES ? a&(~15) : a&(~7));  
  a &=~15;
  //a &= ~7;
  return a;
}

/* Modified by LoraYin 2003/11/27 for switch to AES message */
int C_Connection::send_bytes(void *data, int length, int switch_flag)
{

  char tmp[MAX_CONNECTION_SENDSIZE];
  if (!length) return 0;
  memcpy(tmp,data,length);

  /* Set the switch flag of encrypt_buffer() */
  if (bf_initted>=12) encrypt_buffer(tmp,length,switch_flag);	
  //if (bf_initted>=12) encrypt_buffer(tmp,length);
  
  int write_pos=m_send_pos+m_send_len;
  if (write_pos >= MAX_CONNECTION_SENDSIZE) 
  {
    write_pos-=MAX_CONNECTION_SENDSIZE;
  }

  int len=MAX_CONNECTION_SENDSIZE-write_pos;
  if (len > length) 
  {
    len=length;
  }
    
  memcpy(m_send_buffer+write_pos,tmp,len);

  if (length > len)
  {
    memcpy(m_send_buffer,tmp+len,length-len);
  }
  m_send_len+=length;

  return 0;
}


void C_Connection::decrypt_buffer(void *data, int len)
{
  //debug_printf("connection(%x)::decrypt_buffer() , datalen = %d\n",this,len);

  if (len & 15)
  {
    debug_printf("connection::decrypt_buffer(): len=%d (&15)\n",len);
  }

  
  if (m_recv_encryption == PADLOCKSL_ENCRYPTION_AES) 
  {
    //128-bit boundary address aligned. 
    int* buf = (int*)c5p_malloc(len);
    AES_Result result = e_AES_Failed;
	
    if (c5p_aes_available())
    {
      //use C5P AES to decrypt data
      int temp_iv[4];
      memcpy(temp_iv, m_aes.recv_iv,16);

      memcpy(buf,data,len);
      result = c5p_aes_decrypt(m_aes.key,
                               m_aes_keysize,
                               e_aes_cbc,
                               buf,
                               len,
                               buf,
                               m_aes.recv_iv);

      if (result != e_AES_Successed)
      {
        debug_printf("Error: connection(%x)::decrypt_buffer(): c5p_aes_decrypt() return %d !\n",this,result);
        memcpy(m_aes.recv_iv,temp_iv,16);
      }
    }
    // if c5p aes is not available or the c5p_aes_encrypt result is error,
    // use soft aes.
    if (result != e_AES_Successed)
    {
      memcpy(buf,data,len);

      sft_aes_decrypt(m_aes.key,
                      m_aes_keysize,
                      e_aes_cbc,
                      buf,
                      len,
                      buf,
                      m_aes.recv_iv);
  
    }
    
    // use the last block (16 bytes) of encrypted data as the next recv iv.
    memcpy(m_aes.recv_iv,((unsigned char *)data) + len - 16,16);

    // copy the decrypted orig data to the data buffer.
    memcpy(data,buf,len);
    c5p_free(buf);

    return;
  }
  

  char *t=(char *)data;
  int* tem_buf = (int*)c5p_malloc(len);
 
  AES_Result aes_result = e_AES_Failed;

  if (c5p_aes_available())
  {
    memcpy((char *)tem_buf,t,len);
    aes_result = c5p_aes_decrypt(m_aes.key,
                               16,
                               e_aes_ecb,
                               tem_buf,
                               len,
                               tem_buf,
                               NULL);
 }
 if (aes_result != e_AES_Successed)
 {
   memcpy((char *)tem_buf,t,len);
   sft_aes_decrypt(m_aes.key,
                      16,
                      e_aes_ecb,
                      tem_buf,
                      len,
                      tem_buf,
                      NULL);
	  
 }

 memcpy(t,(char *)tem_buf,len);
 c5p_free(tem_buf);

}

/* Modified by LoraYin 2003/11/27 for switch to AES. 
   if switch_flag !=0 means the data is part of a switch massages
   which indicate that we are going to use AES after this messsge. 
   The message itself can not be encrypted by AES !!
*/
void C_Connection::encrypt_buffer(void *data, int len, int switch_flag)

{
  
  //debug_printf("connection(%x)::encrypt_buffer() , datalen = %d\n",this,len);
 
  if (len & 15)
  {
    debug_printf("connection::encrypt_buffer(): len=%d (&15)\n",len);
  }

  /* 
     set m_send_encryption = AES if get switch_flag == 1 (means it begin to 
	 send the switch message now.)
  */
  if ((m_send_encryption != PADLOCKSL_ENCRYPTION_AES) && (switch_flag == 1))
    m_send_encryption = PADLOCKSL_ENCRYPTION_AES;
  
  if ((m_send_encryption == PADLOCKSL_ENCRYPTION_AES) && (switch_flag == 0))
  {
    int* buf = (int*)c5p_malloc(len);
    AES_Result result = e_AES_Failed;

    if (c5p_aes_available())
    {
      //use C5P AES to encrypt
      int temp_iv[4];
      memcpy(temp_iv, m_aes.send_iv,16);
      memcpy(buf,data,len);
      result = c5p_aes_encrypt(m_aes.key,
                               m_aes_keysize,
                               e_aes_cbc,
                               buf,
                               len,
                               buf,
                               m_aes.send_iv);
      if (result != e_AES_Successed)
      {
        debug_printf("Error: connection(%x)::encrypt_buffer(): c5p_aes_encrypt() return %d !\n",this,result);
        memcpy(m_aes.send_iv,temp_iv,16);
      }
   
    }
    
    // if c5p aes is not available or the c5p_aes_encrypt result is error,
    // use soft aes.
    if (result != e_AES_Successed)
    {
      memcpy(buf,data,len);
      sft_aes_encrypt(m_aes.key,
                      m_aes_keysize,
                      e_aes_cbc,
                      buf,
                      len,
                      buf,
                      m_aes.send_iv);
	  
    }
    // copy the encrypted data to the data buffer.
    memcpy(data,buf,len);

    // use the last block (16 bytes) of encrypted data as the next send iv.
    memcpy((unsigned char*)m_aes.send_iv,(unsigned char*)buf + len - 16,16);
    c5p_free(buf);
    return;
  }
  if (switch_flag != 0)
    debug_printf("connection(%x)::encrypt_buffer(): switch_flag = %d \n",this,switch_flag);
  

  char *t=(char *)data;
  int* tem_buf = (int*)c5p_malloc(len);
  AES_Result aes_result = e_AES_Failed;
  if (c5p_aes_available())
    {
      memcpy((char *)tem_buf,t,len);
      aes_result = c5p_aes_encrypt(m_aes.key,
                               16,
                               e_aes_ecb,
                               tem_buf,
                               len,
                               tem_buf,
                               m_aes.send_iv);
    }
  if (aes_result != e_AES_Successed)
    {
      memcpy((char *)tem_buf,t,len);
      sft_aes_encrypt(m_aes.key,
                      16,
                      e_aes_ecb,
                      tem_buf,
                      len,
                      tem_buf,
                      m_aes.send_iv);
	  
    }
    memcpy(t,(char *)tem_buf,len);
    c5p_free(tem_buf);

}



int C_Connection::recv_bytes_available(void)
{
  if (bf_initted < 12) return 0;
  /* For 128-bits boundary. */
  //return (m_recv_encryption == PADLOCKSL_ENCRYPTION_AES ? m_recv_len&(~15) : m_recv_len&(~7));
  return m_recv_len&(~15);
  //return m_recv_len&(~7);
}

int C_Connection::recv_bytes(void *data, int maxlength)
{  
  
  if (bf_initted < 12) return -1;
  if (maxlength > m_recv_len)
  {
    maxlength=m_recv_len;
  }
  int read_pos=m_recv_pos-m_recv_len;
  if (read_pos < 0) 
  {
    read_pos += MAX_CONNECTION_RECVSIZE;
  }
  int len=MAX_CONNECTION_RECVSIZE-read_pos;
  if (len > maxlength)
  {
    len=maxlength;
  }
  memcpy(data,m_recv_buffer+read_pos,len);
  if (len < maxlength)
  {
    memcpy((char*)data+len,m_recv_buffer,maxlength-len);
  }
     
  decrypt_buffer(data,maxlength);

  m_recv_len-=maxlength;
  
  return maxlength;
}


unsigned long C_Connection::get_interface(void)
{
  if (m_socket==-1) return 0;
  struct sockaddr_in sin;
  memset(&sin,0,sizeof(sin));
  int len=16;
  if (::getsockname(m_socket,(struct sockaddr *)&sin,(socklen_t *)&len)) return 0;
  return (unsigned long) sin.sin_addr.s_addr;
}


void C_Connection::set_aes_keysize(int keysize)
{
  m_aes_keysize = (keysize > g_aes_keysize) ? keysize : g_aes_keysize;
}
