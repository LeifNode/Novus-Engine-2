/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

/**
 *	SHA-1 implementation by Dominik Reichl <dominik.reichl@t-online.de>
 *	http://www.codeproject.com/Articles/2463/CSHA-A-C-Class-Implementation-of-the-SHA-Hash-A
 */

#pragma once

#include <iostream>

 ///////////////////////////////////////////////////////////////////////////
 // Define variable types

#ifndef UINT_8
#ifdef _MSC_VER // Compiling with Microsoft compiler
#define UINT_8 unsigned __int8
#else // !_MSC_VER
#define UINT_8 unsigned char
#endif // _MSC_VER
#endif

#ifndef UINT_32
#ifdef _MSC_VER // Compiling with Microsoft compiler
#define UINT_32 unsigned __int32
#else // !_MSC_VER
#if (ULONG_MAX == 0xFFFFFFFFUL)
#define UINT_32 unsigned long
#else
#define UINT_32 unsigned int
#endif
#endif // _MSC_VER
#endif // UINT_32

#ifndef INT_64
#ifdef _MSC_VER // Compiling with Microsoft compiler
#define INT_64 __int64
#else // !_MSC_VER
#define INT_64 long long
#endif // _MSC_VER
#endif // INT_64

#ifndef UINT_64
#ifdef _MSC_VER // Compiling with Microsoft compiler
#define UINT_64 unsigned __int64
#else // !_MSC_VER
#define UINT_64 unsigned long long
#endif // _MSC_VER
#endif // UINT_64

typedef union
{
	UINT_8 c[64];
	UINT_32 l[16];
} SHA1_WORKSPACE_BLOCK;

namespace novus
{

struct SHA1Hash
{
	friend std::ostream& operator<<(std::ostream& os, const SHA1Hash& hash);

	//SHA1Hash& operator= (const SHA1Hash& o);

	bool operator== (const SHA1Hash& o) const;
	bool operator!= (const SHA1Hash& o) const;
	bool operator< (const SHA1Hash& o) const;
	bool operator> (const SHA1Hash& o) const;
	bool operator<= (const SHA1Hash& o) const;
	bool operator>= (const SHA1Hash& o) const;

	uint8_t Hash[20];
};

class SHA1
{
public:
	SHA1();

	void Reset();

	void Update(const UINT_8* pbData, UINT_32 uLen);

	void Finalize();

	inline bool GetHash(UINT_8* pbDest20) const;
	SHA1Hash GetHash() const;

	/**
	 *	Hashes a block of data and finalizes it. 
	 *	Use GetHash() to get the final hash.
	 */
	void Hash(const UINT_8* data, UINT_32 length);

private:
	void Transform(UINT_32* pState, const UINT_8* pBuffer);

private:
	UINT_32 State[5];
	UINT_32 Count[2];
	UINT_32 reserved0[1];//Padding
	UINT_8  Buffer[64];
	UINT_8  Digest[20];
	UINT_32 reserved1[3];//Padding

	UINT_8  Workspace[64];
	SHA1_WORKSPACE_BLOCK* Block;

	bool Finalized;
};

};