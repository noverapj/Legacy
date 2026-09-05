
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioCreateItemBuffSkill.h"

ioCreateItemBuffSkill::ioCreateItemBuffSkill()
{
	m_CreateState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_bSetHandMesh = false;
}

ioCreateItemBuffSkill::ioCreateItemBuffSkill( const ioCreateItemBuffSkill &rhs )
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
m_bUseClearGauge( rhs.m_bUseClearGauge )
{
}

ioCreateItemBuffSkill::~ioCreateItemBuffSkill()
{
}

void ioCreateItemBuffSkill::LoadProperty( ioINILoader &rkLoader )
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
	m_bUseClearGauge = rkLoader.LoadBool_e( "use_cancle_clear_gauge", false );
}

ioSkill* ioCreateItemBuffSkill::Clone()
{
	return new ioCreateItemBuffSkill( *this );
}

bool ioCreateItemBuffSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioBuffSkill::OnSkillStart( pChar ) )
	{
		//게이지를 날리자
		if( m_bUseClearGauge && m_bCanEnableBuffState == false )
		{
			pChar->SetCurItemSkillGauge( GetOwnerItem()->GetName(), 0.f );
		}

		return false;
	}

	m_CreateState = RS_NONE;
	m_dwCurObjectItem = 0;

	return true;
}

void ioCreateItemBuffSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_CreateState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			BuffSkillSet( pChar );
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
			SetEndState( pChar );
		}
		break;
	}
}

void ioCreateItemBuffSkill::OnSkillEnd( ioBaseChar *pChar )
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

bool ioCreateItemBuffSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_CreateState != RS_END )
		return false;

	if( m_dwEndAniStartTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

void ioCreateItemBuffSkill::BuffSkillSet( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioCreateItemBuffSkill::BuffSkillSet - Not Exist SkillMotion: %s", szSkillMotion.c_str() );
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

void ioCreateItemBuffSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
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
			if( !pItem )
				return;

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

void ioCreateItemBuffSkill::SetEndState( ioBaseChar *pChar )
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

void ioCreateItemBuffSkill::RemoveSkillBuff( ioBaseChar *pChar )
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
			if( pChar->GetState() == CS_DELAY )
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
			else if( pChar->GetState() == CS_RUN )
				pChar->ReleaseObjectItem( __FUNCTION__, CS_RUN );
			else
				pChar->ReleaseObjectItem( __FUNCTION__ );
		}
	}
}

void ioCreateItemBuffSkill::RemoveSkillBuff( ioBaseChar *pChar, const ioHashString &szExceptName )
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

bool ioCreateItemBuffSkill::IsProtected( int iDefenseBreakType ) const
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

bool ioCreateItemBuffSkill::IsEndState() const
{
	if( m_CreateState == RS_END )
		return true;

	return false;
}

DWORD ioCreateItemBuffSkill::GetCreateItemCode( ioBaseChar *pChar )
{
	return m_dwCurObjectItem;
}


