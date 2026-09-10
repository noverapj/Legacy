#include "StdAfx.h"
#include "ioMapCollisionHelper.h"
#include "ioNpcChar.h"

#include "ioPlayStage.h"

namespace ioMapCollisionHelper
{

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 두 백터 충동 검사 가능 간 거리 계산
// 

	bool IsCollisionCheckRange( const D3DXVECTOR3 &vPos1, const D3DXVECTOR3 &vPos2 )
	{
		D3DXVECTOR3 vDiff = vPos1 - vPos2;
		if( D3DXVec3LengthSq( &vDiff ) < CHECK_COLLISION_RANGE * CHECK_COLLISION_RANGE )
			return true;

		return false;
	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 이동 방향 재계산
// 

	bool ModifyMoveDir( IN const ioAxisAlignBox &rkSrcBox,
						IN const ioAxisAlignBox &rkMoveBox,
						IN const ioAxisAlignBox &rkDestBox,
						IN bool bForceMove,
						OUT D3DXVECTOR3 *pMove )
	{
		D3DXVECTOR3 vMin = rkDestBox.GetMinPos();
		D3DXVECTOR3 vMax = rkDestBox.GetMaxPos();
		D3DXVECTOR3 vCenter = rkMoveBox.GetCenter();

		if( COMPARE( vCenter.x, vMin.x, vMax.x ) &&	COMPARE( vCenter.z, vMin.z, vMax.z ) )
			return false;

		D3DXVECTOR3 vPlaneNrm;

		D3DXVECTOR3 vDiff = rkSrcBox.GetCenter() - rkDestBox.GetCenter();
		if( fabsf( vDiff.x ) > fabsf( vDiff.z ) )
		{
			if( vDiff.x > 0.0f )
			{
				vPlaneNrm = ioMath::UNIT_X;
			}
			else
			{
				vPlaneNrm = -ioMath::UNIT_X;
			}

			if( bForceMove )
				pMove->z = 0.0f;
		}
		else
		{
			if( vDiff.z > 0.0f )
			{
				vPlaneNrm = -ioMath::UNIT_Z;
			}
			else
			{
				vPlaneNrm = ioMath::UNIT_Z;
			}

			if( bForceMove )
				pMove->x = 0.0f;
		}

		float fRate = FLOAT09;

		D3DXVECTOR3 vBackMove = vPlaneNrm * ( D3DXVec3Dot( pMove, &vPlaneNrm ) * fRate );
		if( D3DXVec3LengthSq( pMove ) <= D3DXVec3LengthSq( &vBackMove ) )
			return false;

		*pMove -= vBackMove;

		return true;
	}

	bool HasMountedEntity( IN ioGameEntity* pMount, IN const MountedEntityList& rkMount )
	{
		MountedEntityList::const_iterator iter = rkMount.begin();
		for( ; iter != rkMount.end(); ++iter )
		{
			ioGameEntity* pMountedEntity = *iter;
			if( pMountedEntity == pMount )
			{
				return true;
			}
		}

		return false;
	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//위치 재계산 처리
// 

	bool ReCalculateMoveVectorByMountAvailableEntity( IN ioPlayEntity* pEntity, 
													  IN ioAxisAlignBox& kAxisMoveBox,
													  IN const ioAxisAlignBox& kAxisBox,
													  IN bool bMonsterVsMonsterCollision,
													  IN const MountAvailableList& AvailableList,
													  IN const MountedEntityList* pMountList,
													  IN const D3DXVECTOR3& vMoveDir,
													  OUT D3DXVECTOR3 *pMove,
													  OUT bool &bCol )
	{
		MountAvailableList::const_iterator iMount = AvailableList.begin();
		for( ; iMount!=AvailableList.end() ; ++iMount )
		{
			ioGameEntity *pMount = *iMount;

			if( pEntity == pMount )
				continue;

			if( !pMount->IsNowMountEnable() )
				continue;

			if( !IsCollisionCheckRange( pEntity->GetWorldPosition(), pMount->GetWorldPosition() ) )
				continue;

			if( pEntity && pEntity->IsEntityCollisionSkipState( pMount ) )
				continue;

			if( pMountList && HasMountedEntity( pMount, *pMountList ) )
				continue;

			ioDummyChar* pDummy = ToDummyChar( pMount );
			if( pDummy && pDummy->IsDummyCharCollisionSkipState( ToBaseChar( pEntity ) ) )
				continue;

			if( !bMonsterVsMonsterCollision )
			{
				if( ToNpcChar( pEntity ) && ToNpcChar( pMount ) )
					continue;
			}

			const ioAxisAlignBox &rkMountBox = pMount->GetWorldAxisBox();

			ioOrientBox kMountOrient = pMount->GetWorldCollisionBox();
			kMountOrient.SetCenter( rkMountBox.GetCenter() );

			ioOrientBox kOrientBox;
			kOrientBox.SetByAxisBox( kAxisMoveBox );

			if( !ioMath::TestIntersection( kOrientBox, kMountOrient, NULL ) )
			{
				continue;
			}

			D3DXVECTOR3 vCenterDir = rkMountBox.GetCenter() - kAxisBox.GetCenter();
			vCenterDir.y = 0.0f;
			D3DXVec3Normalize( &vCenterDir, &vCenterDir );

			if( D3DXVec3Dot( &vCenterDir, &vMoveDir ) < 0.0f )	// 후면
			{
				continue;
			}

			bCol = true;
			kOrientBox.SetByAxisBox( kAxisBox );

			if( ioMath::TestIntersection( kOrientBox, kMountOrient, NULL ) )
			{
				return false;
			}

			if( ModifyMoveDir( kAxisBox, kAxisMoveBox, rkMountBox, false, pMove ) )
			{
				kAxisMoveBox = kAxisBox;
				kAxisMoveBox.Translate( *pMove );
			}
			else
			{
				return false;
			}
		}

		return true;
	}


	bool ReCalculateMoveVectorByEntity( IN ioPlayEntity* pEntity, 
										IN const GameEntityList& List,
										IN ioAxisAlignBox& kAxisMoveBox,
										IN const ioAxisAlignBox& kAxisBox,
										IN const D3DXVECTOR3& vMoveDir,
										OUT D3DXVECTOR3 *pMove,
										OUT bool &bCol )
	{
		
		for( GameEntityList::const_iterator iter = List.begin(); iter != List.end() ; ++iter )
		{
			ioWallZone *pWallZone = ToWallZone( *iter );
			if( pWallZone == NULL )
				continue;

			if( !ioMapCollisionHelper::IsCollisionCheckRange( pEntity->GetWorldPosition(), pWallZone->GetWorldPosition() ) )
				continue;

			const ioAxisAlignBox &rkMountBox = pWallZone->GetWorldAxisBox();

			ioOrientBox kMountOrient = pWallZone->GetWorldCollisionBox();
			kMountOrient.SetCenter( rkMountBox.GetCenter() );

			ioOrientBox kOrientBox;
			kOrientBox.SetByAxisBox( kAxisMoveBox );

			if( !ioMath::TestIntersection( kOrientBox, kMountOrient, NULL ) )
			{
				continue;
			}

			D3DXVECTOR3 vCenterDir = rkMountBox.GetCenter() - kAxisBox.GetCenter();
			vCenterDir.y = 0.0f;
			D3DXVec3Normalize( &vCenterDir, &vCenterDir );

			if( D3DXVec3Dot( &vCenterDir, &vMoveDir ) < 0.0f )	// 후면
			{
				continue;
			}

			bCol = true;
			kOrientBox.SetByAxisBox( kAxisBox );

			if( ioMath::TestIntersection( kOrientBox, kMountOrient, NULL ) )
			{
				return false;
			}

			if( ioMapCollisionHelper::ModifyMoveDir( kAxisBox, kAxisMoveBox, rkMountBox, false, pMove ) )
			{
				kAxisMoveBox = kAxisBox;
				kAxisMoveBox.Translate( *pMove );
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	bool ReCalculateMoveVectorByWorldMesh( IN ioPlayEntity* pEntity,
										   IN const ioOrientBox& kMoveBox,
										   IN ioOpcodeShape* pShape,
										   IN const D3DXMATRIX* pShapeWorldMat,
										   IN float fInverseMapHeight,
										   IN const D3DXVECTOR3& vMoveDir,
										   IN bool bFloating,
										   OUT D3DXVECTOR3 *pMove,
										   OUT bool &bCol )
	{


		//ioOpcodeShape의 모델 공간으로 투영
		ioOrientBox kCharInvBox = kMoveBox;
		if( pShapeWorldMat )
		{
			D3DXMATRIX matInvModel;
			D3DXMatrixInverse( &matInvModel, NULL, pShapeWorldMat );
			kCharInvBox.Transform( &matInvModel );
		}

		if( !pShape->FindIntersection( pShapeWorldMat, kMoveBox, NULL ) )
			return true;

		const IntVec &rkColFaceList = pShape->GetFindIntersectionFaceList();

		Vector3Vec vSlidingNrmList;
		vSlidingNrmList.reserve( rkColFaceList.size() );

		float fCharMidHeight = pEntity->GetMidHeightByRate();

		//캐릭터 높이 또한 모델 공간으로 투영
		if( fInverseMapHeight != 0.0f )
			fCharMidHeight -= fInverseMapHeight;

		int iColFaceCount = rkColFaceList.size();
		for( int i = 0 ; i < iColFaceCount ; ++i )
		{
			int iFaceIndex = rkColFaceList[i];

			if( !bFloating )
			{
				D3DXVECTOR3 v1, v2, v3;
				pShape->GetTriVertexInModelSpace( iFaceIndex, v1, v2, v3 );

				int iQuantity;
				D3DXVECTOR3 akP[8];
				if( pShapeWorldMat )
				{
					if( !ioMath::FindIntersection( v1, v2, v3, kCharInvBox, iQuantity, akP ) )
						continue;
				}
				else
				{
					if( !ioMath::FindIntersection( v1, v2, v3, kMoveBox, iQuantity, akP ) )
						continue;
				}

				bool bSkipCheck = true;
				for( int j = 0 ; j < iQuantity ; ++j )
				{
					if( akP[j].y >= fCharMidHeight )
					{
						bSkipCheck = false;
						break;
					}
				}

				if( bSkipCheck )
					continue;
			}

			D3DXVECTOR3 vFaceNrm;
			pShape->GetTriNormalInModelSpace( iFaceIndex, vFaceNrm );
			if( D3DXVec3Dot( &vFaceNrm, &ioMath::UNIT_Y ) > 0.85f )	// 경사
				continue;

			if( D3DXVec3Dot( &vFaceNrm, &vMoveDir ) > 0.0f )
				continue;

			bool bExistFaceNrm = false;
			int iSlidingNrmCount = vSlidingNrmList.size();
			for( int k = 0 ; k < iSlidingNrmCount ; ++k )
			{
				if( ioMath::IsEqual( vFaceNrm, vSlidingNrmList[k] ) )	// 같은 노말이 있으면 제외
				{
					bExistFaceNrm = true;
					break;
				}
			}

			if( !bExistFaceNrm )
			{
				vSlidingNrmList.push_back( vFaceNrm );
			}
		}

		// Sliding Vector....
		if( vSlidingNrmList.empty() )
			return true;

		bCol = true;
		D3DXVECTOR3 vTotalNrm = vSlidingNrmList.front();
		for( i = 1; i < (int)vSlidingNrmList.size(); ++i )
		{
			vTotalNrm += vSlidingNrmList[i];
			D3DXVec3Normalize( &vTotalNrm, &vTotalNrm );
		}

		if( D3DXVec3Dot( &vMoveDir, &vTotalNrm ) >= 0.0f )
			return true;

		if( bFloating )
			return false;

		*pMove = *pMove - D3DXVec3Dot( pMove, &vTotalNrm ) * vTotalNrm;
		if( D3DXVec3LengthSq( pMove ) < FLOAT1 )
			return false;

		return true;
	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 엔티티와 메쉬 CMS 간 충돌 체크 및 엔티티 밀림 처리
// 

	bool CheckPlayEntityMapCollision( IN ioPlayStage* pStage, IN ioGameEntity* pEntity, IN ioOpcodeShape *pShape, IN int nMoveCount, IN const D3DXMATRIX* pShapeWorldMat /*= NULL */ )
	{
		if( !pStage || !pEntity || !pShape )
			return false;

		ioDummyChar *pDummyChar = ToDummyChar( pEntity );

		// 맵과 충돌시 특별 처리가 필요한 경우를 위해 IsNeedCheckMapCollision 보다 먼저 해야한다
		if( pDummyChar )
		{
			pDummyChar->TestMapCollision( pShape, pStage, pShapeWorldMat );
		}

		if( !pEntity->IsNeedCheckMapCollision() )
			return false;

		// Ball은 따로 한다
		if( ToBallStruct( pEntity ) )
			return false;

		ioOrientBox kEntityBox;
		kEntityBox.SetByAxisBox( pEntity->GetWorldAxisBox() );

		float fNewExtents = kEntityBox.GetExtents(1) * FLOAT05;
		kEntityBox.SetCenter( kEntityBox.GetCenter() + kEntityBox.GetAxis(1) * fNewExtents );
		kEntityBox.SetExtents( 1, fNewExtents );

		if( !pShape->FindIntersection( pShapeWorldMat, kEntityBox, NULL ) )
			return false;

		ioAxisAlignBox kAxisBox = pEntity->GetWorldAxisBox();

		D3DXVECTOR3 vDiff( 0.0f, 0.0f, 0.0f );
		vDiff.y = ( kAxisBox.GetMaxPos().y - kAxisBox.GetMinPos().y ) * 0.25f;
		kAxisBox.SetMinPos( kAxisBox.GetMinPos() + vDiff );

		const IntVec &rkFaceList = pShape->GetFindIntersectionFaceList();
		Vector3Vec vPushNrmList;
		vPushNrmList.reserve( rkFaceList.size() );

		int iFaceCount = rkFaceList.size();
		for( int i=0 ; i<iFaceCount ; i++ )
		{
			D3DXVECTOR3 vPlaneNrm;
			pShape->GetTriNormalInModelSpace( rkFaceList[i], vPlaneNrm );

			if( D3DXVec3Dot( &ioMath::UNIT_Y, &vPlaneNrm ) > FLOAT09 )
				continue;

			bool bSameNrmExist = false;
			int iPushNrmCount = vPushNrmList.size();
			for( int j=0 ; j<iPushNrmCount ; j++ )
			{
				if( ioMath::IsEqual( vPlaneNrm, vPushNrmList[j] ) )
				{
					bSameNrmExist = true;
					break;
				}
			}

			if( !bSameNrmExist )
			{
				vPushNrmList.push_back( vPlaneNrm );
			}
		}

		if( vPushNrmList.empty() )
			return false;

		D3DXVECTOR3 vPushDir = vPushNrmList.front();
		for( i=1 ; i<(int)vPushNrmList.size() ; i++ )
		{
			vPushDir += vPushNrmList[i];
			D3DXVec3Normalize( &vPushDir, &vPushDir );
		}

		if( pShapeWorldMat )
		{
			D3DXVec3TransformNormal( &vPushDir, &vPushDir, pShapeWorldMat );
			D3DXVec3Normalize( &vPushDir, &vPushDir );
		}

		ioOrientBox kMovingBox;
		kMovingBox.SetByAxisBox( kAxisBox );

		int iMoveCount = 0;
		D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );

		while( iMoveCount < nMoveCount )	// 무한루프 방지
		{
			kMovingBox.SetCenter( kMovingBox.GetCenter() + vPushDir );
			vMoveAmt += vPushDir;

			if( !pShape->TestIntersection( pShapeWorldMat, kMovingBox ) )
				break;

			iMoveCount++;
		}

		pEntity->MapCollisionPushingTranslate( vMoveAmt );

		ioBaseChar *pCheckChar = ToBaseChar( pEntity );
		if( pCheckChar )
		{
			if( pCheckChar->HasBuff( BT_WALL_COLLISION ) || pCheckChar->HasBuff( BT_ATTACHED_DUMMY ) )
				pCheckChar->CheckWallCollisionBuff();

			// Grappling Check
			if( pCheckChar->CheckGrapplingPushedCollision() )
			{
				pCheckChar->CheckWoundGrapplingBuff( "" );
			}
			else if( pCheckChar->CheckGrapplingPushedBuffCollision() )
			{
				pCheckChar->CheckWoundGrapplingPushedBuff();
			}
			else if( pCheckChar->CheckGrapplingSkillCollision() )
			{
				pCheckChar->CheckSkillGrapplingCancelBuff( "", true );
			}
			else if( pCheckChar->CheckGrapplingSwingCollision() )
			{
				pCheckChar->CheckSkillGrapplingCancelBuff( "", true );
			}

			// AdhesiveBuff Check
			pCheckChar->CheckAdhesiveStateCol();

			// check axis limit
			D3DXVECTOR3 vPos = pCheckChar->GetWorldPosition();
			if( pCheckChar->IsAxisLimit() )
			{
				pStage->CheckAxisLimit( &vPos );
				pCheckChar->SetWorldPosition( vPos );
			}
		}

		if( pDummyChar )
		{
			pDummyChar->CheckMapCollision( pStage );
		}

		return true;
	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 시작점과 끝점 사이에 맵이 존재 하는지 검사
//

	bool CheckCollisionLine( OUT D3DXVECTOR3& vColPos, IN ioOpcodeShape *pShape, IN const D3DXVECTOR3 &vStart, IN const D3DXVECTOR3 &vEnd, IN const D3DXMATRIX* pShapeWorldMat )
	{
		D3DXVECTOR3 vOrgDiff = vEnd - vStart;
		float fOrgDiffSq = D3DXVec3LengthSq( &vOrgDiff );

		D3DXVECTOR3 vDir;
		D3DXVec3Normalize( &vDir, &vOrgDiff );

		D3DXVECTOR3 vColPoint;
		ioRay kLineRay( vStart, vDir );
		int iFaceIndex = 0;
		if( pShape && pShape->TestIntersection( pShapeWorldMat, kLineRay, &iFaceIndex, &vColPoint ) )
		{
			D3DXVECTOR3 vColDiff = vColPoint - vStart;
			if( D3DXVec3LengthSq( &vColDiff ) <= fOrgDiffSq )
			{	
				vColPos = vColPoint;
				return false;
			}				
		}

		return true;
	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 올라 탈 수 있는 엔티티와 ioSegment 충돌 체크
//

	bool CheckMountAvailableEntityVsSegment( const MountAvailableList& MountList, const ioSegment& kSegment )
	{	
		for( MountAvailableList::const_iterator iter = MountList.begin(); iter != MountList.end() ; ++iter )
		{
			ioGameEntity *pEntity = *iter;
			if( pEntity->IsNowMountEnable() )
			{
				if( ioMath::TestIntersection( kSegment, pEntity->GetWorldCollisionBox() ) )
				{
					return true;
				}
			}
		}

		return false;
	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 맵 CMS 매쉬와 레이를 충돌 체크(충돌 지점과 시작점과의 거리가 fCheckRange 보다 큰지 체크)
//

	bool CheckRayVsMapCollisionByRange( IN const ioRay& kRay, IN ioOpcodeShape *pShape, IN const D3DXMATRIX* pShapeWorldMat, IN float fCheckRange )
	{
		 D3DXVECTOR3 vColPoint;
		if( pShape && pShape->TestIntersection( pShapeWorldMat, kRay, NULL, &vColPoint ) )
		{
			D3DXVECTOR3 vMapDiff = vColPoint - kRay.GetOrigin();
			if( D3DXVec3Length( &vMapDiff ) < fCheckRange )
				return true;
		}

		return false;
	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 맵 CMS 매쉬 관통 체크
//

	bool CheckWeaponPiercingCollision( IN ioWeapon* pWeapon, IN ioOpcodeShape *pShape, IN const D3DXMATRIX* pShapeWorldMat /*= NULL */ )
	{
		if( !pWeapon || !pShape )
			return false;
				
		D3DXVECTOR3 vPrePos = pWeapon->GetPrePosition();
		D3DXVECTOR3 vCurPos = pWeapon->GetPosition();

		if( ioMath::IsEqual( vPrePos, vCurPos ) )
			return false;

		D3DXVECTOR3 vMoveDir = vCurPos - vPrePos;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		D3DXVECTOR3 vColPoint;
		ioRay kMoveRay( vPrePos, vMoveDir );

		if( pShape->TestIntersection( pShapeWorldMat, kMoveRay, NULL, &vColPoint, false ) )
		{
			D3DXVECTOR3 vPosDiff = vCurPos - vPrePos;
			D3DXVECTOR3 vColDiff = vColPoint - vPrePos;

			if( D3DXVec3LengthSq( &vPosDiff ) >= D3DXVec3LengthSq( &vColDiff ) )
			{
				return true;
			}
		}

		return false;
	}
}



	