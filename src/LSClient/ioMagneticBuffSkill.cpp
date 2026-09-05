#include "stdafx.h"
#include "ioMagneticBuffSkill.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

#include "ioMagneticItem.h"


ioMagneticBuffSkill::ioMagneticBuffSkill()
{
	m_bDefenseKey				= false;
	m_iCurBuff					= -1;
	m_dwDefenseStartCoolTime	= 0;
}

ioMagneticBuffSkill::ioMagneticBuffSkill( const ioMagneticBuffSkill &rhs )
	: ioBuffSkill( rhs ),
m_dwDefenseCoolTime( rhs.m_dwDefenseCoolTime )
{
	m_bDefenseKey				= false;
	m_iCurBuff					= -1;
	m_dwDefenseStartCoolTime	= 0;
}

ioMagneticBuffSkill::~ioMagneticBuffSkill()
{
}

void ioMagneticBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioBuffSkill::LoadProperty( rkLoader );

	m_dwDefenseCoolTime	= rkLoader.LoadFloat_e( "defense_cool_time", 0.0f );
}

ioSkill* ioMagneticBuffSkill::Clone()
{
	return new ioMagneticBuffSkill( *this );
}

bool ioMagneticBuffSkill::OnSkillStart( ioBaseChar *pChar )
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

	// 2020-12-09
	ioWeaponItem *pWeapon = pChar->GetWeapon();		
	ioMagneticItem* pMagneticItem = NULL;
	if( pWeapon && ioWeaponItem::WST_MAGNETIC_ITEM == pWeapon->GetSubType() )
	{
		pMagneticItem = ToMagneticItem( pWeapon );
		if( pMagneticItem )
		{
			pMagneticItem->SetWeaponAttributeBuff(false);
		}
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
	m_dwDefenseStartCoolTime = 0;	// 2020-12-17

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

void ioMagneticBuffSkill::OnSkillEnd( ioBaseChar *pChar )
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

bool ioMagneticBuffSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	// 키 체크 2020-12-17
	if( m_dwDefenseStartCoolTime > 0 && m_dwDefenseStartCoolTime + m_dwDefenseCoolTime < FRAMEGETTIME() )
	{
		CheckKeyInput( pChar );		
	}

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
	{
		return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

void ioMagneticBuffSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioMagneticBuffSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( pOwner->IsDefenseKey() )
	{		
		m_bDefenseKey = true;
		LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::CheckKeyInput %d", m_bDefenseKey );
	}
	else
	{	
		m_bDefenseKey = false;
	}
}

void ioMagneticBuffSkill::OnProcessNotUseSkill( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioMagneticBuffSkill::OnSkillItemRelease( ioBaseChar *pChar )
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

void ioMagneticBuffSkill::BuffSkillSet( ioBaseChar *pChar )
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

	// 2020-12-17
	m_dwDefenseStartCoolTime = FRAMEGETTIME();
}

bool ioMagneticBuffSkill::StartPreDelayAnimation( ioBaseChar *pChar )
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

void ioMagneticBuffSkill::RemoveSkillBuff( ioBaseChar *pChar )
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

void ioMagneticBuffSkill::RemoveSkillBuff( ioBaseChar *pChar, const ioHashString &szExceptName )
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

void ioMagneticBuffSkill::LoadBuffList( ioINILoader &rkLoader )
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

bool ioMagneticBuffSkill::CheckEnableStart( ioBaseChar *pChar )
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

void ioMagneticBuffSkill::CreateAreaWeapon( ioBaseChar *pChar )
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

void ioMagneticBuffSkill::AddOwnerBuff( ioBaseChar *pChar )
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

	// 2020-12-08
	ioWeaponItem *pWeapon = pChar->GetWeapon();		
	ioMagneticItem* pMagneticItem = NULL;
	if( pWeapon && ioWeaponItem::WST_MAGNETIC_ITEM == pWeapon->GetSubType() )
	{
		pMagneticItem = ToMagneticItem( pWeapon );
	}

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


	ioHashString szBuffName;
	szBuffName.Clear();
	m_iCurBuff  = -1;
	int iCnt = m_OwnerBuffList.size();
	bool bWeaponBuff = false;
	for( int i=0; i < iCnt; ++i )
	{		
		szBuffName.Clear();

		//  i = 0 : N극(빨강), i = 1 : S극(파랑)
		//if( pMagneticItem && pMagneticItem->IsWeaponAttributeBuff() )
		//{
		//	if( pMagneticItem->IsGetWeaponAttributeBuffTypeRedN() && i == 0 )
		//	{
		//		m_iCurBuff = i;
		//		szBuffName = m_OwnerBuffList[i].m_BuffName;
		//		bWeaponBuff = true;
		//		LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff IsWeaponAttributeBuff - IsGetWeaponAttributeBuffTypeRedN : %d - %s", i, szBuffName.c_str() );
		//	}
		//	else if( pMagneticItem->IsGetWeaponAttributeBuffTypeBlueS() && i == 1 )
		//	{
		//		m_iCurBuff = i;
		//		szBuffName = m_OwnerBuffList[i].m_BuffName;
		//		bWeaponBuff = true;
		//		LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff IsWeaponAttributeBuff - IsGetWeaponAttributeBuffTypeBlueS : %d - %s", i, szBuffName.c_str() );
		//	}
		//	else
		//	{				
		//		LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff IsWeaponAttributeBuff  : %d - %s", i, szBuffName.c_str() );
		//	}
		//}
		//else if( pMagneticItem && pMagneticItem->IsGetWeaponTypeRedN() && i == 0 )
		//{
		//	m_iCurBuff = i;
		//	szBuffName = m_OwnerBuffList[i].m_BuffName;		
		//	bWeaponBuff = true;
		//	LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff - IsGetWeaponTypeRedN : %d - %s", i, szBuffName.c_str() );
		//}
		//else if( pMagneticItem && pMagneticItem->IsGetWeaponTypeBlueS() && i == 1 )
		//{
		//	m_iCurBuff = i;
		//	szBuffName = m_OwnerBuffList[i].m_BuffName;
		//	bWeaponBuff = true;
		//	LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff - IsGetWeaponTypeBlueS : %d - %s", i, szBuffName.c_str() );
		//}
		//else if( i == 2 )
		//{	
		//	// 잠시 무적 버프 
		//	szBuffName = m_OwnerBuffList[i].m_BuffName;
		//	LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff : %d - %s", i, szBuffName.c_str() );
		//}
		//else 
		//{
		//	szBuffName = m_OwnerBuffList[i].m_BuffName;
		//	LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff : %d - %s", i, szBuffName.c_str() );
		//}

		// 2020-12-16 
		//if( pMagneticItem && pMagneticItem->IsWeaponAttributeBuff() )
		//if( pMagneticItem && pMagneticItem->IsWeaponAttributeBuff(pChar) && m_bDefenseKey )
		if( pMagneticItem && m_bDefenseKey )
		{
			// 무기 속성 변경 2020-12-17
			if( !bWeaponBuff )
			{
				pMagneticItem->SetChangeWeaponAttributeBuffType();
				bWeaponBuff = true;
			}

			if( pMagneticItem->IsGetWeaponAttributeBuffTypeRedN() && i == 0 )
			{
				szBuffName = m_OwnerBuffList[0].m_BuffName;
				//bWeaponBuff = true;
				//m_bDefenseKey = false;
				LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff IsWeaponAttributeBuff - IsGetWeaponAttributeBuffTypeRedN : %d - %s", i, szBuffName.c_str() );
			}
			else if( pMagneticItem->IsGetWeaponAttributeBuffTypeBlueS() && i == 1 )
			{
				szBuffName = m_OwnerBuffList[1].m_BuffName;
				//bWeaponBuff = true;
				//m_bDefenseKey = false;
				LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff IsWeaponAttributeBuff - IsGetWeaponAttributeBuffTypeBlueS : %d - %s", i, szBuffName.c_str() );
			}
			else if( i == 2 )
			{	
				// 잠시 무적 버프 
				szBuffName = m_OwnerBuffList[2].m_BuffName;
				LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff : %d - %s", i, szBuffName.c_str() );
			}
			else
			{				
				LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff IsWeaponAttributeBuff  : %d - %s", i, szBuffName.c_str() );
			}
		}
		else
		{
			if( pMagneticItem && pMagneticItem->IsGetWeaponTypeRedN() && i == 0 )
			{
				szBuffName = m_OwnerBuffList[0].m_BuffName;					
				//bWeaponBuff = true;
				LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff - IsGetWeaponTypeRedN : %d - %s", i, szBuffName.c_str() );
			}
			else if( pMagneticItem && pMagneticItem->IsGetWeaponTypeBlueS() && i == 1 )
			{
				szBuffName = m_OwnerBuffList[1].m_BuffName;				
				//bWeaponBuff = true;
				LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff - IsGetWeaponTypeBlueS : %d - %s", i, szBuffName.c_str() );
			}
			else if( i == 2 )
			{	
				// 잠시 무적 버프 
				szBuffName = m_OwnerBuffList[2].m_BuffName;
				LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff : %d - %s", i, szBuffName.c_str() );
			}
			else 
			{				
				LOG.PrintTimeAndLog( 0, "ioMagneticBuffSkill::AddOwnerBuff : %d - %s", i, szBuffName.c_str() );
			}
		}
		
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

	// 2020-12-17
	//if(	pMagneticItem )
	//{
	//	pMagneticItem->SetWeaponAttributeBuff(bWeaponBuff);
	//}

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

void ioMagneticBuffSkill::AddTargetBuff( ioBaseChar *pChar )
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

bool ioMagneticBuffSkill::HasBuff( int iBuffType ) const
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_OwnerBuffList[i].m_iBuffType == iBuffType )
			return true;
	}

	return false;
}

int ioMagneticBuffSkill::GetBuffType( int iBuffType /*= 0 */ )
{
	if( m_OwnerBuffList.size() <= 0 )
		return -1;

	return m_OwnerBuffList[iBuffType].m_iBuffType;
}

bool ioMagneticBuffSkill::HasOwnerBuff( const ioHashString &szBuffName )
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_OwnerBuffList[i].m_BuffName == szBuffName )
			return true;
	}

	return false;
}

bool ioMagneticBuffSkill::IsOnOffEnable()
{
	return m_bOnOffEnable;
}

bool ioMagneticBuffSkill::CheckUseBuff( ioBaseChar *pChar )
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( pChar->HasBuff( m_OwnerBuffList[i].m_BuffName ) )
		{
			// +S 방어 마그네틱 필드 속성 변경(N극<->S극) 지상에서 방어 시 발동  2020-12-09
			ioItem *pItem = pChar->GetEquipedItem(ES_WEAPON);
			if( ToMagneticItem(pItem) && m_bDefenseKey )
			{
				ioMagneticItem* pMagneticItem = ToMagneticItem(pItem);
				if( pMagneticItem && pMagneticItem->IsWeaponAttributeBuff( pChar ) )
				{
					// 무기 속성 변경 
					//pMagneticItem->SetChangeWeaponAttributeBuffType();

					//// 기존 버프 삭제 
					//ioHashStringVec vNameList;
					//if( pChar->IsNeedSendNetwork() )
					//{
					//	pChar->RemoveBuff( m_OwnerBuffList[m_iCurBuff].m_BuffName );
					//	vNameList.push_back(m_OwnerBuffList[m_iCurBuff].m_BuffName);
					//	int iSendSize = vNameList.size();
					//	SP2Packet kPacket( CUPK_REMOVE_BUFF );
					//	kPacket << pChar->GetCharName();
					//	kPacket << false;
					//	kPacket << iSendSize;
					//	for(int i=0; i < iSendSize; ++i )
					//	{
					//		kPacket << vNameList[i];
					//	}
					//	P2PNetwork::SendToAllPlayingUser( kPacket );			
					//}
					//else
					//{
					//	pChar->RemoveBuff( m_OwnerBuffList[m_iCurBuff].m_BuffName );
					//}
					//RemoveSkillBuff( pChar , m_OwnerBuffList[m_iCurBuff].m_BuffName );

					// 버프 다시 생성
					AddOwnerBuff( pChar );
					m_bDefenseKey = false;
					m_dwDefenseStartCoolTime = FRAMEGETTIME();
				}
			}

			return true;
		}
	}

	if( CheckUseAreaWeapon() )
		return true;

	return false;
}

bool ioMagneticBuffSkill::CheckUseAreaWeapon()
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

void ioMagneticBuffSkill::RemoveAreaWeapon( ioBaseChar* pChar )
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

bool ioMagneticBuffSkill::IsCanShowWeaponItemMesh( ioBaseChar *pChar )
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

bool ioMagneticBuffSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioMagneticBuffSkill::IsEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime <= FRAMEGETTIME() )
		return true;

	return false;
}

void ioMagneticBuffSkill::CheckUseBuffMotionEnd( ioBaseChar *pChar )
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

		// 2020-12-17
		m_dwDefenseStartCoolTime = FRAMEGETTIME();
	}
}

void ioMagneticBuffSkill::CheckRemoveEquipSkillBuff( bool bRemoveEquipSkillBuff )
{
	m_bRemoveEquipSkillBuff = bRemoveEquipSkillBuff;
}

DWORD ioMagneticBuffSkill::GetCreateItemCode( ioBaseChar *pChar )
{
	return 0;
}

