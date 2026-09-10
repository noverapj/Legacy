

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioUroborusGuideArrow.h"

#include "ioGuide2Weapon.h"

#include "SkillDefine.h"

ActionStopType ioGuide2Weapon::m_ActionStopType = AST_NONE;
ActionStopType ioGuide2Weapon::m_DieActionStopType = AST_NONE;

ioGuide2Weapon::ioGuide2Weapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	//SetPeculiarity( WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;
	m_WeaponState = WS_MOVE;

	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_pGuideArrow = NULL;
}

ioGuide2Weapon::~ioGuide2Weapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioGuide2Weapon::~ioGuide2Weapon - UnReleased Resouce" );
	}

	SAFEDELETE( m_pGuideArrow );
}

bool ioGuide2Weapon::InitResource()
{
	return true;
}

void ioGuide2Weapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	// Tracing
	m_dwTraceEnableTime = rkLoader.LoadInt_e( "trace_enable_time", 0 );

	m_fTraceSpeed = rkLoader.LoadFloat_e( "trace_speed", 0.0f );
	m_fTraceRange = rkLoader.LoadFloat_e( "trace_range", 0.0f );
	m_fTraceRotSpeed = rkLoader.LoadFloat_e( "trace_rotate_speed", 0.0f );
	m_dwTraceDuration = (DWORD)rkLoader.LoadInt_e( "trace_duration", 0 );
	m_fTraceEndRange = rkLoader.LoadFloat_e( "trace_end_range", 0.0f );

	m_fTargetTracePosRate = rkLoader.LoadFloat_e( "target_trace_pos_rate", FLOAT05 );
	m_fDownTargetTracePosRate = rkLoader.LoadFloat_e( "down_target_trace_pos_rate", 0.0f );

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedTeamEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueTeamEffect = szBuf;

	//
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioPlayStage *pStage = pOwner->GetCreator();

		SAFEDELETE( m_pGuideArrow );
		m_pGuideArrow = new ioUroborusGuideArrow( pStage );

		m_pGuideArrow->Initialize( rkLoader );
		m_pGuideArrow->SetMaxArrowRange( GetMaxRange() );
	}

	m_bAttachTarget = rkLoader.LoadBool_e( "attach_target", false );
}

void ioGuide2Weapon::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;
}

void ioGuide2Weapon::SetTeam( ioPlayStage *pStage, TeamType eType )
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

	if( !SetFireMesh( szCurWeaponEffect ) )
		DestroyResource();
}

bool ioGuide2Weapon::SetFireMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pFireMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pFireMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pFireMesh->GetCollisionBox();

	AddEffectEvent( m_pFireMesh );
	return true;
}

void ioGuide2Weapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioGuide2Weapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioGuide2Weapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioGuide2Weapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	switch( m_WeaponState )
	{
	case WS_MOVE:
		OnFloating( fTimePerSec, pStage );
		break;
	case WS_TRACING:
		OnTracing( fTimePerSec, pStage );
		break;
	case WS_AFTER_COLLISION:
		OnAfterCollision( fTimePerSec, pStage );
		break;
	case WS_END:
		if( !m_pFireMesh || !m_pFireMesh->IsLive() )
		{
			CheckCreateWeaponList();
			DestroyResource( pStage );
			SetWeaponDead();
			return;
		}
		break;
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioGuide2Weapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	// 
	if( m_WeaponState != WS_MOVE && m_WeaponState != WS_TRACING && m_WeaponState != WS_AFTER_COLLISION )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			pStage->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox(true).GetCenter(), GetScale() );

			PlayWoundedSound( NULL );

			if( pOwner->IsNeedProcess() )
			{
				SetAfterCollisionState( false );
			}

			if( m_bAttachTarget )
				SetWeaponDead();
		}

		return true;
	}

	return false;
}

void ioGuide2Weapon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckOwnerState() )
	{
		SetEndState( false );
		return;
	}

	if( m_dwCreatedTime + m_dwTraceEnableTime < FRAMEGETTIME() )
	{
		if( !m_TargetName.IsEmpty() )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
			SetTraceState( pStage );
			return;
		}
	}

	ioBaseChar *pOwner = GetOwner();

	float fCurSpeed = GetMoveSpeed();
	float fMoveDist = fCurSpeed * fTimePerSec;

	m_vPrePos = GetPosition();
	D3DXVECTOR3 vNewPos = m_vPrePos + (m_vMoveDir * fMoveDist);
	m_fCurMoveRange += fMoveDist;

	float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
	if( m_fCurMoveRange < fMaxRange )
	{
		SetPosition( vNewPos );
	}
	else
	{
		if( pOwner && pOwner->IsNeedProcess() )
		{
			SetAfterCollisionState( true );
		}
	}

	ProcessGuidArrow();
}

void ioGuide2Weapon::OnTracing( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckOwnerState() )
	{
		SetEndState( false );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
		
	if( pTarget )
	{
		if( !m_TargetName.IsEmpty() && pTarget->IsSystemState() )
		{
			m_TargetName.Clear();
		}
		
		if( !m_TargetName.IsEmpty() && !pTarget->IsEnableTarget() )
		{
			m_TargetName.Clear();
		}

		if( !m_TargetName.IsEmpty() )
		{
			m_vPrePos = GetPosition();
			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - m_vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			if( fRangeSq < m_fTraceRange * m_fTraceRange )
			{
				float fTimeGap = fTimePerSec * FLOAT1000;
				float fCurLoopTimeGap;
				float fCurLoopTimePerSec;
				while( fTimeGap > 0.0f )
				{
					if( fTimeGap > FLOAT10 )
					{
						fCurLoopTimeGap = FLOAT10;
						fCurLoopTimePerSec = FLOAT10 / FLOAT1000;

						fTimeGap -= FLOAT10;
					}
					else
					{
						fCurLoopTimeGap = fTimeGap;
						fCurLoopTimePerSec = fTimeGap / FLOAT1000;

						fTimeGap = 0.0f;
					}

					float fPosRate = m_fTargetTracePosRate;
					if( CheckDownTargetState( pTarget ) )
						fPosRate = m_fDownTargetTracePosRate;

					vDiff = pTarget->GetMidPositionByRate(fPosRate) - GetPosition();
					D3DXVec3Normalize( &vDiff, &vDiff );

					D3DXVECTOR3 vPreDir = m_vMoveDir;
					D3DXVECTOR3 vCurForce = m_vMoveDir * m_fTraceSpeed * fCurLoopTimePerSec;
					m_vMoveDir = vCurForce + (vDiff * m_fTraceRotSpeed * fCurLoopTimePerSec);

					D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

					float fCos = D3DXVec3Dot( &vPreDir, &m_vMoveDir );
					// 0도 일때
					if( fCos >= FLOAT1 )
					{
						// 처리 필요없음
					}
					else if( fCos <= -FLOAT1 ) // 180도 일때
					{
						D3DXVECTOR3 vAxis1, vAxis2;
						if( m_vMoveDir != ioMath::UNIT_Y )
						{
							D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_Y );
							D3DXVec3Normalize( &vAxis1, &-vAxis1 );
						}
						else
						{
							D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_X );
							D3DXVec3Normalize( &vAxis1, &-vAxis1 );
						}

						D3DXVec3Cross( &vAxis2, &m_vMoveDir, &vAxis1 );
						D3DXVec3Normalize( &vAxis2, &-vAxis2 );

						GetParentSceneNode()->Rotate( vAxis2, FLOAT1 );
					}
					else // 그외
					{
						float fGapAngle = RADtoDEG( acos( fCos ) );

						D3DXVECTOR3 vAxis;
						D3DXVec3Cross( &vAxis, &vDiff, &m_vMoveDir );
						D3DXVec3Normalize( &vAxis, &-vAxis );

						GetParentSceneNode()->Rotate( vAxis, fGapAngle );
					}
				}
			}
			else
			{
				m_TargetName.Clear();
			}
		}

		if( !m_TargetName.IsEmpty() )
		{
			if( m_dwTraceEndTime < dwCurTime )
			{
				m_TargetName.Clear();
			}
		}

		if( !m_TargetName.IsEmpty() && m_fTraceEndRange > 0.0f )
		{
			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			D3DXVECTOR3 vLength = vPos - GetPosition();
			float fLength = D3DXVec3Length( &vLength );
			if( fLength <= m_fTraceEndRange )
			{
				m_TargetName.Clear();
			}
		}
	}
	else
	{
		m_TargetName.Clear();
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	m_fCurMoveRange += fMoveDist;

	float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
	if( m_fCurMoveRange < fMaxRange )
	{
		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	}
	else
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->IsNeedProcess() )
		{
			SetAfterCollisionState( true );
		}
	}

	UpdateWorldAttackBox( fTimePerSec, false );
	ProcessGuidArrow();
}

void ioGuide2Weapon::OnAfterCollision( float fTimePerSec, ioPlayStage *pStage )
{
	if( !pStage )
	{
		SetEndState( false );
		return;
	}

	if( !CheckOwnerState() )
	{
		SetEndState( false );
		return;
	}

	if( m_bAttachTarget )
	{
		if( m_TargetName.IsEmpty() )
		{
			SetEndState( false );
		}
		else
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
			// 뮤턴트슈트 전용 하드코딩..
			if( pTarget )
			{
				if( pTarget->HasBuff( BT_SKIP_STUN ) )
					SetPosition( pTarget->GetMidPositionByRate() );
				else
					SetWeaponDead();
			}
			else
			{
				SetEndState( false );
			}
		}
	}

	ProcessGuidArrow();
}

void ioGuide2Weapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

const ioOrientBox& ioGuide2Weapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioGuide2Weapon::GetType() const
{
	return WT_GUIDE2;
}

ActionStopType ioGuide2Weapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioGuide2Weapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioGuide2Weapon::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioGuide2Weapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioGuide2Weapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioGuide2Weapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_AFTER_COLLISION:
		m_TargetName.Clear();
		rkPacket >> m_TargetName;
		SetAfterCollisionState( false );
		break;
	case WS_END:
		SetEndState( false );
		break;
	}
}

void ioGuide2Weapon::SetAfterCollisionState( bool bSendNet )
{
	if( m_WeaponState == WS_AFTER_COLLISION )
		return;

	m_WeaponState = WS_AFTER_COLLISION;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	SetSkillInfo( pOwner );

	if( bSendNet )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << m_WeaponState;
		kPacket << m_TargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGuide2Weapon::SetEndState( bool bSendNet )
{
	if( m_WeaponState == WS_END )
		return;

	m_WeaponState = WS_END;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioGuideAttackSkill *pGuideAttackSkill = ToGuideAttackSkill( pOwner->GetCurActiveSkill() );
	if( pGuideAttackSkill )
	{
		pGuideAttackSkill->SetSkillEndState( pOwner );
	}

	ioPlayStage *pStage = pOwner->GetCreator();

	if( !GetWeaponEndEffect().IsEmpty() )
	{
		ioMapEffect *pEndEffect = pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );
		if( pEndEffect )
		{
			D3DXQUATERNION qtRot = GetOrientation();
			pEndEffect->SetWorldOrientation( qtRot );
		}
	}

	if( m_pFireMesh )
	{
		SAFEDELETE( m_pGuideArrow );
		m_pFireMesh->EndEffectGrace();
	}

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << m_WeaponState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioGuide2Weapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_WeaponState != WS_MOVE && m_WeaponState != WS_TRACING )
		return false;

	if( m_WeaponState == WS_AFTER_COLLISION )
		return false;

	ioBaseChar *pTarget = ToBaseChar( pEntity );
	if( pTarget && !pTarget->IsNeedProcess() )
		return false;
	else if( !pTarget )
	{
		if( !GetOwner() || !GetOwner()->IsNeedProcess() )
			return false;
	}

	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}

DWORD ioGuide2Weapon::GetCollisionEnableTime() const
{
	if( m_WeaponState == WS_MOVE || m_WeaponState == WS_TRACING || m_WeaponState == WS_AFTER_COLLISION )
		return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;

	return FRAMEGETTIME() + 1;
}

void ioGuide2Weapon::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

bool ioGuide2Weapon::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( pOwner->GetState() == CS_USING_SKILL )
		return true;

	return false;
}

bool ioGuide2Weapon::CheckDownTargetState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() )
		return true;

	return false;
}

void ioGuide2Weapon::ProcessGuidArrow()
{
	if( !m_pGuideArrow )
		return;

	m_pGuideArrow->Process( GetOwner(), GetPosition() );
}

void ioGuide2Weapon::CheckAfterCollisionState( ioBaseChar *pChar, bool bApplyCol )
{
	if( !pChar )
	{
		SetEndState( true );
		return;
	}

	if( bApplyCol )
	{
		bool bSend = false;
		if( pChar->IsNeedSendNetwork() )
			bSend = true;

		m_TargetName.Clear();
		m_TargetName = pChar->GetCharName();

		SetAfterCollisionState( bSend );
	}
	else
	{
		SetEndState( true );
	}
}

void ioGuide2Weapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
		{
			SetEndState( true );
			return;
		}
	}
}

void ioGuide2Weapon::SetWeaponDead()
{
	if( m_WeaponState != WS_END )
	{
		SetEndState( false );
	}

	ioWeapon::SetWeaponDead();
}

void ioGuide2Weapon::SetSkillInfo( ioBaseChar *pOwner )
{
	if( !pOwner )
		SetEndState( false );

	ioGuideAttackSkill *pGuideAttackSkill = ToGuideAttackSkill( pOwner->GetCurActiveSkill() );
	if( pGuideAttackSkill )
	{
		pGuideAttackSkill->SetInfoByWeapon( pOwner, GetPosition() );
		pGuideAttackSkill->SetSkillMoveState(pOwner);
	}
}

void ioGuide2Weapon::SetTraceTarget( ioHashString &szName )
{
	m_TargetName = szName;
}

void ioGuide2Weapon::SetTraceState( ioPlayStage *pStage )
{
	m_WeaponState = WS_TRACING;
	m_dwTraceEndTime = FRAMEGETTIME() + m_dwTraceDuration;
}