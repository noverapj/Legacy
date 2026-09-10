

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioUroborusGuideArrow.h"

#include "ioUroborusWeapon6.h"

ActionStopType ioUroborusWeapon6::m_ActionStopType = AST_NONE;
ActionStopType ioUroborusWeapon6::m_DieActionStopType = AST_NONE;

ioUroborusWeapon6::ioUroborusWeapon6( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	m_pFireMesh = NULL;
	m_UroborusState = US_MOVE;

	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_fTargetCurRange = 0.0f;

	m_vTargetMovePos = ioMath::VEC3_ZERO;

	m_bTraceTarget = false;
	m_bSkillWeapon = false;
	m_bOneTimeColCheck = false;
	m_bColOccur = false;

	m_fTargetMaxRange = 0;

	m_StartPos = ioMath::VEC3_ZERO;
}

ioUroborusWeapon6::~ioUroborusWeapon6()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioUroborusWeapon6::~ioUroborusWeapon6 - UnReleased Resouce" );
	}
}

bool ioUroborusWeapon6::InitResource()
{
	return true;
}

void ioUroborusWeapon6::LoadProperty( ioINILoader &rkLoader )
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

	//////////////////////////////////////////////////////////////////////////
	m_bUseEffectStartPosMode	= rkLoader.LoadBool_e( "use_effect_start_mode", true );
	m_bUseStartReturnPos		= rkLoader.LoadBool_e( "use_start_return_pos", false );
	m_bUseRotateWeapon			= rkLoader.LoadBool_e( "use_rotate_weapon", false );
	m_fRotateAngle				= rkLoader.LoadFloat_e( "rotate_angle", 0.f );
	m_bUseReturnWeapon			= rkLoader.LoadBool_e( "use_return_weapon", false );
	m_fReturnMaxLength			= rkLoader.LoadFloat_e( "return_move_length", 0.f );

	m_bUseDelayStartTime		= rkLoader.LoadBool_e( "use_delay_start_time", false );
	m_dwDelayStartTime			= (DWORD)rkLoader.LoadInt_e( "delay_start_time", 0 );

	m_fMaxCheckLength			= rkLoader.LoadFloat_e( "check_max_length", 0.f );
	m_fMinCheckLength			= rkLoader.LoadFloat_e( "check_min_length", FLOAT0001 );

	m_CallWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "end_call_attribute_index", 0 );		
	m_CallWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "end_call_attribute_resistance", 0 );		

	rkLoader.LoadString_e( "end_call_attribute_wound_ani", "", szBuf, MAX_PATH );
	m_CallWeaponInfo.m_WoundedAnimation = szBuf;		

	m_CallWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "end_call_attribute_wound_duration", 0 );
	m_CallWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "end_call_attribute_wound_loop_ani", false );
}

void ioUroborusWeapon6::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;
}

void ioUroborusWeapon6::SetTeam( ioPlayStage *pStage, TeamType eType )
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
	else //°³ÀÎÀü
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

bool ioUroborusWeapon6::SetFireMesh( const ioHashString &szFileName )
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


void ioUroborusWeapon6::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	D3DXVECTOR3 vDirTemp = vDir;
	if( m_bUseRotateWeapon )
	{
		float fCurAngle = 0.0f;
		fCurAngle = ioMath::ArrangeHead( m_fRotateAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		vDirTemp = qtAngle * vDirTemp;
	}

	m_vMoveDir = vDirTemp;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioUroborusWeapon6::GetMoveDir()
{
	return m_vMoveDir;
}


D3DXVECTOR3 ioUroborusWeapon6::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

void ioUroborusWeapon6::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() && !m_bUseEffectStartPosMode )
		return;

	SetPosition( vPos );

	m_StartPos = vPos;
}



void ioUroborusWeapon6::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_UroborusState )
	{
	case US_MOVE:
		if( m_bUseDelayStartTime && dwCurTime < (m_dwCreatedTime+m_dwDelayStartTime) )
			return;

		OnFloating( fTimePerSec, pStage );

		break;
	case US_RETURN_MOVE:
		OnReturnMove( fTimePerSec, pStage );
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

	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
	{
		if( pOwner->IsSystemState() )
		{
			SetEndState( false, true, false );
			return;
		}

		D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vPos      = GetPosition();
		vPos = vOwnerPos - vPos;
		float fLength = D3DXVec3Length(&vPos);
		if( fLength > m_fMaxCheckLength )
			SetEndState( false, true, false );
	}
}


void ioUroborusWeapon6::OnFloating( float fTimePerSec, ioPlayStage *pStage )
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

	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 vNewPos = m_vPrePos + (m_vMoveDir * fMoveDist);
	m_fCurMoveRange += fMoveDist;

	float fMaxRange = GetMaxRange();// + m_fGrowthMoveRange;
	if( m_fCurMoveRange < fMaxRange )
	{
		SetPosition( vNewPos );
	}
	else
	{
		SetReturnMove();
		/*if( pOwner && pOwner->IsNeedProcess() )
		{
			SetEndState( false, false, true );
		}*/
	}

	//ProcessGuidArrow();
}


void ioUroborusWeapon6::SetReturnMove()
{
	m_fCurMoveRange = 0.f;

	m_UroborusState = US_RETURN_MOVE;
}


void ioUroborusWeapon6::OnReturnMove( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckOwnerState() )
	{
		SetEndState( false, false, false );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
	{
		SetEndState( false, true, false );
		return;
	}

	D3DXVECTOR3 vOwnerPos= pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vCurDir = vOwnerPos - GetPosition();
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	m_vMoveDir = vCurDir;

	float fCurSpeed = GetMoveSpeed();
	float fMoveDist = fCurSpeed * fTimePerSec;

	//float fMoveDist = m_fTargetMoveSpeed * fTimePerSec;

	m_vPrePos = GetPosition();
	D3DXVECTOR3 vNewPos = m_vPrePos + (vCurDir * fMoveDist);
	m_fCurMoveRange += fMoveDist;

	float fMaxRange =	m_fReturnMaxLength;//GetMaxRange() + m_fGrowthMoveRange;
	if( m_fCurMoveRange < fMaxRange )
	{
		SetPosition( vNewPos );
	}
	else
	{	
		SetEndState( false, true, false );
		return;
	}

	D3DXVECTOR3 vPos      = GetPosition();
	vPos = vOwnerPos - vPos;
	float fLength = D3DXVec3Length(&vPos);
	if( fLength <= m_fMinCheckLength )
		SetEndState( false, true, false );
}

void ioUroborusWeapon6::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

const ioOrientBox& ioUroborusWeapon6::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioUroborusWeapon6::GetType() const
{
	return WT_UROBORUS6;
}

ActionStopType ioUroborusWeapon6::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioUroborusWeapon6::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioUroborusWeapon6::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;// * m_fOwnerChargeSpeedRate;
}

float ioUroborusWeapon6::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange;// * m_fOwnerChargeRangeRate;
}

void ioUroborusWeapon6::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioUroborusWeapon6::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case US_END:
		{
			bool bSkillEnd, bCheckCombo;
			rkPacket >> bSkillEnd >> bCheckCombo;

			SetEndState( bSkillEnd, bCheckCombo, false );
		}
		break;
	}
}


void ioUroborusWeapon6::SetEndState( bool bSkillEnd, bool bCheckCombo, bool bSendNet )
{
	if( m_UroborusState == US_END )
		return;

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

	//if( m_pFireMesh )
		//m_pFireMesh->EndEffectGrace();
	if( m_pFireMesh )
		m_pFireMesh->EndEffectForce();


	DWORD dwCurTime = FRAMEGETTIME();
	g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
		m_CallWeaponInfo,
		GetPosition(),
		-m_vMoveDir,											
		dwCurTime,
		m_dwWeaponIdx );

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


void ioUroborusWeapon6::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

bool ioUroborusWeapon6::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	return true;
}


void ioUroborusWeapon6::SetTraceTargetName( const ioHashString &szName )
{
	m_bTraceTarget = true;
	m_TraceTargetName = szName;
}

void ioUroborusWeapon6::SetWeaponDead()
{
	if( m_UroborusState != US_END )
	{
		SetEndState( false, false, false );
	}

	ioWeapon::SetWeaponDead();
}

