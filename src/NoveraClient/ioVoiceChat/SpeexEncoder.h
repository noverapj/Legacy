// SpeexEncoder.h: interface for the CSpeexEncoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPEEXENCODER_H__B7D97AB4_DACA_4F94_9B8D_CA48E6511E93__INCLUDED_)
#define AFX_SPEEXENCODER_H__B7D97AB4_DACA_4F94_9B8D_CA48E6511E93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "speex/speex.h"

class CSpeexEncoder  
{
	SpeexBits m_Bits;
	int  m_FrameSize;
	void *m_pState;
	
	bool Init();
	void Clear();
public:
	bool Encode(const char *srcData, int srcSize, char *encodeData, int &encodeSize, int encodeDataSize);
public:
	CSpeexEncoder();
	virtual ~CSpeexEncoder();

};

#endif // !defined(AFX_SPEEXENCODER_H__B7D97AB4_DACA_4F94_9B8D_CA48E6511E93__INCLUDED_)
