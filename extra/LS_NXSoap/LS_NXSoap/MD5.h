// MD5.h: interface for the CMD5 class.
//
//////////////////////////////////////////////////////////////////////
 
#pragma once
 
class CMD5  
{
 public:
    CMD5();
    ~CMD5();

public:
 	void Generate(BYTE* file, UINT length);
	BYTE* GetDigest()		{ return m_digest; }
	char* GetDigestHEX();

protected:
	BYTE m_digest[16];
	char m_digestHEX[512];

protected:
    typedef struct 
    {
        UINT state[4];		/* state (ABCD) */
        UINT count[2];		/* number of bits, modulo 2^64 (lsb first) */
        BYTE buffer[64];	/* input buffer */
    } MD5_CTX;

    void MD5Init(MD5_CTX* ctx);
    void MD5Update(MD5_CTX* ctx, BYTE* input, UINT inputlen);
    void MD5Final(BYTE* digest, MD5_CTX* ctx);
};
 