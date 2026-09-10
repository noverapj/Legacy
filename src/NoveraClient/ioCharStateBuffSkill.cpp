

#include "stdafx.h"

#include "ioCharStateBuffSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"


ioCharStateBuffSkill::ioCharStateBuffSkill()
{
	m_bWounded = false;
}

ioCharStateBuffSkill::ioCharStateBuffSkill( const ioCharStateBuffSkill &rhs )
	: ioBuffSkill( rhs ),
	m_OwnerWoundedBuffList( rhs.m_OwnerWoundedBuffList )
{
	m_bWounded = false;
}

ioCharStateBuffSkill::~ioCharStateBuffSkill()
{
}

void ioCharStateBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioBuffSkill::LoadProperty( rkLoader );

	char szTitle[MAX_PATH], szBuff[MAX_PATH];
	ioBuffInfo tBuffInfo;
	m_OwnerWoundedBuffList.clear();

	int iCount = rkLoader.LoadInt_e( "owner_wounded_buff_cnt", 0 );
	for( int i=0; i < iCount; i++ )
	{
		wsprintf_e( szTitle, "owner_wounded_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		tBuffInfo.m_BuffName = szBuff;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerWoundedBuffList.push_back( tBuffInfo );
			}
		}
	}
}

ioSkill* ioCharStateBuffSkill::Clone()
{
	return new ioCharStateBuffSkill( *this );
}

bool ioCharStateBuffSkill::OnSkillStart( ioBaseChar *pChar )
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

	m_CharBlowWoundedState = pChar->GetBlowWoundedState();

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

	m_bWounded = pChar->IsWoundedState();

	if( m_bWounded || !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
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

void ioCharStateBuffSkill::BuffSkillSet( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	if( !m_bWounded )
	{
		int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
		if( iCurSkillAniID == -1 )
		{
			LOG.PrintTimeAndLog( 0, "ioCharStateBuffSkill::BuffSkillSet - Not Exist SkillMotion: %s", szSkillMotion.c_str() );
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

void ioCharStateBuffSkill::RemoveSkillBuff( ioBaseChar *pChar )
{
	if( pChar )
	{
		BuffInfoList& pBuffList = m_OwnerBuffList;
		if( m_bWounded )
			pBuffList = m_OwnerWoundedBuffList;

		int iCnt = pBuffList.size();

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_END_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << false;
			//kPacket << iCnt;
			ioHashStringVec szRemovedBuffList;
			for(int i=0; i < iCnt; ++i )
			{
				ioBuff *pBuff = pChar->GetBuff( pBuffList[i].m_BuffName );
				if( pBuff && m_bRemoveEquipSkillBuff )
				{
					pBuff->CheckRemoveEquipSkillBuff( m_bRemoveEquipSkillBuff );
				}
				if( pChar->RemoveBuff( pBuffList[i].m_BuffName ) )
					szRemovedBuffList.push_back( pBuffList[i].m_BuffName );
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
				pChar->RemoveBuff( pBuffList[i].m_BuffName );
			}
		}
	}
}

void ioCharStateBuffSkill::RemoveSkillBuff( ioBaseChar *pChar, const ioHashString &szExceptName )
{
	ioHashStringVec vNameList;

	if( pChar )
	{
		BuffInfoList& pBuffList = m_OwnerBuffList;
		if( m_bWounded )
			pBuffList = m_OwnerWoundedBuffList;

		int iCnt = pBuffList.size();

		if( pChar->IsNeedSendNetwork() )
		{
			for(int i=0; i < iCnt; ++i )
			{
				if( pBuffList[i].m_BuffName != szExceptName )
				{
					pChar->RemoveBuff( pBuffList[i].m_BuffName );
					vNameList.push_back(pBuffList[i].m_BuffName);
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
				if( pBuffList[i].m_BuffName != szExceptName )
				{
					pChar->RemoveBuff( pBuffList[i].m_BuffName );
				}
			}
		}
	}
}

bool ioCharStateBuffSkill::CheckEnableStart( ioBaseChar *pChar )
{
	m_bCanEnableBuffState = true;

	if( pChar && !pChar->IsNeedProcess() )
		return true;

	BuffInfoList& pBuffList = m_OwnerBuffList;
	if( m_bWounded )
		pBuffList = m_OwnerWoundedBuffList;

	int iBuffSize = pBuffList.size();
	int iCnt = 0;

	// 버프체크
	for( int i=0; i < iBuffSize; ++i )
	{
		if( pChar->HasBuff( pBuffList[i].m_BuffName ) )
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

void ioCharStateBuffSkill::AddOwnerBuff( ioBaseChar *pChar )
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

	BuffInfoList& pBuffInfo = m_OwnerBuffList;
	if( m_bWounded )
		pBuffInfo = m_OwnerWoundedBuffList;

	int iCnt = pBuffInfo.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = pBuffInfo[i].m_BuffName;
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

bool ioCharStateBuffSkill::HasBuff( int iBuffType ) const
{
	if( m_bWounded )
	{
		int iCnt = m_OwnerWoundedBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			if( m_OwnerWoundedBuffList[i].m_iBuffType == iBuffType )
				return true;
		}
	}
	else
	{
		int iCnt = m_OwnerBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			if( m_OwnerBuffList[i].m_iBuffType == iBuffType )
				return true;
		}
	}

	return false;
}

int ioCharStateBuffSkill::GetBuffType( int iBuffType /*= 0 */ )
{
	BuffInfoList& pBuffList = m_OwnerBuffList;
	if( m_bWounded )
		pBuffList = m_OwnerWoundedBuffList;

	if( pBuffList.size() <= 0 )
		return -1;

	return pBuffList[iBuffType].m_iBuffType;
}

bool ioCharStateBuffSkill::HasOwnerBuff( const ioHashString &szBuffName )
{
	BuffInfoList& pBuffList = m_OwnerBuffList;
	if( m_bWounded )
		pBuffList = m_OwnerWoundedBuffList;

	int iCnt = pBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( pBuffList[i].m_BuffName == szBuffName )
			return true;
	}

	return false;
}

bool ioCharStateBuffSkill::IsOnOffEnable()
{
	return m_bOnOffEnable;
}

bool ioCharStateBuffSkill::CheckUseBuff( ioBaseChar *pChar )
{
	BuffInfoList& pBuffList = m_OwnerBuffList;
	if( m_bWounded )
		pBuffList = m_OwnerWoundedBuffList;

	int iCnt = pBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( pChar->HasBuff( pBuffList[i].m_BuffName ) )
			return true;
	}

	if( CheckUseAreaWeapon() )
		return true;

	return false;
}

bool ioCharStateBuffSkill::CheckUseAreaWeapon()
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

void ioCharStateBuffSkill::RemoveAreaWeapon( ioBaseChar* pChar )
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

bool ioCharStateBuffSkill::IsCanShowWeaponItemMesh( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	BuffInfoList& pBuffList = m_OwnerBuffList;
	if( m_bWounded )
		pBuffList = m_OwnerWoundedBuffList;

	int iCnt = pBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioBuff *pBuff = pChar->GetBuff( pBuffList[i].m_BuffName );
		if( pBuff && pBuff->IsInvisibleWeapon() )
		{
			return false;
		}
	}

	return true;
}

bool ioCharStateBuffSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioCharStateBuffSkill::IsEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime <= FRAMEGETTIME() )
		return true;

	return false;
}
