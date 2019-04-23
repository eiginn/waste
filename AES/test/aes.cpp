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
 * This file implements test for software & hardware aes en/decryption 
 * Author: Raymond Cai
 * Date Created: 2003.12.1
*/
#include "aes.h"
#include "C5P.h"
#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#ifdef _WIN32
#include <conio.h>
#elif defined(_LINUX)
#include <curses.h>
#endif

#define BIG_SIZE 29

#define NIST_SIZE 4


int dummy[1024] = {0};


// Standard plaintext for example vectors from NIST 800-38A

// Ciphertexts for the above plaintext
int plain[16] = {0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373, \
		 0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45, \
		 0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a, \
		 0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6};

// using ECB (128 bit key)
int ecb128[16]= {0xb47bd73a, 0x60367a0d, 0xf3ca9ea8, 0x97ef6624, \
		 0x85d5d3f5, 0x9d69b903, 0x5a8985e7, 0xafbafd96, \
		 0x7fcdb143, 0x23ce8e59, 0xe3001b88, 0x880603ed, \
		 0x5e780c7b, 0x3fade827, 0x71202382, 0xd45d7204};

// using CBC (128 bit key)
int cbc128[16]= {0xacab4976, 0x46b21981, 0x9b8ee9ce, 0x7d19e912, \
		 0x9bcb8650, 0xee197250, 0x3a11db95, 0xb2787691, \
		 0xb8d6be73, 0x3b74c1e3, 0x9ee61671, 0x16952222, \
		 0xa1caf13f, 0x09ac1f68, 0x30ca0e12, 0xa7e18675};

// using CFB (128 bit key)
int cfb128[16]= {0x2ed93f3b, 0x20ad2db7, 0xf8493433, 0x4afb3ce8, \
		 0x3745a6c8, 0x3fa9b3a0, 0xadcde3cd, 0x8be51c9f, \
		 0x671f7526, 0x40b1cba3, 0xf18c80b1, 0xdff4a487, \
		 0x35054bc0, 0x0e1c5d7c, 0x6fc6c4ea, 0xe6f2f79f};

// using OFB (128 bit key)
int ofb128[16]= {0x2ed93f3b, 0x20ad2db7, 0xf8493433, 0x4afb3ce8, \
		 0x8d508977, 0x038f9116, 0xda523cf5, 0x25d84ec5, \
		 0x1e054097, 0xf6ec5f9c, 0xa8f74443, 0xcced6022, \
		 0x28654c30, 0x78c759f6, 0xd910a566, 0x5eaed6c1};

// using CTR (128 bit key)
int ctr128[16]= {0x91614d87, 0x26e320b6, 0x6468ef1b, 0xceb60d99, \
		 0x6bf60698, 0xfffd7079, 0x7b181786, 0xfffdffb9, \
		 0x3edfe45a, 0x5ed3d5db, 0x02094f5b, 0xab3eb00d, \
		 0xda1d031e, 0xd103be2f, 0xa0702179, 0xee9c00f3};

// NIST 800-38A key
int stdkey[4] = {0x16157e2b, 0xa6d2ae28, 0x8815f7ab, 0x3c4fcf09};

// NIST 800-38A Initialization vector
int NISTIV[4] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c};

// Need a copy of the IV since we over-write in many chaining modes
int TRUEIV[4] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c};

// Initial CTR vector
int ctrivec[4]= {0xf3f2f1f0, 0xf7f6f5f4, 0xfbfaf9f8, 0xfffefdfc};





// Extended key (NIST SPEC Sample cipher p33)
//
int txt0[8]  = {0xa8f64332, 0x8d305a88, 0xa2983131, 0x340737e0, \
		0x00000000, 0x00000000, 0x00000000, 0x00000000};
		      
int key0[44] = {0x16157e2b, 0xa6d2ae28, 0x8815f7ab, 0x3c4fcf09, \
		0x17fefaa0, 0xb12c5488, 0x3939a323, 0x05766c2a, \
		0xf295c2f2, 0x43b9967a, 0x7a803559, 0x7ff65973, \
		0x7d47803d, 0x3efe1647, 0x447e231e, 0x3b887a6d, \
		0x41a544ef, 0x7f5b52a8, 0x3b2571b6, 0x00ad0bdb, \
		0xf8c6d1d4, 0x879d837c, 0xbcb8f2ca, 0xbc15f911, \
		0x7aa3886d, 0xfd3e0b11, 0x4186f9db, 0xfd9300ca, \
		0x0ef7544e, 0xf3c95f5f, 0xb24fa684, 0x4fdca64e, \
		0x2173d2ea, 0xd2ba8db5, 0x60f52b31, 0x2f298d7f, \
		0xf36677ac, 0x21dcfa19, 0x4129d128, 0x6e005c57, \
		0xa8f914d0, 0x8925eec9, 0xc80c3fe1, 0xa60c63b6};

int res0[4]  = {0x1d842539,0xfb09dc02,0x978511dc,0x320b6a19};


// Extended key (NIST Appendix 3 p35-38)
//
int key1[44] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, \
		0xfd74aad6, 0xfa72afd2, 0xf178a6da, 0xfe76abd6, \
		0x0bcf92b6, 0xf1bd3d64, 0x00c59bbe, 0xfeb33068, \
		0x4e74ffb6, 0xbfc9c2d2, 0xbf0c596c, 0x41bf6904, \
		0xbcf7f747, 0x033e3595, 0xbc326cf9, 0xfd8d05fd, \
		0xe8a3aa3c, 0xeb9d9fa9, 0x57aff350, 0xaa22f6ad, \
		0x7d0f395e, 0x9692a6f7, 0xc13d55a7, 0x6b1fa30a, \
		0x1a70f914, 0x8ce25fe3, 0x4ddf0a44, 0x26c0a94e, \
		0x35874347, 0xb9651ca4, 0xf4ba16e0, 0xd27abfae, \
		0xd1329954, 0x685785f0, 0x9ced9310, 0x4e972cbe, \
		0x7f1d1113, 0x174a94e3, 0x8ba707f3, 0xc5302b4d};


int res1[40] = {0xd8e0c469,0x30047b6a,0x80b7cdd8,0x5ac5b470, \
		0x373ba1c6,0x825b8f87,0x62814f6f,0x79d8c8a1, \
		0x6d4ed135,0x9c273a3e,0x3e341ef0,0xd3dee734, \
		0xfcf8b4ae,0xbabd76f3,0xcb05f69f,0x7942b812, \
		0xd8e0c469,0x30047b6a,0x80b7cdd8,0x5ac5b470, \
		0x373ba1c6,0x825b8f87,0x62814f6f,0x79d8c8a1, \
		0xd8e0c469,0x30047b6a,0x80b7cdd8,0x5ac5b470, \
		0x373ba1c6,0x825b8f87,0x62814f6f,0x79d8c8a1, \
		0xd8e0c469,0x30047b6a,0x80b7cdd8,0x5ac5b470, \
		0x373ba1c6,0x825b8f87,0x62814f6f,0x79d8c8a1};


int txt1[40] = {0x33221100, 0x77665544, 0xbbaa9988, 0xffeeddcc, \
		0x00000000, 0x00000000, 0x00000000, 0x00000000, \
		0x11111111, 0x11111111, 0x11111111, 0x11111111, \
		0x22222222, 0x22222222, 0x22222222, 0x22222222, \
		0x33221100, 0x77665544, 0xbbaa9988, 0xffeeddcc, \
		0x00000000, 0x00000000, 0x00000000, 0x00000000, \
		0x33221100, 0x77665544, 0xbbaa9988, 0xffeeddcc, \
		0x00000000, 0x00000000, 0x00000000, 0x00000000, \
		0x00000000, 0x00000000, 0x00000000, 0x00000000, \
		0x00000000, 0x00000000, 0x00000000, 0x00000000};


int intermediate[36] = {0xe810d889,0x68ce5a85,0xd843182d,0xe48f12cb, \
                        0x8f591549,0xa0d7e555,0xfa94cada,0xf7630a1f, \
                        0x286a63fa,0xc939b325,0x318a6640,0x174d2457, \
                        0x23407224,0xfab36669,0x3275d26e,0x6c5b4288, \
                        0xbc7716c8,0x3bc97a9b,0x92790225,0x961926b0, \
                        0x09e12fc6,0xc3ed5ef7,0x5d3979cc,0x5dcff984, \
                        0x0f6c87d1,0x0a30c479,0xad9455b4,0x1ff46fd6, \
                        0xd2bae3fd,0xd7d0e505,0x4e964735,0xf137fef1, \
                        0x3d7c6ebd,0x9e77b5f2,0x6e21610b,0x89b6108b};
		


int Dmediate[36] = {0xa190d954, 0xb59aa06b, 0x0ef4bb96, 0x2f7011a1, \
		   0xc0221c3e, 0x76bffcb6, 0x6750a88d, 0x950417f6, \
		   0x4c1258b4, 0x018ab668, 0x2ef8994b, 0x4c55155f, \
		   0x90b6dae8, 0x65d47714, 0xe2f5f73f, 0x4fdd47e7, \
		   0x509d3336, 0x2639b5f9, 0x2d092c9f, 0x236d40c4, \
		   0xf07e6d2d, 0x34e3333f, 0xdd023609, 0xc712fb5b, \
		   0x6822d93b, 0x73fb74fc, 0xe0cb6757, 0x2d0e59c0, \
		   0x691abea7, 0x9b73ad97, 0x45cac9d8, 0x618b611f, \
		   0x8ce05363, 0x04e16009, 0x51b770cd, 0xe7d0caba};
		   
			
// Extended key (decrypt - alternate method: NIST Appendix 3 p35-38)
//
int dkey[44] = {0x7f1d1113, 0x174a94e3, 0x8ba707f3, 0xc5302b4d, \
		0xbe29aa13, 0xf6af8f9c, 0x80f570f7, 0x03bff700, \
		0x63a46213, 0x4886258f, 0x765aff6b, 0x834a87f7, \
		0x74fc828d, 0x2b22479c, 0x3edcdae4, 0xf510789c, \
		0x8d09e372, 0x5fdec511, 0x15fe9d78, 0xcbcca278, \
		0x2710c42e, 0xd2d72663, 0x4a205869, 0xde323f00, \
		0x04f5a2a8, 0xf5c7e24d, 0x98f77e0a, 0x94126769, \
		0x91e3c6c7, 0xf13240e5, 0x6d309c47, 0x0ce51963, \
		0x9902dba0, 0x60d18622, 0x9c02dca2, 0x61d58524, \
		0xf0df568c, 0xf9d35d82, 0xfcd35a80, 0xfdd75986, \
		0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c};



// Extended 192 bit key (NIST Appendix 3 p??-??)
//
int k192[52] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, \
		0x13121110, 0x17161514, 0xf9f24658, 0xfef4435c, \
		0x00000000, 0xfaf04758, 0xe9e25648, 0xfef4435c, \
		0xb349f940, 0x4dbdba1c, 0xb843f048, 0x42b3b710, \
		0xab51e158, 0x55a5a204, 0x41b5ff7e, 0x0c084562, \
		0xb44bb52a, 0xf6f8023a, 0x5da9e362, 0x080c4166, \
		0x728501f5, 0x7e8d4497, 0xcac6f1bd, 0x3c3ef387, \
		0x619710e5, 0x699b5183, 0x9e7c1534, 0xe0f151a3, \
		0x2a37a01e, 0x16095399, 0x779e437c, 0x1e0512ff, \
		0x880e7edd, 0x68ff2f7e, 0x42c88f60, 0x54c1dcf9, \
		0x235f9f85, 0x3d5a8d7a, 0x5229c0c0, 0x3ad6efbe, \
		0x781e60de, 0x2cdfbc27, 0x0f8023a2, 0x32daaed8, \
		0x330a97a4, 0x09dc781a, 0x71c218c4, 0x5d1da4e3};
		
int res2[4]  = {0xa47ca9dd, 0xe0df4c86, 0xa070af6e, 0x91710dec};


// Extended 192 bit DECRYPT key
//
int d192[52] = {0x330a97a4, 0x09dc781a, 0x71c218c4, 0x5d1da4e3, \
		0x0dbdbed6, 0x49ea09c2, 0x8073b04d, 0xb91b023e, \
		0x00000000, 0x3968b273, 0x9dd8f9c7, 0x728cc685, \
		0xc16e7df7, 0xef543f42, 0x7f317853, 0x4457b714, \
		0x90654711, 0x3b66cf47, 0x8dce0e9b, 0xf0f10bfc, \
		0xb6a8c1dc, 0x7d3f0567, 0x4a195ccc, 0x2e3a42b5, \
		0xabb0dec6, 0x64231e79, 0xbe5f05a4, 0xab038856, \
		0xda7c1bdd, 0x155c8df2, 0x1dab498a, 0xcb97c4bb, \
		0x08f7c478, 0xd63c8d31, 0x01b75596, 0xcf93c0bf, \
		0x10efdc60, 0xce249529, 0x15efdb62, 0xcf20962f, \
		0xdbcb4e4b, 0xdacf4d4d, 0xc7d75257, 0xdecb4949, \
		0x1d181f1a, 0x191c1b1e, 0xd7c74247, 0xdecb4949, \
		0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c};



// Dextended 256 bit key (NIST Appendix 3 p??-??)
//
int k256[60] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, \
		0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c, \
		0x00000000, 0x98c476a1, 0x93ce7fa9, 0x9cc072a5, \
		0xcda85116, 0xdabe4402, 0xc1a45d1a, 0xdeba4006, \
		0xf0df87ae, 0x681bf10f, 0xfbd58ea6, 0x6715fc03, \
		0x48f1e16d, 0x924fa56f, 0x53ebf875, 0x8d51b873, \
		0x7f8256c6, 0x1799a7c9, 0xec4c296f, 0x8b59d56c, \
		0x753ae23d, 0xe7754752, 0xb49ebf27, 0x39cf0754, \
		0x5f90dc0b, 0x48097bc2, 0xa44552ad, 0x2f1c87c1, \
		0x60a6f545, 0x87d3b217, 0x334d0d30, 0x0a820a64, \
		0x1cf7cf7c, 0x54feb4be, 0xf0bbe613, 0xdfa761d2, \
		0xfefa1af0, 0x7929a8e7, 0x4a64a5d7, 0x40e6afb3, \
		0x71fe4125, 0x2500f59b, 0xd5bb1388, 0x0a1c725a, \
		0x99665a4e, 0xe04ff2a9, 0xaa2b577e, 0xeacdf8cd, \
		0xcc79fc24, 0xe97909bf, 0x3cc21a37, 0x36de686d};

int res3[4]  = {0xcab7a28e, 0xbf456751, 0x9049fcea, 0x8960494b};



// Extended 256 bit DECRYPT key
//
int d256[60] = {0xcc79fc24, 0xe97909bf, 0x3cc21a37, 0x36de686d, \
		0xffd1f134, 0x2faacebf, 0x5fe2e9fc, 0x6e015825, \
		0xeb48165e, 0x0a354c38, 0x46b77175, 0x84e680dc, \
		0x8005a3c8, 0xd07b3f8b, 0x70482743, 0x31e3b1d9, \
		0x138e70b5, 0xe17d5a66, 0x4c823d4d, 0xc251f1a9, \
		0xa37bda74, 0x507e9c43, 0xa03318c8, 0x41ab969a, \
		0x1597a63c, 0xf2f32ad3, 0xadff672b, 0x8ed3cce4, \
		0xf3c45ff8, 0xf3054637, 0xf04d848b, 0xe1988e52, \
		0x9a4069de, 0xe7648cef, 0x5f0c4df8, 0x232cabcf, \
		0x1658d5ae, 0x00c119cf, 0x0348c2bc, 0x11d50ad9, \
		0xbd68c615, 0x7d24e531, 0xb868c117, 0x7c20e637, \
		0x0f85d77f, 0x1699cc61, 0x0389db73, 0x129dc865, \
		0xc940282a, 0xc04c2324, 0xc54c2426, 0xc4482720, \
		0x1d181f1a, 0x191c1b1e, 0x15101712, 0x11141316, \
		0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c};




int loopcounter = 0;
int testcounter = 0;


int ikey[8]   = {0x5a5a5a5a,0x5a5a5a5a,0x5a5a5a5a,0x5a5a5a5a, \
		 0x5a5a5a5a,0x5a5a5a5a,0x5a5a5a5a,0x5a5a5a5a};

int ivec[8]   = {0x5a5a5a5a,0x5a5a5a5a,0x5a5a5a5a,0x5a5a5a5a, \
		 0x5a5a5a5a,0x5a5a5a5a,0x5a5a5a5a,0x5a5a5a5a};


// Buffers for encrypted (ctxt) and decrypted (dtxt) streams

int dtxt[500]  = {0};
int ctxt[500]  = {0};

// Special in-place buffers ....
int zero[120]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int null[120]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int TESTKEY = 0;
int CNT1[3] = {0};

int COUNT   = 0;
int KEY     = 0;
int CNT2[2] = {0};



int buffer_ecb[1200000];


#define PrintKey(key, Num) \
	do{ \
		printf(#key);\
		for (int i=0; i<Num; i++) \
		{ \
			printf("\n");\
			for (int j=0; j<4; j++) \
				printf("%10x ", key.ks[i*4 + j]);\
		}\
		printf("\n");\
	}while(0);

#define CompAndPrint(k1, k2, N) \
	do { \
		PrintKey(k2, N); \
		for (int k=0; k<N*4; k++)\
			if (k1[k] != k2.ks[k]) printf("Is not equal!\n");\
	} while(0);

#define _AES_ENCRYPT
#define _DEBUG_RNG
#define _EXTENDED_KEY
#define _SFT_ENANDDE

#define LDKEY		0x080

#define HWKEY		0x000

#define KEYSIZE_128 0x000 + 0x00a
#define KEYSIZE_192	0x400 + 0x00c
#define KEYSIZE_256	0x800 + 0x00e

#define ENCRYPT		0x000
#define DECRYPT		0x200

		
//static int HwkyDecrypt128[4] = {DECRYPT + HWKEY + KEYSIZE_128, 0, 0, 0};

#include <string.h>
#include <time.h>

//testing the encrypt rate
void Rate_Test()
{
	int *key = (int *)c5p_malloc(8*sizeof(int));
	int *plaintxt = (int *)c5p_malloc(120*sizeof(int));
	int *ciper = (int *)c5p_malloc(120*sizeof(int));
	int *decode = (int *)c5p_malloc(120*sizeof(int));
	int *IV	= (int *)c5p_malloc(4*sizeof(int));

	clock_t begin = clock();
	for(unsigned int i=0; i<10000; i++)
	{
		for (unsigned int j=0; j<1000; j++)
		{
			c5p_aes_encrypt(key, 128, e_aes_ecb, 
					plaintxt, 64, plaintxt, IV);
			c5p_aes_decrypt(key, 128, e_aes_ecb, 
					plaintxt, 64, plaintxt, IV);
		}
	}
	clock_t end = clock();

	printf("%d - %d\n", end, begin);
	printf("One encrypt time is : %12.10f\n", (double)(end - begin)/CLOCKS_PER_SEC);
}

int main()
{

	if (c5p_aes_available()) 
		printf("\nAES exists\n");
	else
		return 0;
	if (c5p_rng_available())
		printf("\nRNG exists\n");
	else
		return 0;

#ifdef _EXTENDED_KEY
	{
	
	int i, j;
	aes_encrypt_ctx key;
	aes_decrypt_ctx dkey;

	c5p_encrypt_key((const void *)key1, 128, &key);
	CompAndPrint(key1, key, 11);
	c5p_decrypt_key((const void *)key1, 128, &dkey);
	CompAndPrint(::dkey, dkey, 11);

	c5p_encrypt_key((const void *)k192, 192, &key);
	CompAndPrint(k192, key, 13);
	c5p_decrypt_key((const void *)k192, 192, &dkey);
	CompAndPrint(d192, dkey, 13);

	c5p_encrypt_key((const void *)k256, 256, &key);
	CompAndPrint(k256, key, 15);
	c5p_decrypt_key((const void *)k256, 256, &dkey);
	CompAndPrint(d256, dkey, 15);
	}
#endif
	
#ifdef _DEBUG_RNG
	unsigned char *rand = (unsigned char *)c5p_malloc(24);

	printf("\nRNG test\n");
	for (int k=0; k<20; k++)
		printf("%x ", (int *)rand[k]);

	c5p_rng_rand(20, rand);

	printf("\n rand:\n");
	for (k=0; k<20; k++)
		printf("%x ", (int *)rand[k]);
	
	rand [19] = '\0';

	c5p_free(rand);
	
#endif

#ifdef _AES_ENCRYPT

	int key192[6] = {
		0x8e73b0f7, 0xda0e6452, 0xc810f32b,
		0x809079e5, 0x62f8ead2, 0x522c6b7b
	};
	char key1922[24] = {
		0x8e, 0x73, 0xb0, 0xf7,0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b,
		0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
	};
	char key256[32] = {
		0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
		0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
	};

	int key128[4] ={
		0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c
	};
	
	int *key = (int *)c5p_malloc(8*sizeof(int));
	int *plaintxt = (int *)c5p_malloc(120*sizeof(int));
	int *ciper = (int *)c5p_malloc(120*sizeof(int));
	int *decode = (int *)c5p_malloc(120*sizeof(int));
	int *IV	= (int *)c5p_malloc(4*sizeof(int));

	printf("\nTest for 128bits Key \n");

	memcpy((void *)key, (void *)stdkey, 4*sizeof(int));
	memcpy((void *)plaintxt, (void *)plain, 16*sizeof(int));
	memcpy((void *)IV, (void *)NISTIV, 4*sizeof(int));


	int i, j;
#define P(buf) \
	printf(#buf"\n");\
	for(i=0; i<4; i++) \
	{ \
		for(j=0;j<4; j++) \
			printf("%10x ", buf[i*4 +j]); \
		printf("\n"); \
	}

	printf("\nKey = \n");
	for(i=0; i<4; i++)
		printf("%10x ", key[i]);
	printf("\n");
	printf("\nReal Key = \n");
	for(i=0; i<4; i++)
		printf("%10x ", stdkey[i]);
	printf("\n");

	printf("\nIV = \n");
	for(i=0; i<4; i++)
		printf("%10x ", IV[i]);
	printf("\n");

	printf("\nTest 128 bits\n");

	aes_encrypt_ctx *extdkey = NULL;
	extdkey = (aes_encrypt_ctx *)c5p_malloc(sizeof(aes_encrypt_ctx));

	memset((void *)ciper, 0, 120*sizeof(int));
	printf("\ntest ECB 1\n");
	
	P(plaintxt);
	c5p_aes_encrypt(key, 128, e_aes_ecb, plaintxt, 64, ciper/*plaintxt*/, IV);
	P(plaintxt); P(ecb128);P(ciper);

	printf("\nDecode\n");
	c5p_aes_decrypt(key, 128, e_aes_ecb, plaintxt, 64, plaintxt, IV);
	P(plaintxt);
			
	printf("\ntest CBC \n");

	memcpy((void *)key, (void *)stdkey, 4*sizeof(int));
	memcpy((void *)plaintxt, (void *)plain, 16*sizeof(int));
	memcpy((void *)IV, (void *)NISTIV, 4*sizeof(int));

	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_encrypt(key, 128, e_aes_cbc, plaintxt, 64, plaintxt, IV);
	P(plaintxt);P(cbc128);
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_decrypt(key, 128, e_aes_cbc, plaintxt, 64, plaintxt, IV);

	if(memcmp((void *)plaintxt, (void *)plain, 16*sizeof(int)))
	{
		P(plaintxt);
		P(plain);
	}

	printf("\n CFB \n");
 	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_encrypt(key, 128, e_aes_cfb, plaintxt, 64, ciper, IV);
	P(ciper); P(cfb128);

	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_decrypt(key, 128, e_aes_cfb, ciper, 64, decode, IV);
	P(decode);

	printf("\ntest OFB \n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_encrypt(key, 128, e_aes_ofb, plaintxt, 64, ciper, IV);
	P(ciper); P(ofb128); 
	
	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_decrypt(key, 128, e_aes_ofb, ciper, 64, decode, IV);
	P(decode);

	printf("\n 192 bits Key ");
	memcpy((void*)key, (void*)key1922, sizeof(int)*6);
	memset((void *)ciper, 0, 120*sizeof(int));
	printf("\nKey = \n");
	for(i=0; i<6; i++)
		printf("%10x ", key[i]);
	printf("\n");

	c5p_encrypt_key(key, 192, extdkey);
	aes_encrypt(plaintxt, ciper, extdkey);
	P(ciper);

	printf("\ntest ECB 1\n");
	c5p_aes_encrypt(key, 192, e_aes_ecb, plaintxt, 64, ciper, IV);
	P(ciper);

	printf("\nDecode\n");
	c5p_aes_decrypt(key, 192, e_aes_ecb, ciper, 64, decode, IV);
	P(decode);
	
	memcpy((void *)IV, (void *)TRUEIV, 16);
	printf("\ntest CBC \n");
	c5p_aes_encrypt(key, 192, e_aes_cbc, plaintxt, 64, ciper, IV);
	P(ciper); 

	memcpy((void *)IV, (void *)TRUEIV, 16);
	printf("\nDecode\n");
	c5p_aes_decrypt(key, 192, e_aes_cbc, ciper, 64, decode, IV);
	P(decode);

	printf("\n CFB \n");
 	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_encrypt(key, 192, e_aes_cfb, plaintxt, 64, ciper, IV);
	P(ciper); 

	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_decrypt(key, 192, e_aes_cfb, ciper, 64, decode, IV);
	P(decode);

	printf("\ntest OFB \n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_encrypt(key, 192, e_aes_ofb, plaintxt, 64, ciper, IV);
	P(ciper); 
	
	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_decrypt(key, 192, e_aes_ofb, ciper, 64, decode, IV);
	P(decode);

	printf("\n 256 bits Key ");
	memcpy((void *)key, (void *)key256, 32);
	memset((void *)ciper, 0, 120*sizeof(int));
	printf("\ntest ECB 1\n");
	c5p_aes_encrypt(key, 256, e_aes_ecb, plaintxt, 64, ciper, IV);
	P(ciper);

	printf("\nDecode\n");
	c5p_aes_decrypt(key, 256, e_aes_ecb, ciper, 64, decode, IV);
	P(decode);
	
	memcpy((void *)IV, (void *)TRUEIV, 16);
	printf("\ntest CBC \n");
	c5p_aes_encrypt(key, 256, e_aes_cbc, plaintxt, 64, ciper, IV);
	P(ciper); 

	memcpy((void *)IV, (void *)TRUEIV, 16);
	printf("\nDecode\n");
	c5p_aes_decrypt(key, 256, e_aes_cbc, ciper, 64, decode, IV);
	P(decode);

	printf("\n CFB \n");
 	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_encrypt(key, 256, e_aes_cfb, plaintxt, 64, ciper, IV);
	P(ciper); 

	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_decrypt(key, 256, e_aes_cfb, ciper, 64, decode, IV);
	P(decode);

	printf("\ntest OFB \n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_encrypt(key, 256, e_aes_ofb, plaintxt, 64, ciper, IV);
	P(ciper); 
	
	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	c5p_aes_decrypt(key, 256, e_aes_ofb, ciper, 64, decode, IV);
	P(decode);
#endif

#ifdef _SFT_ENANDDE

	printf("\n ---------------------software -------------\n");

	memcpy((void *)key, (void *)stdkey, 4*sizeof(int));
	memset((void *)ciper, 0, 120*sizeof(int));
	printf("\ntest ECB 1\n");

	sft_aes_encrypt(key, 128, e_aes_ecb, plaintxt, 64, ciper, IV);
	P(ciper);
	P(ecb128);

	printf("\nDecode\n");
	sft_aes_decrypt(key, 128, e_aes_ecb, ciper, 64, decode, IV);
	P(decode);

	memcpy((void *)key, (void *)stdkey, 4*sizeof(int));
	memset((void *)ciper, 0, 120*sizeof(int));
	memcpy((void *)IV, (void *)TRUEIV, 16);
	memcpy((void *)plaintxt, (void *)plain, 16*sizeof(int));
	printf("\ntest CBC \n");
	sft_aes_encrypt(key, 128, e_aes_cbc, plaintxt, 64, plaintxt, IV);
	P(plaintxt); P(cbc128);

	memcpy((void *)IV, (void *)TRUEIV, 16);
	printf("\nDecode\n");
	sft_aes_decrypt(key, 128, e_aes_cbc, plaintxt, 64, plaintxt, IV);
	P(plaintxt);

	printf("\n CFB \n");
 	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_encrypt(key, 128, e_aes_cfb, plaintxt, 64, plaintxt, IV);
	P(plaintxt); P(cfb128);

	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_decrypt(key, 128, e_aes_cfb, plaintxt, 64, plaintxt, IV);
	P(plaintxt);

	printf("\ntest OFB \n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_encrypt(key, 128, e_aes_ofb, plaintxt, 64, plaintxt, IV);
	P(plaintxt); P(ofb128); 
	
	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_decrypt(key, 128, e_aes_ofb, plaintxt, 64, plaintxt, IV);
	P(plaintxt);

	printf("\n 192 bits Key ");
	memcpy((void*)key, (void*)key1922, sizeof(int)*6);
	memset((void *)ciper, 0, 120*sizeof(int));
	printf("\nKey = \n");
	for(i=0; i<6; i++)
		printf("%10x ", key[i]);
	printf("\n");

	printf("\ntest ECB 1\n");
	sft_aes_encrypt(key, 192, e_aes_ecb, plaintxt, 64, ciper, IV);
	P(ciper);

	printf("\nDecode\n");
	sft_aes_decrypt(key, 192, e_aes_ecb, ciper, 64, decode, IV);
	P(decode);
	
	memcpy((void *)IV, (void *)TRUEIV, 16);
	printf("\ntest CBC \n");
	sft_aes_encrypt(key, 192, e_aes_cbc, plaintxt, 64, ciper, IV);
	P(ciper); 

	memcpy((void *)IV, (void *)TRUEIV, 16);
	printf("\nDecode\n");
	sft_aes_decrypt(key, 192, e_aes_cbc, ciper, 64, decode, IV);
	P(decode);

	printf("\n CFB \n");
 	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_encrypt(key, 192, e_aes_cfb, plaintxt, 64, ciper, IV);
	P(ciper); 

	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_decrypt(key, 192, e_aes_cfb, ciper, 64, decode, IV);
	P(decode);

	printf("\ntest OFB \n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_encrypt(key, 192, e_aes_ofb, plaintxt, 64, ciper, IV);
	P(ciper); 
	
	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_decrypt(key, 192, e_aes_ofb, ciper, 64, decode, IV);
	P(decode);

	printf("\n 256 bits Key ");
	
	memcpy((void*)key, (void*)key256, sizeof(int)*8);
	
	memset((void *)ciper, 0, 120*sizeof(int));
	printf("\ntest ECB 1\n");
	sft_aes_encrypt(key, 256, e_aes_ecb, plaintxt, 64, ciper, IV);
	P(ciper);

	printf("\nDecode\n");
	sft_aes_decrypt(key, 256, e_aes_ecb, ciper, 64, decode, IV);
	P(decode);
	
	memcpy((void *)IV, (void *)TRUEIV, 16);
	printf("\ntest CBC \n");
	sft_aes_encrypt(key, 256, e_aes_cbc, plaintxt, 64, ciper, IV);
	P(ciper); 

	memcpy((void *)IV, (void *)TRUEIV, 16);
	printf("\nDecode\n");
	sft_aes_decrypt(key, 256, e_aes_cbc, ciper, 64, decode, IV);
	P(decode);

	printf("\n CFB \n");
 	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_encrypt(key, 256, e_aes_cfb, plaintxt, 64, ciper, IV);
	P(ciper); 

	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_decrypt(key, 256, e_aes_cfb, ciper, 64, decode, IV);
	P(decode);

	printf("\ntest OFB \n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_encrypt(key, 256, e_aes_ofb, plaintxt, 64, ciper, IV);
	P(ciper); 
	
	printf("\nDecode\n");
	memcpy((void *)IV, (void *)TRUEIV, 16);
	sft_aes_decrypt(key, 256, e_aes_ofb, ciper, 64, decode, IV);
	P(decode);
#endif

	Rate_Test();

	return 0;
}
