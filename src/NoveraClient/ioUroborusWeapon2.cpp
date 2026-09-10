

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioUroborusGuideArrow.h"

#include "ioUroborusWeapon2.h"

#include "SkillDefine.h"

ActionStopType ioUroborusWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioUroborusWeapon2::m_DieActionStopType = AST_NONE;

ioUroborusWeapon2::ioUroborusWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	//SetPeculiarity( WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;
	m_UroborusState = US_MOVE;

	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_fTargetCurRange = 0.0f;

	m_pGuideArrow = NULL;
	m_vTargetMovePos = ioMath::VEC3_ZERO;
}

ioUroborusWeapon2::~ioUroborusWeapon2()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioUroborusWeapon2::~ioUroborusWeapon2 - UnReleased Resouce" );
	}

	SAFEDELETE( m_pGuideArrow );
}

bool ioUroborusWeapon2::InitResource()
{
	return true;
}

void ioUroborusWeapon2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedTeamEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueTeamEffect = szBuf;

	rkLoader.LoadString_e( "target_move_effect", "", szBuf, MAX_PATH );
	m_TargetMoveEffect = szBuf;

	m_fTargetMoveSpeed = rkLoader.LoadFloat_e( "target_move_speed", 0.0f );

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
}

void ioUroborusWeapon2::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;
}

void ioUroborusWeapon2::SetTeam( ioPlayStage *pStage, TeamType eType )
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

bool ioUroborusWeapon2::SetFireMesh( const ioHashString &szFileName )
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

void ioUroborusWeapon2::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioUroborusWeapon2::GetMoveDir()
{
	return m_vMoveDir;
}

void ioUroborusWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioUroborusWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	switch( m_UroborusState )
	{
	case US_MOVE:
		OnFloating( fTimePerSec, pStage );
		break;
	case US_TARGET_MOVE:
		OnTargetMove( fTimePerSec, pStage );
		break;
	case US_END:
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

bool ioUroborusWeapon2::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_UroborusState != US_MOVE && m_UroborusState != US_TARGET_MOVE )
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
				switch( m_UroborusState )
				{
				case US_MOVE:
					SetEndState( true, false, true );
					break;
				case US_TARGET_MOVE:
					SetEndState( true, false, true );
					break;
				}
			}
		}

		return true;
	}

	return false;
}

void ioUroborusWeapon2::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckOwnerState() )
	{
		SetEndState( false, false, false );
		return;
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
			SetEndState( false, false, true );
		}
	}

	ProcessGuidArrow();
}

void ioUroborusWeapon2::OnTargetMove( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckOwnerState() )
	{
		SetEndState( false, false, false );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = m_vTargetMovePos - GetPosition();
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	float fMoveDist = m_fTargetMoveSpeed * fTimePerSec;

	m_vPrePos = GetPosition();
	D3DXVECTOR3 vNewPos = m_vPrePos + (vCurDir * fMoveDist);
	m_fTargetCurRange += fMoveDist;

	if( m_fTargetCurRange < m_fTargetMaxRange )
		SetPosition( vNewPos );
	else
	{
		SetEndState( false, true, false );
		return;
	}

	ProcessGuidArrow();
}

void ioUroborusWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

const ioOrientBox& ioUroborusWeapon2::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioUroborusWeapon2::GetType() const
{
	return WT_UROBORUS2;
}

ActionStopType ioUroborusWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioUroborusWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioUroborusWeapon2::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioUroborusWeapon2::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioUroborusWeapon2::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioUroborusWeapon2::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case US_TARGET_MOVE:
		SetTargetMoveState( false );
		break;
	case US_END:
		{
			bool bSkillEnd, bCheckCombo;
			rkPacket >> bSkillEnd >> bCheckCombo;
			
			SetEndState( bSkillEnd, bCheckCombo, false );
		}
		break;
	}
}

void ioUroborusWeapon2::SetTargetMoveState( bool bSendNet )
{
	if( m_UroborusState == US_TARGET_MOVE )
		return;

	m_UroborusState = US_TARGET_MOVE;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioUroborusSkill *pUroborusSkill = ToUroborusSkill( pOwner->GetCurActiveSkill() );
	ioUroborusSkill2 *pUroborusSkill2 = ToUroborusSkill2( pOwner->GetCurActiveSkill() );
	ioUroborusSkill3 *pUroborusSkill3 = ToUroborusSkill3( pOwner->GetCurActiveSkill() );
	if( pUroborusSkill && pUroborusSkill->GetName() == GetWeaponBySkillName() )
		pUroborusSkill->SetUroborusMotionState( pOwner, this );
	else if( pUroborusSkill2 && pUroborusSkill2->GetName() == GetWeaponBySkillName() )
		pUroborusSkill2->SetUroborusMotionState( pOwner, this );
	else if( pUroborusSkill3 && pUroborusSkill3->GetName() == GetWeaponBySkillName() )
		pUroborusSkill3->SetUroborusMotionState( pOwner, this );

	if( !m_TargetMoveEffect.IsEmpty() )
	{
		ioMapEffect *pEndEffect = pOwner->GetCreator()->CreateMapEffect( m_TargetMoveEffect, GetPosition(), GetScale() );
		if( pEndEffect )
		{
			D3DXQUATERNION qtRot = GetOrientation();
			pEndEffect->SetWorldOrientation( qtRot );
		}
	}

	D3DXVECTOR3 vDiff = m_vTargetMovePos - GetPosition();
	m_fTargetMaxRange = D3DXVec3Length( &vDiff );

	if( bSendNet )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << m_UroborusState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUroborusWeapon2::SetEndState( bool bSkillEnd, bool bCheckCombo, bool bSendNet )
{
	if( m_UroborusState == US_END )
		return;

	m_UroborusState = US_END;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioUroborusSkill *pUroborusSkill = ToUroborusSkill( pOwner->GetCurActiveSkill() );
	ioUroborusSkill2 *pUroborusSkill2 = ToUroborusSkill2( pOwner->GetCurActiveSkill() );
	ioUroborusSkill3 *pUroborusSkill3 = ToUroborusSkill3( pOwner->GetCurActiveSkill() );
	if( pUroborusSkill && bSkillEnd )
	{
		pUroborusSkill->SetSkillEndState( pOwner );
	}
	else if( pUroborusSkill2 )
	{
		// HARDCODE : 반드시 Combo를 먼저 체크해야한다.
		if( bCheckCombo )
			pUroborusSkill2->CheckComboState( pOwner );

		if( bSkillEnd )
			pUroborusSkill2->SetSkillEndState( pOwner );
	}
	else if( pUroborusSkill3 )
	{
		// HARDCODE : 반드시 Combo를 먼저 체크해야한다.
		if( bCheckCombo )
			pUroborusSkill3->CheckComboState( pOwner );

		if( bSkillEnd )
			pUroborusSkill3->SetSkillEndState( pOwner );
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
		kPacket << m_UroborusState;
		kPacket << bSkillEnd;
		kPacket << bCheckCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioUroborusWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_UroborusState != US_MOVE && m_UroborusState != US_TARGET_MOVE )
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

DWORD ioUroborusWeapon2::GetCollisionEnableTime() const
{
	if( m_UroborusState == US_MOVE || m_UroborusState == US_TARGET_MOVE )
		return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;

	return FRAMEGETTIME() + 1;
}

void ioUroborusWeapon2::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

bool ioUroborusWeapon2::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( pOwner->GetState() == CS_USING_SKILL )
		return true;

	return false;
}

bool ioUroborusWeapon2::IsUroborusMove()
{
	if( m_UroborusState == US_MOVE )
		return true;

	return false;
}

bool ioUroborusWeapon2::IsUroborusTargetMove()
{
	if( m_UroborusState == US_TARGET_MOVE )
		return true;

	return false;
}

void ioUroborusWeapon2::ProcessGuidArrow()
{
	if( !m_pGuideArrow )
		return;

	m_pGuideArrow->Process( GetOwner(), GetPosition() );
}

void ioUroborusWeapon2::SetTargetMovePos( D3DXVECTOR3 vTargetPos )
{
	m_vTargetMovePos = vTargetPos;
}

void ioUroborusWeapon2::CheckTargetMoveState( ioBaseChar *pChar, bool bApplyCol )
{
	if( !pChar ) return;

	if( bApplyCol && m_vTargetMovePos != ioMath::VEC3_ZERO )
	{
		bool bSend = false;
		if( pChar->IsNeedSendNetwork() )
			bSend = true;

		SetTargetMoveState( bSend );
	}
	else
	{
		SetEndState( false, false, true );
	}
}

void ioUroborusWeapon2::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
		{
			SetEndState( true, false, true );
			return;
		}
	}
}

void ioUroborusWeapon2::SetWeaponDead()
{
	if( m_UroborusState != US_END )
	{
		SetEndState( false, false, false );
	}

	ioWeapon::SetWeaponDead();
}






