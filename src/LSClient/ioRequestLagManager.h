#pragma once

class ioINILoader;
class SP2Packet;
class ioPlayStage;
class ioBaseChar;

class ioRequestLagManager : public Singleton< ioRequestLagManager >
{
protected:
	//시간 설정
	DWORD m_dwCheckIncreaseTime;
	DWORD m_dwCheckIncreaseInputTime;
	DWORD m_dwCheckDecreaseTime;
	DWORD m_dwCheckAvgFPSTime;

	//비교값 설정
	int m_nCheckValue;
	int m_nIncreaseValue;
	int m_nIncreaseInputValue;
	int m_nDecreaseValue;

	float m_fAvgFPSRate;

	ioHashStringVec m_szBuffList;
	IntVec m_vDisableMode;

protected:	
	int m_nRequestValue;
	bool m_bStart;

	//시간 설정
	DWORD m_dwIncreaseTime;
	DWORD m_dwIncreaseInputTime;
	DWORD m_dwDecreaseTime;
	DWORD m_dwAvgFPSTime;

	float m_fTotalFPS;
	int m_nFPSCnt;
	float m_fAvgFPS;

	bool m_bProcess;

	bool m_bCheckAvgFPS;

	float m_fHalfFPS;
	DWORD m_dwHalfFPSTime;

	float m_fFPSIncreased;

	DWORD m_dwBuffAddTime;
	bool m_bIsCheckLag;

protected:
	void ClearData();
	void InitData();

public:
	void LoadProperty();

public:
	void OnProcess( ioPlayStage *pPlayStage );

protected:
	void RenderFPSInfo( float fCurFPS );

protected:
	bool IsEnableFunc( ioPlayStage *pPlayStage );
	void AvgFPS( float fCurFPS );

protected:
	void IncreaseValue( float fCurFPS );
	void DecreaseValue();

public:
	void IncreaseValueInput();

public:
	bool IsCheckValue();
	void RequestAction( ioPlayStage *pPlayStage );

public:
	void AddLagActionBuff( ioBaseChar *pChar );

public:
	bool IsProcess();
	void SetProcess( bool bProcess );

public:
	static ioRequestLagManager& GetSingleton();

public:
	ioRequestLagManager();
	virtual ~ioRequestLagManager();
};

#define g_RequestLagMgr ioRequestLagManager::GetSingleton()