#include "stdafx.h"

#include "ioBlockManager.h"
#include "ioBlock.h"
#include "ioTileBlock.h"
#include "ioFishingBlock.h"

#include "../ioPlayStage.h"
#include "../ioGameSceneManager.h"
#include "../io3DEngine/ioINILoader.h"

template<> ioBlockManager* Singleton< ioBlockManager >::ms_Singleton = 0;

ioBlockManager::ioBlockManager() : m_pStage( 0 ), m_dwPreUpdatedTime( 0 ), m_dwPlayStartTime( 0 ), m_nTileMax( 0 ), m_nHeightMax( 0 ), m_eType( BMT_GUILD )
{
	D3DXQuaternionRotationYawPitchRoll( &m_BlockRotation, DEGtoRAD( 45.0f ), 0.0f, 0.0f );
}

ioBlockManager::~ioBlockManager()
{
	Destroy();
}

void ioBlockManager::Destroy()
{	
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter )
	{
		SAFEDELETE( *iter );
	}
	m_BlockList.clear();

	for( BlockTempleteMap::iterator iter = m_BlockTempleteMap.begin(); iter != m_BlockTempleteMap.end(); ++iter )
	{
		SAFEDELETE( iter->second );
	}
	m_BlockTempleteMap.clear();
}

ioBlockManager& ioBlockManager::GetSingleton()
{
	return Singleton< ioBlockManager >::GetSingleton();
}

void ioBlockManager::SetBlockModeType( BlockModeType eType )
{	
	m_eType = eType;
}

void ioBlockManager::Initialize( ioPlayStage* pStage )
{
	m_pStage			= pStage;
	m_dwPreUpdatedTime	= 0;	

	char szBuf[MAX_PATH];

	ioINILoader_e kLoader( "config/housing/sp2_block.ini" );
	kLoader.SetTitle_e( "common" );

	int nMax = kLoader.LoadInt( "block_max", 0 );
	for( int i = 0; i < nMax; ++i )
	{
		sprintf_s_e( szBuf, "block%d", i + 1 );
		kLoader.SetTitle( szBuf );

		int nCode = kLoader.LoadInt( "code", 0 );
		if( nCode == 0 )
		{
			LOG.PrintTimeAndLog(0, "Block Manager Load Fail - [Block%d] - Code 0", i + 1 );
			continue;
		}

		ioBlockProperty* pProperty = CreateBlockTemplete( kLoader, nCode );
		if( pProperty )
		{
			pProperty->SetCode( nCode );
			m_BlockTempleteMap.insert( BlockTempleteMap::value_type( nCode, pProperty ) );
		}
		else
		{
			LOG.PrintTimeAndLog(0, "Block Manager Load Fail - [Block%d] - Code 0", i + 1 );
		}
	}

	kLoader.SetTitle_e( "unit" );
	m_fBlockStartHeight	= kLoader.LoadFloat( "block_start_height", 0.0f );
}

void ioBlockManager::SetTileMax( int nMax, int nHeight )
{
	m_nTileMax		= nMax; 
	m_nHeightMax	= nHeight;
}

ioBlockProperty* ioBlockManager::CreateBlockTemplete( ioINILoader& rkLoader, int nCode )
{
	ioBlockProperty* pProperty = NULL;
	switch( nCode / ioBlock::BT_DIVIDE )
	{
	case ioBlock::BT_DEFAULT:	
	case ioBlock::BT_TILE:
		pProperty = new ioBlockProperty();
		break;
	case ioBlock::BT_FISHING:
		pProperty = new ioFishBlockProperty();
		break;
	}

	if( pProperty )
	{
		pProperty->LoadBlockProperty( rkLoader );
		return pProperty;
	}
	else
	{
		LOG.PrintTimeAndLog(0, "Create Block Templete Failed : %d", nCode );
	}

	return NULL;
}

const ioBlockProperty* ioBlockManager::GetBlockTemplete( int nCode )
{
	for( BlockTempleteMap::iterator iter = m_BlockTempleteMap.begin(); iter != m_BlockTempleteMap.end(); ++iter )
	{
		const ioBlockProperty* pTemplete = iter->second;
		if( pTemplete && pTemplete->m_nCode == nCode )
			return pTemplete;
	}

	LOG.PrintTimeAndLog(0, "GetBlockTemplete Fail - %d", nCode );
	return NULL;
}

void ioBlockManager::DestroyBlock( __int64 nUniqueIndex )
{
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end();  )
	{
		ioBlock* pBlock = *iter;
		if( pBlock && pBlock->GetUniqueIndex() == nUniqueIndex )
		{
			pBlock->DestroyAreaWeapon();

			SAFEDELETE( pBlock );
			iter = m_BlockList.erase( iter );
			return;
		}
		else
		{
			++iter;
		}
	}
}

ioBlock* ioBlockManager::CreateBlock( int nCode, __int64 nUniqueIndex )
{
	BlockTempleteMap::iterator iter = m_BlockTempleteMap.find( nCode );
	if( iter != m_BlockTempleteMap.end() )
	{		
		ioBlockProperty* pProperty = iter->second;
		if( !pProperty )
		{
			LOG.PrintTimeAndLog(0, "CreateBlock Fail(%d) : pProperty == NULL", nCode );
			return NULL;
		}

		ioEntityGroup* pGrp = m_pStage->CreateEntityGrpAndSceneNode( pProperty->m_MeshFileName, true );
		if( !pGrp )
		{
			LOG.PrintTimeAndLog(0, "CreateBlock Fail(%d) : SceneNode == NULL", nCode );
			return NULL;
		}

		ioBlock* pBlock = NULL;
		switch( nCode / ioBlock::BT_DIVIDE )
		{
		case ioBlock::BT_DEFAULT:
			pBlock = new ioBlock( pGrp, m_pStage->GetPlayMode(), pProperty, nUniqueIndex );
			break;
		case ioBlock::BT_TILE:
			pBlock = new ioTileBlock( pGrp, m_pStage->GetPlayMode(), pProperty, nUniqueIndex );
			break;
		case ioBlock::BT_FISHING:
			pBlock = new ioFishingBlock( pGrp, m_pStage->GetPlayMode(), pProperty, nUniqueIndex );
			break;
		}
		
		if( pBlock )
		{
			pGrp->SetLightReceiveType( LRT_ALL_REAL_TIME );
			pGrp->SetEachEntityCull( true );
			pGrp->SetLightApplyMask( APPLY_SECOND_MAIN_LIGHT );
			
			switch( pProperty->GetShadowType() )
			{
			case ioBlockProperty::ST_CAST:
				{
					pBlock->SetShadowCastEnable( true ); 
				}
				break;
			case ioBlockProperty::ST_RECIVE:
				{					
					if( Setting::UseShadow() )
						pBlock->SetShadowRecvEnable( true );
					else
						pBlock->SetShadowRecvEnable( false );

					pBlock->AddShadowRecvPass();
				}
				break;
			}

			m_BlockList.push_back( pBlock );
			return pBlock;
		}
		else
		{
			LOG.PrintTimeAndLog(0, "CreateBlock Fail(%d) : new Block == NULL", nCode );
		}
	}

	return NULL;
}

ioBlock* ioBlockManager::GetBlock( __int64 nUniqueIndex )
{
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter )
	{
		ioBlock* pBlock = *iter;
		if( pBlock && pBlock->GetUniqueIndex() == nUniqueIndex )
		{
			return pBlock;
		}
	}

	return NULL;
}

int ioBlockManager::GetBlockCount( int nCode )
{
	int nCount = 0;
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter )
	{
		ioBlock* pBlock = *iter;
		if( pBlock && pBlock->GetProperty() && pBlock->GetProperty()->GetCode() == nCode )
		{
			nCount++;
		}
	}

	return nCount;
}

const BlockList& ioBlockManager::GetAllBlock()
{
	return m_BlockList;
}

int ioBlockManager::GetBlockTotalCount()
{
	return (int)m_BlockList.size();
}

void ioBlockManager::Update()
{
	if( 0 < m_dwPlayStartTime && m_dwPlayStartTime + UPDATE_BOUND_TIME < FRAMEGETTIME() )
	{
		m_dwPlayStartTime = 0;
		UpdateBound();
	}

	DWORD dwCurTime		= FRAMEGETTIME();
	float fTimePerSec	= g_FrameTimer.GetSecPerFrame();
	
	int nDebug = (int)m_BlockList.size();
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter )
	{
		ioBlock* pBlock = *iter;
		if( !pBlock )
			continue;

		pBlock->Update( fTimePerSec, m_dwPreUpdatedTime, dwCurTime );
	}
	m_dwPreUpdatedTime = dwCurTime;
}

void ioBlockManager::UpdateAfter()
{
	float fFrameGap = g_FrameTimer.GetCurFrameGap();

	int nDebug = (int)m_BlockList.size();
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter )
	{
		ioBlock* pBlock = *iter;
		if( !pBlock )
			continue;

		pBlock->UpdateAfter( fFrameGap );
	}
}

void ioBlockManager::StartUpdateBound()
{
	m_dwPlayStartTime = FRAMEGETTIME();
}

void ioBlockManager::UpdateBound()
{
	int nDebug = (int)m_BlockList.size();
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter )
	{
		ioBlock* pBlock = *iter;
		if( !pBlock )
			continue;

		pBlock->UpdateBound();
	}
}

void ioBlockManager::SetShadowMapEnable( bool bEnable )
{
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter )
	{	
		ioBlock* pBlock = *iter;
		if( !pBlock )
			continue;

		if( pBlock->GetProperty() )
		{
			switch( pBlock->GetProperty()->GetShadowType() )
			{
			case ioBlockProperty::ST_CAST:
				{
					pBlock->SetShadowCastEnable( bEnable );
				}
				break;				
			case ioBlockProperty::ST_RECIVE:
				{
					pBlock->SetShadowRecvEnable( bEnable );
				}
				break;
			}
		}
	}
}

void ioBlockManager::EnableMapEdge( bool bEnable )
{
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter )
	{	
		ioBlock* pBlock = *iter;
		if( !pBlock )
			continue;

		if( pBlock )
			pBlock->EnableMapEdge( bEnable );
	}
}

void ioBlockManager::StopAllTextureEffect( bool bStop )
{
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter )
	{	
		ioBlock* pBlock = *iter;
		if( !pBlock )
			continue;

		if( pBlock )
			pBlock->StopAllTextureEffect( bStop );
	}
}

void ioBlockManager::ExportBlockWorld()
{
	if( m_BlockList.empty() )
		return;

	ioHashString szFileName;
	switch( m_eType )
	{
	case BMT_GUILD:
		szFileName = "config/housing/sp2_guild_room_block.ini";
		break;
	case BMT_PERSONAL:	
		szFileName = "config/housing/sp2_personal_room_block.ini";
		break;
	}

	if( szFileName.IsEmpty() )
		return;

	char szBuf[MAX_PATH];
	ioINILoader_e kLoader( szFileName.c_str() );
	kLoader.SetTitle_e( "default_construct" );

	int i = 0;
	for( BlockList::iterator iter = m_BlockList.begin(); iter != m_BlockList.end(); ++iter, ++i )
	{	
		ioBlock* pBlock = *iter;
		if( !pBlock || !pBlock->GetProperty() )
			continue;
				 
		sprintf_s( szBuf, "item%d", i + 1 );
		kLoader.SaveInt( szBuf, pBlock->GetProperty()->GetCode() );

		const TilePos& Pos = pBlock->GetTilePos();
		sprintf_s( szBuf, "XZ%d", i + 1 );
		kLoader.SaveInt( szBuf, Pos.x + Pos.z * g_BlockManager.GetTileMax() );

		sprintf_s( szBuf, "Y%d", i + 1 );
		kLoader.SaveInt( szBuf, Pos.y );

		sprintf_s( szBuf, "direction%d", i + 1 );
		BYTE Rotate = (int)pBlock->GetRotate()/ROTATE_VALUE;
		kLoader.SaveInt( szBuf, Rotate );
	}
}