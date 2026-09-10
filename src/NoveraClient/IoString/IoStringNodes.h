#ifndef __IO_STRING_NODES_H__
#define __IO_STRING_NODES_H__

#define STRING_BUFFER_SIZE 2048

class IoStringNode
{
public:
	IoStringNode();
	virtual ~IoStringNode();

	void			PrintString(BYTE Alpth);
	void			PrintString(float _x, float _y, BYTE Alpth = 255);
	void			PrintStringPixedYPos(float _x, float _y, BYTE Alpth = 255);

	void			SetText(char* pText);
	const char*		GetText(){return m_szText;};
	void			SetStyle(TextStyle eStyle);
	void			SetTextColor(DWORD color);
	void			SetBackColor(DWORD color);
	void			SetTextSize(float fSize);
	void			SetPos(int _x, int _y);
	void			SetVertAlign(TextVertAlign eType);
	void			SetAlignType(TextAlignType eType);
	void			Set_X(int _x);
	void			Set_y(int _y);
	void			SetLineGap(int _Gap);

	float			GetStringWidth();
	float			GetStringHeight();
	int				Get_X(){return m_nPosX;};
	int				Get_Y(){return m_nPosY;};

	DWORD			GetTextColor() { return m_dwTextColor; }

private:
	char			m_szText[STRING_BUFFER_SIZE];
	TextStyle		m_eStyle;
	DWORD			m_dwTextColor;
	DWORD			m_dwBackColor;
	float			m_fTextSize;
	int				m_nPosX;
	int				m_nPosY;

	TextVertAlign	m_eVAlign;
	TextAlignType	m_eAlign;

	float			m_nSizeX;
	float			m_nSizeY;

	int				m_nLineGap;
};

class IoStringLine
{
	
public:
	IoStringLine();
	virtual ~IoStringLine();

	void						AddString(IoStringNode* pStr);
	void						PrintString(BYTE alpth);
	void						PrintStringSplit(int nCount, BYTE alpth = 255);
	void						PrintString(int _x, int _y, BYTE alpth = 255);
	void						PrintStringPixedYPos(int _x, int _y, BYTE alpth = 255);
	void						PrintStringSplit(int _x, int _y, int nCount, BYTE alpth = 255);
	void						PrintStringSplitPixedYPos(int _x, int _y, int nCount, BYTE alpth = 255);
	void						SetAlige(TextAlignType eType);
	void						SetVertAlign(TextVertAlign eType);
	void						ProcessAlige(TextAlignType eType);
	void						GetString(char* str);

	void						AddNode(IoStringNode* pNode);

	int							GetLineWidth();
	int							GetLineHeight();
	int							GetNodeSize(){return m_pStringList.size();};
	IoStringNode*				GetNode(int nSuqIndex){ if(nSuqIndex >= 0 && nSuqIndex < (int)m_pStringList.size()) return m_pStringList[nSuqIndex]; return NULL; };

	void						Release();
private:
	int							m_nLineGap;
	int							m_nLiseNO;
	TextAlignType				m_eAlignType;
	TextVertAlign				m_eVertAlige;
	std::vector<IoStringNode*>	m_pStringList;
};

#endif