#pragma once

#include "../Util/Singleton.h"
#include "../Util/IORandom.h"

class Costume;

class CostumeManager : public Singleton< CostumeManager >
{
public:
	struct CostumeDisassemble
	{
		int iItemType;
		int iGainCode;
		int iRandom;
		int iGainCount;

		CostumeDisassemble()
		{
			iItemType	= 0;
			iGainCode	= 0;
			iGainCount	= 0;
			iRandom		= 0;
		}
	};

	struct CostumeSellInfo
	{
		float fPrice;
		float fRate;
		int iMaxPrice;

		CostumeSellInfo()
		{
			fPrice		= 0.0;
			fRate		= 0.0;
			iMaxPrice	= 0;
		}
	};

public:
	CostumeManager();
	virtual ~CostumeManager();

	void Init();
	void Destroy();

	static CostumeManager& GetSingleton();

public:
	void LoadINI();

protected:
	void LoadAllCostume( ioINILoader &rkLoader );

public:
	bool IsExistCostumeItem(DWORD dwItemCode);
	__int64 SellCostume(Costume* pCostumeInfo);
	bool DisassembleCostume(Costume* pCostumeInfo, CostumeDisassemble& stItemInfo);

	void GetDisassembleItemInfo(CostumeDisassemble& stItemInfo);
	void CalcLimitDateValue( SYSTEMTIME &sysTime, int& iYMD, int& iHM);
	void ConvertCTimeToSystemTime( SYSTEMTIME &sysTime, CTime& cTime);
	
	int GetMortmainItemPrice() { return m_iMortmainItemPrice; }
	int GetTimeItemPrice() { return m_iTimeItemPrice; }
protected:
	typedef std::set<int> CostumInfo;
	typedef std::vector<CostumeDisassemble> DisassembleInfo;

protected:
	CostumInfo m_sAllCostumeInfo;
	DisassembleInfo m_vDisassembleInfo;

	CostumeSellInfo m_stMortmainSellInfo;
	CostumeSellInfo m_stTimeSellInfo;

	int m_iTotalDisassembleRandomValue;
	IORandom m_DisassembleRandom;

	int m_iMortmainItemPrice;
	int m_iTimeItemPrice;

	bool m_bINILoading;
};

#define g_CostumeMgr CostumeManager::GetSingleton()