#pragma once



class ioUIRenderFrame;
class ioUIRenderImage;
class io3DCharEmoticon;

struct CHARACTER;

class ioCharChatBubble
{
protected:
	static ioHashString m_szChatBubble;
	static ioHashString m_szChatBubbleTail;
	static ioHashString m_szEmoticonBack;

	static DWORD m_dwStartTime;
	static DWORD m_dwKeepTime;

	static int   m_iDefatulW;
	static int   m_iDefatulH;
	static int   m_iMaxStringSize;
	static int   m_iMaxStringLine;
	static int   m_iStartStringX;
	static int   m_iStartStringY;
	static int   m_iStringLineStartGap;
	static int   m_iStringLineGap;
	static int   m_iEmoticonX;
	static int   m_iEmoticonY;

	static float m_fEmoticonScale; 

protected:
	enum
	{
		VIEW_WIDTH = 46,
		VIEW_HEIGHT= 46,
	};

	ioUIRenderFrame  *m_pChatBubble;
	ioUIRenderImage  *m_pChatBubbleTail;
	ioUIRenderImage  *m_pEmoticon;
	ioUIRenderImage  *m_pEmoticonBack;

	io3DCharEmoticon *m_p3DChatEmoticon;

protected:
	struct ChatBubble
	{
		DWORD m_dwTextColor;
		ioHashString m_szText;
		ChatBubble()
		{
			m_dwTextColor = 0;
		}
	};
	typedef std::vector< ChatBubble > vChatBubble;
	vChatBubble  m_BubbleList;

	int          m_iBubbleWidth;
	int          m_iBubbleHeight;
	DWORD        m_dwCurrentTime;
	DWORD        m_dwStartAniTime;
	bool         m_bWholeChat;
	ioHashString m_szBubbleName;
	ioHashString m_szBubbleChat;

	DWORD m_dwCurKeepTime;

protected:
	bool IsScreenInRect( int iXPos, int iYPos );
	bool SetScreenInPos( int &rXPos, int &rYPos );
	bool SetVisiblePos( int &rXPos, int &rYPos );

protected:
	void CheckOutScreen( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCamTargetPos, int &iXPos, int &iYPos, bool bVisible );

public:
	static void LoadStaticValues();

public:
	void SetChatBubble( const char *szID, const char *szChat, bool bWholeChat );
	void SetChatBubbleEmoticon( const ioHashString &rkEmoticonName );
	void Set3DChatBubbleEmoticon( const CHARACTER &rkInfo, int iChatLen, bool bOwnerChar, bool bNoSound );
	void SetChatBubbleTime( DWORD dwCurrentTime ){ m_dwCurrentTime = dwCurrentTime; }
	void SetChatBubbleAniTime( DWORD dwAniTime){ m_dwStartAniTime = dwAniTime; }
	void SetChatBubbleDuration( DWORD dwDuration ){ m_dwCurKeepTime = dwDuration; }

public:
	void RenderChatBubble( TeamType eTeamType, int iXPos, int iYPos, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCamTargetPos, bool bVisible );
    
public:
	void UpdateChatBubble( int iXPos, int iYPos );

public:
	bool IsBubbleChat() const;

public:
	const ioHashString &GetChatBubble(){ return m_szBubbleChat; }
	const DWORD &GetCurrentTime(){ return m_dwCurrentTime; }
	bool IsWholeChat(){ return m_bWholeChat; }

public:
	ioCharChatBubble();
	virtual ~ioCharChatBubble();
};

