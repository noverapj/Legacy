
#include "stdafx.h"

#include "ioMapEventSceneMgr.h"

template<> ioMapEventSceneMgr* Singleton< ioMapEventSceneMgr >::ms_Singleton = 0;
ioMapEventSceneMgr::ioMapEventSceneMgr()
{
}

ioMapEventSceneMgr::~ioMapEventSceneMgr()
{
	m_EventSceneMap.clear();
}

ioMapEventSceneMgr& ioMapEventSceneMgr::GetSingleton()
{
	return Singleton< ioMapEventSceneMgr >::GetSingleton();
}

void ioMapEventSceneMgr::ClearSceneData()
{
	EventSceneMap::iterator iCreate;
	for(iCreate = m_EventSceneMap.begin(); iCreate != m_EventSceneMap.end();iCreate++)
	{
		EventSceneData &rkData = iCreate->second;
		rkData.m_vLookAt.clear();
		rkData.m_vOffset.clear();
	}
	m_EventSceneMap.clear();
}

void ioMapEventSceneMgr::LoadMapEventScene()
{
	ClearSceneData();

	ioINILoader_e kLoader( "config/sp2_map_event_scene.ini" );
	
	kLoader.SetTitle_e( "common" );
	int iMaxEventScene = kLoader.LoadInt_e( "max_event_scene", 0 );
	for(int i = 0;i < iMaxEventScene;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "event_scene%d", i + 1 );
		kLoader.SetTitle( szKey );

		EventSceneData kEventSceneData;
		kEventSceneData.m_dwEventDuration = kLoader.LoadInt_e( "camera_duration", 0 );
		int iMaxEventPos = kLoader.LoadInt_e( "event_pos_cnt", 0 );
		for(int j = 0;j < iMaxEventPos;j++)
		{
			D3DXVECTOR3 vLookAt, vOffset;
			// LookAt
			sprintf_e( szKey, "pos%d_x", j+1 );
			vLookAt.x = kLoader.LoadFloat( szKey, 0.0f );

			sprintf_e( szKey, "pos%d_y", j+1 );
			vLookAt.y = kLoader.LoadFloat( szKey, 0.0f );

			sprintf_e( szKey, "pos%d_z", j+1 );
			vLookAt.z = kLoader.LoadFloat( szKey, 0.0f );

			// Camera Offset
			sprintf_e( szKey, "offset%d_x", j+1 );
			vOffset.x = kLoader.LoadFloat( szKey, 0.0f );

			sprintf_e( szKey, "offset%d_y", j+1 );
			vOffset.y = kLoader.LoadFloat( szKey, 0.0f );

			sprintf_e( szKey, "offset%d_z", j+1 );
			vOffset.z = kLoader.LoadFloat( szKey, 0.0f );

			kEventSceneData.m_vLookAt.push_back( vLookAt );
			kEventSceneData.m_vOffset.push_back( vOffset );
		}
		m_EventSceneMap.insert( EventSceneMap::value_type( i + 1, kEventSceneData ) );
	}
}

bool ioMapEventSceneMgr::GetMapEventScene( DWORD dwIndex, Vector3Vec &rLookAt, Vector3Vec &rOffset, DWORD &rEventDuration )
{
	EventSceneMap::iterator iter = m_EventSceneMap.find( dwIndex );
	if( iter == m_EventSceneMap.end() )
		return false;

	rLookAt.clear();
	rOffset.clear();
	EventSceneData kEventSceneData = iter->second;
	if( !kEventSceneData.m_vLookAt.empty() && !kEventSceneData.m_vOffset.empty() )
	{
		rLookAt.resize( kEventSceneData.m_vLookAt.size() );
		std::copy( kEventSceneData.m_vLookAt.begin(), kEventSceneData.m_vLookAt.end(), rLookAt.begin() );

		rOffset.resize( kEventSceneData.m_vOffset.size() );
		std::copy( kEventSceneData.m_vOffset.begin(), kEventSceneData.m_vOffset.end(), rOffset.begin() );

		if( kEventSceneData.m_dwEventDuration != 0 )
			rEventDuration = kEventSceneData.m_dwEventDuration;

		return true;
	}
	return false;
}

DWORD ioMapEventSceneMgr::GetMapEventSceneTime( DWORD dwIndex )
{
	EventSceneMap::iterator iter = m_EventSceneMap.find( dwIndex );
	if( iter == m_EventSceneMap.end() )
		return 0;

	EventSceneData kEventSceneData = iter->second;
	return kEventSceneData.m_dwEventDuration;
}
