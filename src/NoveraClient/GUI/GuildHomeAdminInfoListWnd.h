#pragma once

class ioGuildData;
class GuildHomeAdminInfoListWnd : public ioWnd
{
public:
	enum
	{
		ID_USER_LIST_1		= 0,              //32개의 유저 버튼이 있다.
		ID_USER_LIST_32		= 31,

		ID_OPTION_WND		= 1000,
	};

protected:
	ioGuildData* m_pGuildData;

public:
	void UpdateUserList( ioGuildData* pGuildData );

public:
	void Update();

protected:
	virtual void iwm_show();
	virtual void OnProcess( float fTimePerSec );

public:
	GuildHomeAdminInfoListWnd();
	virtual ~GuildHomeAdminInfoListWnd();
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------

class GuildHomeAdminListBtn : public ioButton
{
public:
	enum
	{
		ID_CHECK_BOX = 1,
	};

protected:
	ioUIRenderImage*	m_pMasterMark;
	ioUIRenderImage*	m_pSecondMasterMark;

protected:	
	ioHashString		m_UserID;
	ioHashString		m_UserPos;
	int					m_nUserLevel;
	int					m_dwUserIdx;

public:
	void SetUserInfo( ioGuildData *pGuildData, const ioHashString& UserID, DWORD dwUserIdx, int nUserLevel, const ioHashString& szUserPos );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetInActive();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();	

public:
	GuildHomeAdminListBtn();
	virtual ~GuildHomeAdminListBtn();
};