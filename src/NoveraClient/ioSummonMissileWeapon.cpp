

#include "stdafx.h"

#include "ioSummonMissileWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ItemDefine.h"

ActionStopType ioSummonMissileWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioSummonMissileWeapon::m_DieActionStopType = AST_NONE;

ioSummonMissileWeapon::ioSummonMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pIceMesh = NULL;
	m_IceState = IS_MOVE;

	m_bFirst = true;
	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_dwCurRotateTime = 0;
}

ioSummonMissileWeapon::~ioSummonMissileWeapon()
{
	if( m_pIceMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioSummonMissileWeapon::~ioSummonMissileWeapon - UnReleased Resouce" );
	}
}

bool ioSummonMissileWeapon::InitResource()
{
	return true;
}

void ioSummonMissileWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedTeamEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueTeamEffect = szBuf;

	m_OwnerRotate = (OwnerRotType)rkLoader.LoadInt_e( "owner_rotate", ORT_NONE );

	m_dwMinRotateTime = (DWORD)rkLoader.LoadInt_e( "min_rotate_speed", 0 );
	m_dwMaxRotateTime = (DWORD)rkLoader.LoadInt_e( "max_rotate_speed", 0 );

	m_fMinMoveSpeed = rkLoader.LoadFloat_e( "min_move_speed", 0.0f );
	m_fMaxMoveSpeed = rkLoader.LoadFloat_e( "max_move_speed", 0.0f );

	m_fRotateStartRange = rkLoader.LoadFloat_e( "rotate_start_range", 0.0f );
	m_fRotateEndRange = rkLoader.LoadFloat_e( "rotate_end_range", 0.0f );

	m_bEndExplosion = rkLoader.LoadBool_e( "end_explosion", false );
	m_fEnableMoveGap = rkLoader.LoadFloat_e( "enable_move_gap_height", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt > 0 )
	{
		m_ChangeDirInfoList.clear();
		m_ChangeDirInfoList.reserve( iCnt );

		for( int i=0; i < iCnt; ++i )
		{
			ChangeDirInfo kInfo;

			wsprintf_e( szBuf, "change_range%d", i+1 );
			kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_speed%d", i+1 );
			kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_angle_h%d", i+1 );
			kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_angle_v%d", i+1 );
			kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

			m_ChangeDirInfoList.push_back( kInfo );
		}

		std::sort( m_ChangeDirInfoList.begin(), m_ChangeDirInfoList.end(), ChangeDirSort() );
	}
}

void ioSummonMissileWeapon::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;

	// Set Seed
	m_Random.SetRandomSeed( m_dwWeaponIdx );
}

void ioSummonMissileWeapon::SetTeam( ioPlayStage *pStage, TeamType eType )
{
	if( !pStage ) return;

	ioWeapon::SetTeam( pStage, eType );

	DestroyResource();

	ioHashString szCurWeaponEffect;

	if( eType == TEAM_BLUE )
	{
		szCurWeaponEffect = m_BlueTeamEffect;
	}
	else if( eType == TEAM_RED )
	{
		szCurWeaponEffect = m_RedTeamEffect;
	}
	else //개인전
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		{
			szCurWeaponEffect = m_BlueTeamEffect;
		}
		else
		{
			szCurWeaponEffect = m_RedTeamEffect;
		}
	}

	if( !SetIceMesh( szCurWeaponEffect ) )
		DestroyResource();
}

bool ioSummonMissileWeapon::SetIceMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pIceMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pIceMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pIceMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pIceMesh->GetSkeletonName().IsEmpty() && m_pIceMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();
			SetPosition( vLinkPosOffset );
		}
	}

	m_pIceMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pIceMesh->GetCollisionBox();

	m_vPrePos = GetPosition();

	m_fDefaultGapHeight = fabs( m_vPrePos.y - pOwner->GetWorldPosition().y );

	AddEffectEvent( m_pIceMesh );
	return true;
}

void ioSummonMissileWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioSummonMissileWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioSummonMissileWeapon::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() ) return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}

void ioSummonMissileWeapon::RotateMoveDir( float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	m_vMoveDir = qtRot * m_vMoveDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	m_vMoveDir = qtRot * m_vMoveDir;

	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioSummonMissileWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pIceMesh && m_pIceMesh->GetSkeletonName().IsEmpty() && m_pIceMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );

	m_fDefaultGapHeight = 0.0f;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_fDefaultGapHeight = fabs( vPos.y - pOwner->GetWorldPosition().y );
	}
}

void ioSummonMissileWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	switch( m_IceState )
	{
	case IS_MOVE:
		OnFloating( fTimePerSec, pStage );
		break;
	case IS_EXPLOSION_WAIT:
		Explosion( pStage );
		break;
	case IS_EXPLOSION:
		{
			CheckCreateWeaponList();
			SetWeaponDead();
		}
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioSummonMissileWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_IceState != IS_MOVE )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->IsNeedProcess() )
			SetExplosion( pStage, true );

		return true;
	}

	return false;
}

void ioSummonMissileWeapon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		CheckChangeDir();

		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			switch( m_OwnerRotate )
			{
			case ORT_NORMAL:
				CheckOwnerRotateDir( fTimePerSec );
				break;
			case ORT_STAFF3:
				{
					ioMagicStaffItem3 *pStaff3 = ToMagicStaffItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
					if( pStaff3 && pStaff3->GetCurFireState() == ioMagicStaffItem3::FS_GATHERING )
					{
						CheckOwnerRotateDir( fTimePerSec );
					}
				}
				break;
			}			
		}

		float fCurSpeed = GetMoveSpeed();
		float fMoveDist = fCurSpeed * fTimePerSec;

		m_vPrePos = GetPosition();
		D3DXVECTOR3 vNewPos = m_vPrePos + (m_vMoveDir * fMoveDist);

		float fHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z, vNewPos.y+m_pIceMesh->GetBoundBox().GetExtents(1) );
		if( fabs( m_vPrePos.y - fHeight ) < m_fEnableMoveGap )
		{
			vNewPos.y = fHeight + m_fDefaultGapHeight;
		}

		SetPosition( vNewPos );
		m_fCurMoveRange += fMoveDist;

		float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
		if( m_fCurMoveRange >= fMaxRange )
		{
			if( m_bEndExplosion )
			{
				Explosion( pStage );
				return;
			}
			else
			{
				SetWeaponDead();
			}
		}
	}

	m_bFirst = false;

	if( m_pIceMesh )
	{
		m_pIceMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioSummonMissileWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pIceMesh )
	{
		g_EffectFactory.DestroyEffect( m_pIceMesh );
		m_pIceMesh = NULL;
	}
}

const ioOrientBox& ioSummonMissileWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioSummonMissileWeapon::GetType() const
{
	return WT_SUMMON_MISSILE;
}

ActionStopType ioSummonMissileWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioSummonMissileWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioSummonMissileWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			DWORD dwGapValue = (DWORD)(m_fMaxMoveSpeed - m_fMinMoveSpeed);
			DWORD dwValue = m_Random.Random( dwGapValue );

			m_fCurMoveSpeed = m_fMinMoveSpeed + dwValue;
		}
	}

	return m_fCurMoveSpeed;
}

DWORD ioSummonMissileWeapon::GetRotateSpeed()
{
	if( m_dwCurRotateTime == 0 )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			DWORD dwGapValue = (DWORD)(m_dwMaxRotateTime - m_dwMinRotateTime);
			DWORD dwValue = m_Random.Random( dwGapValue );

			m_dwCurRotateTime = m_dwMinRotateTime + dwValue;
		}
	}

	return m_dwCurRotateTime;
}

float ioSummonMissileWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioSummonMissileWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();
	m_WorldAttackBoxForMap = m_WorldAttackBox;

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		if( m_fOwnerChargeScaleRate > FLOAT1 )
		{
			D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
			m_WorldAttackBox.Transform( GetPosition(), vScale, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, GetOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		}
	}
}

void ioSummonMissileWeapon::SetExplosion( ioPlayStage *pStage, bool bSendNet )
{
	Explosion( pStage );

	if( bSendNet )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioSummonMissileWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	SetExplosionWait();
}

void ioSummonMissileWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_IceState == IS_EXPLOSION )
		return;

	m_IceState = IS_EXPLOSION;

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos =  GetPosition();
		ioMapEffect *pIceBoom = pStage->CreateMapEffect( m_BoomEffect, vPos, ioMath::UNIT_ALL );
		if( pIceBoom && pIceBoom->GetEffect() )
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
			pIceBoom->SetWorldOrientation( qtRot );

			pIceBoom->GetEffect()->UpdateBounds( true );
			m_LocalAttackBox = pIceBoom->GetEffect()->GetCollisionBox();

			UpdateWorldAttackBox( 0.0f, false );
		}

		ioWeapon::PlayExplosionSound();
	}
}

bool ioSummonMissileWeapon::IsFloating() const
{
	if( m_IceState == IS_MOVE )
		return true;

	return false;
}

bool ioSummonMissileWeapon::IsExplosion() const
{
	if( m_IceState == IS_EXPLOSION )
		return true;

	return false;
}

void ioSummonMissileWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_IceState != IS_MOVE )
		return;

	ioBaseChar *pChar = ToBaseChar(pTarget);
	if( pChar && !pChar->IsNeedProcess() )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
		{
			bCollision = true;
		}
		else if( GetTeam() != pTarget->GetTeam() )
		{
			bCollision = true;
		}
		else if( GetOwner() != pTarget )
		{
			ioBaseChar *pChar = ToBaseChar(pTarget);
			if( pChar )
			{
				if( pChar->IsEnableTeamCollision( this ) )
				{
					bCollision = true;
				}
			}
		}
	}

	if( !bCollision )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) || ToDummyChar(pTarget) || ToWallZone(pTarget) )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->IsNeedProcess() )
			SetExplosion( pStage, true );

		return;
	}
	else if( GetPiercingType() == CPT_NONE )
	{
		ioBaseChar *pChar = ToBaseChar(pTarget);
		if( pChar )
		{
			SetExplosion( pStage, true );
		}

		return;
	}

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

bool ioSummonMissileWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_IceState != IS_EXPLOSION )
		return false;

	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}

DWORD ioSummonMissileWeapon::GetCollisionEnableTime() const
{
	if( m_IceState == IS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioSummonMissileWeapon::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

bool ioSummonMissileWeapon::CheckCollisionLine( ioPlayStage *pStage )
{
	if( !pStage ) return false;

	D3DXVECTOR3 vPos = GetPosition();

	if( m_vPrePos == vPos )
		return false;

	bool bMapCollision = pStage->CheckCollisionLine( m_vPrePos, vPos, true );

	if( !bMapCollision )
	{
		Explosion( pStage );
		return true;
	}

	return false;
}

void ioSummonMissileWeapon::CheckOwnerRotateDir( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_dwCurRotateTime = GetRotateSpeed();

	if( m_dwCurRotateTime == 0 ) return;
	if( !COMPARE( m_fCurMoveRange, m_fRotateStartRange, m_fRotateEndRange ) )
		return;

	D3DXQUATERNION	qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vTargetDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fAngle = D3DXVec3Dot( &vTargetDir, &m_vMoveDir );
	if( fAngle > 0.98f )
	{
		SetOrientation( qtRot );
		m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}
	else
	{
		float fAngleGap = RADtoDEG( acosf(fAngle) );
		float fRate = (fTimePerSec*1000) / m_dwCurRotateTime;
		float fRotateAngle = 360.0f * fRate;

		if( fAngleGap < fRotateAngle )
		{
			SetOrientation( qtRot );
			m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
		else
		{
			D3DXQUATERNION qtRotate;
			ioMath::CalcDirectionQuaternion( qtRotate, -m_vMoveDir );

			bool bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &m_vMoveDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					bLeftRot = true;
			}

			if( bLeftRot )
				fRotateAngle *= -1;

			fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

			D3DXQUATERNION	qtNewRot;
			D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
			m_vMoveDir = qtNewRot * m_vMoveDir;

			ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );
			SetOrientation( qtNewRot );
		}
	}
}

void ioSummonMissileWeapon::SetExplosionWait()
{
	if( m_IceState != IS_MOVE )
		return;

	m_IceState = IS_EXPLOSION_WAIT;
}

