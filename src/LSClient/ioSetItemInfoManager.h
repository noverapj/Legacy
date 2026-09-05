#pragma once

class ioSetItemInfo;

class ioSetItemInfoManager : public Singleton< ioSetItemInfoManager >
{
protected:
	typedef std::vector< ioSetItemInfo* > SetItemInfoList;
	SetItemInfoList m_SetInfoList;

public:
	void LoadInfoList();	// ioItemMaker 생성후 호출

protected:	
	void DestroyAllInfo();
	bool AddSetItemInfo( ioSetItemInfo *pInfo );

	void ParsingSetItemCode();

public:
	const ioSetItemInfo* GetSetInfoByIdx( int iIdx ) const;
	const ioSetItemInfo* GetSetInfoByName( const ioHashString &szName ) const;
	const ioSetItemInfo* GetSetInfoByCode( DWORD dwSetCode ) const;
	const ioSetItemInfo* GetSetInfoByItemCode( DWORD dwItemCode ) const;
	const ioSetItemInfo* GetSetInfoByClass( int iClassType ) const;
	int GetTotalSetCount() const;
	int GetSetIdxByCode( DWORD dwSetCode ) const;

public:
	void GetSetItemSkillName( DWORD dwSetCode, int iLevel, ioHashString &szName ) const;
	int  GetSetItemShopOrder( DWORD dwSetCode ) const;
	void SetSetItemShopOrder( DWORD dwSetCode, int iShopOrder );
	int  GetSetItemShopMarkType( DWORD dwSetCode ) const;
	void SetSetItemShopMarkType( DWORD dwSetCode, int iShopMarkType );

	int  GetSetItemRecommandedCnt();

public:
	static ioSetItemInfoManager& GetSingeton();

public:
	ioSetItemInfoManager();
	virtual ~ioSetItemInfoManager();
};

#define g_SetItemInfoMgr ioSetItemInfoManager::GetSingeton()

