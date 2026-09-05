#include "stdafx.h"
#include "ccddaabb.h"

ccddaabb::ccddaabb(void)
{
}

ccddaabb::~ccddaabb(void)
{
}

void ccddaabb::Setup( const char *szKey, int iMagicP, int iMagicQ )
{
	m_iMagicP = iMagicP;
	m_iMagicQ = iMagicQ;

	//UJ 130913, 속도향상을 위해 m_iTable[0]만 사용 --start
	//unsigned int i, j, k, u=WORD_SIZE/8, A, B, L[KEY_CEIL]; 
	///* Initialize L, then S, then mix key into S */
	//for(i=KEY_SIZE-1,L[KEY_CEIL-1]=0; i!=-1; i--) 
	//{
	//	L[i/u] = (L[i/u]<<8)+szKey[i];
	//}
	//for (m_iTable[0]=m_iMagicP,i=1; i<TABLE_SIZE; i++) 
	//{
	//	m_iTable[i] = m_iTable[i-1]+m_iMagicQ;
	//}
	//for (A=B=i=j=k=0; k<3*TABLE_SIZE; k++,i=(i+1)%TABLE_SIZE,j=(j+1)%KEY_CEIL)   /* 3*t > 3*c */
	//{ 
	//	A = m_iTable[i] = ROTL_fast(m_iTable[i]+(A+B),3);  
	//	B = L[j] = ROTL(L[j]+(A+B),(A+B)); 
	//}
	m_iTable[0] = ROTL_fast(m_iMagicP, 3);
	//UJ --end
}

void ccddaabb::Encrypt( const unsigned int *pt, unsigned int *ct ) /* 2 WORD input pt/output ct */
{
	//UJ 130829, 속도향상을 위해 4bytes 단위로 수행하게 변경 --start
	//unsigned int i, A=pt[0]+m_iTable[0], B=pt[1]+m_iTable[1];
	//for (i=1; i<=ROUND_NUM; i++) 
	//{
	//	A = ROTL(A^B,B)+m_iTable[2*i]; 
	//	B = ROTL(B^A,A)+m_iTable[2*i+1]; 
	//}
	//ct[0] = A; ct[1] = B;
	unsigned int A=pt[0]+m_iTable[0];
	A = ROTL_fast(A^0xFAC6D9,7);
	ct[0] = A;
	//UJ --end
}

void ccddaabb::Decrypt( const unsigned int *ct, unsigned int *pt ) const /* 2 WORD input ct/output pt */
{
	//UJ 130829, 속도향상을 위해 4bytes 단위로 수행하게 변경 --start
	//unsigned int i, B=ct[1], A=ct[0];
	//for (i=ROUND_NUM; i>0; i--) 
	//{ 
	//	B = ROTR(B-m_iTable[2*i+1],A)^A; 
	//	A = ROTR(A-m_iTable[2*i],B)^B; 
	//}
	//pt[1] = B-m_iTable[1]; pt[0] = A-m_iTable[0];
	unsigned int A=ct[0];
	A = ROTR_fast(A,7)^0xFAC6D9; 
	pt[0] = A-m_iTable[0];
	//UJ --end
}

void ccddaabb::EncryptByte( const char *szPlain, char *szCipher, int iSize )
{
	//UJ 130829, 속도향상을 위해 4bytes 단위로 수행하게 변경
	//if( (iSize%8) != 0 )
	if( (iSize%4) != 0 )
		return;

	//for (int i = 0; i < iSize ; i+=8)
	for (int i = 0; i < iSize ; i+=4)
	{
		Encrypt( (const unsigned int*)&szPlain[i], (unsigned int*)&szCipher[i] );
	}
}

void ccddaabb::DecryptByte( const char *szCipher, char *szPlain, int iSize ) const
{
	//UJ 130829, 속도향상을 위해 4bytes 단위로 수행하게 변경
	//if( (iSize%4) != 0 )
	//	return;
	//
	//for (int i = 0; i < iSize ; i+=8)
	for (int i = 0; i < iSize ; i+=4)
	{
		Decrypt( (const unsigned int*)&szCipher[i], (unsigned int*)&szPlain[i] );
	}
}