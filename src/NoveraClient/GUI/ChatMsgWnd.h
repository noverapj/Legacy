#pragma once

#include "../io3DEngine/ioMovingWnd.h"

class ioXMLElement;
class ioUIRenderImage;
class UserInfoNode;

///////////////////////////////////////////////////////////
class CustomSoundBtn : public ioButton
{
protected:
	ioHashString m_szPubicID;
	bool         m_bEnableImg;
	
	ioUIRenderImage *m_pSound;
	ioUIRenderImage *m_pSoundDisable;

public:
	void SetInfo( const ioHashString &rszPublicID , bool bEnableImg );
	inline const ioHashString &GetID() const { return m_szPubicID; }
	void SetEnableImg( bool bEnableImg ) { m_bEnableImg = bEnableImg; }
	bool IsEnableImg() const { return m_bEnableImg; }

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetWindowAlpha( int iAlpha );

protected:
	virtual void OnRender();

public:
	CustomSoundBtn();
	virtual ~CustomSoundBtn();
};
///////////////////////////////////////////////////////////
class ChatMsgWnd : public ioWnd
{	
public:
	enum
	{
		ID_VERT_SCROLL = 1000,

		ID_CHAT_OVER_DUMMY_1 = 1,    /* 12°³ ¿¹¾à*/
	};

	enum
	{
		MAX_CHAT_OVER_DUMMY = 11,
	};

protected:
	ioUIRenderImage *m_pNormalOverBack;
	ioUIRenderImage *m_pCustomOverBack;
	ioUIRenderImage *m_pCustomOverLine;

protected:
	static bool sg_bChatRefresh;

protected:
	int	  m_iCurPage;	
	int   m_iMaxChatMsgCnt;
	DWORD m_dwMouseOverIndex;

	int m_iStartXPos;
	int m_iStartYPos;
	int m_iLineYOffset;
	int m_iLineHeight;

protected:
	typedef std::vector< IMsgItem* > ChatMsgList;
	ChatMsgList m_ChatList;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderOverBack( int iXPos, int iYPos, float fFullWidth, BYTE bAlphaRate, bool bFirstLine, bool bMultiLine );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void iwm_lbuttondown( const ioMouse& mouse );

protected:
	bool IsMouseOverFailed();
	void OnProcessMouseOver();
	void OnProcessScroll( bool bRefresh );
	void UpdateSetPage( int iCurPage );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	DWORD GetMouseOverIndex(){ return m_dwMouseOverIndex; }

	void SetMsgSortFeature( DWORD dwFeature );

public:
	void InitScrollPos();

public:
	static void ChatRefresh(){ sg_bChatRefresh = true; }

public:
	ChatMsgWnd();
	virtual ~ChatMsgWnd();
};

///////////////////////////////////////////////////////////
class KOChatMsgWnd : public ioMovingWnd
{	
public:
	enum
	{
		ID_VERT_SCROLL = 1000,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pNoneKillerIcon;
	ioUIRenderImage *m_pNoneSkillIcon;

protected:
	ioHashString m_szMsgSnd;

protected:
	int	m_iCurPage;	
	int m_iMaxChatViewCnt;
	DWORD m_dwAlphaTime;
	DWORD m_dwAlphaStartTime;
	bool  m_bScrollAlphaInit;

	int m_iStartXPos;
	int m_iStartYPos;
	int m_iLineYOffset;
	int m_iLineHeight;

protected:
	int m_iDefaultX;
	int m_iDefaultY;

protected:
	enum
	{
		KT_KILL = 0,
		KT_PRISONER,
		KT_ESCAPE,
		KT_INFECTION,
	};

	struct KOMsgItem
	{
		int m_iMsgType;
		TeamType     m_eKillerTeam;
		ioHashString m_szKillerName;
		ioHashString m_szKillerSkill;
		DWORD        m_dwKillerWeaponCode;
		TeamType     m_eDierTeam;
		ioHashString m_szDierName;

		//
		DWORD m_dwCurrentTime;
		ioHashString m_szResultText;
		ioUIRenderImage *m_pSkillIcon;		
		
		KOMsgItem()
		{
			m_pSkillIcon = NULL;
			m_iMsgType = KT_KILL;
			m_dwKillerWeaponCode = 0;
			m_eKillerTeam = m_eDierTeam = TEAM_NONE;
			m_dwCurrentTime = 0;
		}
	};
	typedef std::vector< KOMsgItem > KOMsgItemList;
	KOMsgItemList m_ItemList;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	void SetKOMsg( TeamType eKillerTeam, const ioHashString &szKillerName, const ioHashString &szKillerSkillName, DWORD dwKillerItem,
				   TeamType eDierTeam, const ioHashString &szDierName );

	void SetPrisonerMsg( TeamType eKillerTeam, const ioHashString &szKillerName, const ioHashString &szKillerSkillName, DWORD dwKillerItem,
						 TeamType eDierTeam, const ioHashString &szDierName );

	void SetEscapeMsg( TeamType eKillerTeam, const ioHashString &szKillerName, const ioHashString &szKillerSkillName, DWORD dwKillerItem,
					   TeamType eDierTeam, const ioHashString &szDierName );

	void SetInfectionMsg( TeamType eKillerTeam, const ioHashString &szKillerName, const ioHashString &szKillerSkillName, DWORD dwKillerItem,
						  TeamType eDierTeam, const ioHashString &szDierName );

public:
	KOChatMsgWnd();
	virtual ~KOChatMsgWnd();
};

