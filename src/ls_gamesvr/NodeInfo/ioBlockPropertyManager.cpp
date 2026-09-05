#include "stdafx.h"
#include "ioBlockPropertyManager.h"

template<> ioBlockPropertyManager* Singleton< ioBlockPropertyManager >::ms_Singleton = 0;

ioBlockPropertyManager::ioBlockPropertyManager()
{
	Init();
}

ioBlockPropertyManager::~ioBlockPropertyManager()
{
	Destroy();
}

void ioBlockPropertyManager::Init()
{
	m_mBlockPropertyGroup.clear();
}

void ioBlockPropertyManager::Destroy()
{
	m_mBlockPropertyGroup.clear();
}

ioBlockPropertyManager& ioBlockPropertyManager::GetSingleton()
{
	return Singleton< ioBlockPropertyManager >::GetSingleton();
}

void ioBlockPropertyManager::LoadIni()
{
	ioINILoader kLoader;
	char szKey[MAX_PATH]="";

	kLoader.LoadFile( "config/sp2_block.ini" );
	kLoader.SetTitle( "common" );

	int iMaxTable		= kLoader.LoadInt( "block_max", 0);
	
	for( int i = 0; i < iMaxTable; i++ )
	{
		BLOCKCOORDINATEINFO vCoordinateInfo;

		sprintf_s( szKey, "block%d", i + 1 );
		kLoader.SetTitle(szKey);

		DWORD dwCode	= kLoader.LoadInt("code", 0);
		int iDirection	= kLoader.LoadInt("rotate", 0);

		int iMaxCoordinate	= kLoader.LoadInt("collision_tile_max", 0);

		for( int j = 0; j < iMaxCoordinate; j++ )
		{
			CoordinateInfo stCoordinate;
			sprintf_s( szKey, "collision_tile%d_x", j + 1 );
			stCoordinate.iX	= kLoader.LoadInt(szKey, 0);

			sprintf_s( szKey, "collision_tile%d_y", j + 1 );
			stCoordinate.iY	= kLoader.LoadInt(szKey, 0);

			sprintf_s( szKey, "collision_tile%d_z", j + 1 );
			stCoordinate.iZ	= kLoader.LoadInt(szKey, 0);

			vCoordinateInfo.push_back(stCoordinate);
		}

		m_mBlockPropertyGroup.insert( std::make_pair(dwCode, vCoordinateInfo) );
	}
}

void ioBlockPropertyManager::GetCoordinateInfo(const DWORD dwItemCode, BLOCKCOORDINATEINFO& vInfo)
{
	vInfo.clear();

	BLOCKPROPERTYGROUP::iterator it = m_mBlockPropertyGroup.find(dwItemCode);

	if( it == m_mBlockPropertyGroup.end() )
		return;

	vInfo = it->second;
}

BOOL ioBlockPropertyManager::IsValidItemCode(const DWORD dwItemCode)
{
	BLOCKPROPERTYGROUP::iterator it = m_mBlockPropertyGroup.find(dwItemCode);

	if( it == m_mBlockPropertyGroup.end() )
		return FALSE;

	return TRUE;
}