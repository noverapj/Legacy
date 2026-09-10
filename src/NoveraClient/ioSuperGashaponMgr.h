#pragma once

enum SuperGashponUseType
{
	SUPERGASHPON_USE = 0,
	SUPERGASHPON_BUY = 1,
};

typedef std::vector<bool> BoolVec;

class ioSuperGashaponMgr : public Singleton< ioSuperGashaponMgr >
{
public:
	struct SuperGashaponElement
	{		
		short m_iPresentType;
		int   m_iPresentValue1;
		int   m_iPresentValue2;

		SuperGashaponElement()
		{
			m_iPresentType		= 0;
			m_iPresentValue1	= 0;
			m_iPresentValue2	= 0;
		}
	};
	typedef std::vector<SuperGashaponElement> vSuperGashaponElement;

	struct SuperGashaponPackage
	{
		bool m_bWholeAlarm;
		vSuperGashaponElement m_vSuperGashaponElement;

		SuperGashaponPackage()  
		{
			m_bWholeAlarm = false;
		};
	};
	typedef std::vector<SuperGashaponPackage> vSuperGashaponPackage;

	struct SuperGashaponPackageInfo
	{
		vSuperGashaponPackage m_vSuperGashaponPackage;
		vSuperGashaponPackage m_vSuperGashaponSubPackage;

		ioHashStringVec m_vRollIconName;
		ioHashStringVec m_vSubIconName;
		BoolVec	m_vCostume;
		IntVec	m_vGrade;		 
		DWORD	m_dwRollingTotalTime;
	};
	typedef std::map<DWORD, SuperGashaponPackageInfo> SuperGashaponPackageInfoMap;

protected:
	char m_szKey[MAX_PATH];
	char m_szBuffer[MAX_PATH];

protected:
	ioHashString m_szResourcePath;
	SuperGashaponPackageInfoMap m_SuperGashaponPackageInfoMap;	

public:
	static ioSuperGashaponMgr& GetSingleton();

public:
	virtual void Initialize();

protected:
	void LoadPackage( ioINILoader& rkLoader, SuperGashaponPackageInfo& rkInfo );
	void LoadElement( ioINILoader& rkLoader, SuperGashaponPackage& rkPackage, int iPackage, bool bSubPackage = false );

	void LoadRollingIcon( SuperGashaponPackageInfo& rkInfo );
	void LoadRollingIconVec( const vSuperGashaponPackage& vPackage, ioHashStringVec& vRollIconVec, ioHashStringVec& vSubIconVec );

	void LoadCostume( SuperGashaponPackageInfo& rkInfo );
	void LoadCostumeVec( const vSuperGashaponPackage& vPackage, BoolVec& vCostumeVec );

	void LoadGrade( SuperGashaponPackageInfo& rkInfo );
	void LoadGradeVec( const vSuperGashaponPackage& vPackage, IntVec& vGradeVec );

	void LoadRollingTime( ioINILoader& rkLoader, SuperGashaponPackageInfo& rkInfo );

	bool HasIcon( const ioHashStringVec& szIconVec, const ioHashString& szIconName );

public:
	bool SendAutoUse( DWORD dwEtcItemType, byte eType );
	bool IsSuperGashpon( DWORD dwEtcItemType );

	const ioSuperGashaponMgr::SuperGashaponPackage& GetSuperGashaponPackage( DWORD dwEtcItemType, int iPackage );
	const ioSuperGashaponMgr::SuperGashaponPackage& GetSuperGashaponSubPackage( DWORD dwEtcItemType, int iPackage );

	void GetSuperGashaponPackageElement( DWORD dwEtcItemType, int iPackage, ioSuperGashaponMgr::vSuperGashaponElement& vElement );	

	bool PrintSuperGashaponChatAlaram( const ioHashString& szSendID, DWORD dwEtcItemType, int iPackage );

protected:
	void PrintDefaultAlaram( const ioHashString& szSendID, const SuperGashaponPackage& rkPackage, const ioHashString& szEtcItemName );

public:
	const ioHashStringVec& GetRollIconVec( DWORD dwEtcItemType );
	const ioHashStringVec& GetSubIconVec( DWORD dwEtcItemType );
	const BoolVec& GetCostumeVec( DWORD dwEtcItemType );
	const IntVec& GetGradeVec( DWORD dwEtcItemType );
	DWORD GetRollingTime( DWORD dwEtcItemType );

public:
	ioSuperGashaponMgr();
	virtual ~ioSuperGashaponMgr();
};

#define g_SuperGashaponMgr ioSuperGashaponMgr::GetSingleton()