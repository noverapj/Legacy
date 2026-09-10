#include "StdAfx.h"
#include "ioTargetTeleportAttackBuff.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioTargetTeleportAttackBuff::ioTargetTeleportAttackBuff(void)
{
}

ioTargetTeleportAttackBuff::ioTargetTeleportAttackBuff( const ioTargetTeleportAttackBuff &rhs ):
ioBuff( rhs ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_fTargetMinAngle( rhs.m_fTargetMinAngle ),
	m_fTargetMaxAngle( rhs.m_fTargetMaxAngle ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	m_szMarkEffect( rhs.m_szMarkEffect ),
	m_szRangeEffect( rhs.m_szRangeEffect ),
	m_dwAttackDelayTime( rhs.m_dwAttackDelayTime ),
	m_bFindTargetOnlyFirstLoop( rhs.m_bFindTargetOnlyFirstLoop ),
	m_vTargetTeleportOffset( rhs.m_vTargetTeleportOffset ),
	m_NormalTargetAttackAttribute( rhs.m_NormalTargetAttackAttribute ),
	m_AirTargetAttackAttribute( rhs.m_AirTargetAttackAttribute ),
	m_DownTargetAttackAttribute( rhs.m_DownTargetAttackAttribute ),
	m_fEndJumpPower( rhs.m_fEndJumpPower ),
	m_bDeleteBuffByDownState( rhs.m_bDeleteBuffByDownState ),
	m_bDeleteBuffByDropState( rhs.m_bDeleteBuffByDropState ),
	m_bDisableOwnerDownAttack( rhs.m_bDisableOwnerDownAttack ),
	m_bDeleteBuffByWoundedState( rhs.m_bDeleteBuffByWoundedState ),
	m_iAttakMaxCount( rhs.m_iAttakMaxCount ),
	m_bReduceAllGauge( rhs.m_bReduceAllGauge ),
	m_vDeleteBuffName( rhs.m_vDeleteBuffName )
{

}

ioTargetTeleportAttackBuff::~ioTargetTeleportAttackBuff(void)
{
}

ioBuff* ioTargetTeleportAttackBuff::Clone()
{
	return new ioTargetTeleportAttackBuff( *this );
}

void ioTargetTeleportAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];
	//Target
	m_TargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fTargetMinAngle		= rkLoader.LoadFloat_e( "target_min_angle", 0.0f );
	m_fTargetMaxAngle		= rkLoader.LoadFloat_e( "target_max_angle", 0.0f );
	m_fTargetMinRange		= rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTargetMaxRange		= rkLoader.LoadFloat_e( "target_max_range", 0.0f );

	rkLoader.LoadString_e( "target_effect_name", "", szBuf, MAX_PATH );
	m_szMarkEffect = szBuf;
	rkLoader.LoadString_e( "range_effect_name", "", szBuf, MAX_PATH );
	m_szRangeEffect = szBuf;

	m_dwAttackDelayTime = rkLoader.LoadInt_e( "attack_delay_time", 0 );

	m_bFindTargetOnlyFirstLoop = rkLoader.LoadBool_e( "find_target_only_first_loop", false );

	m_vTargetTeleportOffset.x = rkLoader.LoadFloat_e( "target_teleport_offset_x", 0.0f );
	m_vTargetTeleportOffset.y = rkLoader.LoadFloat_e( "target_teleport_offset_y", 0.0f );
	m_vTargetTeleportOffset.z = rkLoader.LoadFloat_e( "target_teleport_offset_z", 0.0f );

	LoadAttackAttribute_e( "normal_target_attack", m_NormalTargetAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "air_target_attack", m_AirTargetAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "down_target_attack", m_DownTargetAttackAttribute, rkLoader );

	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );

	m_bDeleteBuffByDownState = rkLoader.LoadBool_e( "delete_buff_by_down_state", false );
	m_bDeleteBuffByDropState = rkLoader.LoadBool_e( "delete_buff_by_drop_state", false );
	m_bDeleteBuffByWoundedState = rkLoader.LoadBool_e( "delete_buff_by_woundedstate", false );

	m_bDisableOwnerDownAttack = rkLoader.LoadBool( "disable_owner_down_attack", false );

	m_iAttakMaxCount = rkLoader.LoadInt_e( "attack_max_count", 0 );
	m_bReduceAllGauge = rkLoader.LoadBool_e( "reduce_all_gauge", false );

	m_vDeleteBuffName.clear();
	char	szBuffName[MAX_PATH]	= "";
	int	iDeleteBuffCount	= rkLoader.LoadInt_e( "delete_other_buff_count", 0 );
	for( int i = 0; i < iDeleteBuffCount; i++ )
	{
		sprintf_s( szBuffName, "delete_other_buff_name%d", i + 1 );
		rkLoader.LoadString( szBuffName, "", szBuf, MAX_PATH );
		m_vDeleteBuffName.push_back( szBuf );
	}
}

void ioTargetTeleportAttackBuff::ClearData()
{
	//Map Effect
	m_dwMarkMapEffectID = -1;
	m_dwRangeMapEffectID = -1;

	//Attack
	m_szAttackTarget.Clear();
	m_iAttackCount = 0;
	m_bInputKey = false;
	m_dwKeyInputTime = 0;

	m_BuffState = BS_LOOP;
	m_dwFireMotionEndTime = 0;
}

void ioTargetTeleportAttackBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	ClearData();

	ioPlayStage *pStage = m_pOwner->GetCreator();
	if ( pStage )
		CreateRangeEffect( pOwner, pStage, m_szRangeEffect, m_dwRangeMapEffectID );
}

bool ioTargetTeleportAttackBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if ( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	return true;
}

void ioTargetTeleportAttackBuff::ProcessBuff( float fTimePerSec )
{
	if ( !CheckOwnerState() )
	{
		SetReserveEndBuff();
		return;
	}

	if( !IsRightOwnerItem() )
	{
		SetReserveEndBuff();
		return;
	}

	ioPlayStage *pStage = m_pOwner->GetCreator();
	if ( !pStage )
	{
		SetReserveEndBuff();
		return;
	}

	if ( !CheckOperation( m_pOwner, fTimePerSec ) )
		return;

	switch( m_BuffState )
	{
	case BS_LOOP:
		{
			FindTarget( m_pOwner, pStage );
			UpdateMark( m_pOwner, pStage );
			UpdateRangeEffect( m_pOwner, pStage );

			if ( m_pOwner->IsNeedProcess() && CheckInputKey( m_pOwner, pStage ) && CheckEnableAttackState( m_pOwner, pStage ) )
			{
				SetActionState( m_pOwner, pStage );
				break;
			}
			else if( m_pOwner->IsNeedProcess() && CheckInputKey( m_pOwner, pStage ) && m_szAttackTarget.IsEmpty() )
			{
				SetCancelState( m_pOwner );
				break;
			}
		}
		break;
	case BS_ATTACK:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				if ( m_iAttakMaxCount <= m_iAttackCount )
					SetReserveEndBuff();
				SetActionEndState( m_pOwner );
				break;
			}
			UpdateMark( m_pOwner, pStage );
			UpdateRangeEffect( m_pOwner, pStage );
		}
		break;
	}
}

bool ioTargetTeleportAttackBuff::CheckOwnerState()
{
	if( !GetOwner() || !m_pOwner )
		return false;

	if( m_bDeleteBuffByWoundedState )
	{
		if( m_pOwner->IsWoundedState() )
			return false;
	}

	if( m_bDeleteBuffByDownState )
	{
		CharState eState = m_pOwner->GetState();
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			return false;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			return false;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			return false;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			return false;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			return false;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			return false;
	}

	if ( m_bDeleteBuffByDropState )
	{
		CharState eState = m_pOwner->GetState();
		if( eState == CS_DROP_ZONE_DOWN )
			return false;
	}

	return true;
}


bool ioTargetTeleportAttackBuff::CheckOperation( ioBaseChar* pChar, float fTimePerSec )
{
	if ( !pChar )
		return false;
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		{
			DWORD dwCurTime = FRAMEGETTIME();
			float fCurTicGauge = GetCurTicGauge();
			int iTickGap = GetCurTicGap();
			int iCurTicTime = GetCurTicTime();
			if( iTickGap >= iCurTicTime )
			{
				m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

				ioObjectItem *pObject = pChar->GetObject();
				if( pObject && !pObject->EnableReduceGaugeOnSwitchBuff() && pObject->GetItemCode() == m_dwCreateWeaponItemCode )
					break;

				if( fCurTicGauge != 0.0f )
				{
					if( !IsRightOwnerItem() )
						SetReserveEndBuff();

					if( pChar->ReduceCurSkillGaugeBySkill( m_ItemName, fCurTicGauge ) <= 0.0f )
					{
						if( m_pOwner->GetState() != CS_BUFF_ACTION )
							SetReserveEndBuff();
					}
				}
			}
			CheckCallingProcess();
		}

		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	return IsLive();
}

void ioTargetTeleportAttackBuff::FindTarget( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage ) 
		return;
	if( !pChar->IsNeedProcess() )
		return;

	D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fMaxRangeSq = m_fTargetMaxRange * m_fTargetMaxRange;
	float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	//
	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	BaseCharDistanceSqList TargetNameList;

	for ( auto iter = rkCharList.begin(); iter != rkCharList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;

		if( !pTarget )	continue;

		if ( pTarget == pChar )
			continue;

		if ( pTarget->GetTeam() == pChar->GetTeam() )
			continue;

		if ( pTarget->IsSkipState() )
			continue;

		if( !IsEnableTargetState( pTarget, true ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );

		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );

		if ( !COMPARE( fAngleGap, m_fTargetMinAngle, m_fTargetMaxAngle ) )
			continue;

		float fDistanceSq = D3DXVec3LengthSq( &vDiff );

		BaseCharDistanceSq kTarget;
		kTarget.m_Name = pTarget->GetCharName();
		kTarget.m_fDistanceSq = fLengthSq;
		kTarget.m_vPos = pTarget->GetMidPositionByRate();

		TargetNameList.push_back( kTarget );
	}

	bool bChange = false;
	if ( !TargetNameList.empty() )
	{
		std::sort( TargetNameList.begin(), TargetNameList.end(), BaseCharDistanceSqSort2() );
		if( m_szAttackTarget != TargetNameList[0].m_Name )
		{
			m_szAttackTarget	= TargetNameList[0].m_Name;
			bChange	= true;

			if ( !m_szAttackTarget.IsEmpty() )
				CreateMarker( pChar, pStage, m_szAttackTarget, m_szMarkEffect, m_dwMarkMapEffectID );
			else
				DestroyMaker( pChar, pStage, m_dwMarkMapEffectID );
		}
	}
	else
	{
		if( !m_szAttackTarget.IsEmpty() )
		{
			m_szAttackTarget.Clear();
			bChange	= true;
			DestroyMaker( pChar, pStage, m_dwMarkMapEffectID );
		}
	}
	//패킷
	if ( pChar->IsNeedSendNetwork() && bChange )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(BST_CHANGE_TARGET);
		kPacket << m_szAttackTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioTargetTeleportAttackBuff::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	CharState eState = pTarget->GetState();
	switch( eState )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_WARP_STATE:
	case CS_EXCAVATING:
	case CS_ROULETTE:
		return false;
	}

	if( pTarget->IsChatModeState( false ) )
		return false;

	if( pTarget->IsExcavating() )
		return false;

	if( pTarget->GetExperienceMode() == EMS_EXPERIENCE )
		return false;

	if( pTarget->HasHideBuff() )
		return false;

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	return true;
}

void ioTargetTeleportAttackBuff::CreateMarker( ioBaseChar *pChar, ioPlayStage *pStage, const ioHashString& szTargetName, const ioHashString& szEffectName, DWORD& dwEffectID )
{
	if ( !pChar || !pStage )
		return;

	ioBaseChar* pTarget = pStage->GetBaseChar( szTargetName );
	if ( !pTarget )
		return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( !pMapEffect )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pMapEffect = pStage->CreateMapEffect( szEffectName, vPos, vScale );
		if( pMapEffect )
		{
			dwEffectID = pMapEffect->GetUniqueID();

			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
			pMapEffect->SetWorldOrientation( qtCharRot );
		}
	}
}

void ioTargetTeleportAttackBuff::DestroyMaker( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pChar || !pStage )
		return;
	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if ( pMapEffect )
	{
		pMapEffect->EndEffectForce();
		dwEffectID = -1;
	}
}

void ioTargetTeleportAttackBuff::UpdateMark( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwMarkMapEffectID );
	if( pMapEffect )
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( m_szAttackTarget );
		if ( pTarget )
		{
			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			pMapEffect->SetWorldPosition( vPos );
		}
		else
			DestroyMaker( pChar, pStage, m_dwMarkMapEffectID );
	}
}

void ioTargetTeleportAttackBuff::CreateRangeEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffectName, DWORD& dwEffectID )
{
	if ( !pChar || !pStage )
		return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( !pMapEffect )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pMapEffect = pStage->CreateMapEffect( szEffectName, vPos, vScale );
		if( pMapEffect )
		{
			dwEffectID = pMapEffect->GetUniqueID();

			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
			pMapEffect->SetWorldOrientation( qtCharRot );
		}
	}
}

void ioTargetTeleportAttackBuff::DestroyRangeEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pChar || !pStage )
		return;
	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if ( pMapEffect )
	{
		pMapEffect->EndEffectForce();
		dwEffectID = -1;
	}
}

void ioTargetTeleportAttackBuff::UpdateRangeEffect( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwRangeMapEffectID );
	if( pMapEffect )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		pMapEffect->SetWorldPosition( vPos );
	}
}

bool ioTargetTeleportAttackBuff::CheckInputKey( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return false;

	if ( !m_bInputKey )
	{
		if ( pChar->IsDefenseKey() )
		{
			m_dwKeyInputTime = FRAMEGETTIME();
			m_bInputKey = true;
		}
	}
	else
	{
		if ( m_dwKeyInputTime + m_dwAttackDelayTime < FRAMEGETTIME() )
		{
			return true;
		}
	}

	return false;
}

bool ioTargetTeleportAttackBuff::CheckEnableAttackState( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return false;

	if ( m_iAttakMaxCount <= m_iAttackCount )
		return false;

	if ( !CheckEnableOwnerState( pChar, pStage ) )
		return false;

	if( !CheckEnableTargetState( pStage ) )
		return false;

	return true;
}

bool ioTargetTeleportAttackBuff::CheckEnableOwnerState( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return false;

	CharState eState = pChar->GetState();

	if( eState == CS_DROP_ZONE_DOWN )
		return false;

	if ( m_bDisableOwnerDownAttack )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			return false;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			return false;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			return false;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			return false;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			return false;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			return false;
	}
	return true;
}

bool ioTargetTeleportAttackBuff::CheckEnableTargetState( ioPlayStage *pStage )
{
	if ( !pStage )
		return false;

	if ( m_szAttackTarget.IsEmpty() )
		return false;

	ioBaseChar* pTarget = pStage->GetBaseChar( m_szAttackTarget );
	if ( !pTarget )
		return false;

	//Check TargetState
	CharState eState = pTarget->GetState();
	if( eState == CS_DIE )
		return false;
	if( eState == CS_VIEW )
		return false;
	if( eState == CS_OBSERVER )
		return false;
	if( eState == CS_READY )
		return false;
	if( eState == CS_LOADING )
		return false;
	if( eState == CS_FISHING )
		return false;
	if( eState == CS_CREEPING )
		return false;
	if( eState == CS_DROP_ZONE_DOWN )
		return false;
	if( eState == CS_EXCAVATING )
		return false;
	if( eState == CS_ROULETTE )
		return false;
	if ( eState == CS_STARTMOTION )
		return false;
	if ( eState == CS_ENDMOTION )
		return false;
	if ( eState == CS_WARP_STATE )
		return false;
	if ( eState == CS_LOCKUP_BUFF )
		return false;
	if ( pTarget->IsSkipState() )
		return false;
	if( pTarget->IsExcavating() )
		return false;
	if( pTarget->GetExperienceMode() == EMS_EXPERIENCE )
		return false;
	if( pTarget->IsChatModeState( false ) )
		return false;

	return true;
}


void ioTargetTeleportAttackBuff::SetActionState( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioBaseChar* pTarget = pStage->GetBaseChar( m_szAttackTarget );
	if ( !pTarget )
		return;

	pChar->SetState( CS_BUFF_ACTION );

	SetTeleport( pChar, pStage );

	const AttackAttribute& Attri = GetActionAttribute( pChar );

	SetActionAni( pChar, Attri );

	m_iAttackCount++;
	m_BuffState = BS_ATTACK;

	//패킷
	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(BST_ATTACK);
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		kPacket << m_szAttackTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTeleportAttackBuff::SetTeleport( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAttackTarget );
	if( !pTarget )
		return;
	D3DXVECTOR3	vNewPos = pTarget->GetWorldPosition() + pTarget->GetWorldOrientation() * m_vTargetTeleportOffset;

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vNewPos.x, vNewPos.z );
	if ( vNewPos.y <= fMapHeight )
		vNewPos.y = fMapHeight;

	pChar->SetWorldPosition( vNewPos );
	pChar->SetCurMoveSpeed( 0.0f );

	SetTargetRotation( pChar );
}

void ioTargetTeleportAttackBuff::SetTargetRotation( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAttackTarget );

	if( !pTarget )
		return;

	D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	pChar->SetTargetRotToDir( vAimedDir, true );
}

void ioTargetTeleportAttackBuff::SetActionAni( ioBaseChar* pChar, const AttackAttribute& Attri )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( Attri.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	float fTimeRate  = FLOAT1;
	if ( Attri.m_fAttackAniRate > 0.0f )
		fTimeRate = Attri.m_fAttackAniRate;
	DWORD dwPreDelay = Attri.m_dwPreDelay;

	DWORD dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( dwWeaponBaseIndex,
		iAniID,
		Attri.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwFireMotionEndTime = dwCurTime;

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + Attri.m_dwEndDelay;

	m_dwFireMotionEndTime += dwPreDelay + pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	pChar->SetReservedSliding( Attri.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

const AttackAttribute& ioTargetTeleportAttackBuff::GetActionAttribute( ioBaseChar* pChar )
{
	static AttackAttribute attri;
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAttackTarget );
	if( pTarget )
	{
		//Air
		if( IsAirState( pTarget ) )
		{
			if ( m_AirTargetAttackAttribute.m_AttackAnimation.IsEmpty() )
				return m_NormalTargetAttackAttribute;
			return m_AirTargetAttackAttribute;
		}
		//Down
		else if ( IsDownState( pTarget ) )
		{
			if ( m_DownTargetAttackAttribute.m_AttackAnimation.IsEmpty() )
				return m_NormalTargetAttackAttribute;
			return m_DownTargetAttackAttribute;
		}
		//Normal
		else
		{
			return m_NormalTargetAttackAttribute;
		}

	}
	return attri;
}

bool ioTargetTeleportAttackBuff::IsAirState( ioBaseChar* pTarget )
{
	if( pTarget->IsFloating() )
		return true;

	return false;
}

bool ioTargetTeleportAttackBuff::IsDownState( ioBaseChar* pTarget )
{
	CharState eState = pTarget->GetState();
	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() )
		return true;
	else if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;
	else if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;
	else if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) )
		return true;
	else if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() )
		return true;
	else if( eState == CS_ICE_STATE && pTarget->GetBlowStopMotionState() )
		return true;
	else if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) )
		return true;

	return false;
}

void ioTargetTeleportAttackBuff::SetCancelState( ioBaseChar* pChar )
{
	if( !m_vDeleteBuffName.empty() )
	{
		for( int i = 0; i < (int)m_vDeleteBuffName.size(); i++ )
		{
			pChar->RemoveBuff( m_vDeleteBuffName[i] );
		}
	}

	SetReserveEndBuff();

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(BST_CANCEL);
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTeleportAttackBuff::SetActionEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	if ( pChar->IsNeedProcess() )
	{
		if ( pChar->IsFloating() )
			pChar->SetSKillEndJumpState( m_fEndJumpPower, false );
		else
			pChar->SetState( CS_DELAY, true );
	}
	m_szAttackTarget.Clear();
	m_dwKeyInputTime = 0;
	m_bInputKey = false;

	if( !m_vDeleteBuffName.empty() )
	{
		for( int i = 0; i < (int)m_vDeleteBuffName.size(); i++ )
		{
			pChar->RemoveBuff( m_vDeleteBuffName[i] );
		}
	}

	m_BuffState = BS_LOOP;
}

void ioTargetTeleportAttackBuff::EndBuff()
{
	ioBuff::EndBuff();
	if ( !GetOwner() || !m_pOwner )
		return;

	if ( m_bReduceAllGauge )
		m_pOwner->SetCurItemSkillGauge( m_ItemName, 0 );
	ioPlayStage *pStage = m_pOwner->GetCreator();
	if( pStage )
	{
		DestroyMaker( m_pOwner, pStage, m_dwMarkMapEffectID );
		DestroyRangeEffect( m_pOwner, pStage, m_dwRangeMapEffectID );
	}
	ClearData();
}

void ioTargetTeleportAttackBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	BYTE btState;
	rkPacket >> btState;

	switch( btState )
	{
	case BST_CHANGE_TARGET:
		{
			ioHashString	szTarget;
			rkPacket >> szTarget;
			
			if( m_szAttackTarget != szTarget )
			{
				m_szAttackTarget	= szTarget;
				if ( !m_szAttackTarget.IsEmpty() )
					CreateMarker( pOwner, pStage, m_szAttackTarget, m_szMarkEffect, m_dwMarkMapEffectID );
				else
					DestroyMaker( pOwner, pStage, m_dwMarkMapEffectID );
			}
		}
		break;
	case BST_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			pOwner->SetCurMoveSpeed( 0.0f );
			rkPacket >> m_szAttackTarget;
			SetActionState( pOwner, pStage );
		}
		break;
	case BST_CANCEL:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			SetCancelState( pOwner );
		}
		break;
	}
}

bool ioTargetTeleportAttackBuff::IsProtected() const
{
	if( m_dwFireMotionEndTime > 0 && m_dwFireMotionEndTime > FRAMEGETTIME() )
	{
		return true;
	}
	return false;
}
