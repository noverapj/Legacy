#pragma once

class SP2Packet;

class HackProblemWnd : public ioWnd
{
protected:
	enum QuizType
	{
		QT_SPEED,
		QT_ABUSE,
		QT_MAX,
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
	QuizType m_QuizType;
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

	ioEdit *m_pPreEdit;

public:
	void SetHackAnnounce( SP2Packet &rkPacket );
	bool SetHackQuiz( SP2Packet &rkPacket );

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
	HackProblemWnd();
	virtual ~HackProblemWnd();
};
