#pragma once

#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioMovingWnd.h"

class ioPlayStage;
class ioUIRenderImage;
class KickVoteReasonEdit : public ioEdit
{
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void iwm_create();

public:
	KickVoteReasonEdit();
	virtual ~KickVoteReasonEdit();
};
///////////////////////////////////////////////////////////////////////////
class KickVoteReasonWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_VOTE_START = 2,
		ID_CANCEL = 3,

		ID_REASON_BTN1 = 101,
		ID_REASON_BTN2 = 102,
		ID_REASON_BTN3 = 103,
		ID_REASON_BTN4 = 104,
	};
protected:
	ioPlayStage *m_pPlayStage;
	ioHashString m_szKickName;
	ioHashString m_szKickReason;
	TeamType	 m_eKickUserTeam;

	DWORD m_dwCurReasonBtn;
	
protected:
	void OnVoteStart();
	void CheckButton( DWORD dwID );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetKickUserInfo( const ioHashString &rkName );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	//virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	KickVoteReasonWnd();
	virtual ~KickVoteReasonWnd();
};

/////////////////////////////////////////////////////////////////////////////

class KickVoteProgressWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_APPROVE	  = 1,
		ID_OPPOSITION = 2,
	};

protected:
	ioPlayStage *m_pPlayStage;

protected:
	TeamType     m_eProposalTeam;
	ioHashString m_szProposalName;
	TeamType     m_eKickUserTeam;
	ioHashString m_szKickUserName;
	ioHashString m_szKickUserReason;

	char         m_szMainMent[2048];
	ioHashStringVec m_MainMentList;

	int          m_iVoteType;
	DWORD        m_dwVoteTime;
	DWORD        m_dwVoteCurTime;

protected:
	void OnVote( int iVoteType );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetKickVoteProgress( const ioHashString &rkProposalName, const ioHashString &rkKickUserName, const ioHashString &rkKickUserReason, DWORD dwProcessTime );

public:
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	KickVoteProgressWnd();
	virtual ~KickVoteProgressWnd();
};
//////////////////////////////////////////////////////////////////////////
void KickVoteResultHelp( ioPlayStage *pPlayStage, int iVoteType, const ioHashString &rkTargetName, int iValue1, int iValue2, int iValue3 );
