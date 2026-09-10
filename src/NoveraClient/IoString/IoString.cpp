#include <stdafx.h>
#include "IoString.h"

IoString::IoString()
{
	memset(m_szText, 0, STRING_BUFFER_SIZE);
	m_eStyle = TS_NORMAL;
	m_dwTextColor = TCT_DEFAULT_WHITE;
	m_dwBackColor = TCT_DEFAULT_WHITE;
	m_fTextSize = FONT_SIZE_10;
	m_nPosX = 0;
	m_nPosY = 0;
	m_eAlignType = TAT_LEFT;
	m_eVertAlige = TVA_TOP;
	m_nLineGap = 3;
	m_Alpha = 255;

	m_nLineWidth = -1;

	m_dwBeforTicTime = 0l;
	m_dwBeforTime = 0l;
	m_dwTicTime = 0l;
	m_dwSplitViewCount = 0;
}

IoString::~IoString()
{
	Release();
}

void IoString::Set( int _x, int _y, const char* pText, float fSize /*= FONT_SIZE_10*/, int nWidth, TextStyle eStyle /*= TS_NORMAL*/ , DWORD dwTextColor /*= OxFFFFFFFF*/, DWORD dwBackColor /*= OxFFFFFFFF*/, TextAlignType eAType /*= TAT_LEFT*/, TextVertAlign eVType /*= TVA_TOP*/, int nLineGap )
{
	SetPos(_x, _y);
	SetText(pText);
	SetStyle(eStyle);	
	SetTextSize(fSize);
	SetTextColor(dwTextColor);
	SetBackColor(dwBackColor);
	SetTextAlign(eAType);
	SetVartAlign(eVType);
	SetLineGap(nLineGap);
	m_nLineWidth = nWidth;
	Parsing();
}

void IoString::SetPos(int _x, int _y)
{
	Set_x(_x);
	Set_y(_y);
}

void IoString::Set_x(int _x)
{
	m_nPosX = _x;	
}

void IoString::Set_y(int _y)
{
	m_nPosY = _y;
}

void IoString::SetText(const char* pText)
{
	sprintf_s(m_szText, STRING_BUFFER_SIZE, "%s", pText);
}

void IoString::SetStyle(TextStyle eStyle)
{
	m_eStyle = eStyle;
	g_FontMgr.SetTextStyle( m_eStyle );
}

void IoString::SetTextSize(float fSize)
{
	m_fTextSize = fSize;
}

void IoString::SetTextColor(DWORD dwColor)
{
	m_dwTextColor = dwColor;
}

void IoString::SetBackColor(DWORD dwColor)
{
	m_dwBackColor = dwColor;
}

void IoString::SetTextColor(const char* szColor)
{
	m_dwTextColor = strtoul(szColor, NULL, 16);
}

void IoString::SetBackColor(const char* szColor)
{
	m_dwBackColor = strtoul(szColor, NULL, 16);
}

void IoString::SetTextAlign(TextAlignType eType)
{
	m_eAlignType = eType;
}

void IoString::SetVartAlign(TextVertAlign eType)
{
	m_eVertAlige = eType;
}

void IoString::SetLineGap(int nGap)
{
	m_nLineGap = nGap;
}

void IoString::SetLineWidth( int nWidth )
{
	m_nLineWidth = nWidth;
}

IoStringLine* IoString::GetLine(int nLineNO)
{
	if(nLineNO >= 0 && nLineNO < (int)m_pStringLineList.size())
	{
		return m_pStringLineList[nLineNO];
	}
	return NULL;
}

int	IoString::GetLineSize()
{
	return m_pStringLineList.size();	
}

void IoString::Parsing()
{
	Release();

	TextStyle					_eStyle			= m_eStyle;
	DWORD						_dwTextColor	= m_dwTextColor;
	DWORD						_dwBackColor	= m_dwBackColor;
	float						_fTextSize		= m_fTextSize;
	int							_nPosX			= m_nPosX;
	int							_nPosY			= m_nPosY;
	TextAlignType				_eAlignType		= m_eAlignType;
	TextVertAlign				_eVertAlige		= m_eVertAlige;
	int							_nLineGap		= m_nLineGap;
	float						_fLineWidth		= (float)m_nLineWidth;

	float						_fCurrentWidth = 0.0f;
	bool						_bIsMulthByteHead = false;

	char szReadBuffer[STRING_BUFFER_SIZE] = {0,};
	char szProcessBuffer[STRING_BUFFER_SIZE] = {0,};
	int nStringSize = strlen(m_szText);
	int nBufferPos = 0;
	int nLineCount = 0;

	for(int i = 0; i < nStringSize; ++i)
	{
		if(m_szText[i] == 10 || m_szText[i] == 13) // 계행문자 [9/24/2013 안일규])
		{
			i++;
			IoStringNode* pNode = new IoStringNode;
			pNode->SetPos(_nPosX, _nPosY);
			pNode->SetStyle(_eStyle);
			pNode->SetTextColor(_dwTextColor);
			pNode->SetBackColor(_dwBackColor);
			pNode->SetTextSize(_fTextSize);
			pNode->SetText(szProcessBuffer);
			pNode->SetVertAlign(_eVertAlige);
			pNode->SetLineGap(m_nLineGap);
			AddNode(pNode, nLineCount);
				
			memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
			nBufferPos = 0;

			nLineCount++;

			_fCurrentWidth = 0.0f;
			_nPosX = m_nPosX;
			_nPosY += GetLine(nLineCount-1)->GetLineHeight();

		}

		if( i >= nStringSize )
		{
			// need check
			break;
		}

		if( m_szText[i] == 92 && m_szText[i+1] == 110 ) // \ 체크 [9/24/2013 안일규]
		{
			i+=2;

			IoStringNode* pNode = new IoStringNode;
			pNode->SetPos(_nPosX, _nPosY);
			pNode->SetStyle(_eStyle);
			pNode->SetTextColor(_dwTextColor);
			pNode->SetBackColor(_dwBackColor);
			pNode->SetTextSize(_fTextSize);
			pNode->SetText(szProcessBuffer);
			pNode->SetVertAlign(_eVertAlige);
			pNode->SetLineGap(m_nLineGap);
			AddNode(pNode, nLineCount);				

			memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
			nBufferPos = 0;

			nLineCount++;
			_fCurrentWidth = 0.0f;
			_nPosX = m_nPosX;
			_nPosY += GetLine(nLineCount-1)->GetLineHeight();

		}

		if( i >= nStringSize )
		{
			// need check
			break;
		}

		if(m_szText[i] == 60) // '<' 기호의 아스키 코드 테그의 시작을 알린다
		{
			if( (i+13 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 99 && m_szText[i+2] == 58 && m_szText[i+13] == 62) ) // "c:" 텍스트 컬러 변경 코드
			{
				i+=3;

				char color[STRING_BUFFER_SIZE] = {0,};
				color[0] = m_szText[i];
				color[1] = m_szText[i+1];
				color[2] = m_szText[i+2];
				color[3] = m_szText[i+3];
				color[4] = m_szText[i+4];
				color[5] = m_szText[i+5];
				color[6] = m_szText[i+6];
				color[7] = m_szText[i+7];
				color[8] = m_szText[i+8];
				color[9] = m_szText[i+9];

				i+=11;				

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_dwTextColor = strtoul( color, NULL, 16 );
			}

			if( (i+13 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 98 && m_szText[i+2] == 58  && m_szText[i+13] == 62) ) // "b:" 텍스트 벡(그림자/외곽선) 컬러 변경 코드
			{
				i+=3;

				char color[STRING_BUFFER_SIZE] = {0,};
				color[0] = m_szText[i];
				color[1] = m_szText[i+1];
				color[2] = m_szText[i+2];
				color[3] = m_szText[i+3];
				color[4] = m_szText[i+4];
				color[5] = m_szText[i+5];
				color[6] = m_szText[i+6];
				color[7] = m_szText[i+7];
				color[8] = m_szText[i+8];
				color[9] = m_szText[i+9];

				i+=11;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_dwBackColor = strtoul( color, NULL, 16 );
			}

			if( (i+7 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 110 && m_szText[i+2] == 111 && m_szText[i+3] == 114 && m_szText[i+4] == 109 && m_szText[i+5] == 97 && m_szText[i+6] == 108  && m_szText[i+7] == 62) ) // "normal"
			{
				i+=8;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_NORMAL;
			}
			else if( (i+5 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 98 && m_szText[i+2] == 111 && m_szText[i+3] == 108 && m_szText[i+4] == 100 && m_szText[i+5] == 62) ) //"bold"
			{
				i+=6;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);
					
					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_BOLD;
			}
			else if( (i+4 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 111 && m_szText[i+2] == 117 && m_szText[i+3] == 116 && m_szText[i+4] == 62) ) // "out"
			{
				i+=5;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);
					
					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_OUTLINE;
			}
			else if( (i+5 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 111 && m_szText[i+2] == 117 && m_szText[i+3] == 116 && m_szText[i+4] == 50 && m_szText[i+5] == 62) ) // "out2"
			{
				i+=6;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);
					
					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_OUTLINE_2X;
			}
			else if( (i+5 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 111 && m_szText[i+2] == 117 && m_szText[i+3] == 116 && m_szText[i+4] == 102 && m_szText[i+5] == 62) ) // "outf"
			{
				i+=6;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_OUTLINE_FULL;
			}
			else if( (i+6 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 111 && m_szText[i+2] == 117 && m_szText[i+3] == 116 && m_szText[i+4] == 102 && m_szText[i+5] == 50 && m_szText[i+6] == 62) ) // "outf2"
			{
				i+=7;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_OUTLINE_FULL_2X;
			}
			else if( (i+6 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 111 && m_szText[i+2] == 117 && m_szText[i+3] == 116 && m_szText[i+4] == 102 && m_szText[i+5] == 51 && m_szText[i+6] == 62) ) // "outf3"
			{
				i+=7;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_OUTLINE_FULL_3X;
			}
			else if( (i+6 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 111 && m_szText[i+2] == 117 && m_szText[i+3] == 116 && m_szText[i+4] == 102 && m_szText[i+5] == 52 && m_szText[i+6] == 62) ) // "outf4"
			{
				i+=7;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_OUTLINE_FULL_4X;
			}
			else if( (i+7 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 115 && m_szText[i+2] == 104 && m_szText[i+3] == 97 && m_szText[i+4] == 100 && m_szText[i+5] == 111 && m_szText[i+6] == 119  && m_szText[i+7] == 62) ) // "shadow"
			{
				i+=8;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);
					
					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_SHADOW;
			}
			else if( (i+5 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 98 && m_szText[i+2] == 111 && m_szText[i+3] == 117 && m_szText[i+4] == 116 && m_szText[i+5] == 62) ) // "bout"
			{
				i+=6;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_BOLD_OUTLINE;
			}
			else if( (i+8 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 98 && m_szText[i+2] == 115 && m_szText[i+3] == 104 && m_szText[i+4] == 97 && m_szText[i+5] == 100 && m_szText[i+6] == 111 && m_szText[i+7] == 119  && m_szText[i+8] == 62) ) // "bshadow"
			{
				i+=9;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_BOLD_SHADOW;
			}
			else if( (i+6 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 98 && m_szText[i+2] == 111 && m_szText[i+3] == 117 && m_szText[i+4] == 116 && m_szText[i+5] == 102 && m_szText[i+6] == 62) ) // "boutf"
			{
				i+=7;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_BOLD_OUTLINE_FULL;
			}
			else if( (i+7 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 98 && m_szText[i+2] == 111 && m_szText[i+3] == 117 && m_szText[i+4] == 116 && m_szText[i+5] == 102 && m_szText[i+6] == 50 && m_szText[i+7] == 62) ) // "boutf2"
			{
				i+=8;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_BOLD_OUTLINE_FULL_2X;
			}
			else if( (i+7 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 98 && m_szText[i+2] == 111 && m_szText[i+3] == 117 && m_szText[i+4] == 116 && m_szText[i+5] == 102 && m_szText[i+6] == 51 && m_szText[i+7] == 62) ) // "boutf3"
			{
				i+=8;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_BOLD_OUTLINE_FULL_3X;
			}
			else if( (i+7 < STRING_BUFFER_SIZE) && (m_szText[i+1] == 98 && m_szText[i+2] == 111 && m_szText[i+3] == 117 && m_szText[i+4] == 116 && m_szText[i+5] == 102 && m_szText[i+6] == 52 && m_szText[i+7] == 62) ) // "boutf4"
			{
				i+=8;

				if(strlen(szProcessBuffer) > 0)
				{
					IoStringNode* pNode = new IoStringNode;
					pNode->SetPos(_nPosX, _nPosY);
					pNode->SetStyle(_eStyle);
					pNode->SetTextColor(_dwTextColor);
					pNode->SetBackColor(_dwBackColor);
					pNode->SetTextSize(_fTextSize);
					pNode->SetText(szProcessBuffer);
					pNode->SetVertAlign(_eVertAlige);
					pNode->SetLineGap(m_nLineGap);
					AddNode(pNode, nLineCount);

					_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

					memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
					nBufferPos = 0;
				}

				_eStyle = TS_BOLD_OUTLINE_FULL_4X;
			}
		}

		if( i >= nStringSize )
		{
			// need check
			break;
		}

		UINT nCodePage = GetACP();
		BYTE word = (BYTE)m_szText[i];
 		if( IsDBCSLeadByteEx(nCodePage, word ) )
 		{
			if( i+1 >= nStringSize )
			{
				// need check
				break;
			}

			_bIsMulthByteHead = true;
			szReadBuffer[0] = m_szText[i];
			szReadBuffer[1] = m_szText[i+1];
			i++;
		}
		else
		{
			szReadBuffer[0] = m_szText[i];
		}

		if(/*_bIsMulthByteHead == false && */m_nLineWidth > 0)
		{
			float fCurTextWidth = g_FontMgr.GetTextWidth(szReadBuffer, _eStyle, _fTextSize);;
			_fCurrentWidth += fCurTextWidth;

			if(_fCurrentWidth > _fLineWidth)
			{
				IoStringNode* pNode = new IoStringNode;
				pNode->SetPos(_nPosX, _nPosY);
				pNode->SetStyle(_eStyle);
				pNode->SetTextColor(_dwTextColor);
				pNode->SetBackColor(_dwBackColor);
				pNode->SetTextSize(_fTextSize);
				pNode->SetText(szProcessBuffer);
				pNode->SetVertAlign(_eVertAlige);
				pNode->SetLineGap(m_nLineGap);
				AddNode(pNode, nLineCount);

				memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
				nBufferPos = 0;

				nLineCount++;
				_fCurrentWidth = fCurTextWidth;

				_nPosX = m_nPosX;
				_nPosY += GetLine(nLineCount-1)->GetLineHeight();
			}
		}

		if(_bIsMulthByteHead == false)
		{
			szProcessBuffer[nBufferPos] = szReadBuffer[0];
			memset(szReadBuffer, 0, STRING_BUFFER_SIZE);
			nBufferPos++;
		}else{
			szProcessBuffer[nBufferPos] = szReadBuffer[0];
			szProcessBuffer[nBufferPos+1] = szReadBuffer[1];
			memset(szReadBuffer, 0, STRING_BUFFER_SIZE);
			nBufferPos+=2;
			_bIsMulthByteHead = false;
		}
	}

	IoStringNode* pNode = new IoStringNode;
	pNode->SetPos(_nPosX, _nPosY);
	pNode->SetStyle(_eStyle);
	pNode->SetTextColor(_dwTextColor);
	pNode->SetBackColor(_dwBackColor);
	pNode->SetTextSize(_fTextSize);
	pNode->SetText(szProcessBuffer);
	pNode->SetVertAlign(_eVertAlige);
	pNode->SetLineGap(m_nLineGap);
	AddNode(pNode, nLineCount);

	for(int i = 0;i < (int)m_pStringLineList.size(); ++i)
	{
		g_FontMgr.SetAlignType(TAT_LEFT);
		m_pStringLineList[i]->ProcessAlige(_eAlignType);
	}
}

void IoString::AddNode( IoStringNode* pNode, int nLineNo )
{
	if((int)m_pStringLineList.size() > nLineNo)
	{
		m_pStringLineList[nLineNo]->AddNode(pNode);
	}else{
		IoStringLine* pLine = new IoStringLine;
		pLine->SetAlige(m_eAlignType);
		pNode->SetAlignType(m_eAlignType);
		pLine->AddNode(pNode);
		m_pStringLineList.push_back(pLine);
	}
}

void IoString::OnRender()
{
	for(int i = 0; i < (int)m_pStringLineList.size(); ++i)
	{
		g_FontMgr.SetAlignType(TAT_LEFT);
		g_FontMgr.SetVertAlign(m_eVertAlige);
		m_pStringLineList[i]->PrintString(m_Alpha);
	}
}


void IoString::OnRender(int _x, int _y)
{
	for(int i = 0; i < (int)m_pStringLineList.size(); ++i)
	{
		g_FontMgr.SetTextStyle( m_eStyle );
		g_FontMgr.SetAlignType(TAT_LEFT);
		g_FontMgr.SetVertAlign(m_eVertAlige);
		m_pStringLineList[i]->PrintString(_x, _y, m_Alpha);
	}
}

void IoString::OnRenderLine(int nLine, int _x, int _y)
{
	if(nLine >= 0 && nLine < (int)m_pStringLineList.size())
	{
		g_FontMgr.SetAlignType(TAT_LEFT);
		g_FontMgr.SetVertAlign(m_eVertAlige);
		m_pStringLineList[nLine]->PrintString(_x, _y, m_Alpha);
	}
}

void IoString::OnRenderSplit()
{
	if(m_dwSplitViewCount > GetNodeCount() || m_dwSplitViewCount == -1)
	{
		OnRender();
	}else{

		int nCount = m_dwSplitViewCount;
		for(int i = 0; i < (int)m_pStringLineList.size(); ++i)
		{
			g_FontMgr.SetAlignType(TAT_LEFT);
			g_FontMgr.SetVertAlign(m_eVertAlige);

			if(nCount > m_pStringLineList[i]->GetNodeSize())
			{
				m_pStringLineList[i]->PrintStringSplit(-1, m_Alpha);
				nCount -= m_pStringLineList[i]->GetNodeSize();
			}else{
				m_pStringLineList[i]->PrintStringSplit(nCount, m_Alpha);
			}
		}		
	}
}

void IoString::OnRenderSplit(int _x, int _y)
{
	if(m_dwSplitViewCount > GetNodeCount() || m_dwSplitViewCount == -1)
	{
		OnRender(_x, _y);
	}else{

		int nCount = m_dwSplitViewCount;
		for(int i = 0; i < (int)m_pStringLineList.size(); ++i)
		{
			g_FontMgr.SetAlignType(TAT_LEFT);
			g_FontMgr.SetVertAlign(m_eVertAlige);

			if(nCount > m_pStringLineList[i]->GetNodeSize())
			{
				m_pStringLineList[i]->PrintStringSplit(_x, _y, -1, m_Alpha);
				nCount -= m_pStringLineList[i]->GetNodeSize();
			}else{
				m_pStringLineList[i]->PrintStringSplit(_x, _y, nCount, m_Alpha);
				nCount = max(0, nCount - m_pStringLineList[i]->GetNodeSize());
			}
		}		
	}
}

void IoString::Release()
{
	for(int i = 0 ; i < (int)m_pStringLineList.size(); ++i)
	{
		delete(m_pStringLineList[i]);
	}
	m_pStringLineList.clear();
}

int IoString::GetLineWidth( int nLineNO )
{
	if(nLineNO < (int)m_pStringLineList.size())
	{
		return m_pStringLineList[nLineNO]->GetLineWidth();
	}

	return 0;
}

int IoString::GetMaxLineWidth()
{
	int nMaxWidth = 0;

	for(int i = 0; i < (int)GetLineSize(); ++i)
	{
		int nCurLineWidth = GetLine(i)->GetLineWidth();

		nMaxWidth = max( nCurLineWidth, nMaxWidth );
	}

	return  nMaxWidth;
}

int IoString::GetLineHeight(int nLineNO)
{
	if(nLineNO < (int)m_pStringLineList.size())
	{
		return m_pStringLineList[nLineNO]->GetLineHeight();
	}
	return 0;
}

int	IoString::GetMaxHeight()
{
	int nRet = 0;

	for(int i = 0; i < (int)m_pStringLineList.size(); ++i)
	{
		nRet += m_pStringLineList[i]->GetLineHeight();
	}

	return nRet;
}

void IoString::SetByUIText(IoUIText* pText, int _x, int _y, int nWidth, int nLineGap)
{
	if(pText)
	{
		SetPos(_x+pText->GetPosX(), _y+pText->GetPosY());
		SetText(pText->GetText().c_str());
		SetStyle(pText->GetTextStyle());	
		SetTextSize(pText->GetScale() / g_FontMgr.GetFontSize());
		SetTextColor(pText->GetTextColor());
		SetBackColor(pText->GetBackColor());
		SetTextAlign(pText->GetHorzAlign());
		SetVartAlign(pText->GetVertAlign());
		SetLineGap(nLineGap);
		m_nLineWidth = nWidth;
		Parsing();

		pText->SetVisible(false);
	}
}

void IoString::ParsingSplit(DWORD dwTicTime)
{
	Release();

	TextStyle					_eStyle			= m_eStyle;
	DWORD						_dwTextColor	= m_dwTextColor;
	DWORD						_dwBackColor	= m_dwBackColor;
	float						_fTextSize		= m_fTextSize;
	int							_nPosX			= m_nPosX;
	int							_nPosY			= m_nPosY;
	TextAlignType				_eAlignType		= m_eAlignType;
	TextVertAlign				_eVertAlige		= m_eVertAlige;
	int							_nLineGap		= m_nLineGap;
	float						_fLineWidth		= (float)m_nLineWidth;

	float						_fCurrentWidth = 0.0f;
	bool						_bIsMulthByteHead = false;

	char szReadBuffer[STRING_BUFFER_SIZE] = {0,};
	char szProcessBuffer[STRING_BUFFER_SIZE] = {0,};
	int nStringSize = strlen(m_szText);
	int nBufferPos = 0;
	int nLineCount = 0;

	for(int i = 0; i < nStringSize; ++i)
	{
		if(m_szText[i] == 10 || m_szText[i] == 13) // 계행문자 [9/24/2013 안일규])
		{
			i++;

			NormalizationSplit_XPos(GetLine(nLineCount));

			nLineCount++;

			_fCurrentWidth = 0.0f;
			_nPosX = m_nPosX;
			_nPosY += GetLine(nLineCount-1)->GetLineHeight() + m_nLineGap;
		}

		if( i >= nStringSize )
		{
			// need check
			break;
		}

		if( m_szText[i] == 92 && m_szText[i+1] == 110 ) // \ 체크 [9/24/2013 안일규]
		{
			i+=2;

			//NormalizationSplit_XPos(GetLine(nLineCount));

			nLineCount++;
			_fCurrentWidth = 0.0f;
			_nPosX = m_nPosX;
			_nPosY += GetLine(nLineCount-1)->GetLineHeight() + m_nLineGap;

		}

		if( i >= nStringSize )
		{
			// need check
			break;
		}

		if(m_szText[i] == 60) // '<' 기호의 아스키 코드 테그의 시작을 알린다
		{
			if( i+13 < STRING_BUFFER_SIZE && m_szText[i+1] == 99 && m_szText[i+2] == 58 && m_szText[i+13] == 62 ) // "c:" 텍스트 컬러 변경 코드
			{
				i+=3;

				char color[STRING_BUFFER_SIZE] = {0,};
				color[0] = m_szText[i];
				color[1] = m_szText[i+1];
				color[2] = m_szText[i+2];
				color[3] = m_szText[i+3];
				color[4] = m_szText[i+4];
				color[5] = m_szText[i+5];
				color[6] = m_szText[i+6];
				color[7] = m_szText[i+7];
				color[8] = m_szText[i+8];
				color[9] = m_szText[i+9];

				i+=11;				
				
				_dwTextColor = strtoul( color, NULL, 16 );
			}

			if( i+13 < STRING_BUFFER_SIZE && m_szText[i+1] == 98 && m_szText[i+2] == 58 && m_szText[i+13] == 62 ) // "b:" 텍스트 벡(그림자/외곽선) 컬러 변경 코드
			{
				i+=3;

				char color[STRING_BUFFER_SIZE] = {0,};
				color[0] = m_szText[i];
				color[1] = m_szText[i+1];
				color[2] = m_szText[i+2];
				color[3] = m_szText[i+3];
				color[4] = m_szText[i+4];
				color[5] = m_szText[i+5];
				color[6] = m_szText[i+6];
				color[7] = m_szText[i+7];
				color[8] = m_szText[i+8];
				color[9] = m_szText[i+9];

				i+=11;

				_dwBackColor = strtoul( color, NULL, 16 );
			}

			if( i+7 < STRING_BUFFER_SIZE && 
				m_szText[i+1] == 110 &&
				m_szText[i+2] == 111 &&
				m_szText[i+3] == 114 &&
				m_szText[i+4] == 109 &&
				m_szText[i+5] == 97 &&
				m_szText[i+6] == 108 &&
				m_szText[i+7] == 62 ) // "normal"
			{
				i+=8;

				_eStyle = TS_NORMAL;
			}
			else if( i+5 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 98 &&
				     m_szText[i+2] == 111 &&
					 m_szText[i+3] == 108 &&
					 m_szText[i+4] == 100 &&
					 m_szText[i+5] == 62 ) //"bold"
			{
				i+=6;

				_eStyle = TS_BOLD;
			}
			else if( i+4 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 111 &&
				     m_szText[i+2] == 117 &&
					 m_szText[i+3] == 116 &&
					 m_szText[i+4] == 62 ) // "out"
			{
				i+=5;

				_eStyle = TS_OUTLINE;
			}
			else if( i+5 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 111 &&
				     m_szText[i+2] == 117 &&
					 m_szText[i+3] == 116 &&
					 m_szText[i+4] == 50 &&
					 m_szText[i+5] == 62 ) // "out2"
			{
				i+=6;

				_eStyle = TS_OUTLINE_2X;
			}
			else if( i+5 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 111 &&
				     m_szText[i+2] == 117 &&
					 m_szText[i+3] == 116 &&
					 m_szText[i+4] == 102 &&
					 m_szText[i+5] == 62 ) // "outf"
			{
				i+=6;

				_eStyle = TS_OUTLINE_FULL;
			}
			else if( i+6 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 111 &&
				     m_szText[i+2] == 117 &&
					 m_szText[i+3] == 116 &&
					 m_szText[i+4] == 102 &&
					 m_szText[i+5] == 50 &&
					 m_szText[i+6] == 62 ) // "outf2"
			{
				i+=7;

				_eStyle = TS_OUTLINE_FULL_2X;
			}
			else if( i+6 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 111 &&
				     m_szText[i+2] == 117 &&
					 m_szText[i+3] == 116 &&
					 m_szText[i+4] == 102 &&
					 m_szText[i+5] == 51 &&
					 m_szText[i+6] == 62 ) // "outf3"
			{
				i+=7;

				_eStyle = TS_OUTLINE_FULL_3X;
			}
			else if( i+6 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 111 &&
				     m_szText[i+2] == 117 &&
					 m_szText[i+3] == 116 &&
					 m_szText[i+4] == 102 &&
					 m_szText[i+5] == 52 &&
					 m_szText[i+6] == 62 ) // "outf4"
			{
				i+=7;

				_eStyle = TS_OUTLINE_FULL_4X;
			}
			else if( i+7 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 115 &&
				     m_szText[i+2] == 104 &&
					 m_szText[i+3] == 97 &&
					 m_szText[i+4] == 100 &&
					 m_szText[i+5] == 111 &&
					 m_szText[i+6] == 119 &&
					 m_szText[i+7] == 62 ) // "shadow"
			{
				i+=8;

				_eStyle = TS_SHADOW;
			}
			else if( i+5 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 98 &&
				     m_szText[i+2] == 111 &&
					 m_szText[i+3] == 117 &&
					 m_szText[i+4] == 116 &&
					 m_szText[i+5] == 62 ) // "bout"
			{
				i+=6;

				_eStyle = TS_BOLD_OUTLINE;
			}
			else if( i+8 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 98 &&
				     m_szText[i+2] == 115 &&
					 m_szText[i+3] == 104 &&
					 m_szText[i+4] == 97 &&
					 m_szText[i+5] == 100 &&
					 m_szText[i+6] == 111 &&
					 m_szText[i+7] == 119 &&
					 m_szText[i+8] == 62 ) // "bshadow"
			{
				i+=9;

				_eStyle = TS_BOLD_SHADOW;
			}
			else if( i+6 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 98 &&
				     m_szText[i+2] == 111 &&
					 m_szText[i+3] == 117 &&
					 m_szText[i+4] == 116 &&
					 m_szText[i+5] == 102 &&
					 m_szText[i+6] == 62 ) // "boutf"
			{
				i+=7;

				_eStyle = TS_BOLD_OUTLINE_FULL;
			}
			else if( i+7 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 98 &&
				     m_szText[i+2] == 111 &&
					 m_szText[i+3] == 117 &&
					 m_szText[i+4] == 116 &&
					 m_szText[i+5] == 102 &&
					 m_szText[i+6] == 50 &&
					 m_szText[i+7] == 62 ) // "boutf2"
			{
				i+=8;

				_eStyle = TS_BOLD_OUTLINE_FULL_2X;
			}
			else if( i+7 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 98 &&
				     m_szText[i+2] == 111 &&
					 m_szText[i+3] == 117 &&
					 m_szText[i+4] == 116 &&
					 m_szText[i+5] == 102 &&
					 m_szText[i+6] == 51 &&
					 m_szText[i+7] == 62 ) // "boutf3"
			{
				i+=8;

				_eStyle = TS_BOLD_OUTLINE_FULL_3X;
			}
			else if( i+7 < STRING_BUFFER_SIZE &&
					 m_szText[i+1] == 98 &&
				     m_szText[i+2] == 111 &&
					 m_szText[i+3] == 117 &&
					 m_szText[i+4] == 116 &&
					 m_szText[i+5] == 102 &&
					 m_szText[i+6] == 52 &&
					 m_szText[i+7] == 62 ) // "boutf4"
			{
				i+=8;

				_eStyle = TS_BOLD_OUTLINE_FULL_4X;
			}
		}

		if( i >= nStringSize )
		{
			// need check
			break;
		}

		UINT nCodePage = GetACP();
		BYTE word = (BYTE)m_szText[i];
		if( IsDBCSLeadByteEx(nCodePage, word ) )
		{
			if( i+1 >= nStringSize )
			{
				// need check
				break;
			}
			_bIsMulthByteHead = true;
			szReadBuffer[0] = m_szText[i];
			szReadBuffer[1] = m_szText[i+1];
			i++;
		}
		else
		{
			szReadBuffer[0] = m_szText[i];
		}

		if(/*_bIsMulthByteHead == false && */m_nLineWidth > 0)
		{
			_fCurrentWidth += g_FontMgr.GetTextWidth(szReadBuffer, _eStyle, _fTextSize);

			if(_fCurrentWidth > _fLineWidth)
			{
				NormalizationSplit_XPos(GetLine(nLineCount));

				nLineCount++;
				_fCurrentWidth = 0.0f;

				_nPosX = m_nPosX;
				_nPosY += GetLine(nLineCount-1)->GetLineHeight() + m_nLineGap;
			}
		}

		if(_bIsMulthByteHead == false)
		{
			szProcessBuffer[nBufferPos] = szReadBuffer[0];
			memset(szReadBuffer, 0, STRING_BUFFER_SIZE);
			nBufferPos++;
		}else{
			szProcessBuffer[nBufferPos] = szReadBuffer[0];
			szProcessBuffer[nBufferPos+1] = szReadBuffer[1];
			memset(szReadBuffer, 0, STRING_BUFFER_SIZE);
			nBufferPos+=2;
			_bIsMulthByteHead = false;
		}

		IoStringNode* pNode = new IoStringNode;
		pNode->SetPos(_nPosX, _nPosY);
		pNode->SetStyle(_eStyle);
		pNode->SetTextColor(_dwTextColor);
		pNode->SetBackColor(_dwBackColor);
		pNode->SetTextSize(_fTextSize);
		pNode->SetText(szProcessBuffer);
		pNode->SetVertAlign(_eVertAlige);
		//pNode->SetLineGap(m_nLineGap);
		pNode->SetLineGap(0);
		AddNode(pNode, nLineCount);

		_nPosX += g_FontMgr.GetTextWidth(szProcessBuffer, _eStyle, _fTextSize);

		memset(szProcessBuffer, 0, STRING_BUFFER_SIZE);
		nBufferPos = 0;
	}

	for(int i = 0;i < (int)m_pStringLineList.size(); ++i)
	{
		g_FontMgr.SetAlignType(TAT_LEFT);
		m_pStringLineList[i]->ProcessAlige(_eAlignType);
	}

	m_dwTicTime = dwTicTime;
}

int IoString::GetNodeCount()
{
	int nResult = 0;
	for(int i = 0; i < (int)m_pStringLineList.size(); ++i)
	{
		nResult += m_pStringLineList[i]->GetNodeSize();
	}

	return nResult;
}

void IoString::Update()
{
	if( (FRAMEGETTIME() - m_dwBeforTicTime) >= m_dwTicTime )
	{
		if(m_dwSplitViewCount < GetNodeCount())
		{
			m_dwSplitViewCount++;
			m_dwBeforTicTime = FRAMEGETTIME();
		}
	}

	m_dwBeforTime = FRAMEGETTIME();
}

void IoString::NormalizationSplit_XPos( IoStringLine* pLine )
{
	if(!pLine) return;

	if(m_nLineWidth < 0 ) return;

	int nSize = pLine->GetNodeSize();
	float fLineGap = (float)(m_nLineWidth - pLine->GetLineWidth());
	float fNodeGap = fLineGap/(float)nSize;

	int nTemp1 = fNodeGap*10.0f;
	int nTemp2 = nTemp1%10;
	if(nTemp2 >= 5)
	{
		fNodeGap = fNodeGap + 1.0f;
	}

	for(int i = 0; i < nSize; ++i)
	{
		IoStringNode* pNode = pLine->GetNode(i);
		if(pNode)
		{
			int nX = pNode->Get_X();
			pNode->Set_X(nX + (fNodeGap*(float)i));
		}
	}
}
