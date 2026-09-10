#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioUroborusGuideArrow.h"

#include "ioUroborusWeapon.h"

#include "ItemDefine.h"

ActionStopType ioUroborusWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioUroborusWeapon::m_DieActionStopType = AST_NONE;

ioUroborusWeapon::ioUroborusWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	//SetPeculiarity( WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;
	m_UroborusState = US_MOVE;

	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_dwUroborusWaitEndTime = 0;

	m_pGuideArrow = NULL;
}

ioUroborusWeapon::~ioUroborusWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioUroborusWeapon::~ioUroborusWeapon - UnReleased Resouce" );
	}

	SAFEDELETE( m_pGuideArrow );
}

bool ioUroborusWeapon::InitResource()
{
	return true;
}

void ioUroborusWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedTeamEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueTeamEffect = szBuf;

	rkLoader.LoadString_e( "wait_effect", "", szBuf, MAX_PATH );
	m_WaitEffect = szBuf;

	m_dwUroborusWaitTime = (DWORD)rkLoader.LoadInt_e( "uroborus_wait_time", 0 );
	m_dwUroborusJumpTime = (DWORD)rkLoader.LoadInt_e( "uroborus_jump_time", 0 );

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

	m_bPiercingMove = rkLoader.LoadBool_e( "col_piercing_move", false );
}

void ioUroborusWeapon::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;
}

void ioUroborusWeapon::SetTeam( ioPlayStage *pStage, TeamType eType )
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

bool ioUroborusWeapon::SetFireMesh( const ioHashString &szFileName )
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

void ioUroborusWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioUroborusWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioUroborusWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioUroborusWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	if( CheckOwnerUroborusState() )
	{
		OnWaiting( fTimePerSec, pStage );
	}
	else
	{
		switch( m_UroborusState )
		{
		case US_MOVE:
			OnFloating( fTimePerSec, pStage );
			break;
		case US_WAIT:
			OnWaiting( fTimePerSec, pStage );
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
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioUroborusWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_UroborusState != US_MOVE )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			pStage->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox(true).GetCenter(),GetScale() );

			PlayWoundedSound( NULL );

			if( pOwner->IsNeedProcess() )
				SetWaitState( true );
		}

		return true;
	}

	return false;
}

void ioUroborusWeapon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckOwnerState() )
	{
		SetEndState( pStage, false );
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
			SetWaitState( true );
		}
	}

	ProcessGuidArrow();
}

void ioUroborusWeapon::OnWaiting( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckOwnerState() )
	{
		SetEndState( pStage, false );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( !CheckOwnerUroborusState() && (m_dwUroborusWaitEndTime < dwCurTime) )
	{
		SetEndState( pStage, false );
		return;
	}

	ProcessGuidArrow();
}

void ioUroborusWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

const ioOrientBox& ioUroborusWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioUroborusWeapon::GetType() const
{
	return WT_UROBORUS;
}

ActionStopType ioUroborusWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioUroborusWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioUroborusWeapon::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioUroborusWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioUroborusWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioUroborusWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case US_WAIT:
		SetWaitState( false );
		break;
	case US_END:
		SetEndState( pStage, false );
		break;
	}
}

void ioUroborusWeapon::SetWaitState( bool bSendNet )
{
	if( m_UroborusState == US_WAIT )
		return;

	m_UroborusState = US_WAIT;

	m_dwUroborusWaitEndTime = FRAMEGETTIME() + m_dwUroborusWaitTime;

	ioBaseChar *pOwner = GetOwner();

	if( pOwner && !m_WaitEffect.IsEmpty() )
	{
		ioMapEffect *pEndEffect = pOwner->GetCreator()->CreateMapEffect( m_WaitEffect, GetPosition(), GetScale() );
		if( pEndEffect )
		{
			D3DXQUATERNION qtRot = GetOrientation();
			pEndEffect->SetWorldOrientation( qtRot );
		}
	}

	if( bSendNet )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << m_UroborusState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUroborusWeapon::SetEndState( ioPlayStage *pStage, bool bSendNet )
{
	if( m_UroborusState == US_END )
		return;

	m_UroborusState = US_END;

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

	ioBaseChar *pOwner = GetOwner();
	if( bSendNet && pOwner && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << m_UroborusState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioUroborusWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_UroborusState != US_MOVE )
		return false;

	if( CheckOwnerUroborusState() )
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

	if( bCollision )
	{
		bool bSend = false;
		if( pTarget && pTarget->IsNeedSendNetwork() )
			bSend = true;
		if( !pTarget && GetOwner() && GetOwner()->IsNeedSendNetwork() )
			bSend = true;

		//기존 코드는 SetWaitState( bSend );
		//타이탄때 if( !m_bPiercingMove || bMapCollision ) 조건이 추가됨
		if( !m_bPiercingMove || bMapCollision )
			SetWaitState( bSend );
	}

	return bCollision;
}

DWORD ioUroborusWeapon::GetCollisionEnableTime() const
{
	if( m_UroborusState == US_MOVE )
		return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;

	return FRAMEGETTIME() + 1;
}

void ioUroborusWeapon::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

bool ioUroborusWeapon::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
		if( pItem && !pItem->IsChargeAttackState( pOwner ) )
			return false;

		return true;
	}
	else if( pOwner->GetState() == CS_JUMP )
	{
		ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
		if( !pItem )
			return false;

		ioChargeComboJump *pJump = ToChargeComboJump( pItem->GetCurExtendJump() );
		ioChargeComboJump2 *pJump2 = ToChargeComboJump2( pItem->GetCurExtendJump() );
		if( !pJump && !pJump2 )
			return false;

		if( pJump && pJump->GetCurChargeComboState() == ioChargeComboJump::CCS_GATHER_ATTACK )
			return true;

		if( pJump2 && pJump2->GetCurChargeComboState() == ioChargeComboJump2::CCS_TITAN )
			return true;
	}
	else if( pOwner->GetState() == CS_DASH )
	{
		ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_DASH );
		if( !pItem )
			return false;

		ioPowerChargeDash *pDash = ToPowerChargeDash( pItem->GetCurExtendDash() );
		if( !pDash )
			return false;

		if( pDash && pDash->GetCurState() == ioPowerChargeDash::CDS_TITAN )
			return true;

	}
	else if( pOwner->GetState() == CS_UROBORUS )
		return true;

	return false;
}

bool ioUroborusWeapon::IsCanUroborusJump()
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_UroborusState )
	{
	case US_MOVE:
	case US_WAIT:
		if( m_dwCreatedTime+m_dwUroborusJumpTime < dwCurTime )
			return true;
	}

	return false;
}

bool ioUroborusWeapon::IsUroborusMove()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_UroborusState == US_MOVE )
		return true;

	return false;
}

bool ioUroborusWeapon::IsUroborusWait()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_UroborusState == US_WAIT )
		return true;

	return false;
}

void ioUroborusWeapon::ProcessGuidArrow()
{
	if( !m_pGuideArrow )
		return;

	m_pGuideArrow->Process( GetOwner(), GetPosition() );
}

bool ioUroborusWeapon::CheckOwnerUroborusState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( pOwner->GetState() == CS_UROBORUS )
		return true;

	return false;
}