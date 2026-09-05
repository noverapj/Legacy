

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioUroborusGuideArrow.h"

#include "ioUroborusWeapon4.h"

#include "ItemDefine.h"
#include "SkillDefine.h"

ActionStopType ioUroborusWeapon4::m_ActionStopType = AST_NONE;
ActionStopType ioUroborusWeapon4::m_DieActionStopType = AST_NONE;

ioUroborusWeapon4::ioUroborusWeapon4( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
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

	m_bTraceTarget = false;
	m_bSkillWeapon = false;
	m_bOneTimeColCheck = false;
	m_bColOccur = false;

	m_fTargetMaxRange = 0;
	m_bGuidArrowPosMode = false;
}

ioUroborusWeapon4::~ioUroborusWeapon4()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioUroborusWeapon4::~ioUroborusWeapon4 - UnReleased Resouce" );
	}

	SAFEDELETE( m_pGuideArrow );
}

bool ioUroborusWeapon4::InitResource()
{
	return true;
}

void ioUroborusWeapon4::LoadProperty( ioINILoader &rkLoader )
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

	m_bLoopStateWeapon = rkLoader.LoadBool_e( "loop_state", false );

	m_eSetNotDeadType = (SetNotDeadType)rkLoader.LoadInt_e( "set_not_dead", SNDT_DEAD );

	m_bMutanteWeaponMode = rkLoader.LoadBool_e( "mutant_mode_weapon", false );

	m_bUserTargetLuciferItem = rkLoader.LoadBool_e( "use_lucifer_target", false );
	m_bUserTargetSuccubusItem = rkLoader.LoadBool_e( "use_succubus_target", false );
	m_bUserTargetMidnightItem = rkLoader.LoadBool_e( "use_midnight_target", false );
	//
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioPlayStage *pStage = pOwner->GetCreator();

		SAFEDELETE( m_pGuideArrow );
		m_pGuideArrow = new ioUroborusGuideArrow( pStage );

		m_pGuideArrow->Initialize( rkLoader );
		m_pGuideArrow->SetMaxArrowRange( GetMaxRange() );
		if( m_bMutanteWeaponMode )
			m_pGuideArrow->SetMaxArrowRange( GetMaxRange() * 2.f );
	}
}

void ioUroborusWeapon4::SetGuidArrowPosMode()
{
	if( m_pGuideArrow )
		m_pGuideArrow->SetStartPosMode( true, GetPosition() );
}

void ioUroborusWeapon4::SetGuidArrowPosMode( bool bEnable, const D3DXVECTOR3&  vStartPos)
{
	if( m_pGuideArrow )
		m_pGuideArrow->SetStartPosMode( bEnable, vStartPos );
}

void ioUroborusWeapon4::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;
}

void ioUroborusWeapon4::SetTeam( ioPlayStage *pStage, TeamType eType )
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

bool ioUroborusWeapon4::SetFireMesh( const ioHashString &szFileName )
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


void ioUroborusWeapon4::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioUroborusWeapon4::GetMoveDir()
{
	return m_vMoveDir;
}


D3DXVECTOR3 ioUroborusWeapon4::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

void ioUroborusWeapon4::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioUroborusWeapon4::Process( float fTimePerSec, ioPlayStage *pStage )
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
	case US_LOOP:
		ProcessGuidArrow();
		break;
	case US_END:
		if( !m_pFireMesh || !m_pFireMesh->IsLive() )
		{
			if ( m_eSetNotDeadType != SNDT_NOT_DEAD || m_bMutanteWeaponMode )
			{
				CheckCreateWeaponList();
				DestroyResource( pStage );
				SetWeaponDead();
				return;
			}
		}
		break;
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioUroborusWeapon4::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	if( m_bMutanteWeaponMode )
		return false;

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

void ioUroborusWeapon4::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckOwnerState() && (m_eSetNotDeadType == SNDT_DEAD || m_bMutanteWeaponMode ) )
	{
		SetEndState( false, false, false, true );
		return;
	}

	ioBaseChar *pOwner = GetOwner();
	float fCurSpeed = GetMoveSpeed();
	float fMoveDist = fCurSpeed * fTimePerSec;

	m_vPrePos = GetPosition();

	//////////////////////////////////////////////////////////////////////////
	if( m_bTraceTarget && !m_TraceTargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TraceTargetName );
		if( pTarget )
		{
			if( pTarget->IsSystemState() )
				m_TraceTargetName.Clear();

			if( !pTarget->IsEnableTarget() )
				m_bTraceTarget = false;


			if( m_bTraceTarget )
			{
				D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - m_vPrePos;
				D3DXVec3Normalize( &vDiff, &vDiff );
				m_vMoveDir = vDiff;
			}
		}
	}
	
	D3DXVECTOR3 vNewPos = m_vPrePos + (m_vMoveDir * fMoveDist);
	//////////////////////////////////////////////////////////////////////////
	if( m_eSetNotDeadType == SNDT_DEAD )
	{
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
	}
	else
	{
		float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
		if( m_fCurMoveRange < fMaxRange )
		{
			m_fCurMoveRange += fMoveDist;
			SetPosition( vNewPos );
		}
	}	

	ProcessGuidArrow();
}

void ioUroborusWeapon4::OnTargetMove( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckOwnerState() && m_eSetNotDeadType == SNDT_DEAD )
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

	if ( m_eSetNotDeadType == SNDT_DEAD )
	{
		m_fTargetCurRange += fMoveDist;

		if( m_fTargetCurRange < m_fTargetMaxRange )
		{
			SetPosition( vNewPos );		
		}
		else
		{	
			SetEndState( false, true, false );
			return;
		}
	}
	else
	{
		if( m_fTargetCurRange < m_fTargetMaxRange )
		{
			m_fTargetCurRange += fMoveDist;
			SetPosition( vNewPos );		
		}
	}

	ProcessGuidArrow();
}

void ioUroborusWeapon4::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

const ioOrientBox& ioUroborusWeapon4::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioUroborusWeapon4::GetType() const
{
	return WT_UROBORUS4;
}

ActionStopType ioUroborusWeapon4::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioUroborusWeapon4::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioUroborusWeapon4::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioUroborusWeapon4::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioUroborusWeapon4::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioUroborusWeapon4::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case US_TARGET_MOVE:
		SetTargetMoveState( false );
		break;
	case US_LOOP:
		{	
			ioHashString szTraceTargetName;
			rkPacket >> szTraceTargetName;
			SetLoopState( false, szTraceTargetName );
		}
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


void ioUroborusWeapon4::SetTargetMoveState( bool bSendNet )
{
	if( m_UroborusState == US_TARGET_MOVE )
		return;

	m_UroborusState = US_TARGET_MOVE;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	//Case1
	//스킬에서 사용할때
	ioTraceSkill2 *pTraceSkill2 = ToTraceSkill2( pOwner->GetCurActiveSkill() );
	if(	m_bSkillWeapon && pTraceSkill2 && pTraceSkill2->GetName() == GetWeaponBySkillName() )
	{
		m_vTargetMovePos = pTraceSkill2->CalcReturnUroborusPosition( this );

		D3DXVECTOR3 vPos =	GetPosition();
		vPos = m_vTargetMovePos - vPos;
		D3DXVec3Normalize( &vPos, &vPos );


		m_vTargetMovePos = m_vTargetMovePos - vPos*10;


		D3DXVECTOR3 vDiff = m_vTargetMovePos - GetPosition();
		m_fTargetMaxRange = D3DXVec3Length( &vDiff );
	}
	else
	{
		//Case2
		//아이템에서 사용할때 
		ioWeaponItem *pWeapon = pOwner->GetWeapon();
		if( !pWeapon )
		{
			SetWeaponDead();
			return;
		}

		ioLuciferItem *pLuciferItem = ToLuciferItem( pWeapon );
		if( !pLuciferItem )
		{
			SetWeaponDead();
			return;
		}

		if( pOwner->GetState() != CS_LUCIFER_FLY )
		{
			SetWeaponDead();
			return;
		}

		m_vTargetMovePos = pLuciferItem->CalcReturnUroborusPosition( this );

		D3DXVECTOR3 vDiff = m_vTargetMovePos - GetPosition();
		m_fTargetMaxRange = D3DXVec3Length( &vDiff );
	}

	//////////////////////////////////////////////////////////////////////////
	if( !m_TargetMoveEffect.IsEmpty() )
	{
		ioMapEffect *pEndEffect = pOwner->GetCreator()->CreateMapEffect( m_TargetMoveEffect, GetPosition(), GetScale() );
		if( pEndEffect )
		{
			D3DXQUATERNION qtRot = GetOrientation();
			pEndEffect->SetWorldOrientation( qtRot );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	if( bSendNet )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << m_UroborusState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioUroborusWeapon4::SetLoopState( bool bSendNet, const ioHashString &szTraceTargetName )
{
	m_UroborusState = US_LOOP;
	m_TraceTargetName = szTraceTargetName;

	//////////////////////////////////////////////////////////////////////////
	ioBaseChar *pOwner = GetOwner();
	ioWeaponItem *pWeaponItem = pOwner->GetWeapon();
	if( !pWeaponItem )
	{
		SetEndState( false, false, true );
		return;
	}

	ioLuciferItem *pLuciferItem = ToLuciferItem( pWeaponItem );
	if( !pLuciferItem && pOwner->GetState() == CS_LUCIFER_FLY )
	{
		SetEndState( false, false, true );
		return;
	}

	ioSuccubusItem *pSuccubusItem = ToSuccubusItem( pWeaponItem );
	if( !pSuccubusItem && pOwner->GetState() == CS_SUCCUBUS_ATTACK )
	{
		SetEndState( false, false, true );
		return;
	}

	ioMidnightItem* pMidnightItem = ToMidnightItem( pWeaponItem );
	if( !pMidnightItem && pOwner->GetState() == CS_MIDNIGHT_SPECIAL )
	{
		SetEndState( false, false, true );
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	ioSkill *pCurSkill = pOwner->GetCurActiveSkill();	
	ioChainTraceSkill *pChainTraceSkill = ToChainTrace( pCurSkill );
	ioFantasticWhipAttack *pFantastic = ToFantasticWhipAttack( pCurSkill );
	if( pChainTraceSkill )
	{
		pChainTraceSkill->SetTarget( m_TraceTargetName, GetOwner(), this );
	}
	else if( pFantastic )
	{
		pFantastic->SetTarget( m_TraceTargetName, GetOwner() );
	}
	else if( m_bUserTargetSuccubusItem && pSuccubusItem )
		pSuccubusItem->SetTarget( m_TraceTargetName, GetOwner() );
	else if( m_bUserTargetMidnightItem && pMidnightItem )
		pMidnightItem->SetTarget( m_TraceTargetName, GetOwner() );

	if( bSendNet )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << m_UroborusState;
		kPacket << m_TraceTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioUroborusWeapon4::SetEndState( bool bSkillEnd, bool bCheckCombo, bool bSendNet, bool bMust /*= false */ )
{
	if( !bMust )
	{
		if( m_UroborusState == US_END || m_eSetNotDeadType == SNDT_NOT_DEAD )
			return;
	}

	m_UroborusState = US_END;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

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

bool ioUroborusWeapon4::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_bMutanteWeaponMode )
		return false;

	if( m_eSetNotDeadType == SNDT_NOT_DEAD )
		return false;

	if( m_UroborusState != US_MOVE && m_UroborusState != US_TARGET_MOVE )
		return false;

	if( m_bOneTimeColCheck && m_bColOccur )
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

	if( m_bOneTimeColCheck && bCollision )
		m_bColOccur = true;

	return bCollision;
}

DWORD ioUroborusWeapon4::GetCollisionEnableTime() const
{
	if( m_UroborusState == US_MOVE || m_UroborusState == US_TARGET_MOVE )
		return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;

	return FRAMEGETTIME() + 1;
}

void ioUroborusWeapon4::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

bool ioUroborusWeapon4::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( pOwner->GetState() == CS_USING_SKILL || pOwner->GetState() == CS_LUCIFER_FLY || pOwner->GetState() == CS_SUCCUBUS_ATTACK || pOwner->GetState() == CS_MUTANT_SPECIAL_STATE || pOwner->GetState() == CS_MIDNIGHT_SPECIAL )
		return true;

	return false;
}

bool ioUroborusWeapon4::IsUroborusMove()
{
	if( m_UroborusState == US_MOVE )
		return true;

	return false;
}


bool ioUroborusWeapon4::IsUroborusTargetMove()
{
	if( m_UroborusState == US_TARGET_MOVE )
		return true;

	return false;
}


void ioUroborusWeapon4::ProcessGuidArrow()
{
	if( !m_pGuideArrow )
		return;

	m_pGuideArrow->Process( GetOwner(), GetPosition() );
}


void ioUroborusWeapon4::SetTraceTargetName( const ioHashString &szName )
{
	m_bTraceTarget = true;
	m_TraceTargetName = szName;
}


void ioUroborusWeapon4::CheckTargetMoveState( ioBaseChar *pChar, bool bApplyCol )
{
	if( !pChar ) return;

	//스킬에서 발생했을때
	if( m_bSkillWeapon )
	{
		if( bApplyCol )//&& m_vTargetMovePos != ioMath::VEC3_ZERO )
		{
			bool bSend = false;
			if( pChar->IsNeedSendNetwork() )
				bSend = true;

			m_TraceTargetName = pChar->GetCharName();
			SetTargetMoveState( bSend );
		}
		else
		{
			SetEndState( false, false, true );
		}

		return;
	}

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || ( pOwner->GetState() != CS_LUCIFER_FLY && pOwner->GetState() != CS_SUCCUBUS_ATTACK && pOwner->GetState() != CS_MUTANT_SPECIAL_STATE && pOwner->GetState() != CS_MIDNIGHT_SPECIAL ) 
		&& pOwner->GetState() != CS_USING_SKILL )
	{
		SetEndState( false, false, true );
		return;
	}

	ioWeaponItem *pWeaponItem = pOwner->GetWeapon();
	if( !pWeaponItem )
	{
		SetEndState( false, false, true );
		return;
	}

	ioLuciferItem *pLuciferItem = ToLuciferItem( pWeaponItem );
	if( !pLuciferItem && pOwner->GetState() == CS_LUCIFER_FLY )
	{
		SetEndState( false, false, true );
		return;
	}

	ioSuccubusItem *pSuccubusItem = ToSuccubusItem( pWeaponItem );
	if( !pSuccubusItem && pOwner->GetState() == CS_SUCCUBUS_ATTACK )
	{
		SetEndState( false, false, true );
		return;
	}

	ioMidnightItem* pMidnightItem = ToMidnightItem( pWeaponItem );
	if( !pMidnightItem && pOwner->GetState() == CS_MIDNIGHT_SPECIAL )
	{
		SetEndState( false, false, true );
		return;
	}

	if( m_bLoopStateWeapon && bApplyCol)
	{
		bool bSend = false;
		if( pChar->IsNeedSendNetwork() )
			bSend = true;

		m_TraceTargetName = pChar->GetCharName();

		//ioSkill *pCurSkill = pOwner->GetCurActiveSkill();
		//ioChainTraceSkill *pChainTraceSkill = ToChainTrace( pCurSkill );
		//if( pChainTraceSkill )
		//{
		//	pChainTraceSkill->SetTarget( m_TraceTargetName, GetOwner(), this );
		//	//SetWeaponMoveStop( bSend );
		//	SetLoopState(bSend);
		//	return;
		//}

		//if( pSuccubusItem )
		//	pSuccubusItem->SetTarget( m_TraceTargetName, GetOwner() );

		SetLoopState(bSend, m_TraceTargetName);
		return;
	}

	if( bApplyCol )//&& m_vTargetMovePos != ioMath::VEC3_ZERO )
	{
		bool bSend = false;
		if( pChar->IsNeedSendNetwork() )
			bSend = true;

		m_TraceTargetName = pChar->GetCharName();

		if( m_bUserTargetLuciferItem && pLuciferItem )
			pLuciferItem->SetTarget( m_TraceTargetName );
		else if( m_bUserTargetSuccubusItem && pSuccubusItem )
			pSuccubusItem->SetTarget( m_TraceTargetName, pChar );
		else if( m_bUserTargetMidnightItem && pMidnightItem )
			pMidnightItem->SetTarget( m_TraceTargetName, pChar );

		SetTargetMoveState( bSend );
	}
	else
	{
		SetEndState( false, false, true );
	}
}

void ioUroborusWeapon4::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_bMutanteWeaponMode )
		return;

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

void ioUroborusWeapon4::SetWeaponDead()
{
	if( m_UroborusState != US_END )
	{
		SetEndState( false, false, false );
		
		if ( m_eSetNotDeadType == SNDT_NOT_DEAD )
		{
			CheckCreateWeaponList();
		}
	}

	ioWeapon::SetWeaponDead();
}


void ioUroborusWeapon4::SetWeaponMoveStop( bool bSendNet )
{
	/*if( bSendNet )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << US_STOP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}

void ioUroborusWeapon4::SetVisible( bool bSetVisble )
{
	if ( m_pFireMesh )
	{
		m_pFireMesh->SetVisible( bSetVisble );
	}
	if ( m_pGuideArrow )
	{
		m_pGuideArrow->SetEffectVisble( bSetVisble );
	}
}

bool ioUroborusWeapon4::IsHasArrow() const
{
	if ( m_pGuideArrow )
	{
		return m_pGuideArrow->IsHasArrowList();
	}
	return false;
}

bool ioUroborusWeapon4::IsMutanteWeaponMode()
{
	return m_bMutanteWeaponMode;
}