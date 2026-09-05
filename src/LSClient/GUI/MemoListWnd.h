#pragma once

class ioUIRenderImage;
class ioUIRenderFrame;
class MemoInfoListWnd : public ioWnd
{
public:
	enum
	{
		ID_NICK_NAME	= 1,
		ID_FRIEND_INSERT= 2,
		ID_ANSWER       = 3,
		ID_DELETE       = 4,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderFrame *m_pGreenstate;
	ioUIRenderFrame *m_pRedState;

	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pRecvIcon;
	ioUIRenderImage *m_pSendIcon;

protected:
	ioHashString m_szUserID;
	ioHashString m_szMemo;
	bool         m_bOnline;
	bool         m_bViewMemo;
	bool         m_bFromMemo;
	DWORD        m_dwOnlineTimer;
	DWORD        m_dwMemoInfoArray;

	typedef std::vector< ioHashString > vMemoString;
	vMemoString m_MemoList;

	void SplitMemo();
	void OnApplicationFriend();
	void SetNickNameBtnTitle( const ioHashString &szName, bool bFrom );

public:
	void SetMemoInfo( const ioHashString &szUserID, const ioHashString &szMemo, bool bOnline, bool bViewMemo, bool bFromMemo ,DWORD dwMemoInfoArray);

	const ioHashString &GetUserID(){ return m_szUserID; }
	const ioHashString &GetMemo(){ return m_szMemo; }
	bool  IsViewMemo(){ return m_bViewMemo; }
	void  SetOnline( bool bOnline ){ m_bOnline = bOnline; }

	void  ClearInfo();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	MemoInfoListWnd();
	virtual ~MemoInfoListWnd();
};

class MemoListWnd : public ioWnd
{
public:
	enum
	{
		//BTN
		ID_MEMO_SEND  = 1,
		ID_LEFT_PAGE  = 2,
		ID_RIGHT_PAGE = 3,

		ID_EDIT_ID    = 10,
		ID_EDIT_MEMO  = 11,
		
		//MEMO LIST
		ID_MEMO_LIST_1 = 100,
		ID_MEMO_LIST_2 = 101,
	};

	enum
	{
		MAX_LIST_SIZE = 2,
	};
	
protected:
	int m_iCurPage;
	int m_iMaxPage;
	ioWnd *m_pPreEdit;

	void UpdateSetPage( int iNewPage );
	void SendMemo();

	void MemoIDClearString( ioWnd *pEdit );
	void MemoClearString( ioWnd *pEdit );

public:
	void InviteResult( int iResult, ioHashString szInvitedName );
	void ChannelCreate( int iResult, ioHashString szManToManName );
	void SetUserID( const char *szID );
	bool IsChatBlock();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	MemoListWnd();
	virtual ~MemoListWnd();
};

