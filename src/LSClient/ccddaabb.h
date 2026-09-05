#pragma once

/* Rotation operators. x must be unsigned, to get logical right shift*/
#define ROTL(x,y) (((x)<<(y&(WORD_SIZE-1))) | ((x)>>(WORD_SIZE-(y&(WORD_SIZE-1)))))
#define ROTR(x,y) (((x)>>(y&(WORD_SIZE-1))) | ((x)<<(WORD_SIZE-(y&(WORD_SIZE-1)))))

//UJ 130903, 최적화: AND 연산2회 생략, 대신 y는 0~31까지만 허용함
#define ROTL_fast(x,y) (((x)<<y) | ((x)>>(WORD_SIZE-y)))
#define ROTR_fast(x,y) (((x)>>y) | ((x)<<(WORD_SIZE-y)))

class ccddaabb
{
public:
	enum
	{
		WORD_SIZE   = 32,			/* word size in bits                 */ 
		ROUND_NUM   = 1,			/* number of rounds                  */  // 12가 디폴트, 속도 향상을 위해서 1로 설정
		KEY_SIZE    = 16,			/* number of bytes in key            */  
		KEY_CEIL    = 4,			/* number  words in key = ceil(8*b/w)*/
		TABLE_SIZE  = 1,//26,		/* size of table S = 2*(r+1) words   */  //UJ: TABLE_SIZE도 26->1로 줄임
	};
protected:
	//unsigned int                       /* Should be 32-bit = 4 bytes        */
	unsigned int m_iTable[TABLE_SIZE];   /* expanded key table                */
	unsigned int m_iMagicP;              /* magic constants             */
	unsigned int m_iMagicQ;  

protected:
	void Encrypt( const unsigned int *pt, unsigned int *ct); /* 2 WORD input pt/output ct */
	void Decrypt( const unsigned int *ct, unsigned int *pt) const; /* 2 WORD input ct/output pt */ 

public:
	void Setup( const char *szKey, int iMagicP, int iMagicQ ); /* secret input key K[0...b-1] */
	void EncryptByte( const char *szPlain,  char *szCipher, int iSize ); 
	void DecryptByte( const char *szCipher, char *szPlain, int iSize ) const; 
	
public:
	ccddaabb(void); 
	virtual ~ccddaabb(void);
};
