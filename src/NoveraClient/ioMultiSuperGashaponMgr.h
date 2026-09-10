#pragma once

#include "DataHeaders/LSC_New_Gashapon_info.h"

enum MultiSuperGashponUseType
{
	MULTISUPERGASHPON_USE = 0,
	MULTISUPERGASHPON_BUY = 1,
};

typedef std::vector<bool> BoolVec;

typedef struct tagNewSuperGashaponElement
{
	//DWORD	 PackageIndex;
	DWORD    ElementIndex;
	WORD     CategoryType;
	DWORD    Category_Package_Element_Type;
	DWORD    Category_Package_Element_Value1;
	DWORD    Category_Package_Element_Value2;
	DWORD    Period;
	BYTE     Package_Alarm;
	DWORD	 Category_Package_Ment;

	tagNewSuperGashaponElement()
	{
		Init();
	}

	void Init()
	{
		//PackageIndex = 0;
		ElementIndex = 0;
		CategoryType = 0;
		Category_Package_Element_Type = 0;
		Category_Package_Element_Value1 = 0;
		Category_Package_Element_Value2 = 0;
		Period = 0;
		Package_Alarm = 0;
		Category_Package_Ment = 0;
	}

} NewSuperGashaponElement;

typedef std::vector<NewSuperGashaponElement> NewSuperGashaponElementList;
typedef std::map<DWORD, NewSuperGashaponElementList> NewSuperGashaponPackageList;
typedef std::map<DWORD, NewSuperGashaponPackageList> NewSuperGashaponMap;


struct NewSuperGashaponPackageInfo
{
	NewSuperGashaponMap m_vNewSuperGashaponMap;		// Category map

	DWORD	m_dwRollingTotalTime;
};
typedef std::map<DWORD, NewSuperGashaponPackageInfo> NewSuperGashaponPackageInfoMap;	// Etc Item map



class ioMultiSuperGashaponMgr  : public Singleton< ioMultiSuperGashaponMgr >
{
protected:
	char m_szKey[MAX_PATH];
	char m_szBuffer[MAX_PATH];

protected:
	ioHashString m_szResourcePath;
	NewSuperGashaponPackageInfoMap m_MultiSuperGashaponMap;

public:
	static ioMultiSuperGashaponMgr& GetSingleton();

public:
	virtual void Initialize();

protected:
	//void LoadPackage( ioINILoader& rkLoader, SuperGashaponPackageInfo& rkInfo );
	//void LoadElement( ioINILoader& rkLoader, SuperGashaponPackage& rkPackage, int iPackage, bool bSubPackage = false );

	//void LoadRollingIcon( SuperGashaponPackageInfo& rkInfo );
	//void LoadRollingIconVec( const vSuperGashaponPackage& vPackage, ioHashStringVec& vRollIconVec, ioHashStringVec& vSubIconVec );

	//void LoadCostume( SuperGashaponPackageInfo& rkInfo );
	//void LoadCostumeVec( const vSuperGashaponPackage& vPackage, BoolVec& vCostumeVec );

	//void LoadGrade( SuperGashaponPackageInfo& rkInfo );
	//void LoadGradeVec( const vSuperGashaponPackage& vPackage, IntVec& vGradeVec );

	//void LoadRollingTime( ioINILoader& rkLoader, SuperGashaponPackageInfo& rkInfo );

	bool HasIcon( const ioHashStringVec& szIconVec, const ioHashString& szIconName );

public:
	bool SendAutoUse( DWORD dwEtcItemType, byte eType );
	bool IsMultiSuperGashpon( DWORD dwEtcItemType );

	const NewSuperGashaponElementList& GetSuperGashaponPackage( DWORD dwEtcItemType, DWORD dwCategory, DWORD dwPackage );

	//void GetSuperGashaponPackageElement( DWORD dwEtcItemType, int iPackage, ioMultiSuperGashaponMgr::vSuperGashaponElement& vElement );	

	bool PrintSuperGashaponChatAlaram( const ioHashString& szSendID, DWORD dwEtcItemType, DWORD dwCategory, DWORD dwPackage );

protected:
	void PrintDefaultAlaram( const ioHashString& szSendID, const NewSuperGashaponElementList& rkPackage, const ioHashString& szEtcItemName );

public:
	void GetRollIconVec( DWORD dwEtcItemType, ioHashStringVec& vRollIconVec );
	void GetCostumeVec( DWORD dwEtcItemType, BoolVec& vCostumeVec );
	void GetGradeVec( DWORD dwEtcItemType, IntVec& vGradeVec );
	DWORD GetRollingTime( DWORD dwEtcItemType );

public:
	ioMultiSuperGashaponMgr(void);
	virtual ~ioMultiSuperGashaponMgr(void);
};

#define g_MultiSuperGashaponMgr ioMultiSuperGashaponMgr::GetSingleton()