#ifndef __IO_STRING_H__
#define __IO_STRING_H__

#include "IoStringNodes.h"

class IoString
{
public:
	IoString();
	virtual ~IoString();

	void Set(int _x, int _y, const char* pText, float fSize = FONT_SIZE_12, int nWidth = -1, TextStyle eStyle = TS_NORMAL, DWORD dwTextColor = 0xffffffff, DWORD dwBackColor = 0xffffffff, TextAlignType eAType = TAT_LEFT, TextVertAlign eVType = TVA_TOP, int nLineGap = 0);
	void SetByUIText(IoUIText* pText, int _x = 0, int _y = 0, int nWidth = -1, int nLineGap = 0);
	void SetPos(int _x, int _y);
	void Set_x(int _x);
	void Set_y(int _y);
	void SetText(const char* pText);
	void SetStyle(TextStyle eStyle);
	void SetTextSize(float fSize);
	void SetTextColor(DWORD dwColor);
	void SetBackColor(DWORD dwColor);
	void SetTextColor(const char* szColor);
	void SetBackColor(const char* szColor);
	void SetTextAlign(TextAlignType eType);
	void SetVartAlign(TextVertAlign eType);
	void SetLineGap(int nGap);
	void SetLineWidth(int nWidth);
	void SetAlpha(BYTE alpha){m_Alpha = alpha;};
	void SetAlpthRate(float rate){m_Alpha = (BYTE)(255.0f * rate);};

	IoStringLine*	GetLine(int nLineNO);
	int				GetLineSize();
	int				GetLineWidth(int nLineNO);
	int				GetMaxLineWidth();

	int				GetLineHeight(int nLineNO);
	int				GetMaxHeight();
	int				GetNodeCount();

	int				Get_x(){return m_nPosX;};
	int				Get_y(){return m_nPosY;};
	const char*		GetText(){return m_szText;};
	TextStyle		GetStyle(){return m_eStyle;};
	float			GetTextSize(){return m_fTextSize;};
	DWORD			GetTextColor(){return m_dwTextColor;};
	DWORD			GetBackColor(){return m_dwBackColor;};
	TextAlignType	GetTextAlign(){return m_eAlignType;};
	TextVertAlign	GetVartAlign(){return m_eVertAlige;};
	int				GetLineGap(){return m_nLineGap;};
	int				GetLineWidth(){return m_nLineWidth;};

	void			Parsing();
	void			ParsingSplit(DWORD dwTicTime = 50);
	void			NormalizationSplit_XPos(IoStringLine* pLine);

	int				GetCurrentSplitCount(){return m_dwSplitViewCount;};
	void			ResetSplit(){m_dwSplitViewCount = 0;}
	void			EndSplit(){m_dwSplitViewCount = GetNodeCount();};
	bool			IsEndSpilt(){if(m_dwSplitViewCount >= GetNodeCount())return true; return false;};

	void			OnRender();
	void			OnRender(int _x, int _y);
	void			OnRenderLine(int nLine, int _x, int _y);
	void			OnRenderSplit();
	void			OnRenderSplit(int _x, int _y);

	void			Release();

	void			Update();

protected:

	void			AddNode(IoStringNode* pNode, int nLineNo);

	char						m_szText[STRING_BUFFER_SIZE];
	TextStyle					m_eStyle;
	DWORD						m_dwTextColor;
	DWORD						m_dwBackColor;
	float						m_fTextSize;
	int							m_nPosX;
	int							m_nPosY;
	TextAlignType				m_eAlignType;
	TextVertAlign				m_eVertAlige;
	int							m_nLineGap;
	int							m_nLineWidth;
	BYTE						m_Alpha;

	std::vector<IoStringLine*> m_pStringLineList;

	DWORD						m_dwBeforTicTime;
	DWORD						m_dwBeforTime;
	DWORD						m_dwTicTime;
	int							m_dwSplitViewCount;
};

#endif