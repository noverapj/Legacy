#pragma once
#include "../Util/Singleton.h"
#include "../Util/IORandom.h"
#include "../DataHeaders/LSC_New_Gashapon_info.h"


class User;

class cBoxInfo
{
public:
	cBoxInfo()
	{
		dwCategoryIndex = 0;
		dwPackageIndex	= 0;
		bPackageAlarm	= false;
	}

public:
	DWORD	dwCategoryIndex;
	DWORD	dwPackageIndex;
	bool	bPackageAlarm; 

};

class ioRandomBoxManager : public Singleton< ioRandomBoxManager >
{
public:

public:
	struct sPackageBoxElement
	{
		short m_iPresentType;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		short m_iBoxMent;
		int	  m_iBoxPeriod;

		DWORD m_dwIndex;
		
		sPackageBoxElement()
		{
			m_iPresentType		= 0;
			m_iPresentValue1	= 0;
			m_iPresentValue2	= 0;
			m_iBoxMent			= 0;
			m_iBoxPeriod		= 0;
		}
	};
	typedef std::vector< sPackageBoxElement* > vPackageBoxElement;

protected:

	// 슈퍼가챠폰 Package
	struct RandomBoxPackage
	{
		vPackageBoxElement m_vPackageBoxElement;
		DWORD m_dwRand;
		DWORD m_dwIndex;
		bool m_bWholeAlarm;					// 전체 알림 여부

		RandomBoxPackage()
		{
			m_vPackageBoxElement.clear();

			m_dwRand				= 0;
			m_dwIndex				= 0;
			m_bWholeAlarm			= false;
		}
	};

	typedef std::vector< RandomBoxPackage* > vRandomBoxPackage;

	struct RandomBoxCategoryInfo
	{
		vRandomBoxPackage		m_vRandomBoxPackageList;	 //기본 패키지

		DWORD					m_dwRandomBoxPackageSeed;
		IORandom				m_RandomBoxRandom;
		DWORD					m_dwCategoryRand;

		DWORD					m_dwCategoryType;
		DWORD					m_dwCategoryIndex;

		RandomBoxCategoryInfo()
		{
			m_vRandomBoxPackageList.clear();

			m_dwRandomBoxPackageSeed		= 0;
			m_dwCategoryRand				= 0;

			m_dwCategoryType				= 0;
			m_dwCategoryIndex				= 0;
		}
	};

	typedef std::vector< RandomBoxCategoryInfo* > vRandomBoxCategoryInfo;
	vRandomBoxCategoryInfo m_vRandomBoxCategoryInfoList;

	struct RandomBoxInfo
	{
		vRandomBoxCategoryInfo	m_vRandomBoxCategoryInfoList;	 //기본 패키지

		IORandom				m_RandomBoxRandom;

		DWORD					m_dwEtcItemCode;
		
		DWORD					m_dwRandomBoxSeed;

		RandomBoxInfo()
		{
			m_vRandomBoxCategoryInfoList.clear();

			m_dwEtcItemCode					= 0;
			m_dwRandomBoxSeed				= 0;
		}
	};

	typedef std::map<DWORD, RandomBoxInfo* > mRandomBoxInfo;
	mRandomBoxInfo m_mRandomBoxInfoList;

protected:
	LSC_New_Gashapon_info_Manager* m_pRandomboxManager;

public:
	void LoadINI();
	void CheckNeedReload();	

protected:
	void LoadRandomBoxPackage();
	void LoadCategory( LSC_New_Gashapon_info* pkInfo, RandomBoxInfo* pkBoxInfo );

	RandomBoxInfo* GetRandomBoxInfo( DWORD dwEtcItemCode );

public:
	bool SendRandomBoxRandPackage( User *pSendUser, DWORD dwEtcItemCode, vector<cBoxInfo> &vPackageBox );

protected:
	void SendRandomBoxSelectPackage( User *pSendUser, DWORD dwEtcItemCode, const RandomBoxPackage* pkPackage, const ioHashString& szGashaponSendID );

public:
	static ioRandomBoxManager& GetSingleton();

public:
	ioRandomBoxManager();
	~ioRandomBoxManager();
};


#define g_RandomBoxManager ioRandomBoxManager::GetSingleton()
