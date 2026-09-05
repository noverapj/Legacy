#pragma once

class ioINILoader;
class ioBaseChar;
class SP2Packet;

class ioNoviceGradeBenefitManager : public Singleton< ioNoviceGradeBenefitManager >
{
protected:
	enum
	{
		NGBS_Add_Benefit_Buff,
		NGBS_Remove_Benefit_Buff,
	};

protected:
	int m_nMinGrade;
	int m_nMaxGrade;
	ioHashStringVec m_szBenefitBuffList;
	IntVec m_vDisableMode;

protected:
	void ClearData();
	void InitData();

public:
	void LoadProperty();

public:
	void OnProcess( ioBaseChar *pOwner );

protected:
	bool CheckEnableBenefit( ioBaseChar *pOwner );
	void AddBenefitBuff( ioBaseChar *pOwner );
	void RemoveBenefitBuff( ioBaseChar *pOwner );

public:
	bool IsBenefitGrade( int nGrade );

public:
	static ioNoviceGradeBenefitManager& GetSingleton();

public:
	ioNoviceGradeBenefitManager();
	virtual ~ioNoviceGradeBenefitManager();
};

#define g_NoviceGradeBenefitMgr ioNoviceGradeBenefitManager::GetSingleton()