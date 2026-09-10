

#include "stdafx.h"

#include "ioSelectPositionMoveWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "FindPredicateImpl.h"

ActionStopType ioSelectPositionMoveWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioSelectPositionMoveWeapon::m_DieActionStopType = AST_NONE;

ioSelectPositionMoveWeapon::ioSelectPositionMoveWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_MINE_EXPLOSION | WP_WOUND_NO_SOUND );

	m_pMesh							= NULL;

	m_State							= WS_PRE;

	m_bResistance					= false;
	m_bDisableMove					= false;

	m_dwCurMapCollisionDelayTime	= 0;
	m_dwWaitEndEndTime				= 0;

	m_fCurMoveRange					= 0;
	m_fTotalMoveAmount				= 0;
	m_fCurMoveAmount				= 0;

	m_fMoveRangeBySkill				= 0.0f;

	m_vTracePos						= ioMath::VEC3_ZERO;
}

ioSelectPositionMoveWeapon::~ioSelectPositionMoveWeapon()
{
	if( m_pMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioSelectPositionMoveWeapon::~ioSelectPositionMoveWeapon - UnReleased Resouce" );
	}
}

bool ioSelectPositionMoveWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioSelectPositionMoveWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_WeaponStyle = (WeaponStyle)rkLoader.LoadInt_e( "weapon_style", WS_DEFALT );

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_szBoomEffect = szBuf;

	rkLoader.LoadString_e( "map_collision_effect", "", szBuf, MAX_PATH );
	m_szMapCollisionEffect		= szBuf;
	m_dwMapCollisionDelayTime	= rkLoader.LoadFloat_e( "map_collision_effect_delay", 500 );

	m_bDisableUpDown		= rkLoader.LoadBool_e( "disable_updown", false );
	m_bPlayWoundSound		= rkLoader.LoadBool_e( "play_wound_sound", false );
	m_bDisableMapCollision	= rkLoader.LoadBool_e( "disable_map_collision", false );

	m_fTraceCollisionRange	= rkLoader.LoadFloat_e( "trace_collision_range", 25.0f );
	m_dwWaitEndDuration		= rkLoader.LoadInt_e( "wait_end_duration", 100 );

	m_fTraceMoveSpeed		= rkLoader.LoadFloat_e( "trace_move_speed", FLOAT1000 );

	rkLoader.LoadString_e( "target_buff", "", szBuf, MAX_PATH );
	m_TargetBuff = szBuf;
}

bool ioSelectPositionMoveWeapon::SetMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return false;

	m_pMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pMesh )	
		return false;

	GetParentSceneNode()->AttachObject( m_pMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pMesh->GetSkeletonName().IsEmpty() && m_pMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pMesh->GetCollisionBox();

	AddEffectEvent( m_pMesh );
	return true;
}

void ioSelectPositionMoveWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMesh )
	{
		g_EffectFactory.DestroyEffect( m_pMesh );
		m_pMesh = NULL;
	}
}

const ioOrientBox& ioSelectPositionMoveWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioSelectPositionMoveWeapon::GetType() const
{
	return WT_SELECT_POSITION_MOVE;
}

ActionStopType ioSelectPositionMoveWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioSelectPositionMoveWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioSelectPositionMoveWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioSelectPositionMoveWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pMesh && m_pMesh->GetSkeletonName().IsEmpty() && m_pMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	m_vStartPos = GetPosition();
}

void ioSelectPositionMoveWeapon::SetTracePosition( const D3DXVECTOR3 &vPos )
{
	m_vTracePos = vPos;
	
	D3DXVECTOR3 vTraceLength = GetPosition() - m_vTracePos;
	m_fTotalMoveAmount = fabs(D3DXVec3Length(&vTraceLength));

	D3DXVECTOR3 vDir = vPos - GetPosition();
	D3DXVec3Normalize( &vDir, &vDir );
	SetMoveDir( vDir );

	m_State = WS_TRACING;
}

void ioSelectPositionMoveWeapon::PlayWoundedSound( ioPlayEntity *pEntity )
{
	if( HasPeculiarity( WP_WOUND_NO_SOUND ) && !m_bPlayWoundSound )
		return;

	const ioHashString &szSound = GetAttribute()->m_WoundedSound;

	if( ToBaseChar( pEntity ) )
	{
		std::string szNewSound = ToBaseChar(pEntity)->GetWoundedSoundName( szSound );
		g_SoundMgr.PlaySound( ioHashString(szNewSound.c_str()), GetParentSceneNode() );
	}
	else
	{
		g_SoundMgr.PlaySound( szSound, GetParentSceneNode() );
	}

	StopLoopSound();
}

void ioSelectPositionMoveWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case WS_PRE:
		{
			m_State = WS_FLOATING;
		}
		break;
	case WS_FLOATING:
		{
			OnFloating( fTimePerSec, dwCurTime, pStage );			
		}
		break;
	case WS_WAITING:
		{
			OnWaiting( fTimePerSec );
		}
		break;
	case WS_TRACING:
		{
			OnTracing( fTimePerSec, dwCurTime, pStage );
		}
		break;
	case WS_WAIT_END:
		{
			OnWaitEnd( fTimePerSec, dwCurTime );
		}
		break;
	case WS_END:
		{
			if( m_pMesh )
			{
				m_pMesh->Update( fTimePerSec * FLOAT1000 );
			}

			if( m_WeaponStyle == WS_SPACE_MASTER )
			{
				SetWeaponDead();
			}
		}
	}

	PorcessTargetMove( GetOwner(), pStage );
	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioSelectPositionMoveWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_State != WS_FLOATING )
		return true;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwCurMapCollisionDelayTime < dwCurTime )
	{
		if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
		{
			//벽 통과 이펙트 생성
			CreateMapCollisionEffect( pStage );
			m_dwCurMapCollisionDelayTime = dwCurTime + m_dwMapCollisionDelayTime;

			if( m_WeaponStyle == WS_SPACE_MASTER )
			{				
				SetWeaponDead();
			}
			return true;
		}
	}

	return false;
}

void ioSelectPositionMoveWeapon::OnFloating( float fTimePerSec, DWORD dwCurTime, ioPlayStage *pStage )
{
	if( m_pMesh )
	{
		m_pMesh->Update( fTimePerSec * FLOAT1000 );
	}

	if( m_bDisableMove )
	{
		if( m_bMoveControlSkillName.IsEmpty() )
		{
			SetWeaponDead();
		}
		else if( !GetOwner() || !GetOwner()->GetCurActiveSkill() || GetOwner()->GetCurActiveSkill()->GetName() != m_bMoveControlSkillName )
		{
			SetWeaponDead();
		}
		return;
	}

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	m_fCurMoveRange += fMoveDist;

	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	if( GetMoveRange() < m_fCurMoveRange )
	{
		switch( m_WeaponStyle )
		{
		case WS_SPACE_MASTER:
			m_State = WS_END;
			break;
		default:
			m_State = WS_WAITING;
			break;
		}
	}
}

void ioSelectPositionMoveWeapon::OnWaiting( float fTimePerSec )
{	
	if( m_pMesh )
	{
		m_pMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioSelectPositionMoveWeapon::OnTracing( float fTimePerSec, DWORD dwCurTime, ioPlayStage *pStage )
{
	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	m_fCurMoveAmount += fMoveDist;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	if( m_fTotalMoveAmount < m_fCurMoveAmount )
	{
		SetPosition( m_vTracePos );
		SetWaitEndState( dwCurTime );
	}

	if( m_pMesh )
	{
		m_pMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioSelectPositionMoveWeapon::SetWaitEndState( DWORD dwCurTime )
{
	m_State = WS_WAIT_END;
	m_dwWaitEndEndTime = dwCurTime;
	m_dwWaitEndEndTime += m_dwWaitEndDuration;
}

void ioSelectPositionMoveWeapon::OnWaitEnd( float fTimePerSec, DWORD dwCurTime )
{
	if( 0 < m_dwWaitEndEndTime && m_dwWaitEndEndTime < dwCurTime )
	{
		m_State = WS_END;
	}

	if( m_pMesh )
	{
		m_pMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioSelectPositionMoveWeapon::PorcessTargetMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	for( ioHashStringVec::iterator iter = m_TargetNameList.begin(); iter != m_TargetNameList.end(); ++iter )
	{
		ioBaseChar* pTarget = pChar->GetBaseChar( *iter );
		if( pTarget )
		{
			ioBuff* pBuff = pTarget->GetBuff( m_TargetBuff );
			if( pBuff && pBuff->GetCreatorName() == pChar->GetCharName() )
			{
				pTarget->SetWorldPosition( GetPosition() );
			}
		}
	}
}

void ioSelectPositionMoveWeapon::RemoveTargetBuff( ioBaseChar *pChar, ioPlayStage *pStage )
{
	for( ioHashStringVec::iterator iter = m_TargetNameList.begin(); iter != m_TargetNameList.end(); ++iter )
	{
		ioBaseChar* pTarget = pChar->GetBaseChar( *iter );
		if( pTarget )
		{
			pTarget->SetCurJumpPowerAmt( 0.0f );
			pTarget->SetForcePowerAmt( 0.0f );
			pTarget->SetGravityAmt( 0.0f );
			pTarget->SetCurMoveSpeed( 0.0f );

			ioBuff* pBuff = pTarget->GetBuff( m_TargetBuff );
			if( pBuff && pBuff->GetCreatorName() == pChar->GetCharName() )
			{
				pBuff->SetReserveEndBuff();
			}
		}
	}
}

bool ioSelectPositionMoveWeapon::isWeaponEnd()
{
	if( m_State == WS_END )
		return true;

	return false;
}

void ioSelectPositionMoveWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioSelectPositionMoveWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;
	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	return bCollision;
}

float ioSelectPositionMoveWeapon::GetMoveSpeed()
{
	if( m_State == WS_FLOATING )
		return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
	else
		return m_fTraceMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioSelectPositionMoveWeapon::GetMoveRange()
{
	if( m_WeaponStyle == WS_SPACE_MASTER && 0.0f < m_fMoveRangeBySkill )
		return m_fMoveRangeBySkill * m_fOwnerChargeRangeRate;

	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioSelectPositionMoveWeapon::CreateMapCollisionEffect( ioPlayStage* pStage )
{
	pStage->CreateMapEffect( m_szMapCollisionEffect, GetPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
}

void ioSelectPositionMoveWeapon::SetWeaponDead()
{
	if( GetOwner() && GetOwner()->GetCreator() )
	{
		GetOwner()->GetCreator()->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox().GetCenter(), GetScale() );
		PlayExplosionSound();

		if( m_WeaponStyle == WS_SPACE_MASTER )
		{
			RemoveTargetBuff( GetOwner(), GetOwner()->GetCreator() );
		}
	}

	StopLoopSound();
	m_bLive = false;
}