#pragma once

#include "../DataHeaders/LSC_heroitem_info.h"
#include "../../include/cSingleton.h"

class ioSetItemInfo;

class ioHeroManager : public LSC_heroitem_info_Manager
{
private:
	typedef std::vector< ioSetItemInfo* > HeroInfoList;
	HeroInfoList	m_HeroInfoList;
	int				m_iMaxHeroIndex;

public:
	ioHeroManager(void);
	virtual ~ioHeroManager(void);

protected:
	void Destroy();
	void InitData();

public:
	BOOL Load();
	bool AddItemInfo( ioSetItemInfo *pInfo );

public:
	void SetHeroInfo();
	void SetHeroList();	

	int GetInfoListSize() { return m_HeroInfoList.size(); }
	int GetMaxHeroIndex() { return (m_iMaxHeroIndex - SET_ITEM_CODE); }
	int GetTimeCharResellPeso( int iClassType );
	int GetHeroGrade( int iClassType );
	int GetHeroPresetSex( int iClassType );

	BOOL CheckData();

public:
	const ioSetItemInfo* GetHeroInfo( DWORD dwSetCode ) const;

};

#define  g_ioHeroManager	(*cSingleton<ioHeroManager>::GetInstance())