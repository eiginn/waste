/*
WASTE - blowfish.hpp (Blowfish cipher)
Copyright (C) 2003 Nullsoft, Inc.
Copyright (C) 2004 WASTE Development Team

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
Current class and templates written by Md5Chap
*/

#ifndef _BLOWFISH_H
#define _BLOWFISH_H

//#include "intdefs.hpp"

struct _BLOWFISH_CTX
{
	uint32_t P[16+2];
	uint32_t S[4*256];
};

class CBlowfish
{
	public:
		enum _eIV
		{
			IV_BOTH,
			IV_ENC,
			IV_DEC
		};
		CBlowfish();
		CBlowfish(void* key, uint32_t len);
		~CBlowfish();
		void Init(void* key, uint32_t len);
		void SetIV(_eIV which, uint32_t IV[2]);
		void SetIV(_eIV which, uint32_t left, uint32_t right);
		void GetIV(_eIV which, uint32_t IV[2]);
		void GetIV(_eIV which, uint32_t &left, uint32_t &right);
		void Final();
		void EncryptECB(void *data, uint32_t len);
		void DecryptECB(void *data, uint32_t len);
		void EncryptCBC(void *data, uint32_t len);
		void DecryptCBC(void *data, uint32_t len);
		void EncryptPCBC(void *data, uint32_t len);
		void DecryptPCBC(void *data, uint32_t len);
	private:
		bool m_bInited;
		uint32_t m_IV_Enc[2];
		uint32_t m_IV_Dec[2];
		_BLOWFISH_CTX m_ctx;
};

#endif//_BLOWFISH_H
