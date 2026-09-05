#include "stdafx.h"
#include "ioBlockDBItem.h"

#include "ioMapCollisionModule.h"
#include "ioBlockWorld.h"

#include "../ioMapCollisionHelper.h"
#include "../ioWeapon.h"
#include "../ioBaseChar.h"

ioMapCollisionModule::ioMapCollisionModule()
{
}

ioMapCollisionModule::~ioMapCollisionModule()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 블럭 Entity 리스트에서 높이 값 가져오기
//

float ioMapCollisionModule::GetHeight( float fXPos, float fZPos )
{
	return GetHeight( D3DXVECTOR3( fXPos, 100000.0f, fZPos ) );
}

float ioMapCollisionModule::GetHeight( float fXPos, float fYPos, float fZPos )
{
	return GetHeight( D3DXVECTOR3( fXPos, fYPos, fZPos ) );
}

float ioMapCollisionModule::GetHeight( D3DXVECTOR3 vPos )
{
	if( !m_pMapBase )
		return 0.0f;
	
	ioBlockList List;
	m_pMapBase->GetBlockByTop( vPos, List );
	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List[i];
		if( pBlock && pBlock->GetCollShape() )
		{
			ioRay kYRay( D3DXVECTOR3( vPos.x, vPos.y, vPos.z ), -ioMath::UNIT_Y );
			D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
			if( pBlock->GetCollShape()->TestIntersection( pBlock->GetTransform(), kYRay, NULL, &vColPoint ) )
			{
				return vColPoint.y;
			}
		}
	}

	return 0.0f;
}

float ioMapCollisionModule::GetHeightByOBB( float fXPos, float fZPos )
{
	return GetHeightByOBB( D3DXVECTOR3( fXPos, 100000.0f, fZPos ) );
}

float ioMapCollisionModule::GetHeightByOBB( float fXPos, float fYPos, float fZPos )
{
	return GetHeightByOBB( D3DXVECTOR3( fXPos, fYPos, fZPos ) );
}

float ioMapCollisionModule::GetHeightByOBB( D3DXVECTOR3 vPos )
{
	if( !m_pMapBase )
		return 0.0f;

	ioBlockList List;
	m_pMapBase->GetBlockByTop( vPos, List );
	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List[i];
		if( !pBlock )
			continue;
		
		ioRay kYRay( D3DXVECTOR3( vPos.x, vPos.y, vPos.z ), -ioMath::UNIT_Y );
		D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
		if( pBlock->TestIntersection( kYRay, &vColPoint ) )
		{
			return vColPoint.y;
		}
	}

	return 0.0f;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 엔티티와 블럭 간 충돌시 엔티티 위치 계산(어디까지 이동 가능한지)
// 
bool ioMapCollisionModule::ReCalculateMoveVectorByBlock( IN ioPlayEntity* pEntity,
														 IN const ioAxisAlignBox& kAxisMoveBox,									   
														 IN const D3DXVECTOR3& vMoveDir,
														 IN bool bFloating,
														 OUT D3DXVECTOR3 *pMove,
														 OUT bool &bCol )
{
	if( !pEntity )
		return true;
	
	if( !m_pMapBase )
		return true;

	D3DXVECTOR3 vPos = pEntity->GetMidPositionByRate();

	BlockMapBuffer List;
	m_pMapBase->GetAdjacentBlockList( vPos, pEntity->GetWorldCollisionBox().GetBoxRadius(), List );
	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List.m_pBuffer[i];
		if( !pBlock || pBlock->IsCollChecked() || !pBlock->GetCollShape() )
			continue;

		pBlock->SetCollChecked( true );

		//스피어 체크
		ioSphere kSphere( vPos, pEntity->GetWorldCollisionBox().GetBoxRadius() );
		if( !ioMath::TestIntersection( kSphere, pBlock->GetCollSphere() ) )
			continue;

		if( !ioMapCollisionHelper::ReCalculateMoveVectorByWorldMesh( pEntity, 
																	 pEntity->GetWorldCollisionBox(),
																	 pBlock->GetCollShape(),
																	 pBlock->GetTransform(),
																	 g_BlockManager.GetBlockStartHeight(),
																	 vMoveDir,
																	 bFloating,
																	 pMove,
																	 bCol ) )
		{
			ResetColl( &List );
			return false;
		}
	}
	
	ResetColl( &List );
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 엔티티와 블럭 간 충돌 체크 및 엔티티 밀림 처리
// 

void ioMapCollisionModule::CheckPlayEntityBlockCollision( ioGameEntity* pEntity, ioPlayStage* pStage )
{
	if( !m_pMapBase )
		return;

	if( !pEntity )
		return;

	if( ToWorldDecoration( pEntity ) )
		return;
		
	ioBaseChar* pChar = ToBaseChar( pEntity );
	if( pChar && pChar->GetState() == CS_BLOCK_INTEPLAY )
		return;

	TilePos Pos;
	m_pMapBase->WorldCoordToTileCoord( pEntity->GetWorldPosition(), Pos );

	D3DXVECTOR3 vPos = pEntity->GetMidPositionByRate();

	BlockMapBuffer List;
	m_pMapBase->GetAdjacentBlockList( vPos, pEntity->GetWorldCollisionBox().GetBoxRadius(), List );
	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List.m_pBuffer[i];
		if( !pBlock || pBlock->IsCollChecked() || !pBlock->GetCollShape() )
			continue;

		pBlock->SetCollChecked( true );

		//스피어 체크
		ioSphere kSphere( vPos, pEntity->GetWorldCollisionBox().GetBoxRadius() );
		if( !ioMath::TestIntersection( kSphere, pBlock->GetCollSphere() ) )
			continue;

		if( ioMapCollisionHelper::CheckPlayEntityMapCollision( pStage, pEntity, pBlock->GetCollShape(), 50, pBlock->GetTransform() ) )
		{
			//맵 충돌 관련 이벤트 처리
		}
	}

	ResetColl( &List );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 시작점과 끝점 사이에 블럭이 존재 하는지 검사
// 

bool ioMapCollisionModule::CheckCollLine( IN const D3DXVECTOR3 &vStart, IN const D3DXVECTOR3 &vEnd, OUT D3DXVECTOR3& vColPos )
{
	if( !m_pMapBase )
		return false;

	D3DXVECTOR3 vDiff = vEnd - vStart;
	float fDiff = D3DXVec3Length( &vDiff );

	BlockMapBuffer List;
	m_pMapBase->GetAdjacentBlockList( vStart, fDiff, List );
	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List.m_pBuffer[i];
		if( !pBlock || pBlock->IsCollChecked() || !pBlock->GetCollShape() )
			continue;

		pBlock->SetCollChecked( true );

		D3DXVECTOR3 vDir = vEnd - vStart;
		D3DXVec3Normalize( &vDir, &vDir );
		ioRay kLineRay( vStart, vDir );

		if( !ioMath::TestIntersection( kLineRay, pBlock->GetCollSphere() ) )
			continue;

		if( ioMapCollisionHelper::CheckCollisionLine( vColPos, pBlock->GetCollShape(), vStart, vEnd, pBlock->GetTransform() ) )
		{
			ResetColl( &List );
			return true;
		}
	}

	ResetColl( &List );
	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 레이와 충돌 하는 블럭이 있는지 검사
// 

bool ioMapCollisionModule::TestBlockMeshCollision( const ioRay &rkRay, D3DXVECTOR3 *pColPoint )
{
	if( !m_pMapBase )
		return false;

	BlockMapBuffer List;
	float fDiff = D3DXVec3Length( &rkRay.GetDirection() );
	m_pMapBase->GetAdjacentBlockList( rkRay.GetOrigin(), fDiff, List );

	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List.m_pBuffer[i];
		if( !pBlock || pBlock->IsCollChecked() || !pBlock->GetCollShape() )
			continue;

		pBlock->SetCollChecked( true );

		if( !ioMath::TestIntersection( rkRay, pBlock->GetCollSphere() ) )
			continue;

		ioOpcodeShape *pShape = pBlock->GetCollShape();
		if( pShape && pShape->TestIntersection( pBlock->GetTransform(), rkRay, NULL, pColPoint ) )
		{
			ResetColl( &List );
			return true;
		}
	}

	ResetColl( &List );
	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// OBB박스와 충돌 하는 블럭이 있는지 검사
// 

bool ioMapCollisionModule::TestBlockMeshCollision( const ioOrientBox &rkBox )
{
	if( !m_pMapBase )
		return false;

	BlockMapBuffer List;
	m_pMapBase->GetAdjacentBlockList( rkBox.GetCenter(), rkBox.GetBoxRadius(), List );
	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List.m_pBuffer[i];
		if( !pBlock || pBlock->IsCollChecked() || !pBlock->GetCollShape() )
			continue;

		pBlock->SetCollChecked( true );

		if( !ioMath::TestIntersection( rkBox, pBlock->GetCollSphere() ) )
			continue;

		ioOpcodeShape *pShape = pBlock->GetCollShape();
		if( pShape && pShape->TestIntersection( pBlock->GetTransform(), rkBox ) )
		{
			ResetColl( &List );
			return true;
		}
	}

	ResetColl( &List );
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 블럭 CMS 매쉬와 레이를 충돌 체크(충돌 지점과 시작점과의 거리가 fCheckRange 보다 큰지 체크)
//

bool ioMapCollisionModule::CheckCollRayVsBlockByRange( IN const ioRay& rkRay, IN float fCheckRange )
{
	if( !m_pMapBase )
		return false;

	BlockMapBuffer List;
	m_pMapBase->GetAdjacentBlockList( rkRay.GetOrigin(), fCheckRange, List );

	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List.m_pBuffer[i];
		if( !pBlock || pBlock->IsCollChecked() || !pBlock->GetCollShape() )
			continue;

		pBlock->SetCollChecked( true );

		if( !ioMath::TestIntersection( rkRay, pBlock->GetCollSphere() ) )
			continue;

		if( ioMapCollisionHelper::CheckRayVsMapCollisionByRange( rkRay, pBlock->GetCollShape(), pBlock->GetTransform(), fCheckRange ) )
		{
			ResetColl( &List );
			return true;
		}
	}

	ResetColl( &List );
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 블럭 CMS 매쉬와 웨폰 충돌 체크
//

bool ioMapCollisionModule::CheckWeaponVSBlockCollision( IN ioPlayStage* pStage, IN ioWeapon* pWeapon, bool bPiercingCheck /*= false */ )
{
	if( !m_pMapBase )
		return false;

	BlockMapBuffer List;
	m_pMapBase->GetAdjacentBlockList( pWeapon->GetPosition(), pWeapon->GetWorldAttackBox(true).GetBoxRadius(), List );

	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pBlock = List.m_pBuffer[i];
		if( !pBlock )
			continue;

		if( pBlock->IsCollChecked() )
			continue;

		if( !pBlock->GetCollShape() )
			continue;

		pBlock->SetCollChecked( true );

		if( !ioMath::TestIntersection( pWeapon->GetWorldAttackBox(true), pBlock->GetCollSphere() ) )
			continue;

		if( bPiercingCheck && ioMapCollisionHelper::CheckWeaponPiercingCollision( pWeapon, pBlock->GetCollShape(), pBlock->GetTransform() ) )
		{
			ResetColl( &List );
			return true;
		}

		if( pWeapon->TestMapCollision( pBlock->GetCollShape(), pStage, pBlock->GetTransform() ) )
		{
			ResetColl( &List );
			return true;
		}
	}

	ResetColl( &List );
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 블럭간 충돌 체크
//
// 
bool ioMapCollisionModule::CheckBlockVSBlockCollision( ioBlock* pBlock, const TilePos& NextPos, float fNextRotate )
{
	if( !m_pMapBase )
		return false;

	if( !pBlock )
		return false;

	BlockMapBuffer List;
	if( pBlock->GetBlockType() == ioBlock::BT_TILE )
	{
		m_pMapBase->GetAdjacentBlockListByTile( pBlock->GetTilePos(), pBlock->GetCollBlockSphere().GetRadius(), List );
	}
	else
	{
		m_pMapBase->GetAdjacentBlockList( pBlock->GetWorldPosition(), pBlock->GetCollBlockSphere().GetRadius(), List );
	}

	for( int i = 0; i < (int)List.size(); ++i )
	{
		ioBlock* pDestBlock = List.m_pBuffer[i];
		if( !pDestBlock || pDestBlock->IsCollChecked() )
			continue;

		pBlock->SetCollChecked( true );
		if( pBlock->TestPreIntersection( pDestBlock, NextPos, fNextRotate ) )
		{
			ResetColl( &List );
			return true;
		}
	}

	ResetColl( &List );
	return false;
}

void ioMapCollisionModule::ResetColl( BlockMapBuffer* List )
{
	if( List == NULL )
		return;

	for( int i = 0; i < (int)List->size(); ++i )
	{
		ioBlock* pBlock = List->m_pBuffer[i];
		if( pBlock && pBlock->IsCollChecked() )
			pBlock->SetCollChecked( false );
	}
}