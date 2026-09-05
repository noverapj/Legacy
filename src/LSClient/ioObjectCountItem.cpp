
#include "stdafx.h"

#include "ioObjectCountItem.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

ioObjectCountItem::ioObjectCountItem()
{
	m_ObjectState = OS_NONE;
	m_dwAttackEndTime = 0;
	m_iCurCombo = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	
	m_iCurUseCount = 0;
	m_bFirstAttack = false;
}

ioObjectCountItem::ioObjectCountItem( const ioObjectCountItem &rhs )
	: ioObjectItem( rhs ),
	  m_bEnableObjectDefense( rhs.m_bEnableObjectDefense ),
	  m_bEnableObjectDash( rhs.m_bEnableObjectDash ),
	  m_dwProtectTime( rhs.m_dwProtectTime ),
	  m_bJumpAttackProtect( rhs.m_bJumpAttackProtect ),
	  m_bSetUseCount( rhs.m_bSetUseCount ),
	  m_bComboAttack( rhs.m_bComboAttack ),
	  m_ResetTimeList( rhs.m_ResetTimeList )
{
	m_ObjectState = OS_NONE;
	m_dwAttackEndTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	
	m_iCurUseCount = 0;
	m_bFirstAttack = false;
}

ioObjectCountItem::~ioObjectCountItem()
{
}

void ioObjectCountItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );
	
	m_bSetUseCount = rkLoader.LoadBool( "set_use_count", true );
	
	m_bEnableObjectDefense = rkLoader.LoadBool_e( "enable_object_defense", false );
	m_bEnableObjectDash = rkLoader.LoadBool_e( "enable_object_dash", false );

	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );
	m_bJumpAttackProtect = rkLoader.LoadBool_e( "jump_attack_protect", false );

	char szBuf[MAX_PATH];
	int nDelayCnt = rkLoader.LoadInt_e( "reset_time_cnt", 0 );
	m_ResetTimeList.clear();
	for ( int i=0; i<nDelayCnt; i++ )
	{
		wsprintf_e( szBuf, "reset_attack%d_time", i+1 );
		DWORD dwResetTime = rkLoader.LoadInt( szBuf, 0 );
		m_ResetTimeList.push_back( dwResetTime );
	}
}

ioItem* ioObjectCountItem::Clone()
{
	return new ioObjectCountItem( *this );
}

ioObjectItem::ObjectSubType ioObjectCountItem::GetObjectSubType() const
{
	return OST_COUNT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioObjectCountItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_ObjectState = OS_ATTACK;
	m_iCurCombo = iCurCombo;

	if ( m_bFirstAttack && !m_ResetTimeList.empty() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		int nListNum = 0;
		//중간 일때 리스트 처음부터
		if ( m_iCurCombo > 0 && m_iCurCombo < (int)m_ResetTimeList.size() )
			nListNum = m_iCurCombo - 1;
		//마지막 일때 리스트 마지막 값 적용
		else if ( m_iCurCombo == 0 )
			nListNum = (int)m_ResetTimeList.size() - 1;

		if ( m_dwAttackEndTime > 0 && m_dwAttackEndTime + m_ResetTimeList[ nListNum ] < dwCurTime )
		{
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
			m_iCurCombo = 0;
		}
	}

	pOwner->SetState( CS_ATTACK );

	m_bFirstAttack = true;
}

void ioObjectCountItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ObjectState )
	{
	case OS_ATTACK:
		SetAttackState( pOwner );
		break;
	case OS_ATTACK_END:
		SetAttackEndState( pOwner );
		break;
	}
}

bool ioObjectCountItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) 
		return false;

	ClearAirJump();
	
	const AttackAttribute *pAttr = GetAttackAttribute( AT_JUMP_ATTACK );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectCountItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectCountItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	m_bSetJumpAttack = true;

	fTimeRate = pAttr->m_fAttackAniRate;
	SetAirJump( pOwner, iAniID, fTimeRate );

	CheckActiveCount( pOwner );

	//콤보수 초기화
	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	m_iCurCombo = 0;
	
	return false;
}

void ioObjectCountItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioObjectCountItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
	
	if( m_bSetJumpAttack )
		m_bSetJumpAttack = false;
}

void ioObjectCountItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case OS_ATTACK:
		{
			rkPacket >> m_iCurCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			SetAttackState( pOwner );
		}
		break;
	}
}

bool ioObjectCountItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwEquipTime + m_dwProtectTime > FRAMEGETTIME() )
	{
		return true;
	}

	if( m_bJumpAttackProtect && m_bSetJumpAttack )
	{
		return true;
	}

	return false;
}

void ioObjectCountItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );

	m_dwEquipTime = FRAMEGETTIME();
	m_bSetJumpAttack = false;
	m_iCurUseCount = 0;

	//콤보수 초기화
	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	m_iCurCombo = 0;
}

void ioObjectCountItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );
	m_bSetJumpAttack = false;
	m_iCurUseCount = 0;

	//콤보수 초기화
	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	m_iCurCombo = 0;
	m_bFirstAttack = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioObjectCountItem::IsOneTimeWeaponItem() const
{
	if( m_bSetUseCount && m_iMaxUseCount > 0 && m_iCurUseCount == m_iMaxUseCount )
		return true;

	return false;
}

bool ioObjectCountItem::IsEnableDash() const
{
	return m_bEnableObjectDash;
}

bool ioObjectCountItem::IsEnableDefense() const
{
	return m_bEnableObjectDefense;
}

void ioObjectCountItem::CheckAttackCount( ioBaseChar *pOwner )
{
	m_iCurUseCount++;
	m_iCurUseCount = min( m_iCurUseCount, m_iMaxUseCount );
}

void ioObjectCountItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}

bool ioObjectCountItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;
			
			if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetDashState( false );
				pOwner->SendDashState( false );
				
				return true;
			}
			else if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

//현재 콤보 수를 초기화 할것인가?
bool ioObjectCountItem::IsAttackComboInit() const
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioObjectCountItem::SetAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
		
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
		
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );	

	m_ObjectState = OS_ATTACK_END;
	CheckActiveCount( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ObjectState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectCountItem::SetAttackEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	pOwner->CheckKeepComboAttack();

	//공격 종료 시간 갱신
	m_dwAttackEndTime = FRAMEGETTIME();

	//공격 종료시 콤보수 초기화(예외 대비)
	if ( m_bSetUseCount && m_iCurUseCount >= m_iMaxUseCount )
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	}
}

void ioObjectCountItem::CheckActiveCount( ioBaseChar *pOwner )
{
	ioObjectItem::CheckActiveCount( pOwner );

	if( !pOwner || !m_bSetUseCount )
		return;

	if( !m_ActiveCountBuff.IsEmpty() )
	{
		ioBuff *pBuff = pOwner->GetBuff( m_ActiveCountBuff );
		ioActiveCountBuff2* pActivetBuff = ToActiveCountBuff2( pBuff );
		if( pActivetBuff )
			pActivetBuff->CheckActiveCount();
	}
}