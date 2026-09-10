#pragma once

class SP2Packet;

class AbuseProblemWnd : public ioWnd
{
protected:
	enum AbuseType
	{
		AT_KEY,
		AT_DAMAGE,
		AT_MACRO,
	};

	enum QuizOperator
	{
		QP_ADD,
		QP_MINUS,
		QP_MULTIPLY,
		QP_DIVIDE,
	};

	enum
	{
		ID_OK = 1,
		ID_ANSWER_EDIT = 2,
	};

protected:
	AbuseType m_AbuseType;
	CEncrypt<DWORD> m_dwQuizStartTime;
	CEncrypt<DWORD> m_dwQuizLimitTime;

	CEncrypt<int> m_iFirstOperand;
	CEncrypt<int> m_iSecondOperand;
	QuizOperator m_Operator;

	CEncrypt<int> m_iCurQuizChance;
	CEncrypt<int> m_iMaxQuizChance;

	CEncrypt<int> m_iPreQuizFirst;
	CEncrypt<int> m_iPreQuizSecond;
	int m_iSendedAnwser;

	bool m_bNowOpenSpace;
	ioEdit *m_pPreEdit;

	ModeType m_eModeType;

public:
	void SetAbuseAnnounce( SP2Packet &rkPacket, bool bOpenSpace, ModeType eModeType );
	bool SetAbuseQuiz( SP2Packet &rkPacket );

protected:
	void SetAnswerEditFocus();
	void SendQuizAnswer();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	AbuseProblemWnd();
	virtual ~AbuseProblemWnd();
};
//////////////////////////////////////////////////////////////////////////
// [1/12/2012 진호] - 로비에서 활성화되는 윈도우로 로비의 모든 기능이 비활성된다.
class LobbyLockProblemWnd : public ioWnd
{
public:
	enum
	{
		ID_OK = 1,
		ID_ANSWER_EDIT = 2,
	};

public:
	enum LockType
	{
		LT_NONE = 0,
		LT_MACRO,
		LT_HACK,
		LT_ABUSE,
		LT_SPEED,
	};

protected:
	CEncrypt<DWORD> m_dwLockType;

protected:
	CEncrypt<int> m_iFirstOperand;
	CEncrypt<int> m_iSecondOperand;
	
protected:
	ioEdit *m_pPreEdit;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void CheckAnswer();

public:
	void SetLockType( DWORD dwLockType );

public:
	LobbyLockProblemWnd();
	virtual ~LobbyLockProblemWnd();
};
//////////////////////////////////////////////////////////////////////////
// [1/13/2012 진호] - 전투 플레이중 매크로 신고 & 매크로 감시에 걸리면 전투 플레이 종료 후 전투 대기방에서 활성화
class BattleLockProblemWnd : public ioWnd
{
public:
	enum
	{
		ID_OK = 1,
		ID_ANSWER_EDIT = 2,
	};

public:
	enum LockType
	{
		LT_NONE = 0,
		LT_MACRO,
		LT_HACK,
		LT_ABUSE,
		LT_SPEED,
		LT_MACRO_EXTENSION,
	};

protected:
	CEncrypt<DWORD> m_dwLockType;

protected:
	CEncrypt<int> m_iFirstOperand;
	CEncrypt<int> m_iSecondOperand;

protected:
	CEncrypt<int> m_iCurQuizChance;
	CEncrypt<int> m_iMaxQuizChance;
	CEncrypt<DWORD> m_dwQuizStartTime;
	CEncrypt<DWORD> m_dwQuizLimitTime;

protected:
	ioEdit *m_pPreEdit;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void CheckAnswer();

public:
	void SetLockType( DWORD dwLockType );

public:
	BattleLockProblemWnd();
	virtual ~BattleLockProblemWnd();
};
