#include <stdafx.h>
#include "IoStringNodes.h"

IoStringNode::IoStringNode()
{
	memset(m_szText, 0, STRING_BUFFER_SIZE);
	m_eStyle = TS_NORMAL;
	m_dwTextColor = TCT_DEFAULT_WHITE;
	m_dwBackColor = TCT_DEFAULT_WHITE;
	m_fTextSize = FONT_SIZE_10;
	m_nPosX = 0;
	m_nPosY = 0;
	m_eVAlign = TVA_TOP;
	m_nLineGap = 0;
}
IoStringNode::~IoStringNode()
{

}

void IoStringNode::PrintString(BYTE Alpth)
{
	g_FontMgr.SetTextColor(m_dwTextColor);
	g_FontMgr.SetTextStyle(m_eStyle);
	g_FontMgr.SetBkColor(m_dwBackColor);
	g_FontMgr.SetVertAlign(m_eVAlign);

	g_FontMgr.PrintTextAlpha(m_nPosX, m_nPosY, m_fTextSize, Alpth, (const char*)m_szText);
}

void IoStringNode::PrintString(float _x, float _y, BYTE Alpth)
{
	g_FontMgr.SetTextColor(m_dwTextColor);
	g_FontMgr.SetTextStyle(m_eStyle);
	g_FontMgr.SetBkColor(m_dwBackColor);
	g_FontMgr.SetVertAlign(m_eVAlign);
	g_FontMgr.SetAlignType(TAT_LEFT);

	g_FontMgr.PrintTextAlpha(_x + m_nPosX, _y + m_nPosY, m_fTextSize, Alpth, (const char*)m_szText);
}

void IoStringNode::PrintStringPixedYPos(float _x, float _y, BYTE Alpth)
{
	g_FontMgr.SetTextColor(m_dwTextColor);
	g_FontMgr.SetTextStyle(m_eStyle);
	g_FontMgr.SetBkColor(m_dwBackColor);
	g_FontMgr.SetVertAlign(m_eVAlign);
	g_FontMgr.SetAlignType(TAT_LEFT);

	g_FontMgr.PrintTextAlpha(_x + m_nPosX, _y, m_fTextSize, Alpth, (const char*)m_szText);
}

void IoStringNode::SetText(char* pText)
{
	sprintf_e(m_szText, "%s", pText);
}

void IoStringNode::SetStyle(TextStyle eStyle)
{
	m_eStyle = eStyle;
}

void IoStringNode::SetTextColor(DWORD color)
{
	m_dwTextColor = color;
}

void IoStringNode::SetBackColor(DWORD color)
{
	m_dwBackColor = color;
}

void IoStringNode::SetTextSize(float fSize)
{
	m_fTextSize = fSize;
}

void IoStringNode::SetPos(int _x, int _y)
{
	m_nPosX = _x;
	m_nPosY = _y;
}

void IoStringNode::SetVertAlign(TextVertAlign eType)
{
	m_eVAlign = eType;
}

void IoStringNode::SetAlignType(TextAlignType eType)
{
	m_eAlign = eType;
}

void IoStringNode::Set_X(int _x)
{
	m_nPosX = _x;
}

void IoStringNode::Set_y(int _y)
{
	m_nPosY = _y;
}

void IoStringNode::SetLineGap( int _Gap )
{
	m_nLineGap = _Gap;
}

float IoStringNode::GetStringWidth()
{
	return g_FontMgr.GetTextWidth(m_szText, m_eStyle, m_fTextSize);
}

float IoStringNode::GetStringHeight()
{
	return g_FontMgr.GetTextHeight(m_szText, m_eStyle, m_fTextSize, m_nLineGap);
}

IoStringLine::IoStringLine()
{
	m_nLiseNO = 0;
	m_eAlignType = TAT_LEFT;
	m_eVertAlige = TVA_TOP;
	m_pStringList.clear();
	m_nLineGap = 0;
}

IoStringLine::~IoStringLine()
{
	Release();

}

void IoStringLine::AddString( IoStringNode* pStr )
{
	m_pStringList.push_back(pStr);
}

void IoStringLine::PrintString(BYTE alpth)
{
	std::vector<IoStringNode*>::iterator it = m_pStringList.begin();

	for(; it != m_pStringList.end(); ++it)
	{
		(*it)->PrintString(alpth);
	}
}

void IoStringLine::PrintStringSplit(int nCount, BYTE alpth)
{
	if( nCount > (int)m_pStringList.size() || nCount == -1)
	{
		nCount = m_pStringList.size();
	}
	
	for(int i = 0; i < nCount; ++i)
	{
		m_pStringList[i]->PrintString(alpth);
	}
}

void IoStringLine::PrintString(int _x, int _y, BYTE alpth)
{
	std::vector<IoStringNode*>::iterator it = m_pStringList.begin();

	float nX = (float)_x;

	for(; it != m_pStringList.end(); ++it)
	{
		(*it)->PrintString(_x, _y, alpth);
		nX = nX + (*it)->GetStringWidth();
	}
}

void IoStringLine::PrintStringPixedYPos(int _x, int _y, BYTE alpth)
{
	std::vector<IoStringNode*>::iterator it = m_pStringList.begin();

	float nX = (float)_x;

	for(; it != m_pStringList.end(); ++it)
	{
		(*it)->PrintStringPixedYPos(_x, _y, alpth);
		nX = nX + (*it)->GetStringWidth();
	}
}

void IoStringLine::PrintStringSplit(int _x, int _y, int nCount, BYTE alpth )
{
	float nX = (float)_x;

	if( nCount > (int)m_pStringList.size() || nCount == -1)
	{
		nCount = m_pStringList.size();
	}

	for(int i = 0; i < nCount; ++i)
	{
		m_pStringList[i]->PrintString(_x, _y, alpth);
		nX = nX + m_pStringList[i]->GetStringWidth();
	}
}
void IoStringLine::PrintStringSplitPixedYPos(int _x, int _y, int nCount, BYTE alpth )
{
	float nX = (float)_x;

	if( nCount > (int)m_pStringList.size() || nCount == -1)
	{
		nCount = m_pStringList.size();
	}

	for(int i = 0; i < nCount; ++i)
	{
		m_pStringList[i]->PrintStringPixedYPos(_x, _y, alpth);
		nX = nX + m_pStringList[i]->GetStringWidth();
	}
}

void IoStringLine::SetAlige( TextAlignType eType )
{
	m_eAlignType = eType;
}

void IoStringLine::SetVertAlign( TextVertAlign eType )
{
	m_eVertAlige = eType;
}

void IoStringLine::ProcessAlige(TextAlignType eType)
{
	float fTotalWidth = 0.0f;
	
	for(int i = 0; i < (int)m_pStringList.size(); ++i)
	{
		fTotalWidth = fTotalWidth + m_pStringList[i]->GetStringWidth();
	}

	switch(eType)
	{
	case TAT_CENTER:
		{
			float fCenter = fTotalWidth * 0.5f;

			for(int i = 0; i < (int)m_pStringList.size(); ++i)
			{
				m_pStringList[i]->Set_X( m_pStringList[i]->Get_X() - (int)fCenter);
			}
		}
		break;
	case TAT_RIGHT:
		{
			for(int i = 0; i < (int)m_pStringList.size(); ++i)
			{
				m_pStringList[i]->Set_X( m_pStringList[i]->Get_X() - (int)fTotalWidth);
			}
		}
		break;;
	}


}

void IoStringLine::AddNode( IoStringNode* pNode)
{
	m_pStringList.push_back(pNode);
}

void IoStringLine::Release()
{
	for(int i = 0; i < (int)m_pStringList.size(); ++i)
	{
		delete(m_pStringList[i]);
	}

	m_pStringList.clear();
}

void IoStringLine::GetString(char* str)
{
	for(int i = 0; i < (int)m_pStringList.size(); ++i)
	{
		strcat(str, m_pStringList[i]->GetText());
	}
}

int IoStringLine::GetLineWidth()
{
	int nRet = 0;

	for(int i = 0; i < (int)m_pStringList.size(); ++i)
	{
		nRet += m_pStringList[i]->GetStringWidth();
	}

	return nRet;
}

int IoStringLine::GetLineHeight()
{
	int nHeight = 0;

	for(int i = 0; i < (int)m_pStringList.size(); ++i)
	{
		int nCurHeight = m_pStringList[i]->GetStringHeight();

		nHeight = max(nHeight, nCurHeight);
	}

	return nHeight;
}
