#pragma once


class FSMState
{
public:
	enum { NONE_VALUE = -1};

protected:
	int    m_iState;
	IntVec m_vInput;
	IntVec m_vOutput;
	IntVec m_vDelay;

public:
	int GetOutput( int a_iInput  , DWORD a_dwCurTime ); 
	int GetState()const { return m_iState; }

	void Add( int a_iInput, int a_iOutput, int a_iDelay = NONE_VALUE );
	void Delete( int a_iInput );

	int GetDelay( int a_iInput );
public:
	FSMState(int a_iState );
	virtual ~FSMState();
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------

class FSMStateMgr
{
protected:
	enum 
	{
		NONE_CHECK_TIME = 0,
	};
protected:
	int   m_iCurState;
	DWORD m_dwStartTime;
	DWORD m_dwCurTime;
	int   m_iCurDelay;
	int   m_iCurArray;

	typedef std::vector< FSMState *> vState;
	vState m_vState;

protected:
	int GetDelay( int a_iInput );
	int GetArray();

public:
	inline int GetCurState() const { return m_iCurState; }
	void SetCurState(int iCurState);

	inline DWORD GetCurTime() const { return m_dwCurTime; }

	void Add( FSMState *a_pState );
	void Delete( int a_iState );

	int ChangeState( int a_iInput );
	inline int GetDelay() const { return m_iCurDelay; }

	void UpdateTime();

public:
	void ClearState();

public: 
	FSMStateMgr();
	virtual ~FSMStateMgr();
};



