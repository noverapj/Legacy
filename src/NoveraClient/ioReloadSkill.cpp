

#include "stdafx.h"

#include "ioReloadSkill.h"
#include "ItemDefine.h"

ioReloadSkill::ioReloadSkill()
{
}

ioReloadSkill::ioReloadSkill( const ioReloadSkill &rhs )
: ioNormalSkill( rhs ),
m_vForceInfoList( rhs.m_vForceInfoList ),
m_vDownForceInfoList( rhs.m_vDownForceInfoList ),
m_OwnerBuffList( rhs.m_OwnerBuffList )
{
}

ioReloadSkill::~ioReloadSkill()
{
}

void ioReloadSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "reload_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_vForceInfoList.reserve( iForceCnt );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "reload_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "reload_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		wsprintf_e( szBuf, "reload_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vForceInfoList.push_back( kInfo );
	}

	m_vDownForceInfoList.clear();
	int iDownForceCnt = rkLoader.LoadInt_e( "down_reload_force_cnt", 0 );
	if( iDownForceCnt > 0 )
		m_vDownForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iDownForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "down_reload_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "down_reload_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		wsprintf_e( szBuf, "down_reload_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vDownForceInfoList.push_back( kInfo );
	}

	ioBuffInfo tBuffInfo;
	m_OwnerBuffList.clear();
	int iCount = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i=0; i<iCount; ++i )
	{
		wsprintf_e( szKey, "owner_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
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

ioSkill* ioReloadSkill::Clone()
{
	return new ioReloadSkill( *this );
}

bool ioReloadSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_bChangeDirRot = false;
	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	CheckChangeDirRot( pChar );

	if( !StartPreDelayAnimation( pChar ) )	// Delay AnimationÀÌ ¾ø´Ù
	{
		SetNormalSkill( pChar );
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	for (int i=0; i < (int)m_OwnerBuffList.size(); ++i)
	{
		ioBuffInfo pOwnerBuff = m_OwnerBuffList[i];
		pChar->AddNewBuff( pOwnerBuff.m_BuffName, pChar->GetCharName(),	GetOwnerItem()->GetName(), this);					
	}
	return true;
}

void ioReloadSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 ) return;

	DWORD dwPreDelayTime = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetNormalSkill( pChar );
	}
}

void ioReloadSkill::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		if( !m_vForceInfoList.empty() )
			pChar->SetReservedSliding( m_vForceInfoList, iCurSkillAniID, fTimeRate, 0 );
		break;
	case SUT_DOWN:
		if( !m_vDownForceInfoList.empty() )
			pChar->SetReservedSliding( m_vDownForceInfoList, iCurSkillAniID, fTimeRate, 0 );
		break;
	}

	if( m_dwMotionStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	// Reload
	ioItem *pItem = pChar->GetEquipedItem(ES_WEAPON);
	ioAutoShotItem *pAutoShotItem = ToAutoShotItem( pItem );
	if( pAutoShotItem )
	{
		int iMax = pAutoShotItem->GetMaxBullet();
		pAutoShotItem->SetCurBullet( iMax );
	}

	ioVulcanItem *pVulcanItem = ToVulcanItem( pItem );
	if( pVulcanItem )
	{
		int iMax = pVulcanItem->GetMaxBullet();
		pVulcanItem->SetCurBullet( iMax );
	}
}

void ioReloadSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );
}