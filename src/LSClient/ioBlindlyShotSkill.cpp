

#include "stdafx.h"

#include "ioBlindlyShotSkill.h"

#include "ioBaseChar.h"

#include "ioGrowthUpInfo.h"

ioBlindlyShotSkill::ioBlindlyShotSkill()
{
	ClearData();
}

ioBlindlyShotSkill::ioBlindlyShotSkill( const ioBlindlyShotSkill &rhs )
	: ioAttackSkill( rhs ),
m_szPreLoopMotion( rhs.m_szPreLoopMotion ),
m_szRunLoopMotion( rhs.m_szRunLoopMotion ),
m_fPreMotionRate( rhs.m_fPreMotionRate ),
m_fRunLoopMotionRate( rhs.m_fRunLoopMotionRate ),
m_fNeedFireGuage( rhs.m_fNeedFireGuage ),
m_dwMaxBulletCnt( rhs.m_dwMaxBulletCnt ),
m_dwCurBulletCnt( rhs.m_dwCurBulletCnt ),
m_OwnerBuffList( rhs.m_OwnerBuffList ),
m_bDisableGrowth( rhs.m_bDisableGrowth ),
m_fGuagePerTic( rhs.m_fGuagePerTic ),
m_dwTicTime( rhs.m_dwTicTime ),
m_PreAttackAttribute( rhs.m_PreAttackAttribute ),
m_bNotUseActiveCount( rhs.m_bNotUseActiveCount )
{
	ClearData();
}

ioBlindlyShotSkill::~ioBlindlyShotSkill()
{
}

void ioBlindlyShotSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );
	m_szPreLoopMotion = szBuf;
	m_fPreMotionRate = rkLoader.LoadFloat_e( "delay_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_szRunLoopMotion = szBuf;
	m_fRunLoopMotionRate = rkLoader.LoadFloat_e( "move_animation_rate", FLOAT1 );

	m_fNeedFireGuage = rkLoader.LoadFloat_e( "need_fire_guage", FLOAT1 );

	m_dwMaxBulletCnt = rkLoader.LoadInt_e( "bullet_cnt", 1 );

	LoadOwnerBuffList( rkLoader );

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	LoadAttackAttribute_e( "pre_attack", m_PreAttackAttribute, rkLoader );

	m_bNotUseActiveCount = rkLoader.LoadBool_e( "not_use_active_count", false );
}

void ioBlindlyShotSkill::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

ioSkill* ioBlindlyShotSkill::Clone()
{
	return new ioBlindlyShotSkill( *this );
}

void ioBlindlyShotSkill::ClearData()
{
	m_bSetHandMesh = false;

	m_PreMotionState = MS_RUN;
	m_CurMotionState = MS_PRE;

	m_State = SS_PRE;
	m_SubState = ASS_ACTION;

	m_bReduceGauge = false;
	m_dwCurBulletCnt = m_dwMaxBulletCnt;

	m_dwKeyReserveTime = 0;

	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
}

bool ioBlindlyShotSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );	
	
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	ClearData();
		
	pChar->ClearSkillReserveKey();

	m_dwCheckTime = FRAMEGETTIME();
	if( !m_bDisableGrowth )
	{		
		ioPlayStage *pStage = pChar->GetCreator();
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

	return true;
}

void ioBlindlyShotSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	switch( m_State )
	{
	case SS_PRE: 
		{		
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				if ( !m_PreAttackAttribute.m_AttackAnimation.IsEmpty() )
					SetPreAttackState( pChar, pStage );
				else
				{
					SetLoopState( pChar, pStage );
					SetOwnerBuffList( pChar, pStage );
				}
			}
		}
		break;
	case SS_PRE_ACTION:
		if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
		{
			SetLoopState( pChar, pStage );
			SetOwnerBuffList( pChar, pStage );
			return;
		}
		break;
	case SS_LOOP:
		{			
			if( pChar->IsSettedDirection() )
			{
				m_CurMotionState = MS_RUN;
				CheckMove( pChar, pStage );
			}
			else			
			{
				m_CurMotionState = MS_PRE;
				CheckMove( pChar, pStage );
			}

			if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
			{
				//AAS_END, AAS_LOOP로 변환(isNeedProcess()가 true일 경우)
				//_클라이언트가 스킬 시전자가 아닐 경우 ASS_ACTION으로 고대로감
				SetActionState( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		{
			if( 0 < m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime && m_SubState != ASS_END )
			{
				if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
				{
					m_SubState = ASS_ACTION;
				}
			}

			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				if( m_SubState == ASS_ACTION )
				{					
					SetActionState( pChar, pStage );
				}
				else if( m_SubState == ASS_LOOP )
				{					
					SetLoopState( pChar, pStage );

					m_PreMotionState = MS_PRE;
					m_CurMotionState = MS_RUN;
				}
				else
				{
					SetEndState( pChar, pStage );
				}
			}
		}
		break;
	case SS_END:
		{

		}
		break;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;
	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		if( fCurTicGauge != 0.0f )
		{
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f 
				&& ( m_State != SS_PRE_ACTION && m_State != SS_ACTION ) )
			{
				SetEndState( pChar, pStage );
			}
		}
	}
}

void ioBlindlyShotSkill::SetActionState( ioBaseChar *pChar, ioPlayStage* pStage, bool bSend )
{
	if( !pChar )
		return;

	pChar->ReduceCurSkillGaugeBySkill( NULL, m_fNeedFireGuage );
	
	//마지막 발 사격 후 종료!
	if(  m_dwCurBulletCnt == 1 )
		m_SubState = ASS_END;	
	else		
		m_SubState = ASS_LOOP;		

	pChar->SetSkillMoveEnable( false, false );
	m_CurMotionState = MS_PRE;

	m_State = SS_ACTION;
	ioAttackSkill::AttackSkillFire( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate = GetAttackTimeRate();

	m_dwKeyReserveTime += FRAMEGETTIME();
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	
	DWORD dwTrackingTime = 0;

	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
		if( iAniID != -1 )
		{
			dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationFullTime( iAniID );

			if( IsUseAutoTargetValue() )
			{
				pChar->SetAutoTarget( ATT_SKILL );
				pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
			}
		}
	}

	m_dwCurBulletCnt--;
	m_dwCurBulletCnt = max( 0, m_dwCurBulletCnt );

	if( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBlindlyShotSkill::SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	m_State = SS_LOOP;	
	DWORD dwCurTime = FRAMEGETTIME();
	
	pChar->SetSkillMoveEnable( true, true );
	CheckMove( pChar, pStage );

	m_dwKeyReserveTime = 0;

	//Auto Target Clear
	pChar->SetAutoTargetTrackingEndTime( 0 );
}

void ioBlindlyShotSkill::SetEndState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	m_State = SS_END;

	//Auto Target Clear
	pChar->SetAutoTargetTrackingEndTime( 0 );
}

void ioBlindlyShotSkill::CheckMove( ioBaseChar* pChar, ioPlayStage* pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;	

	if( m_PreMotionState != m_CurMotionState && m_CurMotionState == MS_RUN )
	{
		iAniID = pGrp->GetAnimationIdx( m_szRunLoopMotion );
		if( iAniID == -1 )		
			return;

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );		
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fRunLoopMotionRate );
		m_PreMotionState = m_CurMotionState;
	}
	else if( m_PreMotionState != m_CurMotionState && m_CurMotionState == MS_PRE )
	{
		iAniID = pGrp->GetAnimationIdx( m_szPreLoopMotion );
		if( iAniID == -1 )
			return;

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fPreMotionRate );
		m_PreMotionState = m_CurMotionState;
	}
}

void ioBlindlyShotSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}
	
	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;
	ClearData();

	pChar->SetSkillMoveEnable( false, false );	
	pChar->ClearSkillReserveKey();
}

bool ioBlindlyShotSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_State )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_ACTION:
	case SS_PRE_ACTION:
		return false;	
	case SS_END:
		return true;
	}
	return true;
}

bool ioBlindlyShotSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioBlindlyShotSkill::IsCanSwitchSkill() const
{
	if( m_dwEnableSwitchTime > 0 && m_dwEnableSwitchTime < FRAMEGETTIME() )
		return true;

	return false;
}

void ioBlindlyShotSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
}

void ioBlindlyShotSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	
	if( iState == SSC_ACTION )
	{
		D3DXQUATERNION qRot;
		rkPacket >> qRot;
		pChar->SetTargetRotToRotate( qRot, true, true );

		SetActionState( pChar, pStage, false );
	}
}


bool ioBlindlyShotSkill::SetOwnerBuffList( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if( !pChar->IsNeedProcess() ) return false;
	if( m_OwnerBuffList.empty() ) return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpInfo )
	{		
		fValue = pUpInfo->GetValue(pStage);
	}

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
			continue;


		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuff( szBuffName, pChar->GetCharName(), szItemName, this );
			if( pBuff )
			{				
				pBuff->SetExtraDurationByGrowth( fValue );

				bResult = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}

	if( bResult )
	{
		int iCurBuffCnt = vBuffList.size();
		if( pChar->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff1 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pChar->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << true;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << fValue;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bResult;
}

void ioBlindlyShotSkill::SetPreAttackState( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if ( !pChar || !pStage )
		return;

	m_State = SS_PRE_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_PreAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate  = m_PreAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_PreAttackAttribute.m_dwPreDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_PreAttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionStartTime = m_dwMotionEndTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_PreAttackAttribute.m_dwEndDelay;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );
	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	
	pChar->SetReservedSliding( m_PreAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_bReduceGauge = true;
}
