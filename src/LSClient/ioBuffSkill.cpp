

#include "stdafx.h"

#include "ioBuffSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"


ioBuffSkill::ioBuffSkill()
{
	m_dwAttackEndTime = 0;
	m_dwPreDelayStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwAreaWeaponIndex = 0;
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;

	m_bRemoveEquipSkillBuff = false;

	m_SkillActivateEffectID = -1;
}

ioBuffSkill::ioBuffSkill( const ioBuffSkill &rhs )
: ioSkill( rhs ),
 m_OwnerBuffList( rhs.m_OwnerBuffList ),
 m_TargetBuffList( rhs.m_TargetBuffList ),
 m_EnemyBuffList( rhs.m_EnemyBuffList ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_bNeedTarget( rhs.m_bNeedTarget ),
 m_bOnOffEnable( rhs.m_bOnOffEnable ),
 m_dwOnOffEnableTime( rhs.m_dwOnOffEnableTime ),
 m_bUseBuffMotionEnd( rhs.m_bUseBuffMotionEnd ),
 m_bStateEscape( rhs.m_bStateEscape ),
 m_bSkillMoveEnableDelayTime( rhs.m_bSkillMoveEnableDelayTime ),
 m_SkillActivateEffect( rhs.m_SkillActivateEffect ),
 m_vForceInfoList( rhs.m_vForceInfoList )
{
	m_dwAttackEndTime = 0;
	m_dwPreDelayStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwAreaWeaponIndex = 0;
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;

	m_bRemoveEquipSkillBuff = false;
	m_SkillActivateEffectID = -1;
}

ioBuffSkill::~ioBuffSkill()
{
}

void ioBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bNeedTarget = rkLoader.LoadBool_e( "need_target", false );
	m_bOnOffEnable = rkLoader.LoadBool_e( "on_off_enable", false );
	m_dwOnOffEnableTime = (DWORD)rkLoader.LoadInt_e( "on_off_enable_time", 0 );
	m_bUseBuffMotionEnd = rkLoader.LoadBool_e( "use_buff_motion_end", false );
	m_bStateEscape = rkLoader.LoadBool_e( "state_escape", false );
	m_bSkillMoveEnableDelayTime = rkLoader.LoadBool_e( "skill_move_enable_delay_time", false );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;
	
	rkLoader.LoadString_e( "skill_activate_effect", "", szBuf, MAX_PATH );
	m_SkillActivateEffect = szBuf;

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "skill_motion_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		m_vForceInfoList.reserve( iForceCnt );
	}

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "skill_motion_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "skill_motion_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vForceInfoList.push_back( kInfo );
	}

	LoadBuffList( rkLoader );
}

ioSkill* ioBuffSkill::Clone()
{
	return new ioBuffSkill( *this );
}

int ioBuffSkill::GetTypeID() const
{
	return ST_BUFF;
}

bool ioBuffSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !CheckEnableStart( pChar ) )
		return false;

	if( !m_bNoCheckSkillmotion && !HasSkillMotion() )
		return false;

	// 타겟이 필요한 버프가 있는지 체크
	if( m_bNeedTarget )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( GetOwnerItem()->GetTargetName() );
		if( !pTarget )
			return false;
	}

	if( !ioSkill::OnSkillStart( pChar ) )
		return false;

	m_dwAttackEndTime = 0;
	m_dwPreDelayStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_bSetHandMesh = false;
	m_bRemoveEquipSkillBuff = false;

	m_SkillState = SS_NONE;

	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );

		// 사용 아이템 처리
		if( !m_HandMesh.IsEmpty() )
		{
			pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
			m_bSetHandMesh = true;
		}
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		BuffSkillSet( pChar );
	}

	if( m_bSkillMoveEnableDelayTime )
		pChar->SetSkillMoveEnable( true, true );

	if( m_SkillActivateEffectID != -1 )
	{
		pChar->EndEffect( m_SkillActivateEffectID, false );
		m_SkillActivateEffectID = -1;
	}

	return true;
}

void ioBuffSkill::OnSkillEnd( ioBaseChar *pChar )
{
	pChar->SetSkillMoveEnable( false, false );
	EndPreDelayEffect( pChar );

	if( m_bUseBuffMotionEnd )
	{
		m_bReduceGauge = true;
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	}
	
	GetOwnerItem()->ClearTarget();

	// 기본무기 장착
	if( m_bUseItemMesh && IsCanShowWeaponItemMesh( pChar ) )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;

	pChar->EndEffect( m_SkillEffectName );

	ioSkill::OnSkillEnd( pChar );
}

bool ioBuffSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	// 온/오프 가능이고, 사용중인지 체크
	if( m_bOnOffEnable )
	{
		if( m_dwOnOffEnableTime <= 0 )
		{
			// 버프/영역 체크
			if( CheckUseBuff( pChar ) )
			{
				return true;
			}
		}
		else if( m_dwOnOffEnableTime > 0 && m_dwSkillStartTime + m_dwOnOffEnableTime <= FRAMEGETTIME() )
		{
			// 버프/영역 체크
			if( CheckUseBuff( pChar ) )
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else if( CheckUseBuff( pChar ) )
	{
		return false;
	}

	// 사용가능인지 체크
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable && !pAttackable->IsCanFire( pChar ) )
		return false;

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

void ioBuffSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();

			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				pChar->SetSkillMoveEnable( false, false );
				BuffSkillSet( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			DWORD dwLooseTime = pChar->GetActionStopTotalDelay();

			if( IsSkillMotionEnd( dwLooseTime, pChar ) )
			{
				m_SkillState = SS_END;
			}
		}
		break;
	}
}

void ioBuffSkill::OnProcessNotUseSkill( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;
	if( pChar->GetState() == CS_USING_SKILL )	return;

	if( m_SkillActivateEffectID == -1 && CheckGaugeMax( pChar->GetCharName() ) )
	{
		ioEffect *pEff = pChar->AttachEffect( m_SkillActivateEffect );
		if( pEff )
			m_SkillActivateEffectID = pEff->GetUniqueID();
	}
	ioSkill::OnProcessNotUseSkill( pChar, pStage );
}

void ioBuffSkill::OnSkillItemRelease( ioBaseChar *pChar )
{
	if( !pChar )	return;
	
	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )	return;

	if( m_SkillActivateEffectID != -1 )
	{
		pChar->EndEffect( m_SkillActivateEffectID, false );
		m_SkillActivateEffectID = -1;
	}
	ioSkill::OnSkillItemRelease(pChar);
}

void ioBuffSkill::BuffSkillSet( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioBuffSkill::BuffSkillSet - Not Exist SkillMotion: %s", szSkillMotion.c_str() );
		return;
	}

	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )* fTimeRate;

	m_dwFireStartTime = m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = FRAMEGETTIME() + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	if( !m_vForceInfoList.empty() )
	{
		pChar->SetReservedSliding( m_vForceInfoList, iCurSkillAniID, fTimeRate, 0 );
	}

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( !m_bUseBuffMotionEnd )
	{
		ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
		if( pStage && !m_SkillMapEffectName.IsEmpty() )
		{
			D3DXVECTOR3 vPos = pChar->GetWorldPosition();
			D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
			pStage->CreateMapEffect( m_SkillMapEffectName, vPos, vScale );
		}

		AddOwnerBuff( pChar );
		AddTargetBuff( pChar );
		CreateAreaWeapon( pChar );

		m_bReduceGauge = true;
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	}

	m_SkillState = SS_ACTION;
}

bool ioBuffSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	ioHashString szPreDelay = GetPreDelayAniName();

	if( szPreDelay.IsEmpty() )
	{
		m_dwPreDelayStartTime = 0;
		return false;
	}
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( szPreDelay );
	if( iAniID == -1) return false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );
	
	m_dwPreDelayStartTime = FRAMEGETTIME();
	StartPreDelayEffect( pChar );
	
	return true;
}

void ioBuffSkill::RemoveSkillBuff( ioBaseChar *pChar )
{
	if( pChar )
	{
		int iCnt = m_OwnerBuffList.size();

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_END_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << false;
			//kPacket << iCnt;
			ioHashStringVec szRemovedBuffList;
			for(int i=0; i < iCnt; ++i )
			{
				ioBuff *pBuff = pChar->GetBuff( m_OwnerBuffList[i].m_BuffName );
				if( pBuff && m_bRemoveEquipSkillBuff )
				{
					pBuff->CheckRemoveEquipSkillBuff( m_bRemoveEquipSkillBuff );
				}
				if( pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName ) )
					szRemovedBuffList.push_back( m_OwnerBuffList[i].m_BuffName );
				//kPacket << m_OwnerBuffList[i].m_BuffName;
			}

			kPacket << (int)szRemovedBuffList.size();
			for ( int i=0 ; i<(int)szRemovedBuffList.size() ; ++i )
				kPacket << szRemovedBuffList[i];

			P2PNetwork::SendToAllPlayingUser( kPacket );			
		}
		else
		{
			for(int i=0; i < iCnt; ++i )
			{
				pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
			}
		}
	}
}

void ioBuffSkill::RemoveSkillBuff( ioBaseChar *pChar, const ioHashString &szExceptName )
{
	ioHashStringVec vNameList;

	if( pChar )
	{
		int iCnt = m_OwnerBuffList.size();

		if( pChar->IsNeedSendNetwork() )
		{
			for(int i=0; i < iCnt; ++i )
			{
				if( m_OwnerBuffList[i].m_BuffName != szExceptName )
				{
					pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
					vNameList.push_back(m_OwnerBuffList[i].m_BuffName);
				}
			}

			int iSendSize = vNameList.size();
			SP2Packet kPacket( CUPK_END_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << false;
			kPacket << iSendSize;

			for(int i=0; i < iSendSize; ++i )
			{
				kPacket << vNameList[i];
			}

			P2PNetwork::SendToAllPlayingUser( kPacket );			
		}
		else
		{
			for(int i=0; i < iCnt; ++i )
			{
				if( m_OwnerBuffList[i].m_BuffName != szExceptName )
				{
					pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
				}
			}
		}
	}
}

void ioBuffSkill::LoadBuffList( ioINILoader &rkLoader )
{
	int i, iCount;
	ioBuffInfo tBuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_OwnerBuffList.clear();

	iCount = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );
		
		tBuffInfo.m_BuffName = szBuff;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}

	m_TargetBuffList.clear();
	iCount = rkLoader.LoadInt_e( "target_buff_cnt", 0 );
	for( i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "target_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		tBuffInfo.m_BuffName = szBuff;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_TargetBuffList.push_back( tBuffInfo );
			}
		}
	}

	m_EnemyBuffList.clear();
	iCount = rkLoader.LoadInt_e( "enemy_buff_cnt", 0 );
	for( i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "enemy_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		tBuffInfo.m_BuffName = szBuff;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_EnemyBuffList.push_back( tBuffInfo );
			}
		}
	}
}

bool ioBuffSkill::CheckEnableStart( ioBaseChar *pChar )
{
	m_bCanEnableBuffState = true;

	if( pChar && !pChar->IsNeedProcess() )
		return true;

	int iBuffSize = m_OwnerBuffList.size();
	int iCnt = 0;

	// 버프체크
	for( int i=0; i < iBuffSize; ++i )
	{
		if( pChar->HasBuff( m_OwnerBuffList[i].m_BuffName ) )
			iCnt++;
	}

	if( iCnt > 0 )
	{
		RemoveSkillBuff( pChar );
	}

	// 영역체크
	if( CheckUseAreaWeapon() )
	{
		RemoveAreaWeapon( pChar );
		iCnt++;
	}

	if( iCnt != 0 )
	{
		m_bCanEnableBuffState = false;
		return false;
	}

	return true;
}

void ioBuffSkill::CreateAreaWeapon( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_AreaWeaponName.IsEmpty() ) return;

	if( m_dwAreaWeaponIndex != 0 )
	{
		if( g_AreaWeaponMgr.FindAttachedAreaWeapon( m_dwAreaWeaponIndex ) )
		{
			LOG.PrintTimeAndLog( 0, "ioBuffSkill::CreateAreaWeapon - Create Fail! Already Exist: %d",
									m_dwAreaWeaponIndex );

			return;
		}
	}

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXQUATERNION qtRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																  pChar->GetCharName(),
																  GetName(),
																  m_AreaWeaponName,
																  vPos,
																  qtRot,
																  ioAreaWeapon::CT_ATTACHED );
	
	if( pAreaWeapon )
	{
		m_dwAreaWeaponIndex = pAreaWeapon->GetAreaWeaponIndex();
	}
}

void ioBuffSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( m_OwnerBuffList.empty() ) return;

	ioBaseChar *pTarget = pChar->GetBaseChar( GetOwnerItem()->GetTargetName() );
	if( !m_TargetBuffList.empty() && pTarget )
		return;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )
		return;

	//
	ioGrowthNormalUpInfo *pUpInfo = NULL;

	float fCoolValue = 0.0f;
	float fDurationValue = 0.0f;
	float fTicValue = 0.0f;

	// 쿨타임 성장요소 관련
	pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
	if( pUpInfo )
		fCoolValue = pUpInfo->GetValue(pStage);

	fCoolValue = min( fCoolValue, FLOAT100 );
	fCoolValue = FLOAT1 + (fCoolValue/FLOAT100);

	// 유지시간 성장요소 관련
	pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpInfo )
		fDurationValue = pUpInfo->GetValue(pStage);

	// 틱 성장요소 관련
	pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
	if( pUpInfo )
		fTicValue = pUpInfo->GetValue(pStage);

	//
	DWORD dwCreateItemCode = GetCreateItemCode( pChar );

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i].m_BuffName;
		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName,
															  pChar->GetCharName(),
															  szItemName,
															  this,
															  GetSkillMotionTime() );
			if( pBuff )
			{
				// 쿨타임 성장요소 관련
				pBuff->SetTicGaugeRateByGrowth( fCoolValue );

				// 유지시간 성장요소 관련
				pBuff->SetExtraDurationByGrowth( fDurationValue );

				// 틱 성장요소 관련
				pBuff->SetTicTimeByGrowth( fTicValue );
				
				if( dwCreateItemCode > 0 )
					pBuff->SetItemCode( dwCreateItemCode );

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
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff7 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pChar->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << false;					// 타격횟수 체크
			kPacket << fCoolValue;
			kPacket << fDurationValue;
			kPacket << fTicValue;
			kPacket << COWT_BUFFSKILL_OWNER;	// Check CloseOrderBuff(12.07.13 수정)
			kPacket << dwCreateItemCode;

			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioBuffSkill::AddTargetBuff( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	ioBaseChar *pTarget = pChar->GetBaseChar( GetOwnerItem()->GetTargetName() );
	if( !pTarget )
		return;

	if( pTarget->GetTeam() == pChar->GetTeam() )
	{
		if( m_TargetBuffList.empty() )
			return;

		int iCnt = m_TargetBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_TargetBuffList[i].m_BuffName;
			if( !szBuffName.IsEmpty() )
			{
				if( pTarget->IsHasCrown() && m_bDisableKingSkill )
				{
					continue;
				}

				ioBuff *pBuff = pTarget->AddNewBuffWithAnimateTime( szBuffName,
																	pChar->GetCharName(),
																	szItemName,
																	this,
																	GetSkillMotionTime() );

				if( pBuff && pChar )
				{
					pBuff->SetItemCode( pChar->GetWeaponItemCode() );
				}
			}
		}
	}
	else if( pTarget->GetTeam() != pChar->GetTeam() )
	{
		if( m_EnemyBuffList.empty() )
			return;

		if( pTarget->IsProtectState() )
			return;

		int iCnt = m_EnemyBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_EnemyBuffList[i].m_BuffName;
			if( !szBuffName.IsEmpty() )
			{
				if( pTarget->IsHasCrown() && m_bDisableKingSkill )
				{
					continue;
				}

				ioBuff *pBuff = pTarget->AddNewBuffWithAnimateTime( szBuffName,
																	pChar->GetCharName(),
																	szItemName,
																	this,
																	GetSkillMotionTime() );

				if( pBuff && pChar )
				{
					pBuff->SetItemCode( pChar->GetWeaponItemCode() );
				}
			}
		}
	}
}

bool ioBuffSkill::HasBuff( int iBuffType ) const
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_OwnerBuffList[i].m_iBuffType == iBuffType )
			return true;
	}

	return false;
}

int ioBuffSkill::GetBuffType( int iBuffType /*= 0 */ )
{
	if( m_OwnerBuffList.size() <= 0 )
		return -1;

	return m_OwnerBuffList[iBuffType].m_iBuffType;
}

bool ioBuffSkill::HasOwnerBuff( const ioHashString &szBuffName )
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_OwnerBuffList[i].m_BuffName == szBuffName )
			return true;
	}

	return false;
}

bool ioBuffSkill::IsOnOffEnable()
{
	return m_bOnOffEnable;
}

bool ioBuffSkill::CheckUseBuff( ioBaseChar *pChar )
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( pChar->HasBuff( m_OwnerBuffList[i].m_BuffName ) )
			return true;
	}

	if( CheckUseAreaWeapon() )
		return true;

	return false;
}

bool ioBuffSkill::CheckUseAreaWeapon()
{
	if( m_dwAreaWeaponIndex != 0 )
	{
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAttachedAreaWeapon( m_dwAreaWeaponIndex );
		
		if( !pAreaWeapon )
		{
			m_dwAreaWeaponIndex = 0;
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

void ioBuffSkill::RemoveAreaWeapon( ioBaseChar* pChar )
{
	if( m_dwAreaWeaponIndex != 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
		m_dwAreaWeaponIndex = 0;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_END_AREA );
			kPacket << pChar->GetCharName();
			kPacket << m_Name;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioBuffSkill::IsCanShowWeaponItemMesh( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioBuff *pBuff = pChar->GetBuff( m_OwnerBuffList[i].m_BuffName );
		if( pBuff && pBuff->IsInvisibleWeapon() )
		{
			return false;
		}
	}

	return true;
}

bool ioBuffSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioBuffSkill::IsEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime <= FRAMEGETTIME() )
		return true;

	return false;
}

void ioBuffSkill::CheckUseBuffMotionEnd( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( m_bUseBuffMotionEnd )
	{
		ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
		if( pStage && !m_SkillMapEffectName.IsEmpty() )
		{
			D3DXVECTOR3 vPos = pChar->GetWorldPosition();
			D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
			pStage->CreateMapEffect( m_SkillMapEffectName, vPos, vScale );
		}

		AddOwnerBuff( pChar );
		AddTargetBuff( pChar );
		CreateAreaWeapon( pChar );
	}
}

void ioBuffSkill::CheckRemoveEquipSkillBuff( bool bRemoveEquipSkillBuff )
{
	m_bRemoveEquipSkillBuff = bRemoveEquipSkillBuff;
}

DWORD ioBuffSkill::GetCreateItemCode( ioBaseChar *pChar )
{
	return 0;
}

