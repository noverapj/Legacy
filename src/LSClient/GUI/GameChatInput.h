#pragma once

#include "../ChatMessageFunc.h"

class ioXMLElement;
class ioPlayStage;
class ioEdit;
class ioUIRenderFrame;

class GameChatInput : public ioWnd
{
public:
	enum
	{
		ID_CHAT_EDIT	= 1,
		ID_CHAT_TYPE_BTN= 2,
	};

protected:
	static ChatType sg_InputLinkChat;
	static ioHashString sg_InputLinkString;

protected:
	ioUIRenderFrame *m_pFocusBoxFrm;

protected:
	ioHashString m_Name;
	TeamType     m_OwnerTeam;	

	ioPlayStage *m_pStage;
	ioEdit      *m_pPreEdit;
	
	// 로비 채팅 도배 방지
	int   m_iCurChatLimitCount;
	int   m_iChatLimitCount;
	DWORD m_dwCurChatLimitTime;
	DWORD m_dwChatLimitTime;

	// 외침 채팅 도배 방지
	int   m_iCurWholeChatLimitCount;
	int   m_iWholeChatLimitCount;
	DWORD m_dwCurWholeChatLimitTime;
	DWORD m_dwWholeChatLimitTime;

	bool  m_bShowEditFocus;  

	ChatType m_eChatType;

	ioHashString    m_szChatWarningText;
	ioHashString    m_szChatBlockText;

protected:
	enum ChatEnableState
	{
		CES_ENABLE,
		CES_WARNING,
		CES_WARNING_EDITING,
		CES_BLOCK,
		CES_BLOCK_EDITING
	};
	ChatEnableState m_ChatEnableState;

protected:
	void ShowChatInputBox();
	bool CheckChatLimit();
	bool CheckWholeChatLimit();
	void ProcessChangeChatBtn();
	bool IsBeforeStageChatTypeLink();

public:
	void SetFirstChatTypeToBtn( ChatType eChatType );
	void SetChatTypeToBtn( ChatType eChatType );
	void ShowChat( const ioHashString &rkName, TeamType eTeam, ioPlayStage *pStage );
	void ShowChatConv( const ioHashString &rkName, TeamType eTeam, ioPlayStage *pStage );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void CheckBtnChatType();
	void ChangeBtnChatType();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void OnChatInput( ioEdit *pEdit, DWORD param );
	bool IsWholeChatItem();
	bool IsWholeChatRainbowItem();
	bool IsParentWndCheck( ioWnd *pCheckWnd, ioWnd *pParent );	
	bool IsCheckStealthMode();

public:
	bool IsEnableEnter();

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnProcessFocus( ioEdit *pEdit );

public:
	GameChatInput();
	virtual ~GameChatInput();
};

