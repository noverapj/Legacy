#include "StdAfx.h"

#include "ioBlockWorld.h"
#include "ioUserBlockStorage.h"
#include "ioBlockEventParameter.h"
#include "ioPosMapQuadTree.h"
#include "ioBlockSetupManager.h"

#include "../io3DEngine/ioRay.h"
#include "../io3DEngine/ioCamera.h"

#include "../ioBaseChar.h"
#include "../ioPlayStage.h"

#include "../StateClass/ioBlockInterplaySpecialState.h"

#include "ioFishingBlock.h"

ioBlockWorld::ioBlockWorld( ioPlayStage* pStage ) : m_pGameStage( pStage )
{
	m_BlockCollRender	= RENDER_NONE;

	m_pMapBase			= NULL;	
	m_pSetupMgr			= NULL;	

	m_CurrSetupBlock	= NULL;		
}

ioBlockWorld::~ioBlockWorld()
{
	DestroyBaseCharEvent();
	DestroySetupBlock();
	
	SAFEDELETE( m_pSetupMgr );
	SAFEDELETE( m_pMapBase );
	SAFEDELETE( m_pCullingSetUpCamera );
	SAFEDELETE( m_pCullingCharCamera );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 초기화
//

void ioBlockWorld::Initialize( BlockModeType eType )
{
	m_BlockModeType = eType;

	SAFEDELETE( m_pSetupMgr );
	SAFEDELETE( m_pMapBase );

	m_pMapBase = new ioBlockMapBase;
	if( m_pMapBase )
		m_pMapBase->Initialize( eType );

	m_pSetupMgr = new ioBlockSetupManager;
	if( m_pSetupMgr )
		m_pSetupMgr->Initialize( m_pGameStage, eType );

	ioMapCollisionModule::SetMapBase( m_pMapBase );

	m_pCullingSetUpCamera = new ioCamera;
	if( m_pCullingSetUpCamera )
	{
		m_pCullingSetUpCamera->SetFov( 45.0f );

		ioLookAtCameraController *pCtrl = new ioLookAtCameraController;
		if( pCtrl )
		{
			pCtrl->SetPosition( D3DXVECTOR3( 0, 0, 0 ) );
			pCtrl->SetLookAt( D3DXVECTOR3( 0, 0, 0 ) );
			pCtrl->SetSpeed( 0.0f, 0.0f, 0.0f );
			m_pCullingSetUpCamera->SetController( pCtrl );
		}
	}

	m_pCullingCharCamera = new ioCamera;
	if( m_pCullingCharCamera )
	{
		m_pCullingCharCamera->SetFov( 15.0f );

		ioLookAtCameraController *pCtrl = new ioLookAtCameraController;
		if( pCtrl )
		{
			pCtrl->SetPosition( D3DXVECTOR3( 0, 0, 0 ) );
			pCtrl->SetLookAt( D3DXVECTOR3( 0, 0, 0 ) );
			pCtrl->SetSpeed( 0.0f, 0.0f, 0.0f );
			m_pCullingCharCamera->SetController( pCtrl );
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// DB에 저장된 블럭 생성
//

void ioBlockWorld::LoadPlayModeBlock()
{
	if( !m_pMapBase )
		return;

	ioUserBlockStorage* pStorage = NULL;
	switch( m_BlockModeType )
	{
	case BMT_GUILD:
		pStorage = g_App.GetGuildStorage();
		break;
	case BMT_PERSONAL:
		pStorage = g_App.GetHomeStorage();
		break;
	}

	if( pStorage == NULL )
		return;

	const ioBlockDBItemContainer* pContainer = pStorage->GetBlockDBItemContainer();
	if( pContainer == NULL )
		return;

	LOG.PrintTimeAndLog(0, "Housing Block Load Start" );

	for( int i = 0; i < (int)pContainer->ItemList.size(); ++i )
	{
		const ioBlockDBItem& DBItem = pContainer->ItemList[i];

		float fRtoate = DBItem.GetRotate() * ROTATE_VALUE;
		CreateBlock( DBItem.GetCode(), DBItem.GetTilePos(), fRtoate, DBItem.GetUniqueIndex() );
	}

	LOG.PrintTimeAndLog(0, "Housing Block Load End" );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 맵에 블럭 엔티티 생성
//

void ioBlockWorld::CreateBlock( int nCode, int nX, int nY, int nZ, float fRotate, __int64 nUniqueIndex )
{
	TilePos Pos( nX, nY, nZ );
	CreateBlock( nCode, Pos, fRotate, nUniqueIndex );
}

void ioBlockWorld::CreateBlock( int nCode, const TilePos& Pos, float fRotate, __int64 nUniqueIndex )
{
	ioBlock* pBlock = g_BlockManager.CreateBlock( nCode, nUniqueIndex );
	if( pBlock )
	{
		pBlock->SetCollBoxRotaion( g_BlockManager.GetBlockRotation() );
		pBlock->SetTitleCoord( Pos.x, Pos.y, Pos.z );
		pBlock->Rotate( fRotate );

		if( pBlock->GetBlockType() == ioBlock::BT_TILE )
		{
			ioTileBlock* pTile = ToTileBlock( pBlock );
			if( pTile )
			{
				TileMapContainer* pContainer = m_pMapBase->GetTileMapContainer();
				if( pContainer )
					pContainer->AddElement( pTile, Pos.x + Pos.z * g_BlockManager.GetTileMax() );
			}
		}
		else
		{
			PosMapContainer* pLevelContainer = m_pMapBase->GetPosMapContainer( Pos.y );
			if( pLevelContainer )
				pLevelContainer->AddElement( pBlock, Pos.x + Pos.z * g_BlockManager.GetTileMax() );

			TilePosList PosList;
			pBlock->GetPosList( PosList );
			for( int i = 0; i < (int)PosList.size(); ++i )
			{
				const TilePos& IneerPos = PosList[i];
				PosMapContainer* pLevelContainer = m_pMapBase->GetPosMapContainer( IneerPos.y );
				if( pLevelContainer )
					pLevelContainer->AddElement( pBlock, IneerPos.x + IneerPos.z * g_BlockManager.GetTileMax() );
			}
		}
	}
}

void ioBlockWorld::DestroyBlock( const TilePos& Pos )
{
	if( m_pMapBase == NULL )
		return;

	PosMapContainer* pLevelContainer = m_pMapBase->GetPosMapContainer( Pos.y );
	if( pLevelContainer )
	{
		ioBlock* pBlock = pLevelContainer->GetElement( Pos.x + Pos.z * g_BlockManager.GetTileMax() );
		if( pBlock )
		{
			pLevelContainer->RemoveElement( Pos.x + Pos.z * g_BlockManager.GetTileMax() );

			TilePosList PosList;
			pBlock->GetPosList( PosList );
			for( int i = 0; i < (int)PosList.size(); ++i )
			{
				const TilePos& IneerPos = PosList[i];
				PosMapContainer* pLevelContainer = m_pMapBase->GetPosMapContainer( IneerPos.y );
				if( pLevelContainer )
					pLevelContainer->RemoveElement( IneerPos.x + IneerPos.z * g_BlockManager.GetTileMax() );
			}

			g_BlockManager.DestroyBlock( pBlock->GetUniqueIndex() );
			return;
		}
	}
	
	TileMapContainer* pContainer = m_pMapBase->GetTileMapContainer();
	if( pContainer )
	{
		ioBlock* pBlock = pContainer->GetElement( Pos.x + Pos.z * g_BlockManager.GetTileMax() );
		if( pBlock )
		{
			pContainer->RemoveElement( Pos.x + Pos.z * g_BlockManager.GetTileMax() );
			g_BlockManager.DestroyBlock( pBlock->GetUniqueIndex() );
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 맵에 설치 할 블럭 엔티티 생성
//

void ioBlockWorld::DestroySetupBlock()
{	
	if( m_CurrSetupBlock )
	{
		g_BlockManager.DestroyBlock( m_CurrSetupBlock->GetUniqueIndex() );
		m_CurrSetupBlock = NULL;
	}
}

void ioBlockWorld::CreateSetupBlock( DWORD dwCode, const TilePos& Pos )
{
	DestroySetupBlock();

	ioBlock* pBlock = g_BlockManager.CreateBlock( dwCode, 0 );
	if( pBlock )
	{
		pBlock->SetCollBoxRotaion( g_BlockManager.GetBlockRotation() );
		pBlock->SetTitleCoord( Pos.x, Pos.y, Pos.z );
		pBlock->DisableInterPlay();
		pBlock->DisableAreaWeapon();
		m_CurrSetupBlock = pBlock;
	}
}

ioBlock* ioBlockWorld::GetSetUpBlock()
{
	return m_CurrSetupBlock;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 캐릭터 이벤트 처리 
//

void ioBlockWorld::DestroyBaseCharEvent()
{
	for( BaseCharEventMap::iterator iter = m_BaseCharEventMap.begin(); iter != m_BaseCharEventMap.end(); ++iter )
	{
		SAFEDELETE( iter->second );
	}
	m_BaseCharEventMap.clear();
}

ioBlockWorld::BaseCharEvent* ioBlockWorld::GetBaseCharEvent( const ioHashString& szName )
{
	BaseCharEventMap::iterator iter = m_BaseCharEventMap.find( szName );
	if( iter == m_BaseCharEventMap.end() )
	{
		BaseCharEvent* pEvent = new BaseCharEvent;
		m_BaseCharEventMap.insert( BaseCharEventMap::value_type( szName, pEvent ) );
		return pEvent;
	}
	else
	{
		return iter->second;
	}
}

void ioBlockWorld::CheckAfterLoop( ioCamera *pCamera )
{
	if( !m_pGameStage || !m_pMapBase )
		return;

	const BaseCharList& CharList = m_pGameStage->GetBaseCharList();
	for( unsigned int i = 0; i < CharList.size(); ++i )
	{
		const ioBaseChar* pChar = CharList[i];
		if( !pChar )
			continue;
		
		BaseCharEvent* pEvent = GetBaseCharEvent( pChar->GetCharName() );
		if( !pEvent )
			continue;
		
		TilePos Pos;
		m_pMapBase->WorldCoordToTileCoord( pChar->GetMidPositionByRate(), Pos );
		pEvent->m_CurrPos = Pos;

		if( m_CurrSetupBlock )
		{
			pEvent->m_SetUpPos = m_CurrSetupBlock->GetTilePos();
		}
		else
		{
			pEvent->m_SetUpPos.Init();
		}
	}

	ProcessBaseCharAlphaEvent( m_pGameStage->GetOwnerChar(), pCamera );
}

void ioBlockWorld::ProcessBaseCharAlphaEvent( const ioPlayEntity* pEntity, const ioCamera* pCamera, const TilePos& Pos, ioCamera* pCullingCam )
{
	if( ! pEntity || !pCamera || !m_pMapBase || !pCullingCam )
		return;

	if( pEntity->GetMidPositionByRate().y < g_BlockManager.GetBlockStartHeight() )
		return;
	
	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCullingCam->GetController() );
	if( !pCtrl )
		return;

	pCtrl->SetPosition( pEntity->GetMidPositionByRate() );
	pCtrl->SetLookAt( pCamera->GetPosition() );
	pCtrl->SetSpeed( 0.0f, 0.0f, 0.0f );

	ioLookAtCameraInput kInput;
	pCullingCam->Update( &kInput );

	ioBlockList InList, OutList;
	GetCameraVisibleList( pCamera, pCullingCam, pEntity, Pos, InList, OutList );
	for( int i = 0; i < (int)InList.size(); ++i )
	{
		ioBlock* pBlock = InList[i];
		if( !pBlock )
			continue;
				
		pBlock->SetShadowRecvEnable( false );
		pBlock->FadeIn();
	}

	for( int i = 0; i < (int)OutList.size(); ++i )
	{
		ioBlock* pBlock = OutList[i];
		if( !pBlock )
			continue;

		pBlock->FadeOut();
	}
}

void ioBlockWorld::GetCameraVisibleList( IN const ioCamera* pDisplayCamera, 
										 IN const ioCamera* pCullingCamera,
										 IN const ioPlayEntity* pEntity,
										 IN const TilePos& Pos,
										 OUT ioBlockList& InList,
										 OUT ioBlockList& OutList )
{
	if( pCullingCamera == NULL || pDisplayCamera == NULL )
		return;

	const ioBlock* pSetUp = ToBlockConst( pEntity );

	//ioMapBase에는 여러 위치에 같은 데이터가 중복 되기 때문에 
	//직접 매니저에서 리스트를 가져와서 컬링 대상과 아닌 대상을 분류
	const BlockList& List = g_BlockManager.GetAllBlock();
	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List[i];
		if( !pBlock )
			continue;

		//타일류는 대상 제외
		if( pBlock->GetBlockType() == ioBlock::BT_TILE )
			continue;

		//알파처리 Off 대상은 제외
		if( pBlock->GetProperty() && pBlock->GetProperty()->GetBlockAlphaType() == ioBlockProperty::BAT_OFF )
			continue;

		if( pBlock == pEntity )
			continue;

		//설치불가/회수불가 상태 등은 제외
		if( pBlock->IsColorState() != ioBlock::CSS_NONE )
		{
			OutList.push_back( pBlock );
			continue;
		}

		//설치 대상 오브젝트와 겹쳐지는 오브젝트는 추가
		if( pSetUp )
		{
			TilePosList List;
			pBlock->GetPosList( List );
			if( pSetUp->TestIntersection( List ) )
			{
				InList.push_back( pBlock );
				continue;
			}
		}
		
		//JSM 150911 최적화 - CPU 점유 비율이 높아 주석 처리
		/*
		D3DXVECTOR3 vEntityLength	= pDisplayCamera->GetPosition() - pEntity->GetWorldPosition();
		D3DXVECTOR3 vBlockLength	= pDisplayCamera->GetPosition() - pBlock->GetWorldPosition();
		if( D3DXVec3Length( &vEntityLength ) < D3DXVec3Length( &vBlockLength ) )
		{
			OutList.push_back( pBlock );
			continue;
		}
		*/

		ioSphere Sphere( pBlock->GetCollSphere().GetCenter(), pBlock->GetCollSphere().GetRadius() * 0.75f );
		if( Pos.y <= pBlock->GetTilePos().y && pCullingCamera->IsVisible( Sphere ) )
			InList.push_back( pBlock );
		else
			OutList.push_back( pBlock );
	}
}

bool ioBlockWorld::CheckFishEnable( const D3DXVECTOR3& vTargetPos )
{	
	if( GetMapBase() == NULL )
		return false;

	TilePos Pos;
	GetMapBase()->WorldCoordToTileCoord( vTargetPos, Pos );

	ioFishingBlock* pFishBlock = ToFishingBlock( GetMapBase()->GetBlockByTilePos( Pos ) );
	if( pFishBlock && pFishBlock->IsFishing( Pos ) )
		return true;

	return false;
}

void ioBlockWorld::ProcessBaseCharAlphaEvent( ioBaseChar* pOwner, ioCamera *pCamera )
{
	if( ! pOwner || !pCamera )
		return;

	if( m_CurrSetupBlock )
	{
		TilePos Pos;
		m_pMapBase->WorldCoordToTileCoord( m_CurrSetupBlock->GetMidPositionByRate(), Pos );
		ProcessBaseCharAlphaEvent( m_CurrSetupBlock, pCamera, Pos, m_pCullingSetUpCamera );
	}
	else
	{
		TilePos Pos;
		m_pMapBase->WorldCoordToTileCoord( pOwner->GetMidPositionByRate(), Pos );
		ProcessBaseCharAlphaEvent( pOwner, pCamera, Pos, m_pCullingCharCamera );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 상호작용 처리 
//

bool ioBlockWorld::CheckInterPlay( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;

	//ioMapBase에는 여러 위치에 같은 데이터가 중복 되기 때문에 
	//직접 매니저에서 리스트를 가져옴
	const BlockList& List = g_BlockManager.GetAllBlock();
	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List[i];
		if( !pBlock )
			continue;

		if( pBlock->GetProperty() == NULL )
			continue;

		if( pBlock->GetProperty()->GetInterPlayInfoList().empty() )
			continue;

		if( !pBlock->GetInterPlayChar().IsEmpty() )
			continue;

		int nCollIndex = INT_MAX;
		D3DXVECTOR3 vCollPos = ioMath::VEC3_ZERO;
		if( pBlock->TestInterPlayIntersection( pOwner, nCollIndex, vCollPos ) )
		{
			const ioBlockProperty::InterPlayInfo* pInfo = pBlock->GetProperty()->GetInterPlayInfo( nCollIndex );
			if( pOwner->IsDefenseKey() && pOwner->GetState() == CS_DELAY )
			{
				pOwner->EndUnLimitedEmoticon();
				pOwner->SetState( CS_BLOCK_INTEPLAY );
				ioBlockInterplaySpecialState* pState = ToBlockInterplaySpecialState( pOwner->GetSpecialState() );
				if( pState )
				{
					pState->Start( pOwner, vCollPos, pBlock->GetWorldOrientation() * pBlock->GetCollRotate(), pBlock->GetUniqueIndex(), nCollIndex );
				}
				else
				{
					pOwner->SetState( CS_DELAY );
				}
				return true;
			}
			else
			{
				if( !pOwner->IsEmoticonEvent() )
					 pOwner->SetBlockInterPlayEmoticon( pInfo->m_EmoticonMent, pInfo->m_EmoticonMentColor );				

				return false;
			}
		}
	}

	pOwner->EndUnLimitedEmoticon();
	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 매크로 이벤트 처리 
//

void ioBlockWorld::ReciveEvent( ioBlockEventListener::EventType eType, ioBlockEventParameter* param )
{
	switch( eType )
	{
	case ioBlockEventListener::ET_BLOCK_COLL_NONE_VIEW:
		{
			SetBlockCollRender( RENDER_NONE );
		}
		break;
	case ioBlockEventListener::ET_BLOCK_COLL_AABB_VIEW:
		{
			SetBlockCollRender( RENDER_BOX );
		}
		break;
	case ioBlockEventListener::ET_BLOCK_COLL_SPHERE_VIEW:
		{
			SetBlockCollRender( RENDER_SPHERE );
		}
		break;
	case ioBlockEventListener::ET_BLOCK_INTERPLAY_COLL_VIEW:
		{
			SetBlockCollRender( RENDER_INTERPLAY_BOX );
		}
		break;
	case ioBlockEventListener::ET_BLOCK_CREATE:
		{
			ioBlockEventCreate* pParam = dynamic_cast<ioBlockEventCreate*>( param );
			if( pParam )
			{
				TilePos Pos( pParam->nTileX, pParam->nTileY, pParam->nTileZ );				
				g_BlockSetupManager.SendSetUpBlock( pParam->nCode, pParam->nTileX, pParam->nTileY, pParam->nTileZ, 0 );
			}
		}
		break;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 업데이트
//

void ioBlockWorld::ProcessUpdate()
{
	g_BlockManager.Update();	
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 오브젝트 렌더
//

void ioBlockWorld::RenderObject( ioCamera* pCamera )
{
	D3DXVECTOR3 vVtxArray[8];
	RenderSystem().SetModelMatrixIdentity();

	switch( m_BlockCollRender )
	{
	case RENDER_BOX:
		{
			m_pMapBase->RenderPosQuadTree( m_pGameStage, pCamera );

			if( m_CurrSetupBlock )
				m_CurrSetupBlock->RenderBlockAABB( m_BlockCollRender, m_pGameStage, pCamera );

			const BaseCharList& CharList = m_pGameStage->GetBaseCharList();
			for( unsigned int i = 0; i < CharList.size(); ++i )
			{
				const ioBaseChar* pChar = CharList[i];
				if( !pChar || !pCamera->IsVisible( pChar->GetWorldAxisBox() ) )
					continue;	

				const ioOrientBox &rkOrientBox = pChar->GetWorldCollisionBox();
				rkOrientBox.GetVertices( vVtxArray );
				m_pGameStage->RenderBox( vVtxArray, 0xFF0000FF );	

				D3DXVECTOR3 vPos[2];
				vPos[0] = pChar->GetWorldPosition();
				vPos[1] = pChar->GetWorldPosition();
				vPos[1].y += FLOAT1000;

				m_pGameStage->RenderLine( vPos, 0x00FF00FF );
			}
		}
		break;
	}

	m_pMapBase->RenderBlockAABB( m_BlockCollRender, m_pGameStage, pCamera );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// UI 렌더
//

void ioBlockWorld::RenderUI()
{
	if( !m_pGameStage )
		return;

	if( !Setting::ShowExtraInfo() )
		return;

	for( BaseCharEventMap::const_iterator iter = m_BaseCharEventMap.begin(); iter != m_BaseCharEventMap.end(); ++iter )
	{
		const BaseCharEvent* pEvent = iter->second;
		ioBaseChar* pChar = m_pGameStage->GetBaseChar( iter->first );
		if( pEvent && pChar && pChar->IsOwnerChar() )
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.PrintText( FLOAT1, 164.0f, 0.7f, "Curr  Tile POS - Y : %d, X : %d, Z : %d", pEvent->m_CurrPos.y,  pEvent->m_CurrPos.x,  pEvent->m_CurrPos.z );
			g_FontMgr.PrintText( FLOAT1, 184.0f, 0.7f, "SetUp Tile POS - Y : %d, X : %d, Z : %d", pEvent->m_SetUpPos.y, pEvent->m_SetUpPos.x, pEvent->m_SetUpPos.z );
			g_FontMgr.PrintText( FLOAT1, 264.0f, 0.7f, "Curr  Wrld POS - Y : %f, X : %f, Z : %f", pChar->GetWorldPosition().x, pChar->GetWorldPosition().y, pChar->GetWorldPosition().z );
		}
	}

	const BlockList& List = g_BlockManager.GetAllBlock();
	g_FontMgr.PrintText( FLOAT1, 204.0f, 0.7f, "Curr Block Count : %d", (int)List.size() );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// TCP
//

bool ioBlockWorld::ProcessTCPPacket( SP2Packet &rkPacket )
{
	if( g_BlockSetupManager.ProcessTCPPacket( rkPacket ) )
		return true;

	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// UDP
// 

bool ioBlockWorld::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( g_BlockSetupManager.ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 전체 블럭 리스트 검증
// 

void ioBlockWorld::TestMapBlock( SP2Packet &rkPacket )
{
	bool bConsecutivePacket;
	int nCount;
	rkPacket >> bConsecutivePacket >> nCount;

	for( int i = 0; i < nCount; ++i )
	{
		__int64 nItemIndex;
		int	nCode;
		rkPacket >> nItemIndex >> nCode;

		int nCoordCount;
		rkPacket >> nCoordCount;

		TilePosList PosList;
		for( int j = 0; j < nCoordCount; ++j )
		{
			int XZ, Y;
			rkPacket >> XZ >> Y;

			TilePos Pos;
			Pos.x = XZ % g_BlockManager.GetTileMax();
			Pos.z = XZ / g_BlockManager.GetTileMax();
			Pos.y = Y;
			
			PosList.push_back( Pos );
		}

		ioBlock* pBlock = g_BlockManager.GetBlock( nItemIndex );
		if( pBlock )
		{
			pBlock->TestBlock( PosList );
		}
		else
		{
			LOG.PrintTimeAndLog(0, "[TEST BLOCK POS : %d, %d, %d] - %d Block Not Matched(Find Fail)", pBlock->GetTilePos().x, pBlock->GetTilePos().y, pBlock->GetTilePos().z, nCode );
		}
	}

	if( bConsecutivePacket )
	{
		LOG.PrintTimeAndLog(0, "[TEST BLOCK POS] -Test Complete" );
		LOG.PrintTimeAndLog(0, "----------------------------------------------------------------------------------------------" );
	}
}