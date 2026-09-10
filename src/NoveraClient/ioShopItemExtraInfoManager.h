#pragma once

class ioShopItemExtraInfoManager : public Singleton< ioShopItemExtraInfoManager >
{
public:
	typedef struct tagShopItemMovieInfo
	{
		ioHashString m_szMovieDir;
		ioHashString m_szMovieFile;
		ioHashString m_szMovieUrl;
		bool m_bAfterProcess;
	}ShopItemMovieInfo;

	typedef std::vector<ShopItemMovieInfo> ShopItemMovieInfoList;

	typedef struct tagShopItemExtraInfo
	{
		int m_nItemType;
		DWORD m_dwItemCode;
		bool m_bSetPreview;
		bool m_bSetChangeSex;
		bool m_bSetMovie;
		bool m_bSetMovieDown;

		ShopItemMovieInfoList m_ShopItemMovieInfoList;

		IntVec m_MachineList;
		IntVec m_SkillList;
	}ShopItemExtraInfo;

	typedef std::vector<ShopItemExtraInfo> ShopItemExtraInfoData;

protected:
	ShopItemExtraInfoData m_ShopItemExtraInfoData;

public:
	static ioShopItemExtraInfoManager& GetSingleton();

public:
	//데이터 비우기
	void ClearData();
	//데이터 읽기
	void LoadProperty();
	void LoadFile( const char* szFilePath );
	void LoadMachineList( ioINILoader &rkLoader, IntVec& vList );
	void LoadSkillList( ioINILoader &rkLoader, IntVec& vList );
	void LoadMovieInfoList( ioINILoader &rkLoader, ShopItemMovieInfoList& vList );

public:
	ShopItemExtraInfo* FindExtraInfo( int nItemType, int nItemValue, DWORD dwItemCode );

protected:
	bool IsApplyMachine( int nItemValue, const IntVec& vList );

public:
	ioShopItemExtraInfoManager();
	virtual ~ioShopItemExtraInfoManager();
};

#define g_ShopItemExtraInfoMgr ioShopItemExtraInfoManager::GetSingleton()