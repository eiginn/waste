/*
    WASTE - connection.h (Secured TCP connection class)
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
    $Id: connection.h,v 1.6 2004/02/11 02:57:30 lorayin Exp $
*/

#ifndef _C_CONNECTION_H_
#define _C_CONNECTION_H_

#include "asyncdns.h"
#include "rsa/global.h"
#include "rsa/rsaref.h"
#include "sha.h"

#define MAX_CONNECTION_SENDSIZE 32768
#define MAX_CONNECTION_RECVSIZE 32768

//size of key that encrypts the hash of public key, bytes
#define CONNECTION_BFPUBKEY 16   

// size of hash of public key to be sent
#define CONNECTION_PKEYHASHSIZE ((SHA_OUTSIZE+15)&~15) 

// max RSA session key encrypted size (max keysize in bytes)
#define CONNECTION_PADSKEYSIZE (4096/8) 

// size of actual session key, bytes
#define CONNECTION_KEYSIZE 56           


// encryption algorithm type value.
#define PADLOCKSL_ENCRYPTION_AES	  0x0001        // AES

// flag indicate the program is PadLockSL or not.
#define FLAG_NON_PADLOCKSL	0x0000
#define FLAG_PADLOCKSL		0x0001

// context for AES encryption.
typedef struct {
  int *key;        // key 
  int *recv_iv;    // receive IV
  int *send_iv;    // send IV
} AES_CTX;


class C_Connection
{
  public:
    typedef enum 
    { 
      STATE_ERROR, STATE_RESOLVING, STATE_CONNECTING, 
      STATE_CONNECTED, STATE_IDLE, STATE_CLOSING, STATE_CLOSED 
    } state;

    C_Connection(int s, struct sockaddr_in *loc=NULL);
    C_Connection(char *hostname, int port, C_AsyncDNS *dns=NULL);
    ~C_Connection();

    state run(int max_send_bytes, int max_recv_bytes);

    void close(int quick=0);

    void set_max_sendsize(int value) { if (value < 64) value=64; if (value > MAX_CONNECTION_SENDSIZE) value=MAX_CONNECTION_SENDSIZE; m_remote_maxsend=value; }
    void set_saturatemode(int mode) { m_satmode=mode; }
    int get_saturatemode() { return m_satmode; }

    int getMaxSendSize();

    int send_bytes_in_queue(void);
    int send_bytes_available(void);
    
    /* Modified by LoraYin 2003/11/27 for switch to AES, 
       pass the switch_flag to the encrypt_buffer()
    */
    int send_bytes(void *data, int length, int switch_flag = 0); // multiples of 16 only pls
    //int send_bytes(void *data, int length); // multiples of 8 only pls

    int recv_bytes_available(void);
    int recv_bytes(void *data, int maxlength); // multiples of 16

    unsigned long get_interface(void);
    unsigned long get_remote(void) { return m_saddr.sin_addr.s_addr; }
    short get_remote_port(void) { return m_remote_port; }

    int get_total_bytes_recv() { return m_recv_bytes_total; }
    int was_ever_connected(void) { return m_ever_connected; }
    void calc_bps(int *send, int *recv);

    void get_last_bps(int *send, int *recv) 
    { 
      if (send) *send=m_last_sendbps;
      if (recv) *recv=m_last_recvbps;
    }
    unsigned char *get_remote_pkey_hash() { return m_remote_pkey; }
    
    void set_aes_keysize(int keysize) ; 
    void set_send_encryption(int encryption) { m_send_encryption = encryption; }
    void set_recv_encryption(int encryption) { m_recv_encryption = encryption; }
    void set_peer_padlocksl_flag(int flag) { m_peer_padlocksl = flag; }
    int  get_peer_padlocksl_flag() { return m_peer_padlocksl; }
    int get_send_encryption() { return m_send_encryption ; }
    int get_recv_encryption() { return m_recv_encryption; }

  protected:
    int  m_socket;
    short m_remote_port;
    char m_recv_buffer[MAX_CONNECTION_RECVSIZE];
    char m_send_buffer[MAX_CONNECTION_SENDSIZE];
    int  m_recv_pos;
    int  m_recv_len;
    int  m_send_pos;
    int  m_send_len;

    int m_remote_maxsend;
    int m_satmode;

    int m_last_sendbps, m_last_recvbps;

    int  m_send_bytes_total;
    int  m_recv_bytes_total;

    struct sockaddr_in m_saddr;
    char m_host[256];

    C_AsyncDNS *m_dns;

    void do_init(void);
    state m_state;
    int m_ever_connected;

    struct
    {
      unsigned int time_ms;
      unsigned int send_bytes;
      unsigned int recv_bytes;
    } bps_count[30];

    int bps_count_pos;
    unsigned int m_start_time;

    /* Modified by LoraYin 2003/11/27 for switch to AES 
    if switch_flag !=0 means the data is part of a switch massages
    which indicate that we are going to use AES after this messsge. 
    The message itself can not be encrypted by AES !!
    */
    void encrypt_buffer(void *data, int len, int switch_flag = 0);
    //void encrypt_buffer(void *data, int len);
    
    void decrypt_buffer(void *data, int len);
    int bf_initted; 
    int m_cansend;
    void init_crypt();
    void init_crypt_gotpkey();
    void init_crypt_decodekey();
    unsigned char m_mykeyinfo[CONNECTION_KEYSIZE+8+CONNECTION_BFPUBKEY];

    unsigned char m_remote_pkey[CONNECTION_PKEYHASHSIZE];
    unsigned int m_send_cbc[2];
    unsigned int m_recv_cbc[2];

    R_RSA_PUBLIC_KEY remote_pubkey;
   
    int m_send_encryption;  // AES, decided by self cap MLC_PADLOCKSL_SWITCH_TO_AES.
    int m_recv_encryption;  // AES, decided by peer cap MLC_PADLOCKSL_SWITCH_TO_AES.
    int m_peer_padlocksl;    // Indicate if peer site is a PadLockSL. decided by peer cap MLC_PADLOCKSL_AES_KEYSIZE.
    int m_aes_keysize;   // AES keysize, decided by max of g_aes_keysize and peer cap MLC_PADLOCKSL_AES_KEYSIZE.
    AES_CTX m_aes;       // AES encryption context, include key and IV.
};

#endif // _C_CONNECTION_H_
