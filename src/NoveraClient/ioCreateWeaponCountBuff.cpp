

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioCreateWeaponCountBuff.h"

ioCreateWeaponCountBuff::ioCreateWeaponCountBuff()
{
	m_dwDelayEffectID = -1;
	m_dwCheckFireTime = 0;
	m_dwCheckGavityTime = 0;
}

ioCreateWeaponCountBuff::ioCreateWeaponCountBuff( const ioCreateWeaponCountBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_dwFireGapTime( rhs.m_dwFireGapTime ),
m_WeaponInfo( rhs.m_WeaponInfo ),
m_szDelayEffect( rhs.m_szDelayEffect ),
m_szFireEffect( rhs.m_szFireEffect ),
m_dwGravityTime( rhs.m_dwGravityTime ),
m_fGravityRate( rhs.m_fGravityRate ),
m_szFireSound( rhs.m_szFireSound ),
m_CreateWeaponType( rhs.m_CreateWeaponType ),
m_nTotalWeaponCnt( rhs.m_nTotalWeaponCnt )
{
	m_dwDelayEffectID = -1;
	m_dwCheckFireTime = 0;
	m_dwCheckGavityTime = 0;

	//현재 무기 생성한 갯수
	m_nCurWeaponCnt = 0;
}

ioCreateWeaponCountBuff::~ioCreateWeaponCountBuff()
{
}

void ioCreateWeaponCountBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );

	m_dwFireGapTime = rkLoader.LoadInt_e( "fire_gap_time", 0 );

	char szBuf[MAX_PATH] = "";
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "attack_index", NO_WEAPON_ATTRIBUTE );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "attack_index_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "attack_wounded", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "attack_wounded_time", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attack_wounded_loop_ani", false );

	rkLoader.LoadString_e( "delay_effect", "", szBuf, MAX_PATH );
	m_szDelayEffect = szBuf;
	rkLoader.LoadString_e( "attack_effect", "", szBuf, MAX_PATH );
	m_szFireEffect = szBuf;

	m_dwGravityTime = rkLoader.LoadInt_e( "gravity_time", 0 );
	m_fGravityRate = rkLoader.LoadFloat_e( "gravity_amt", 0.0f );

	rkLoader.LoadString_e( "fire_sound", "", szBuf, MAX_PATH );
	m_szFireSound = szBuf;

	//추가 읽기
	LoadWeaponCreateProperty( rkLoader );
}

ioBuff* ioCreateWeaponCountBuff::Clone()
{
	return new ioCreateWeaponCountBuff( *this );
}

void ioCreateWeaponCountBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwDelayEffectID = -1;

	m_BuffState = BS_DELAY;

	m_dwCheckFireTime = FRAMEGETTIME();
	m_dwCheckGavityTime = 0;

	//현재 무기 생성한 갯수
	m_nCurWeaponCnt = 0;
}

bool ioCreateWeaponCountBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwDelayEffectID = -1;

	m_BuffState = BS_DELAY;
	m_dwCheckFireTime = FRAMEGETTIME();		
	m_dwCheckGavityTime = 0;

	//현재 무기 생성한 갯수
	m_nCurWeaponCnt = 0;

	return true;
}

void ioCreateWeaponCountBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		//종료 조건 추가
		if( !CheckSwitchType( fTimePerSec ) || 
			( m_CreateWeaponType == CWBT_COUNT && m_nCurWeaponCnt >= m_nTotalWeaponCnt ))
		{
			if( GetOwner() && CheckRestoreStateCheck() )
				GetOwner()->SetState( CS_DELAY, m_bSendDelayNetWork );

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}		
		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				if( CheckRestoreStateCheck() )
				{
					m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				}

				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	ProcessState();
	ProcessKeyInput();
}

bool ioCreateWeaponCountBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioCreateWeaponCountBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

void ioCreateWeaponCountBuff::EndBuff()
{
	if( m_dwDelayEffectID != -1 )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwDelayEffectID );
		if( pEffect )
		{
			pEffect->EndEffectForce();
			m_dwDelayEffectID = -1;
		}
	}

	ioBuff::EndBuff();

	if ( m_CreateWeaponType == CWBT_COUNT )
	{
		//스킬게이지 설정
		SetSkillGauge( 0.0f );
	}
}

void ioCreateWeaponCountBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );
	rkPacket << (int)m_BuffState;
}

void ioCreateWeaponCountBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::ApplyBuffInfo( rkPacket );
	int iState;
	rkPacket >> iState;
	m_BuffState = (BuffState)iState;
}

void ioCreateWeaponCountBuff::ProcessKeyInput()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	CharState eState = pOwner->GetState();
	if( eState == CS_WOUNDED ||
		eState == CS_BLOW_WOUNDED ||
		eState == CS_BOUND_BLOW_WOUNDED ||
		eState == CS_GRAPPLING_WOUNDED ||
		eState == CS_GRAPPLING_PUSHED ||
		eState == CS_GRAPPLING_WOUNDED_SKILL ||
		eState == CS_GRAPPLING_WOUNDED_SWING )
		return;

	if( pOwner->IsAttackKey() && pOwner->IsNeedProcess() )
	{
		if( m_BuffState == BS_DELAY )
		{
			//카운트에 의한 무기 생성이면 카운트
			if ( m_CreateWeaponType == CWBT_COUNT && m_nCurWeaponCnt < m_nTotalWeaponCnt )
			{
				m_nCurWeaponCnt++;
			}
			else if ( m_CreateWeaponType == CWBT_COUNT && m_nCurWeaponCnt >= m_nTotalWeaponCnt )
			{
				return;
			}

			m_BuffState = BS_FIRE;
			CreateWeapon();
		}
	}
}

void ioCreateWeaponCountBuff::ProcessState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	switch( m_BuffState )
	{
	case BS_DELAY:
		{
			if( m_dwDelayEffectID == -1 )
			{
				ioEffect *pEffect = pOwner->AttachEffect( m_szDelayEffect );			
				if( pEffect )
					m_dwDelayEffectID = pEffect->GetUniqueID();
			}
		}
		break;
	case BS_FIRE:
		{
			if( m_dwCheckFireTime + m_dwFireGapTime < FRAMEGETTIME() )
			{
				m_BuffState = BS_DELAY;
				m_dwCheckFireTime = FRAMEGETTIME();
			}

			if( m_dwDelayEffectID != -1 )
			{
				ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwDelayEffectID );
				if( pEffect )
				{
					pEffect->EndEffectForce();
					m_dwDelayEffectID = -1;
				}
			}
		}
		break;
	}

	if( m_dwCheckGavityTime > 0 )
	{
		if( m_dwCheckGavityTime + m_dwGravityTime > FRAMEGETTIME() )
		{
			pOwner->SetGravityAmt( m_fGravityRate );
		}
		else
		{
			m_dwCheckGavityTime = 0;
		}
	}
}

void ioCreateWeaponCountBuff::CreateWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase();
	pCreator->DummyExplicit( m_WeaponInfo,
							 vPos,
							 dwWeaponIndex,
							 false,
							 pOwner->GetCharName(),
							 m_Name,
							 m_bUseCallAttWeaponCol );

	pOwner->AttachEffect( m_szFireEffect );

	m_dwCheckGavityTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (int)m_BuffState;
		//추가
		kPacket << m_nCurWeaponCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp &&!m_szFireSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szFireSound, pGrp->GetParentSceneNode() );
	}
}

void ioCreateWeaponCountBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase();
	pCreator->DummyExplicit( m_WeaponInfo,
							 vPos,
							 dwWeaponIndex,
							 false,
							 pOwner->GetCharName(),
							 m_Name,
							 m_bUseCallAttWeaponCol );

	pOwner->AttachEffect( m_szFireEffect );

	m_dwCheckGavityTime = FRAMEGETTIME();

	int iState;
	rkPacket >> iState;
	m_BuffState = (BuffState)iState;

	//추가
	rkPacket >> m_nCurWeaponCnt;

	m_dwCheckFireTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp &&!m_szFireSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szFireSound, pGrp->GetParentSceneNode() );
	}
}

//추가 읽기
void ioCreateWeaponCountBuff::LoadWeaponCreateProperty( ioINILoader &rkLoader )
{
	m_CreateWeaponType = (CreateWeaponByType)rkLoader.LoadInt_e( "set_weapon_create_type", CWBT_NORMAL );
	m_nTotalWeaponCnt = rkLoader.LoadInt_e( "total_weapon_count", 0 );
}

//스킬게이지 설정
void ioCreateWeaponCountBuff::SetSkillGauge( float fSkillGauge )
{
	//버프의 주인 얻기
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	//주인의 해당 아이템의 스킬 게이지를 재설정
	pOwner->SetCurItemSkillGauge( m_ItemName, fSkillGauge );
}