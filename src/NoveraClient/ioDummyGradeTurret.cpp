
#include "stdafx.h"
#include "ioDummyGradeTurret.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "FindPredicateImpl.h"

#include "ioTraceWeapon.h"
#include "ioZoneEffectWeapon.h"
#include "ioNpcChar.h"

ioDummyGradeTurret::ioDummyGradeTurret( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{	
	m_TurretState			= DMS_START;
	m_GradeState			= GS_FIND_TARGET;
	m_DummyEvent			= DE_NONE;
	m_dwPosSyncStartTime	= 0;

	m_vAttackDir			= D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	m_vTargetDir			= D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	m_bLeftRot				= false;

	m_UpgradeRangeEffectID	= - 1;
}

ioDummyGradeTurret::~ioDummyGradeTurret()
{
	DestroyUnit();
	DestroyRangeEffect( GetOwner() );
}

void ioDummyGradeTurret::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
		
	m_TurretGrade		= rkLoader.LoadInt_e( "turret_grade", 0 );

	m_fFindTargetMinRange		= rkLoader.LoadFloat_e( "turret_find_target_rotate_min_range", 0.0f );
	m_fFindTargetMaxRange		= rkLoader.LoadFloat_e( "turret_find_target_rotate_max_range", FLOAT500 );
	m_dwFindTargetRotateSpeed	= rkLoader.LoadFloat_e( "turret_find_target_rotate_speed", 3000 );

	LoadAttrbuteProperty( rkLoader, "turret_grade_attack" );

	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	m_szExplosionSound = szBuf;

	rkLoader.LoadString_e( "upgrade_range_effect", "", szBuf, MAX_PATH );
	m_szUpgradeRangeEffect = szBuf;

	m_fCheckColskipRange = rkLoader.LoadFloat_e( "check_col_skip_ragne", FLOAT500 );
	m_fCheckSetUpYRange = rkLoader.LoadFloat_e( "setup_y_ragne", FLOAT500 );

	rkLoader.LoadString_e( "weapon_fire_effect", "", szBuf, MAX_PATH );
	m_WeaponFireEffect = szBuf;
	rkLoader.LoadString_e( "weapon_fire_sound", "", szBuf, MAX_PATH );
	m_WeaponFireSound = szBuf;
}

void ioDummyGradeTurret::LoadAttrbuteProperty( ioINILoader &rkLoader, const char* szTitle )
{
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_max_count", szTitle );
	int iAttributeMax = rkLoader.LoadInt( szKey, 0 );

	for( int i = 0; i < iAttributeMax; ++i )
	{
		GradeAttackAttribute Attribute;

		wsprintf_e( szKey, "%s%d_fire_weapon_max", szTitle, i + 1 );
		int iMax = rkLoader.LoadInt( szKey, 0 );
		for( int j = 0; j < iMax; ++j )
		{
			AttackInfo Info;
			wsprintf_e( szKey, "%s%d_fire_weapon%d", szTitle, i + 1, j + 1 );
			Info.m_dwWeaponIndex = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "%s%d_fire_weapon%d_fire_time", szTitle, i + 1, j + 1 );
			Info.m_dwFireTime = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "%s%d_fire_weapon%d_offset_x", szTitle, i + 1, j + 1 );
			Info.m_vOffset.x = rkLoader.LoadFloat( szKey, 0.0f );

			wsprintf_e( szKey, "%s%d_fire_weapon%d_offset_y", szTitle, i + 1, j + 1 );
			Info.m_vOffset.y = rkLoader.LoadFloat( szKey, 0.0f );

			wsprintf_e( szKey, "%s%d_fire_weapon%d_offset_z", szTitle, i + 1, j + 1 );
			Info.m_vOffset.z = rkLoader.LoadFloat( szKey, 0.0f );

			Attribute.m_AttackInfoList.push_back( Info );
		}

		wsprintf_e( szKey, "%s%d_fire_gap_time", szTitle, i + 1 );
		Attribute.m_dwGapTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s%d_min_range", szTitle, i + 1 );
		Attribute.m_fMinRange = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s%d_max_range", szTitle, i + 1 );
		Attribute.m_fMaxRange = rkLoader.LoadFloat( szKey, FLOAT500 );

		wsprintf_e( szKey, "%s%d_min_height", szTitle, i + 1 );
		Attribute.m_fHeightMin = rkLoader.LoadFloat( szKey, FLOAT500 );

		m_GradeAttackAttributeList.push_back( Attribute );
	}
}

void ioDummyGradeTurret::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	DestroyUnit();
	DestroyRangeEffect( GetOwner() );

	if ( !m_szExplosionSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szExplosionSound );
	}
	if( !m_WeaponFireEffect.IsEmpty() )
	{
		m_pGroup->EndEffect( m_WeaponFireEffect, true );
	}

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

void ioDummyGradeTurret::CreateUnit()
{
	DestroyUnit();

	for( int i = 0; i < m_TurretGrade; ++i )
	{
		if( COMPARE( i, 0, (int)m_GradeAttackAttributeList.size() ) )
		m_DummyGradeTurretUnitList.push_back( new ioDummyGradeTurretUnit( m_GradeAttackAttributeList[i], i ) );
	}
}

ioDummyGradeTurretUnit* ioDummyGradeTurret::GetUnit( int iIndex )
{
	if( COMPARE( iIndex, 0, (int)m_DummyGradeTurretUnitList.size() ) )
	{
		return m_DummyGradeTurretUnitList[iIndex];
	}
	
	return NULL;
}

void ioDummyGradeTurret::DestroyUnit()
{
	for( DummyGradeTurretUnitList::iterator iter = m_DummyGradeTurretUnitList.begin(); iter != m_DummyGradeTurretUnitList.end(); ++iter )
	{
		ioDummyGradeTurretUnit* pUnit = *iter;
		SAFEDELETE( pUnit );
	}
	m_DummyGradeTurretUnitList.clear();
}

void ioDummyGradeTurret::CreateRangeEffect( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DestroyRangeEffect( pOwner );
	
	D3DXVECTOR3 vPos = GetWorldPosition();
	float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if( 0.0f < fHeight )
		vPos.y = fHeight + 5.0f;

	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szUpgradeRangeEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pMapEffect )
	{
		m_UpgradeRangeEffectID = pMapEffect->GetUniqueID();
	}
}

void ioDummyGradeTurret::UpdateRangeEffect( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioMapEffect* pEffect = pStage->FindMapEffect( m_UpgradeRangeEffectID );
	if( pEffect )
		pEffect->SetWorldPosition( GetWorldPosition() );

}

void ioDummyGradeTurret::DestroyRangeEffect( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_UpgradeRangeEffectID != -1 )
	{
		ioMapEffect* pEffect = pStage->FindMapEffect( m_UpgradeRangeEffectID );
		if( pEffect )
			pEffect->EndEffectGrace();

		m_UpgradeRangeEffectID = -1;
	}
}

void ioDummyGradeTurret::SetReserveUpgradeInfo( const D3DXVECTOR3& vPos, const D3DXVECTOR3& vAttackDir, const D3DXQUATERNION& qtRot )
{
	m_vUpgradePos		= vPos;
	m_vUpgradeAttackDir	= vAttackDir;
	m_qtUpgradeRot		= qtRot;
	m_DummyEvent		= DE_UPGRADE;
}

void ioDummyGradeTurret::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	if( !GetOwner() )
	{
		SetDieState( true );
		return;
	}

	switch( m_TurretState )
	{
	case DMS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DMS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DMS_END:
		SendDieState( true );
		break;
	}

	UpdateRangeEffect( GetOwner() );
	ProcessCheckLifeTime( fTimePerSec );
	if( ProcessMoveDown( GetOwner(), fTimePerSec ) )
	{		
		m_TurretState = DMS_END;
	}
}

void ioDummyGradeTurret::ProcessCheckLifeTime( float fTimePerSec )
{
	if( !GetOwner() || !GetOwner()->IsNeedProcess() )
		return;

	if( 0 < m_dwLifeTime && m_dwStartTime + m_dwLifeTime < FRAMEGETTIME() && m_State != DCS_DIE )
	{
		m_TurretState = DMS_END;
	}
}

bool ioDummyGradeTurret::ProcessMoveDown( ioBaseChar* pOwner, float fTimePerSec )
{
	if( !pOwner )
		return true;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return true;

	D3DXVECTOR3	vPos = GetWorldPosition();
	vPos.y -= pStage->GetRoomGravity() * fTimePerSec;

	float fHeight = pStage->GetMapHeight( vPos.x, vPos.z, this );
	if( vPos.y < fHeight )
		vPos.y = fHeight;
	
	if( fHeight <= 0.0f && pOwner->IsNeedProcess() )
	{
		return true;
	}

	SetWorldPosition( vPos );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwPosSyncStartTime + POS_SYNC_TIME < dwCurTime )
	{
		m_dwPosSyncStartTime = dwCurTime;
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket	<< (int)DUMMY_CHAR_TURRET_SYNC;
			kPacket << (int)SS_POS;
			kPacket << vPos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return false;
}

void ioDummyGradeTurret::ProcessStartState( float fTimePerSec )
{
	if( !GetOwner() )
	{
		SetDieState( true );
		return;
	}

	if( m_fCheckSetUpYRange < GetWorldPosition().y - GetOwner()->GetWorldPosition().y )
	{
		SendDieState( true );	// 2020-07-17
		return;
	}

	if( m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		m_TurretState	= DMS_DELAY;
		m_GradeState	= GS_FIND_TARGET;
		m_vTargetDir	= D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		m_bLeftRot		= false;

		CreateUnit();
		CreateRangeEffect( GetOwner() );

		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );
			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/m_fDelayAniRate );
		}

		switch(m_DummyEvent)
		{
		case DE_MID_SYNC:
			{
				m_GradeState	= m_MidTurretSync.eState;
				m_vTargetDir	= m_MidTurretSync.vTargetDir;
				m_vAttackDir	= m_MidTurretSync.vAttackDir;
				m_bLeftRot		= m_MidTurretSync.bLeftRot;
				m_TargetName	= m_MidTurretSync.szTargetName;
			}
			break;
		case DE_UPGRADE:
			{
				m_vTargetDir	= m_vUpgradeAttackDir;
				m_vAttackDir	= m_vUpgradeAttackDir;

				SetWorldPosition( m_vUpgradePos );
				SetWorldOrientation( m_qtUpgradeRot );
			}
			break;
		default:
			{
				m_vTargetDir	= m_vAttackDir;
			}
			break;
		}
	}
	else if( m_dwStartAniEndTime == 0 )
	{
		SetDieState( true );
	}
}

void ioDummyGradeTurret::ProcessDelayState( float fTimePerSec )
{
	switch( m_GradeState )
	{
	case GS_FIND_TARGET:
		ProcessFindTarget( GetOwner() );
		break;
	case GS_ROTATE:
		ProcessRotate( m_dwFindTargetRotateSpeed );
		break;
	}

	for( DummyGradeTurretUnitList::iterator iter = m_DummyGradeTurretUnitList.begin(); iter != m_DummyGradeTurretUnitList.end(); ++iter )
	{
		ioDummyGradeTurretUnit* pUnit = *iter;
		if( pUnit )
		{
			pUnit->ProcessState( GetOwner(), this );
		}
	}
}

ioBaseChar* ioDummyGradeTurret::FindTarget( ioBaseChar *pOwner, float fMinDist, float fLimitDist, float fMinHeight, bool bDownAttack )
{	
	if( !pOwner )
		return NULL;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return NULL;

	const BaseCharList& BaseList = pStage->GetBaseCharList();
	for( BaseCharList::const_iterator iter = BaseList.begin() ; iter!= BaseList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( FindPredicateUtil::CheckTargetState( pTarget, bDownAttack )  && pTarget != pOwner && pTarget->GetTeam() != pOwner->GetTeam() )
		{
			D3DXVECTOR3 vDiff	= pTarget->GetWorldPosition() - GetWorldPosition();
			float fCurDistSq	= D3DXVec3LengthSq( &vDiff );
			float fHeightGap	= pTarget->GetWorldPosition().y - GetWorldPosition().y;

			if( 0.0f < fMinHeight && fHeightGap < fMinHeight )
				continue;

			if( fLimitDist*fLimitDist >= fCurDistSq && fMinDist*fMinDist <= fCurDistSq )
			{
				return pTarget;
			}
		}
	}

	return NULL;
}

void ioDummyGradeTurret::ProcessFindTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioBaseChar *pTarget = FindTarget( pOwner, m_fFindTargetMinRange, m_fFindTargetMaxRange, 0, true );
	if( pTarget )
	{
		D3DXVECTOR3 vTarget = pTarget->GetWorldPosition() - GetWorldPosition();
		D3DXVec3Normalize( &vTarget, &vTarget );
		vTarget.y		= 0.0f;
		
		m_GradeState	= GS_ROTATE;
		m_vTargetDir	= vTarget;
		m_bLeftRot		= IsLeftRotate();
		m_TargetName	= pTarget->GetCharName();
	}
}

bool ioDummyGradeTurret::IsLeftRotate()
{	
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vTargetDir );
	float fYawR = ioMath::QuaterToYaw( qtRot );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir	= qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vDir		= m_vAttackDir;
	D3DXQUATERNION qtCurRot = GetWorldOrientation();

	if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
	{
		float fCurYaw, fTargetYaw;
		fCurYaw		= RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
		fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

		fCurYaw		= ioMath::ArrangeHead( fCurYaw );
		fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );

		float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
		if( fRotAngle < 0.0f )
			return true;
	}

	return false;
}

void ioDummyGradeTurret::ProcessRotate( DWORD dwRotateTime )
{
	if( dwRotateTime <= 0 )
		return;

	float fFrameGap		= g_FrameTimer.GetCurFrameGap();
	float fRate			= fFrameGap / dwRotateTime;
	float fRotateAngle	= 360.0f * fRate;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vTargetDir );	
	float fYawR = ioMath::QuaterToYaw( qtRot );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	float fCurYaw		= RADtoDEG( ioMath::QuaterToYaw( GetWorldOrientation() ) );
	float fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw		= ioMath::ArrangeHead( fCurYaw );
	fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_GradeState = GS_FIND_TARGET;
	}
	else
	{
		if( m_bLeftRot )
			fRotateAngle *= -1;

		fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
		m_vAttackDir = qtAngle * m_vAttackDir; 
		D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
		ioMath::CalcDirectionQuaternion( qtAngle, -m_vAttackDir );
		SetWorldOrientation( qtAngle );
	}
}

bool ioDummyGradeTurret::IsActivateWeapon()
{
	if( m_TurretState == DMS_START )
		return false;

	return true;
}

void ioDummyGradeTurret::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage )
		return;	

	int eType;
	rkPacket >> eType;

	if( eType == DUMMY_CHAR_TURRET_SYNC )
	{
		int iSyncType;
		rkPacket >> iSyncType;
		switch( iSyncType )
		{
		case SS_UNIT:
			{
				int iIndex;
				rkPacket >> iIndex;

				ioDummyGradeTurretUnit* pUnit = GetUnit( iIndex );
				if( pUnit )
				{
					pUnit->ApplyDummyCharSync( this, rkPacket );
				}
			}
			break;
		case SS_POS:
			{
				D3DXVECTOR3 vPos;
				rkPacket >> vPos;
				SetWorldPosition( vPos );
			}
			break;
		}
	}
	else
	{
		ApplyDummyCharSyncByType( eType, rkPacket, pStage );
	}
}

void ioDummyGradeTurret::FillDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket )
{
	DWORD dwCurTime = FRAMEGETTIME();

	rkPacket << (int)m_GradeState;

	rkPacket << m_vTargetDir;
	rkPacket << m_vAttackDir;
	rkPacket << m_bLeftRot;
	rkPacket << m_TargetName;
}

void ioDummyGradeTurret::ApplyDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket )
{
	m_DummyEvent = DE_MID_SYNC;

	int eState;
	rkPacket >> eState;
	m_MidTurretSync.eState = (GradeState)eState;

	rkPacket >> m_MidTurretSync.vTargetDir;
	rkPacket >> m_MidTurretSync.vAttackDir;
	rkPacket >> m_MidTurretSync.bLeftRot;
	rkPacket >> m_MidTurretSync.szTargetName;
	
}

bool ioDummyGradeTurret::IsMountingAvailable() const
{
	return m_bMountEnable;
}

bool ioDummyGradeTurret::IsMountAvailable() const
{
	return m_bMountEnable;
}

bool ioDummyGradeTurret::IsNowMountEnable() const
{
	if( m_bCurMountEnable && m_bMountEnable )
		return true;

	return false;
}

int ioDummyGradeTurret::DontMoveEntityLevel() const
{
	return 1;
}

void ioDummyGradeTurret::NotifyDontMoveCollisionPushingSkip( ioGameEntity* pColEntity )
{
	ioNpcChar* pNpc = ToNpcChar( ToBaseChar( pColEntity ) );
	if( pNpc && COMPARE( pNpc->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC ) )
	{
		m_TurretState = DMS_END;
	}
}

bool ioDummyGradeTurret::IsDummyVsDummyCollisionSkipState( const ioDummyChar *pDummy )
{
	return ioDummyChar::IsDummyVsDummyCollisionSkipState( pDummy );
}

bool ioDummyGradeTurret::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	ioNpcChar* pNpc = ToNpcChar( ToBaseChar( pColEntity ) );
	if( pNpc && COMPARE( pNpc->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC ) )
	{
		D3DXVECTOR3 vDiff = GetWorldPosition() - pNpc->GetWorldPosition();		
		if( D3DXVec3LengthSq( &vDiff ) < m_fCheckColskipRange * m_fCheckColskipRange && ioMath::TestIntersection( GetWorldAxisBox(), pNpc->GetWorldAxisBox() ) )
			return true;
	}

	return false;
}

void ioDummyGradeTurret::PlayFireEffectNSound()
{
	if( !m_WeaponFireEffect.IsEmpty() )
		m_pGroup->AttachEffect( m_WeaponFireEffect, NULL );

	if( !m_WeaponFireSound.IsEmpty() )
		PlaySound( GetOwner(), m_WeaponFireSound, false );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDummyGradeTurretUnit::ioDummyGradeTurretUnit()
{
	Init();
}

ioDummyGradeTurretUnit::ioDummyGradeTurretUnit( GradeAttackAttribute Attribute, int iUnitIndex )
	: m_GradeAttackAttribute( Attribute ), m_iUnitIndex( iUnitIndex )
{
	Init();
}

ioDummyGradeTurretUnit::~ioDummyGradeTurretUnit()
{
}

void ioDummyGradeTurretUnit::Init()
{
	m_UnitState			= US_FIND_TARGET;
	m_AttackDir			= ioMath::VEC3_ZERO;
	m_dwAttackStartTime	= 0;
	m_dwBaseWeaponIndex	= 0;

	m_TargetName.Clear();
}

void ioDummyGradeTurretUnit::ProcessState( ioBaseChar *pOwner, ioDummyGradeTurret* pTurret )
{
	switch( m_UnitState )
	{
	case US_FIND_TARGET:
		{
			ProcessFindTarget( pOwner, pTurret );
		}
		break;
	case US_ATTACK:
		{
			ProcessAttackState( pOwner, pTurret );
		}
		break;
	}
}

void ioDummyGradeTurretUnit::ProcessFindTarget( ioBaseChar *pOwner, ioDummyGradeTurret* pTurret )
{
	if( !pTurret )
		return;

	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioBaseChar *pTarget = pTurret->FindTarget( pOwner, m_GradeAttackAttribute.m_fMinRange, m_GradeAttackAttribute.m_fMaxRange, m_GradeAttackAttribute.m_fHeightMin, false );
	if( pTarget )
	{
		D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pTurret->GetWorldPosition();
		D3DXVec3Normalize( &vDir, &vDir );

		if( 0.0f < m_GradeAttackAttribute.m_fHeightMin )
		{
			m_AttackDir = vDir;
		}
		else
		{
			vDir.y = 0;
			m_AttackDir = vDir;
		}

		m_TargetName		= pTarget->GetCharName();
		m_UnitState			= US_ATTACK;
		m_dwBaseWeaponIndex	= pOwner->IncreaseWeaponIndexBase();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << pTurret->GetOwnerName();
			kPacket << pTurret->GetDummyCharIdx();
			kPacket	<< (int)ioDummyChar::DUMMY_CHAR_TURRET_SYNC;
			kPacket << (int)ioDummyGradeTurret::SS_UNIT;
			kPacket << (int)m_iUnitIndex;
			kPacket << (int)SUS_FIND_TARGET;
			kPacket << m_TargetName;
			kPacket << m_AttackDir;
			kPacket << m_dwBaseWeaponIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		Init();
	}
}

void ioDummyGradeTurretUnit::ProcessAttackState( ioBaseChar *pOwner, ioDummyGradeTurret* pTurret )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwAttackStartTime == 0 )
	{
		m_dwAttackStartTime = dwCurTime;

		m_CurrAttackList.clear();
		for( AttackInfoList::const_iterator iter = m_GradeAttackAttribute.m_AttackInfoList.begin(); iter != m_GradeAttackAttribute.m_AttackInfoList.end(); ++iter )
		{
			AttackInfo Info = *iter;

			//발사 타임 조정
			Info.m_dwFireTime += dwCurTime;
			m_CurrAttackList.push_back( Info );
		}
	}
	else if( m_dwAttackStartTime + m_GradeAttackAttribute.m_dwGapTime < dwCurTime )
	{
		//다시 타겟팅으로
		Init();
	}
	else
	{
		ProcessAttackFireList( pOwner, pTurret );
	}
}

void ioDummyGradeTurretUnit::ProcessAttackFireList( ioBaseChar *pOwner, ioDummyGradeTurret* pTurret )
{
	int iWeaponBaseIndex = 0;
	for( AttackInfoList::const_iterator iter = m_CurrAttackList.begin(); iter != m_CurrAttackList.end(); )
	{
		const AttackInfo& Info = *iter;
		if( Info.m_dwFireTime < FRAMEGETTIME() )
		{
			D3DXVECTOR3 vDir = pTurret->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSide;
			D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );
			D3DXVECTOR3 vPos = pTurret->GetWorldPosition();
			vPos	+= vDir * Info.m_vOffset.z;
			vPos.y	+= Info.m_vOffset.y;
			vPos	+= vSide * Info.m_vOffset.x;

			ioWeapon* pWeapon = AttackFire( pOwner, Info.m_dwWeaponIndex, vPos, m_AttackDir, m_dwBaseWeaponIndex + iWeaponBaseIndex++ );

			pTurret->PlayFireEffectNSound();

			ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
			if( pZone )
			{
				pZone->SetCollisionDir( m_AttackDir );
				pZone->SetZoneValue( true, false );
			}

			iter = m_CurrAttackList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

ioWeapon* ioDummyGradeTurretUnit::AttackFire( ioBaseChar *pOwner, DWORD dwAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, DWORD dwWeaponIndex )
{
	if ( !pOwner )
		return NULL;	

	FireTime kFireTime;
	kFireTime.dwStart				= FRAMEGETTIME();
	kFireTime.iAttributeIdx			= dwAttackIndex;
	kFireTime.iResistanceIdx		= 0;
	kFireTime.szWoundedAni			= "";
	kFireTime.dwWoundedAniDuration	= 0;
	kFireTime.bLoopWoundedAni		= false;
	kFireTime.dwWeaponIdx			= dwWeaponIndex;
	kFireTime.eFireTimeType			= FTT_DUMMY_CHAR_ATTACK;

	return pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
}

void ioDummyGradeTurretUnit::ApplyDummyCharSync( ioDummyGradeTurret* pTurret, SP2Packet &rkPacket )
{
	int eState;
	rkPacket >> eState;

	switch( eState )
	{
	case SUS_FIND_TARGET:
		{
			rkPacket >> m_TargetName;
			rkPacket >> m_AttackDir;
			rkPacket >> m_dwBaseWeaponIndex;
			m_UnitState	= US_ATTACK;

			if( pTurret && pTurret->GetOwner() )
				pTurret->GetOwner()->SetWeaponIndexBase( m_dwBaseWeaponIndex );
		}
		break;
	}
}