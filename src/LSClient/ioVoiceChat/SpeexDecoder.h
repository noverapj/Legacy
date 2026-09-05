// SpeexDecoder.h: interface for the CSpeexDecoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPEEXDECODER_H__0081B2ED_AF8A_418E_BE7F_639DFF180B0A__INCLUDED_)
#define AFX_SPEEXDECODER_H__0081B2ED_AF8A_418E_BE7F_639DFF180B0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "speex/speex.h"

class CSpeexDecoder  
{
	SpeexBits m_Bits;
	int  m_FrameSize;
	void *m_pState;

	bool Init();
	void Clear();
public:
	bool Decode(const char *srcData, int srcSize, char *decodeData, int &decodeSize, int decodeDataSize);
public:
	CSpeexDecoder();
	virtual ~CSpeexDecoder();

};

#endif // !defined(AFX_SPEEXDECODER_H__0081B2ED_AF8A_418E_BE7F_639DFF180B0A__INCLUDED_)
