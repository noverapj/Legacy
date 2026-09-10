#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioMutlipleObjectItemSkill.h"

#include "ItemDefine.h"

ioMultipleObjectItemSkill::ioMultipleObjectItemSkill()
{
	m_CreateState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_bSetHandMesh = false;
}

ioMultipleObjectItemSkill::ioMultipleObjectItemSkill( const ioMultipleObjectItemSkill &rhs )
: ioNormalSkill( rhs ),
m_fOffSet( rhs.m_fOffSet ),
m_LoopAnimation( rhs.m_LoopAnimation ),
m_dwLoopTime( rhs.m_dwLoopTime ),
m_EndAnimation( rhs.m_EndAnimation ),
m_fEndAniRate( rhs.m_fEndAniRate ),
m_CreateEffect( rhs.m_CreateEffect ),
m_StartHandMesh( rhs.m_StartHandMesh ),
m_vObjectItemList( rhs.m_vObjectItemList ),
m_bImmediatelyEquip( rhs.m_bImmediatelyEquip ),
m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration ),
m_OwnerBuffList( rhs.m_OwnerBuffList )
{
	m_CreateState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_bSetHandMesh = false;
	
}

ioMultipleObjectItemSkill::~ioMultipleObjectItemSkill()
{
}

void ioMultipleObjectItemSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_fOffSet = rkLoader.LoadFloat_e( "position_offset", 0.0f );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwLoopProtectDuration = rkLoader.LoadInt_e( "loop_protect_duration", 0 );

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_hand_mesh", "", szBuf, MAX_PATH );
	m_StartHandMesh = szBuf;

	rkLoader.LoadString_e( "create_item_effect", "", szBuf, MAX_PATH );
	m_CreateEffect = szBuf;

	int iCnt = rkLoader.LoadInt_e( "create_item_cnt", 0 );

	m_vObjectItemList.clear();
	m_vObjectItemList.reserve( iCnt );

	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "create_item_code%d", i+1 );
		DWORD dwItemCode = rkLoader.LoadInt( szKey, 0 );

		m_vObjectItemList.push_back( dwItemCode );
	}

	m_bImmediatelyEquip = rkLoader.LoadBool_e( "immediately_equip", false );
	LoadOwnerBuffList( rkLoader );	
}

void ioMultipleObjectItemSkill::LoadOwnerBuffList( ioINILoader &rkLoader )
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

ioSkill* ioMultipleObjectItemSkill::Clone()
{
	return new ioMultipleObjectItemSkill( *this );
}

bool ioMultipleObjectItemSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_CreateState == RS_ACTION )
		return true;

	if( m_CreateState == RS_LOOP )
	{
		if( m_dwLoopProtectDuration == 0 )
			return true;

		if( m_dwProtectTime > dwCurTime )
			return true;
	}

	if( m_CreateState == RS_END )
		return false;

	return false;
}

bool ioMultipleObjectItemSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioMultipleObjectItemSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_CreateState != RS_END )
		return false;

	if( m_dwEndAniStartTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioMultipleObjectItemSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_CreateState = RS_NONE;
	m_dwProtectTime = 0;
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetActionState( pChar );
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioMultipleObjectItemSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_CreateState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
		}
		break;
	case RS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			m_dwMotionStartTime = 0;
			SetLoopState( pChar, pStage );
		}
		break;
	case RS_LOOP:
		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < FRAMEGETTIME() )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar, pStage );
		}
		break;
	}
}

void ioMultipleObjectItemSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	// 기본무기 장착
	if( m_bUseItemMesh && pOwner )
	{
		if( !m_bImmediatelyEquip )
		{
			pOwner->ShowWeaponItemMesh( true );
		}
		else if( !pOwner->GetEquipSlot()->IsEquipSlot(ES_OBJECT) &&
			!pOwner->GetEquipSlot()->IsEquipWait( ES_OBJECT ) )
		{
			pOwner->ShowWeaponItemMesh( true );
		}
	}

	if( m_bSetHandMesh && !m_StartHandMesh.IsEmpty() )
	{
		pOwner->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
	}

	m_bSetHandMesh = false;

	EndPreDelayEffect( pOwner );
	GetOwnerItem()->ClearTarget();

	pOwner->EndEffect( m_SkillEffectName );

	m_bReduceGauge = false;
	pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	pOwner->RestoreSkillProtection();

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pOwner->AttachEffect( m_SkillEndEffectName );
}

void ioMultipleObjectItemSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );
}

void ioMultipleObjectItemSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );
}

void ioMultipleObjectItemSkill::SetActionState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate - FLOAT100;

	m_CreateState = RS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioMultipleObjectItemSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	D3DXVECTOR3 vCreatePos = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vCreatePos *= m_fOffSet;
	vCreatePos += pChar->GetMidPositionByRate();

	float fCheckHeight = pStage->GetMapHeight( vCreatePos.x, vCreatePos.z, NULL, false );
	float fGap = fCheckHeight - vCreatePos.y;
	if( fGap > 50.0f )
		vCreatePos = pChar->GetMidPositionByRate();
	else if( fGap > 0.0f )
		vCreatePos.y = fCheckHeight;

	ioHashString szOwnerItem;
	if( GetOwnerItem() )
	{
		szOwnerItem = GetOwnerItem()->GetName();
	}

	int iSize = m_vObjectItemList.size();
	DWORD dwCreateItemCode = 0;

	if( iSize > 0 )
	{
		IORandom eRandom;
		eRandom.SetRandomSeed( timeGetTime() );
		int iRand = eRandom.Random( iSize );

		dwCreateItemCode = m_vObjectItemList[iRand];
	}

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			if( m_bImmediatelyEquip )
				pChar->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << vCreatePos;
			kPacket << dwCreateItemCode;
			kPacket << szOwnerItem;
			kPacket << pChar->GetCharName();
			kPacket << m_bImmediatelyEquip;

			if( m_bImmediatelyEquip )
				kPacket << static_cast<int>( ioObjectItem::OCT_EQUIP_SKILL );
			
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		if( dwCreateItemCode != 0 )
		{
			ioItem *pItem = pStage->CreateItem( dwCreateItemCode );			
			if( !pItem )
				return;

			pItem->ReLoadProperty();

			ioObjectItem *pObject = ToObjectItem( pItem );
			if( pObject )
			{
				pObject->SetObjectCreateItem( szOwnerItem );

				if( m_bImmediatelyEquip )
					pObject->SetObejctCreateType( ioObjectItem::OCT_EQUIP_SKILL );
			}

			// 싱글모드 뿐만아니라 로비에서도 이쪽에서 처리된다.
			if( m_bImmediatelyEquip )
			{
				pChar->EquipItem( pItem );
			}
			else
			{
				ioFieldItem *pField = pStage->CreateFieldItem( pItem, false );
				if( pField )
				{
					pField->SetWorldPositionByServerSide( vCreatePos );
					pField->SetDropState( pField->GetWorldPosition() );
					pField->CreateDropEffect();
					//pField->CreateItemEffect();
				}
			}
		}
	}

	pChar->CreateMapEffectBySkill( m_CreateEffect, vCreatePos );

	if( m_LoopAnimation.IsEmpty() )
	{
		m_dwLoopStartTime = 0;
		return;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_CreateState = RS_LOOP;
}

void ioMultipleObjectItemSkill::SetEndState( ioBaseChar *pChar, ioPlayStage* pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iCurSkillAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );

	m_dwEndAniStartTime = FRAMEGETTIME();		
	m_dwEndAniEndTime = m_dwEndAniStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*m_fEndAniRate;

	m_CreateState = RS_END;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();

	if( pItem )
		szItemName = pItem->GetName();	
	else
		return;

	SetOwnerBuffList( pChar, pStage );
}

bool ioMultipleObjectItemSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioMultipleObjectItemSkill::SetOwnerBuffList( ioBaseChar *pChar, ioPlayStage* pStage )
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
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff9 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
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
