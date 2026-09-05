#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioEntityGroup.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/HelpFunc.h"

#include "ioComboJumpAttackSkill.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioComboJumpAttackSkill::ioComboJumpAttackSkill(void)
{
	ClearData();
}

ioComboJumpAttackSkill::ioComboJumpAttackSkill( const ioComboJumpAttackSkill &rhs )
	:ioAttackSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_szRunLoopMotion( rhs.m_szRunLoopMotion ),
	m_fJumpPower( rhs.m_fJumpPower ),
	m_dwJumpDuration( rhs.m_dwJumpDuration ),
	m_dwKeyAddDuration( rhs.m_dwKeyAddDuration ),
	m_dwKeyEnableTime( rhs.m_dwKeyEnableTime ),
	m_WeaponInfo( rhs.m_WeaponInfo ),
	m_fGravityAmt( rhs.m_fGravityAmt ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_fSetHeightGap( rhs.m_fSetHeightGap )
{
	ClearData();
}

ioComboJumpAttackSkill::~ioComboJumpAttackSkill(void)
{
}

void ioComboJumpAttackSkill::ClearData()
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;

	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;

	m_dwLoopStartTime = 0;

	m_dwKeyCheckStartTime = 0;
	m_bAddKey = false;

	m_bEnd = false;

	m_bWeaponAttack = false;
}

void ioComboJumpAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_szRunLoopMotion = szBuf;

	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	m_dwJumpDuration = (DWORD)rkLoader.LoadInt_e( "jump_duration", 0 );

	m_dwKeyAddDuration	= (DWORD)rkLoader.LoadInt_e( "key_add_duration", 0 );
	m_dwKeyEnableTime	= (DWORD)rkLoader.LoadInt_e( "key_enable_time", 0 );

	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "landing_attack_type", NO_WEAPON_ATTRIBUTE );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "landing_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "attack_type_wounded", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = (DWORD)rkLoader.LoadInt_e( "landing_attack_type_wounded_time", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "landing_attack_type_loop_ani", false );

	m_fGravityAmt = rkLoader.LoadFloat_e( "attack_gravity_amt", 0.0f );
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_fSetHeightGap = rkLoader.LoadFloat_e( "landing_height_gap", FLOAT10 );
}

ioSkill* ioComboJumpAttackSkill::Clone()
{
	return new ioComboJumpAttackSkill( *this );
}

bool ioComboJumpAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();

	m_dwCheckTime = m_dwSkillStartTime = FRAMEGETTIME();

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_SkillSoundID = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	ioPlayStage *pStage = pChar->GetCreator();

	if( !m_bDisableGrowth )
	{		
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);
		}
	}

	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	// Delay Animation이 없다
	if( !StartPreDelayAnimation( pChar ) )
	{		
		SetLoopState( pChar );
	}

	SetCameraBuff( pChar );

	CheckChangeDirRot( pChar );

	return true;
}

void ioComboJumpAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwLoopStartTime > 0 && m_dwLoopStartTime + m_dwJumpDuration < dwCurTime )
			{
				SetLoopState( pChar );
			}

			if( pChar->IsNeedProcess() )
			{
				DWORD dwCheck1 = m_dwKeyCheckStartTime + m_dwKeyEnableTime;
				DWORD dwCheck2 = m_dwKeyCheckStartTime + m_dwKeyAddDuration;

				if ( !m_bAddKey && dwCheck1 < dwCheck2 )
				{
					if( m_dwKeyCheckStartTime > 0 && COMPARE( dwCurTime, dwCheck1, dwCheck2 ) )
					{
						if( pChar->IsAttackKey() )
						{
							m_bAddKey = true;
						}
					}
				}

				if( m_bAddKey )
				{
					m_bAddKey = false;
					m_dwKeyCheckStartTime = dwCurTime;
					SetActionState( pChar );
				}
			}
		}
		break;
	case SS_ACTION:
		{
			pChar->SetGravityAmt( m_fGravityAmt );
			if ( m_bWeaponAttack )
			{
				SetLoopState( pChar );
				m_bWeaponAttack = false;
			}

			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				if ( CheckLanding( pChar ) )
				{
					LandingExplosion( pChar );
					SetLoopState( pChar );
				}
			}
		}
		break;
	case SS_END:
		m_bEnd = true;
		break;
	}

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		if( fCurTicGauge != 0.0f )
		{
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
			{
				m_SkillState = SS_END;
				m_bEnd = true;
			}
		}
	}
}

void ioComboJumpAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	
	EndCameraBuff( pChar );
	ClearData();
}

void ioComboJumpAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );
	
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}

	ioContactWeapon *pContact = ToContactWeapon( pWeapon );
	if ( pContact )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if ( pChar )
		{
			pContact->SetSkillWeapon( true );
		}
	}
}

bool ioComboJumpAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_LOOP:
	case SS_ACTION:
		return false;

	case SS_END:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;
	}

	return true;
}

void ioComboJumpAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SS_ACTION:
		{
			D3DXQUATERNION qRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qRot;
			rkPacket >> vPos;
			pChar->SetTargetRotToRotate( qRot, true, true );
			pChar->SetWorldPosition( vPos );
			SetActionState( pChar );
		}
		break;
	}
}

void ioComboJumpAttackSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	
	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_szRunLoopMotion );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	pChar->SetJumpPower( m_fJumpPower );

	pChar->SetSkillMoveEnable( true, true );
	
	m_dwLoopStartTime = m_dwKeyCheckStartTime = dwCurTime;
}


bool ioComboJumpAttackSkill::CheckLanding( ioBaseChar *pChar )
{
	float fHeightGap = 0.0f;
	float fHeight = pChar->GetWorldPosition().y;
	float fMapHeight = pChar->GetBottomHeight();

	//if( pChar->CheckGhostState() )
	//{
	//	fHeightGap = fHeight - pChar->GetPreHeightForGhostState();
	//}
	
	//       if( m_fLastHeight > 0.0f && fMapHeight <= 0.0f )
	//{
	//	fHeightGap = fHeight - m_fLastHeight;
	//}
	//else
	//{
		fHeightGap = fHeight - fMapHeight;
	//}

	if( abs( fHeightGap ) > m_fSetHeightGap || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}

void ioComboJumpAttackSkill::SetActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_SkillState = SS_ACTION;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( GetSkillMotionName() );
	if( iAniID == -1 )
		return;

	float fTimeRate  = GetSkillMotionRate();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->SetSkillMoveEnable( false, false );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_AttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		0 );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ACTION;
		kPacket << pChar->GetTargetRot();
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioComboJumpAttackSkill::LandingExplosion( ioBaseChar* pChar )
{
	if( !pChar )	return;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SkillFireExplicit( m_WeaponInfo,	GetName(),
			m_dwWeaponBaseIndex, pChar->GetWorldPosition() );
}

bool ioComboJumpAttackSkill::IsEndState() const
{
	if( m_SkillState == SS_END && m_bEnd )
		return true;

	return false;
}

bool ioComboJumpAttackSkill::IsAttackEndState() const
{
	return IsEndState();
}

bool ioComboJumpAttackSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return ioSkill::IsProtected( iDefenseBreakType );
}