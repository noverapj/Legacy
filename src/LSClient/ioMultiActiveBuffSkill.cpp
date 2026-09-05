
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMultiActiveBuffSkill.h"

ioMultiActiveBuffSkill::ioMultiActiveBuffSkill()
{
	m_CreateState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_bSetHandMesh = false;

	m_iCurLinearCombo = 0;
}

ioMultiActiveBuffSkill::ioMultiActiveBuffSkill( const ioMultiActiveBuffSkill &rhs )
	: ioBuffSkill( rhs ),
	m_LoopAnimation( rhs.m_LoopAnimation ),
	m_dwLoopTime( rhs.m_dwLoopTime ),
	m_EndAnimation( rhs.m_EndAnimation ),
	m_fEndAniRate( rhs.m_fEndAniRate ),
	m_CreateEffect( rhs.m_CreateEffect ),
	m_StartHandMesh( rhs.m_StartHandMesh ),
	m_vObjectItemList( rhs.m_vObjectItemList ),
	m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration ),
	m_bProtectOnUsingSkill( rhs.m_bProtectOnUsingSkill ),

	m_stMyCheckBuffName( rhs.m_stMyCheckBuffName ),
	m_stTargetCheckBuffName( rhs.m_stTargetCheckBuffName ),
	m_ContactAttack( rhs.m_ContactAttack ),
	m_vComboListLand( rhs.m_vComboListLand ),
	m_vComboListAir( rhs.m_vComboListAir ),
	m_vComboListDown( rhs.m_vComboListDown ),

	m_fSkillRange( rhs.m_fSkillRange ),
	m_fSkillAngle( rhs.m_fSkillAngle ),
	m_fOffSet( rhs.m_fOffSet ),

	m_TargetColType( rhs.m_TargetColType ),
	m_TargetWoundType( rhs.m_TargetWoundType ),

	m_bSelectTeleportDirection( rhs.m_bSelectTeleportDirection ),
	m_fRangeEmptyTarget( rhs.m_fRangeEmptyTarget ),

	m_fUpHeight( rhs.m_fUpHeight ),
	m_fUnderHeight( rhs.m_fUnderHeight ),
	m_fWidth( rhs.m_fWidth ),

	m_fHeightRange( rhs.m_fHeightRange ),
	m_fBackRange( rhs.m_fBackRange ),
	m_bNoGravityCombo( rhs.m_bNoGravityCombo )
{
	m_State = TSS_NONE;

	m_iCurLinearCombo = 0;
}

ioMultiActiveBuffSkill::~ioMultiActiveBuffSkill()
{
}

void ioMultiActiveBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioBuffSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

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

	m_bProtectOnUsingSkill = rkLoader.LoadBool_e( "protect_on_using_skill", false );
	//////////////////////////////////////////////////////////////////////////
	m_fBackRange = rkLoader.LoadFloat_e( "back_move_range", 0.0f );
	m_fHeightRange = rkLoader.LoadFloat_e( "back_move_height_range", 0.0f );

	m_bNoGravityCombo = rkLoader.LoadBool_e( "no_gravity_combo", false );

	int iComboCnt = rkLoader.LoadInt_e( "linear_land_combo_cnt", 0 );
	m_vComboListLand.clear();
	if( iComboCnt > 0 )
	{
		m_vComboListLand.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_land_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboListLand.push_back( kComboAttack );
		}
	}

	iComboCnt = rkLoader.LoadInt_e( "linear_air_combo_cnt", 0 );
	m_vComboListAir.clear();
	if( iComboCnt > 0 )
	{
		m_vComboListAir.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_air_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboListAir.push_back( kComboAttack );
		}
	}

	iComboCnt = rkLoader.LoadInt_e( "linear_down_combo_cnt", 0 );
	m_vComboListDown.clear();
	if( iComboCnt > 0 )
	{
		m_vComboListDown.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_down_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboListDown.push_back( kComboAttack );
		}
	}

	m_bSelectTeleportDirection = rkLoader.LoadBool_e( "select_teleport_direction", false );
	m_fRangeEmptyTarget = rkLoader.LoadFloat_e( "range_empty_target", 0.0f );

	rkLoader.LoadString_e( "my_check_buff", "", szBuf, MAX_PATH );
	m_stMyCheckBuffName = szBuf;
	rkLoader.LoadString_e( "target_check_buff", "", szBuf, MAX_PATH );
	m_stTargetCheckBuffName = szBuf;

	LoadAttackAttribute_e( "contact_attack", m_ContactAttack, rkLoader );

	//////////////////////////////////////////////////////////////////////////
	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_collision_type", TCT_SPHERE );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fSkillRange = rkLoader.LoadFloat_e( "apply_range", 0.0f );
	m_fSkillAngle = rkLoader.LoadFloat_e( "apply_angle", 0.0f );
	m_fOffSet = rkLoader.LoadFloat_e( "apply_position_offset", 0.0f );

	m_fUpHeight = rkLoader.LoadFloat_e( "apply_up_height", 0.0f );
	m_fUpHeight = max( FLOAT1, m_fUpHeight );
	m_fUnderHeight = rkLoader.LoadFloat_e( "apply_under_height", 0.0f );
	m_fUnderHeight = max( FLOAT1, m_fUnderHeight );
	m_fWidth = rkLoader.LoadFloat_e( "apply_width", 0.0f );
}

ioSkill* ioMultiActiveBuffSkill::Clone()
{
	return new ioMultiActiveBuffSkill( *this );
}

bool ioMultiActiveBuffSkill::OnSkillStart( ioBaseChar *pChar )
{
	//if( !ioBuffSkill::OnSkillStart( pChar ) )
		//return false;

	//////////////////////////////////////////////////////////////////////////
	if( !HasSkillMotion() )
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

	DWORD dwCurTime = FRAMEGETTIME();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		BuffSkillSet( pChar );
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += dwCurTime;
		else
			dwTrackingTime = dwCurTime + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	if( m_bSkillMoveEnableDelayTime )
		pChar->SetSkillMoveEnable( true, true );

	if( m_SkillActivateEffectID != -1 )
	{
		pChar->EndEffect( m_SkillActivateEffectID, false );
		m_SkillActivateEffectID = -1;
	}
	//////////////////////////////////////////////////////////////////////////

	m_CreateState = RS_NONE;
	m_dwCurObjectItem = 0;

	if( pChar->HasBuff( m_stMyCheckBuffName ) )	
	{
		m_State = TSS_CHECK_TEL_LIST;
	}
	else 
	{
		if( !m_bReduceGauge )
		{
			float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
			m_bReduceGauge = true;
		}
		m_State = TSS_NO_BUFF;
	}

	m_SkillState = SS_NONE;
	return true;
}

void ioMultiActiveBuffSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case TSS_NO_BUFF:
		{
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				//pChar->AddNewBuff( m_stMyCheckBuffName, pChar->GetCharName(), "", this );
				ioBuffSkill::BuffSkillSet( pChar );
				ChangeContactFireState(pChar);
			}
		}
		break;
	case TSS_CON_ACTION:
		{
			if( dwCurTime > m_dwMotionEndTime )
				ChangeEndState(pChar);
		}
		break;
	case TSS_CHECK_TEL_LIST:
		if( pChar->IsNeedProcess() && m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;
			SetTeleportList( pChar, pStage );
		}
		break;
	case TSS_TEL_ACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
				ChangeEndState(pChar);
		}
		break;
	case TSS_END:
		break;
	}
}

void ioMultiActiveBuffSkill::OnSkillEnd( ioBaseChar *pChar )
{
	EndPreDelayEffect( pChar );

	AddOwnerBuff( pChar );
	AddTargetBuff( pChar );
	CreateAreaWeapon( pChar );

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

bool ioMultiActiveBuffSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_CreateState != RS_END )
		return false;

	if( m_dwEndAniStartTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

void ioMultiActiveBuffSkill::BuffSkillSet( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMultiActiveBuffSkill::BuffSkillSet - Not Exist SkillMotion: %s", szSkillMotion.c_str() );
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

	if( !m_vForceInfoList.empty() )
	{
		pChar->SetReservedSliding( m_vForceInfoList, iCurSkillAniID, fTimeRate, 0 );
	}

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	m_CreateState = RS_ACTION;

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_bReduceGauge = true;
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
}

void ioMultiActiveBuffSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	D3DXVECTOR3 vCreatePos = pChar->GetMidPositionByRate();

	ioHashString szOwnerItem;
	if( GetOwnerItem() )
	{
		szOwnerItem = GetOwnerItem()->GetName();
	}

	int iSize = m_vObjectItemList.size();
	m_dwCurObjectItem = 0;

	if( iSize > 0 )
	{
		IORandom eRandom;
		eRandom.SetRandomSeed( timeGetTime() );
		int iRand = eRandom.Random( iSize );

		m_dwCurObjectItem = m_vObjectItemList[iRand];
	}

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			pChar->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << vCreatePos;
			kPacket << m_dwCurObjectItem;
			kPacket << szOwnerItem;
			kPacket << pChar->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_EQUIP_SKILL );
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		if( m_dwCurObjectItem != 0 )
		{
			ioItem *pItem = pStage->CreateItem( m_dwCurObjectItem );
			if( !pItem )	return;

			pItem->ReLoadProperty();

			// 싱글모드 뿐만아니라 로비에서도 이쪽에서 처리된다.
			ioObjectItem *pObject = ToObjectItem( pItem );
			if( pObject )
			{
				pObject->SetObjectCreateItem( szOwnerItem );
				pObject->SetObejctCreateType( ioObjectItem::OCT_EQUIP_SKILL );
			}
			pChar->EquipItem( pItem );
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

void ioMultiActiveBuffSkill::SetEndState( ioBaseChar *pChar )
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
}

void ioMultiActiveBuffSkill::RemoveSkillBuff( ioBaseChar *pChar )
{
	if( pChar )
	{
		int iCnt = m_OwnerBuffList.size();

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_END_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << true;
			kPacket << m_dwCurObjectItem;

			kPacket << iCnt;

			for(int i=0; i < iCnt; ++i )
			{
				pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
				kPacket << m_OwnerBuffList[i].m_BuffName;
			}

			P2PNetwork::SendToAllPlayingUser( kPacket );			
		}
		else
		{
			for(int i=0; i < iCnt; ++i )
			{
				pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
			}
		}

		if( m_dwCurObjectItem > 0 )
		{
			pChar->ObjectItemUse( m_dwCurObjectItem, pChar->GetState() );
		}
	}
}

void ioMultiActiveBuffSkill::RemoveSkillBuff( ioBaseChar *pChar, const ioHashString &szExceptName )
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
			kPacket << true;
			kPacket << m_dwCurObjectItem;
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

		if( m_dwCurObjectItem > 0 )
		{
			pChar->ObjectItemUse( m_dwCurObjectItem, pChar->GetState() );
		}
	}
}

bool ioMultiActiveBuffSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bProtectOnUsingSkill )
		return true;

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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioMultiActiveBuffSkill::IsEndState() const
{
	if( m_CreateState == RS_END )
		return true;

	if( m_State == TSS_END )
		return true;

	return false;
}

DWORD ioMultiActiveBuffSkill::GetCreateItemCode( ioBaseChar *pChar )
{
	return m_dwCurObjectItem;
}


bool ioMultiActiveBuffSkill::IsEnableMultiActive( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	ioItem *pItem = GetOwnerItem();
	if( !pItem )
		return false;

	if( pChar->HasBuff(m_stMyCheckBuffName) && pItem->GetCurSkillGuage() )
		return true;

	return false;
}

void ioMultiActiveBuffSkill::ChangeContactFireState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_ContactAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_ContactAttack.m_dwPreDelay;

	float fTimeRate = m_ContactAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	m_State = TSS_CON_ACTION;

	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_ContactAttack.m_fAttackAniRate );

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_ContactAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );
	//SetExtraAniJump( pOwner, iAniID, fTimeRate );
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetReservedSliding( m_ContactAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}


void ioMultiActiveBuffSkill::SetTeleportList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioMultiActiveBuffSkill::SetTeleportList() - Owner is NULL" );
		return;
	}

	if( !pStage )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fSkillRange;
	float fHalfCosine = cosf( DEGtoRAD( m_fSkillAngle ) * FLOAT05 );

	ioBaseChar *pTargetChar = pOwner;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);

		if( !pChar || IsCollisioned( pChar ) )
			continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pOwner->GetCharName() == pChar->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pChar->GetTeam() )
			continue;

		if( pChar->IsSystemState() )
			continue;
		if( !pChar->IsEnableTarget() )
			continue;
		//타겟 버프
		if( !m_stTargetCheckBuffName.IsEmpty() && !pChar->HasBuff( m_stTargetCheckBuffName) )
			continue;

		if( m_TargetColType == TCT_SPHERE )
		{
			D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pOwner->GetWorldPosition();
			D3DXVECTOR3 vTargetDir = vDiff;
			D3DXVec3Normalize( &vTargetDir, &vTargetDir );

			if( D3DXVec3Dot( &vSightDir, &vTargetDir ) < fHalfCosine )
				continue;

			float fDistSq = D3DXVec3LengthSq( &vDiff );
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				pTargetChar = pChar;
				fPrevDistSq = fDistSq;
			}
		}
		else if( m_TargetColType == TCT_CYLINDER )
		{
			D3DXVECTOR3 vTop, vBottom, vCenter;
			vCenter = pOwner->GetMidPositionByRate();
			vTop = vBottom = vCenter;
			vTop.y += m_fUpHeight;
			vBottom.y -= m_fUnderHeight;
			ioCylinder kCylinder( vTop, vBottom, m_fSkillRange );
			vCenter = kCylinder.GetCenter();

			D3DXVECTOR3 vCollisionPoint;
			if( !pChar->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
				continue;

			D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition();
			D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
			vTargetPos.y = 0.0f;
			vOwnerPos.y = 0.0f;
			D3DXVECTOR3 vDiff = vTargetPos - vOwnerPos;
			D3DXVECTOR3 vTargetDir = vDiff;
			D3DXVec3Normalize( &vTargetDir, &vTargetDir );

			if( D3DXVec3Dot( &vSightDir, &vTargetDir ) < fHalfCosine )
				continue;

			float fDistSq = D3DXVec3LengthSq( &vDiff );
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				pTargetChar = pChar;
				fPrevDistSq = fDistSq;
			}
		}
	}

	float fCurGauge = pOwner->ReduceCurSkillGaugeBySkill( GetOwnerItem(), GetFireNeedGauge(pOwner->GetCharName()) );
	m_bReduceGauge = true;

	if( pOwner && pTargetChar && pOwner != pTargetChar )
	{
		m_AttackType = GetAttackType( pTargetChar );

		D3DXVECTOR3 vOffSet( 0.0f, m_fHeightRange, 0.0f );
		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition() + vOffSet;
		D3DXQUATERNION qtTargetOrientation = pTargetChar->GetWorldOrientation();	
		D3DXVECTOR3 vNewPos;
		if( m_bSelectTeleportDirection )
		{
			if( pOwner->GetCharName() == pTargetChar->GetCharName() )
			{
				m_fRangeEmptyTarget;
				vNewPos = vTargetPos + ( pOwner->GetMoveDir() * m_fRangeEmptyTarget );
				float fMapHeight = pStage->GetMapHeight( vNewPos.x, vNewPos.z );
				if( fMapHeight > 0.0f && fMapHeight > vNewPos.y )
					vNewPos.y = fMapHeight;

				pOwner->SetWorldPosition( vNewPos );
			}
			else
			{
				D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
				D3DXVECTOR3 vDir = vTargetPos - vOwnerPos;
				D3DXVec3Normalize( &vDir, &vDir );
				vNewPos = vTargetPos + ( vDir * m_fBackRange );
				pOwner->SetWorldPosition( vNewPos );
				pOwner->SetTargetRotToTargetPos( vTargetPos, true );
			}
		}
		else
		{
			vNewPos = vTargetPos - ( ( qtTargetOrientation * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) ) * m_fBackRange );
			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotAndMoveDirChange( qtTargetOrientation );
		}

		if( pStage )
			pStage->SetCameraToOwnerExplicit( pOwner );

		SetSkillActionState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << TSS_TEL_ACTION;
			kPacket << (int)m_AttackType;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			kPacket << fCurGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( pOwner && pTargetChar && pOwner == pTargetChar )
	{
		m_AttackType = CCAT_FAIL;

		if( pStage )
			pStage->SetCameraToOwnerExplicit( pOwner );

		SetSkillActionState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << TSS_TEL_ACTION;
			kPacket << (int)m_AttackType;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			kPacket << fCurGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


bool ioMultiActiveBuffSkill::IsCollisioned( ioBaseChar *pChar )
{
	ioHashStringVec::iterator iter = std::find( m_vCollisionedList.begin(),
		m_vCollisionedList.end(),
		pChar->GetCharName() );

	if( iter != m_vCollisionedList.end() )
		return true;

	return false;
}


void ioMultiActiveBuffSkill::ChangeEndState( ioBaseChar *pOwner )
{
	m_State = TSS_END;

	m_SkillState = SS_END;
}


ioMultiActiveBuffSkill::CharAttackType ioMultiActiveBuffSkill::GetAttackType( ioBaseChar *pChar )
{
	float fBottomHeight = pChar->GetBottomHeight();
	float fPosY = pChar->GetWorldPosition().y;

	if( (pChar->GetState() == CS_BLOW_WOUNDED || pChar->GetState() == CS_BOUND_BLOW_WOUNDED) &&	(fBottomHeight == fPosY) )
		return CCAT_DOWN;

	if( fBottomHeight == fPosY )
		return CCAT_LAND;

	return CCAT_AIR;
}

void ioMultiActiveBuffSkill::SetNextLinearCombo( ioBaseChar *pChar, bool bSend )
{
	if( !pChar ) return;

	m_dwEnableReserveTime = 0;

	int iMaxCombo = m_vComboListLand.size();
	if( !COMPARE( m_iCurLinearCombo, 0, iMaxCombo ) )
		return;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() && !m_bSelectTeleportDirection )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack;
	switch( m_AttackType )
	{
	case CCAT_LAND:
		kAttack = m_vComboListLand[m_iCurLinearCombo];
		break;
	case CCAT_AIR:
		kAttack = m_vComboListAir[m_iCurLinearCombo];
		break;
	case CCAT_DOWN:
		kAttack = m_vComboListDown[m_iCurLinearCombo];
		break;
	default : 
		kAttack = m_ContactAttack;
		break;
	}

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );
	m_bUsedComboList = true;

	if( m_bNoGravityCombo )
	{
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
		pChar->SetCurJumpPowerAmt( 0.0f );
	}

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
	}

	if( m_iCurLinearCombo == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
		iAniID,
		kAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = kAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( kAttack.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndTimeRate,
			fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( kAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	//
	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	//if( bSend && pChar->IsNeedSendNetwork() )
	//{
	//	SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
	//	kPacket << pChar->GetCharName();
	//	kPacket << GetName();
	//	//kPacket << ST_COMBO;
	//	kPacket << m_iCurLinearCombo;
	//	kPacket << (int)m_CurDirKey;

	//	if( IsUseAutoTargetValue() )
	//	{
	//		pChar->FillAutoTargetInfo( kPacket );
	//	}
	//	P2PNetwork::SendToAllPlayingUser( kPacket );
	//}

	m_iCurLinearCombo++;
}


void ioMultiActiveBuffSkill::SetActionState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iCurSkillAniID == -1 )
	{
		m_dwMotionStartTime = FRAMEGETTIME();
		LOG.PrintTimeAndLog( 0, "ioRangeSkill::SetActionState() - %s, Not Exist ActionAnimation", GetName().c_str() );
	}
	else
	{
		float fAniRate = GetSkillMotionRate();

		float fTimeRate = fAniRate * m_fExtraAniRate;

		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		m_dwMotionStartTime = FRAMEGETTIME();
		m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate - FLOAT100;
	}

	//m_RangeState = RS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}


void ioMultiActiveBuffSkill::SetSkillActionState( ioBaseChar *pChar )
{
	m_State = TSS_TEL_ACTION;

	m_iCurLinearCombo = 0;
	SetNextLinearCombo( pChar, false );
}

void ioMultiActiveBuffSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case TSS_TEL_ACTION:
		{
			int nAttackType;
			D3DXVECTOR3 vTargetPos;
			D3DXQUATERNION qtTargetOrientation;
			float fCurGauge;
			rkPacket >> nAttackType >> vTargetPos >> qtTargetOrientation >> fCurGauge;
			m_AttackType = (CharAttackType)nAttackType;
			pChar->SetWorldPosition( vTargetPos );
			pChar->SetTargetRotAndMoveDirChange( qtTargetOrientation );
			pChar->SetCurItemSkillGauge( GetOwnerItem()->GetName(), fCurGauge );

			SetSkillActionState( pChar );
		}
		break;
	}
}


bool ioMultiActiveBuffSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
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
	else if( CheckUseBuff( pChar ) && !IsEnableMultiActive(pChar) )
	{
		return false;
	}

	// 사용가능인지 체크
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable && !pAttackable->IsCanFire( pChar ) )
		return false;

	if( !IsEnableMultiActive(pChar) && !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}