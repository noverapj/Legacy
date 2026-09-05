

#ifndef _ioSetItemInfoManager_h_
#define _ioSetItemInfoManager_h_

#include "../Util/Singleton.h"

class ioSetItemInfo;

class ioSetItemInfoManager : public Singleton< ioSetItemInfoManager >
{
	typedef std::vector< ioSetItemInfo* > SetItemInfoList;
	SetItemInfoList m_SetInfoList;

//	ioINILoader m_SetInfoINI;

public:
//	void LoadInfoList( const char *szFileName );		// ioItemInfoManager 생성후 호출
	void LoadINI();

protected:	
	void DestroyAllInfo();
	void Init();
	void LoadSetItem( ioINILoader &rkLoader );
	bool AddSetItemInfo( ioSetItemInfo *pInfo );

	void ParsingSetItemCode();

public:
	int GetTotalSetCount() const;
	int GetMaxItemInfo() const;

	const ioSetItemInfo* GetSetInfoByIdx( int iIdx ) const;
	const ioSetItemInfo* GetSetInfoByName( const ioHashString &szName ) const;
	const ioSetItemInfo* GetSetInfoByCode( DWORD dwSetCode ) const;
	
public:
	const DWORDVec& GetSetItemListByIdx( int iIdx ) const;
	const DWORDVec& GetSetItemListByName( const ioHashString &szName ) const;
	const DWORDVec& GetSetItemListByCode( DWORD dwSetCode ) const;

//	ioINILoader& GetSetItemInfoINI();

public:
	static ioSetItemInfoManager& GetSingeton();

public:
	ioSetItemInfoManager();
	virtual ~ioSetItemInfoManager();

protected:
	bool m_bINILoading;
	int  m_iMaxInfo;
};

#define g_SetItemInfoMgr ioSetItemInfoManager::GetSingleton()

#endif
