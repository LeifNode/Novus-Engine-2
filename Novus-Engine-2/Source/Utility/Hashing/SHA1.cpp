#include "SHA1.h"
#include <memory>
#include <cassert>
#include <iostream>

#define SHA1_MAX_FILE_BUFFER (32 * 20 * 820)

// Rotate p_val32 by p_nBits bits to the left
#ifndef ROL32
#ifdef _MSC_VER
#define ROL32(p_val32,p_nBits) _rotl(p_val32,p_nBits)
#else
#define ROL32(p_val32,p_nBits) (((p_val32)<<(p_nBits))|((p_val32)>>(32-(p_nBits))))
#endif
#endif

#ifdef SHA1_LITTLE_ENDIAN
#define SHABLK0(i) (m_block->l[i] = \
	(ROL32(Block->l[i],24) & 0xFF00FF00) | (ROL32(Block->l[i],8) & 0x00FF00FF))
#else
#define SHABLK0(i) (Block->l[i])
#endif

#define SHABLK(i) (Block->l[i&15] = ROL32(Block->l[(i+13)&15] ^ \
	Block->l[(i+8)&15] ^ Block->l[(i+2)&15] ^ Block->l[i&15],1))

// SHA-1 rounds
#define S_R0(v,w,x,y,z,i) {z+=((w&(x^y))^y)+SHABLK0(i)+0x5A827999+ROL32(v,5);w=ROL32(w,30);}
#define S_R1(v,w,x,y,z,i) {z+=((w&(x^y))^y)+SHABLK(i)+0x5A827999+ROL32(v,5);w=ROL32(w,30);}
#define S_R2(v,w,x,y,z,i) {z+=(w^x^y)+SHABLK(i)+0x6ED9EBA1+ROL32(v,5);w=ROL32(w,30);}
#define S_R3(v,w,x,y,z,i) {z+=(((w|x)&y)|(w&x))+SHABLK(i)+0x8F1BBCDC+ROL32(v,5);w=ROL32(w,30);}
#define S_R4(v,w,x,y,z,i) {z+=(w^x^y)+SHABLK(i)+0xCA62C1D6+ROL32(v,5);w=ROL32(w,30);}

#pragma warning(push)
// Disable compiler warning 'Conditional expression is constant'
#pragma warning(disable: 4127)

namespace novus
{

std::ostream& operator<< (std::ostream& os, const SHA1Hash& hash)
{
	for (int i = 0; i < 20; i++)
	{
		os << std::hex << static_cast<int>(hash.Hash[i]);
	}

	return os;
}

//SHA1Hash& SHA1Hash::operator= (const SHA1Hash& o)
//{
//	memcpy(this->Hash, o.Hash, 20);
//
//	return *this;
//}

bool SHA1Hash::operator== (const SHA1Hash& o) const
{
	return (memcmp(this->Hash, o.Hash, 20) == 0);
}

bool SHA1Hash::operator!= (const SHA1Hash& o) const
{
	return (memcmp(this->Hash, o.Hash, 20) != 0);
}

bool SHA1Hash::operator< (const SHA1Hash& o) const
{
	return (memcmp(this->Hash, o.Hash, 20) < 0);
}

bool SHA1Hash::operator> (const SHA1Hash& o) const
{
	return (memcmp(this->Hash, o.Hash, 20) > 0);
}

bool SHA1Hash::operator<= (const SHA1Hash& o) const
{
	return (memcmp(this->Hash, o.Hash, 20) <= 0);
}

bool SHA1Hash::operator>= (const SHA1Hash& o) const
{
	return (memcmp(this->Hash, o.Hash, 20) >= 0);
}

SHA1::SHA1()
	:Block((SHA1_WORKSPACE_BLOCK*)Workspace)
{
	Reset();
}

void SHA1::Reset()
{
	Finalized = false;

	// SHA1 initialization constants
	State[0] = 0x67452301;
	State[1] = 0xEFCDAB89;
	State[2] = 0x98BADCFE;
	State[3] = 0x10325476;
	State[4] = 0xC3D2E1F0;

	Count[0] = 0;
	Count[1] = 0;
}

void SHA1::Transform(UINT_32* pState, const UINT_8* pBuffer)
{
	UINT_32 a = pState[0], b = pState[1], c = pState[2], d = pState[3], e = pState[4];

	memcpy(Block, pBuffer, 64);

	// 4 rounds of 20 operations each, loop unrolled
	S_R0(a, b, c, d, e, 0); S_R0(e, a, b, c, d, 1); S_R0(d, e, a, b, c, 2); S_R0(c, d, e, a, b, 3);
	S_R0(b, c, d, e, a, 4); S_R0(a, b, c, d, e, 5); S_R0(e, a, b, c, d, 6); S_R0(d, e, a, b, c, 7);
	S_R0(c, d, e, a, b, 8); S_R0(b, c, d, e, a, 9); S_R0(a, b, c, d, e, 10); S_R0(e, a, b, c, d, 11);
	S_R0(d, e, a, b, c, 12); S_R0(c, d, e, a, b, 13); S_R0(b, c, d, e, a, 14); S_R0(a, b, c, d, e, 15);
	S_R1(e, a, b, c, d, 16); S_R1(d, e, a, b, c, 17); S_R1(c, d, e, a, b, 18); S_R1(b, c, d, e, a, 19);
	S_R2(a, b, c, d, e, 20); S_R2(e, a, b, c, d, 21); S_R2(d, e, a, b, c, 22); S_R2(c, d, e, a, b, 23);
	S_R2(b, c, d, e, a, 24); S_R2(a, b, c, d, e, 25); S_R2(e, a, b, c, d, 26); S_R2(d, e, a, b, c, 27);
	S_R2(c, d, e, a, b, 28); S_R2(b, c, d, e, a, 29); S_R2(a, b, c, d, e, 30); S_R2(e, a, b, c, d, 31);
	S_R2(d, e, a, b, c, 32); S_R2(c, d, e, a, b, 33); S_R2(b, c, d, e, a, 34); S_R2(a, b, c, d, e, 35);
	S_R2(e, a, b, c, d, 36); S_R2(d, e, a, b, c, 37); S_R2(c, d, e, a, b, 38); S_R2(b, c, d, e, a, 39);
	S_R3(a, b, c, d, e, 40); S_R3(e, a, b, c, d, 41); S_R3(d, e, a, b, c, 42); S_R3(c, d, e, a, b, 43);
	S_R3(b, c, d, e, a, 44); S_R3(a, b, c, d, e, 45); S_R3(e, a, b, c, d, 46); S_R3(d, e, a, b, c, 47);
	S_R3(c, d, e, a, b, 48); S_R3(b, c, d, e, a, 49); S_R3(a, b, c, d, e, 50); S_R3(e, a, b, c, d, 51);
	S_R3(d, e, a, b, c, 52); S_R3(c, d, e, a, b, 53); S_R3(b, c, d, e, a, 54); S_R3(a, b, c, d, e, 55);
	S_R3(e, a, b, c, d, 56); S_R3(d, e, a, b, c, 57); S_R3(c, d, e, a, b, 58); S_R3(b, c, d, e, a, 59);
	S_R4(a, b, c, d, e, 60); S_R4(e, a, b, c, d, 61); S_R4(d, e, a, b, c, 62); S_R4(c, d, e, a, b, 63);
	S_R4(b, c, d, e, a, 64); S_R4(a, b, c, d, e, 65); S_R4(e, a, b, c, d, 66); S_R4(d, e, a, b, c, 67);
	S_R4(c, d, e, a, b, 68); S_R4(b, c, d, e, a, 69); S_R4(a, b, c, d, e, 70); S_R4(e, a, b, c, d, 71);
	S_R4(d, e, a, b, c, 72); S_R4(c, d, e, a, b, 73); S_R4(b, c, d, e, a, 74); S_R4(a, b, c, d, e, 75);
	S_R4(e, a, b, c, d, 76); S_R4(d, e, a, b, c, 77); S_R4(c, d, e, a, b, 78); S_R4(b, c, d, e, a, 79);

	// Add the working vars back into State
	pState[0] += a;
	pState[1] += b;
	pState[2] += c;
	pState[3] += d;
	pState[4] += e;
}

void SHA1::Update(const UINT_8* pData, UINT_32 length)
{
	UINT_32 j = ((Count[0] >> 3) & 0x3F);

	if ((Count[0] += (length << 3)) < (length << 3))
		++Count[1]; // Overflow

	Count[1] += (length >> 29);

	UINT_32 i;
	if ((j + length) > 63)
	{
		i = 64 - j;
		memcpy(&Buffer[j], pData, i);
		Transform(State, Buffer);

		for (; (i + 63) < length; i += 64)
			Transform(State, &pData[i]);

		j = 0;
	}
	else
		i = 0;

	if ((length - i) != 0)
		memcpy(&Buffer[j], &pData[i], length - i);
}

void SHA1::Finalize()
{
	UINT_32 i;

	UINT_8 pbFinalCount[8];
	for (i = 0; i < 8; ++i)
		pbFinalCount[i] = static_cast<UINT_8>((Count[((i >= 4) ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 0xFF); // Endian independent

	Update((UINT_8*)"\200", 1);

	while ((Count[0] & 504) != 448)
		Update((UINT_8*)"\0", 1);

	Update(pbFinalCount, 8); // Cause a Transform()

	for (i = 0; i < 20; ++i)
		Digest[i] = static_cast<UINT_8>((State[i >> 2] >> ((3 - (i & 3)) * 8)) & 0xFF);

	Finalized = true;
}

bool SHA1::GetHash(UINT_8* pbDest20) const
{
	assert(Finalized);

	if (pbDest20 == NULL) return false;
	memcpy(pbDest20, Digest, 20);
	return true;
}

SHA1Hash SHA1::GetHash() const
{
	assert(Finalized);

	SHA1Hash hash;
	memcpy(hash.Hash, Digest, 20);

	return hash;
}

void SHA1::Hash(const UINT_8 * data, UINT_32 length)
{
	Reset();
	Update(data, length);
	Finalize();
}

};