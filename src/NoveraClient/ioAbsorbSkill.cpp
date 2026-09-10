
#include "stdafx.h"

#include "ioAbsorbSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "WeaponDefine.h"
#include "ItemDefine.h"

ioAbsorbSkill::ioAbsorbSkill()
{
	m_dwCurMapEffect = -1;
	m_dwCurAbsorbEffect = -1;

	m_dwAbsorbStartTime = 0;
	m_dwFireMotionEndTime = 0;

	m_SkillState = AS_NONE;
}

ioAbsorbSkill::ioAbsorbSkill( const ioAbsorbSkill &rhs )
: ioNormalSkill( rhs ),
m_fOffSet( rhs.m_fOffSet ),
m_RedCircle( rhs.m_RedCircle ),
m_BlueCircle( rhs.m_BlueCircle ),
m_SkillEndMotion( rhs.m_SkillEndMotion ),
m_fSkillEndMotionRate( rhs.m_fSkillEndMotionRate ),
m_AbsorbEffect( rhs.m_AbsorbEffect ),
m_dwAbsorbTime( rhs.m_dwAbsorbTime ),
m_fAbsorbRange( rhs.m_fAbsorbRange ),
m_fAbsorbGauge( rhs.m_fAbsorbGauge )
{
	m_dwCurMapEffect = -1;
	m_dwCurAbsorbEffect = -1;

	m_dwAbsorbStartTime = 0;
	m_dwFireMotionEndTime = 0;

	m_SkillState = AS_NONE;
}

ioAbsorbSkill::~ioAbsorbSkill()
{
}

void ioAbsorbSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );
	m_fAbsorbRange = rkLoader.LoadFloat_e( "circle_radius", 0.0f );


	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fSkillEndMotionRate = rkLoader.LoadFloat_e( "skill_end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;

	rkLoader.LoadString_e( "absorb_effect", "", szBuf, MAX_PATH );
	m_AbsorbEffect = szBuf;

	m_fAbsorbGauge = rkLoader.LoadFloat_e( "absorb_gauge", 0.0f );
	m_dwAbsorbTime = rkLoader.LoadInt_e( "absorb_time", 0 );
}

ioSkill* ioAbsorbSkill::Clone()
{
	return new ioAbsorbSkill( *this );
}

bool ioAbsorbSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
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

bool ioAbsorbSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_SkillState == AS_LOOP )
		return true;

	return false;
}

bool ioAbsorbSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_SkillState != AS_END )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwSkillEndMotionEndTime > 0 && m_dwSkillEndMotionEndTime <= dwCurTime )
		return true;

	return false;
}

bool ioAbsorbSkill::OnSkillStart( ioBaseChar *pChar )
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

	m_dwSkillEndMotionEndTime = 0;

	// 범위정보 갱신
	ioMagicStaffItem *pStaff = ToMagicStaffItem( GetOwnerItem() );
	if( pStaff )
	{
		m_fAbsorbRange = pStaff->GetCircleRadius();
	}

	// 범위정보 갱신
	ioFireMageExpansionItem *pFireMageExpansionItem = ToFireMageExpansionItem( GetOwnerItem() );
	if( pFireMageExpansionItem )
	{
		m_fAbsorbRange = pFireMageExpansionItem->GetCircleRadius();
	}

	// 2020-02-14
	ioNuClearSoldierItem* pNuClearSoldierItem = ToNuClearSoldierItem( GetOwnerItem() );
	if( pNuClearSoldierItem )
	{
		m_fAbsorbRange = pNuClearSoldierItem->GetCircleRadius();
	}

	//CreateMapEffect
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, 0.0f, 0.0f, true, true );
	}

	float fCurOffSet = GetOwnerItem()->GetMagicCircleOffSet();
	if( fCurOffSet == 0.0f )
		fCurOffSet = m_fOffSet;

	m_vCreatePos = pChar->GetMagicCirclePos( fCurOffSet, m_bCircleNoChangeY );
	
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCreatePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCreatePos );
	}
	else
	{
		if( pChar->IsOwnerChar() )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCreatePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCreatePos );
	}

	if( pMapEffect )
	{
		m_dwCurMapEffect = pMapEffect->GetUniqueID();
	}
	//

	m_SkillState = AS_DELAY;

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
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

void ioAbsorbSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case AS_DELAY:
		OnProcessDelayState( pChar, pStage );
		break;
	case AS_LOOP:
		OnProcessLoopState( pChar, pStage );
		break;
	}
}

void ioAbsorbSkill::OnProcessDelayState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelayTime = GetPreDelayTime();

	if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetActionState( pChar );
	}
}

void ioAbsorbSkill::OnProcessLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwAbsorbStartTime > 0 && m_dwAbsorbStartTime < dwCurTime )
	{
		CheckAbsorb( pChar, pStage );
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;

		m_dwAbsorbStartTime = 0;
	}
}

void ioAbsorbSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pOwner->EndMapEffect( m_dwCurMapEffect );

	if( m_dwCurAbsorbEffect != -1 )
		pOwner->EndMapEffect( m_dwCurAbsorbEffect );

	pOwner->DestroyMagicCircle();

	ioNormalSkill::OnSkillEnd( pOwner );
}

void ioAbsorbSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );
}

void ioAbsorbSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );
}

void ioAbsorbSkill::SetActionState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )
	{
		SetEndState( pChar );
		return;
	}

	ioMapEffect *pMapEffect = pMapEffect = pChar->CreateMapEffectBySkill( m_AbsorbEffect, m_vCreatePos );
	if( pMapEffect )
		m_dwCurAbsorbEffect = pMapEffect->GetUniqueID();

	pChar->AttachEffect( m_SkillEffectName );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate = GetSkillMotionRate();

	pGrp->SetLoopAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwAbsorbStartTime = dwCurTime;
	m_dwAbsorbStartTime += m_dwAbsorbTime;

	m_SkillState = AS_LOOP;
}

void ioAbsorbSkill::SetEndState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwSkillEndMotionEndTime = dwCurTime;
	m_dwEnableReserveTime = dwCurTime;

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect );

	if( m_dwCurAbsorbEffect != -1 )
		pChar->EndMapEffect( m_dwCurAbsorbEffect );

	pChar->DestroyMagicCircle();
	
	m_SkillState = AS_END;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSkillEndMotionRate );

	m_dwSkillEndMotionEndTime += pGrp->GetAnimationFullTime(iCurSkillAniID) * m_fSkillEndMotionRate;
}

void ioAbsorbSkill::CheckAbsorb( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar->IsNeedProcess() ) return;

	IntVec vMineList, vItemList, vStructList, vStructList2;
	vMineList.clear();
	vItemList.clear();
	vStructList.clear();
	vStructList2.clear();

	D3DXVECTOR3 vPos, vDiff;
	float fCurRangeSq = 0.0f;
	float fRangeSq = m_fAbsorbRange * m_fAbsorbRange;

	// FindMine
	ioWeaponManager::WeaponIterator kMine = g_WeaponMgr.GetMineWeaponIterator();
	while( kMine.HasMoreElements() )
	{
		ioMineWeapon *pMine = ToMineWeapon( kMine.Next() );
		if( !pMine || !pMine->IsLive() )
			continue;

		vPos = pMine->GetPosition();
		vDiff = m_vCreatePos - vPos;

		fCurRangeSq = D3DXVec3LengthSq( &vDiff );
		if( fRangeSq >= fCurRangeSq )
		{
			int iIndex = pMine->GetWeaponIndex();
			vMineList.push_back( iIndex );
		}
	}

	// FindFieldItem
	ioPlayStage::GameEntityIterator kFieldItem = pStage->GetFieldItemIterator();
	while( kFieldItem.HasMoreElements() )
	{
		ioFieldItem *pFieldItem = ToFieldItem( kFieldItem.Next() );
		if( !pFieldItem )
			continue;
		if( pFieldItem->GetCrownItemType() != ioItem::MCT_NONE )
			continue;

		vPos = pFieldItem->GetWorldPosition();
		vDiff = m_vCreatePos - vPos;

		fCurRangeSq = D3DXVec3LengthSq( &vDiff );
		if( fRangeSq >= fCurRangeSq )
		{
			int iIndex = pFieldItem->GetItemCreateIndex();
			vItemList.push_back( iIndex );
		}
	}

	// FindStruct
	int i = 0;
	int iPushCnt = g_PushStructListMgr.GetPushStructCnt();
	for( i=0 ; i<iPushCnt ; ++i )
	{
		ioPushStruct *pPush = g_PushStructListMgr.GetPushStruct( i );
		if( !pPush ) continue;
		if( pPush->GetState() != ioPushStruct::PS_DELAY )
			continue;
		if( !pPush->IsObjectCollision() )
			continue;

		vPos = pPush->GetWorldPosition();
		vDiff = m_vCreatePos - vPos;

		fCurRangeSq = D3DXVec3LengthSq( &vDiff );
		if( fRangeSq >= fCurRangeSq )
		{
			int iIndex = pPush->GetPushArrayIdx();
			vStructList.push_back( iIndex );
			vStructList2.push_back( i );
		}
	}

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CTPK_ABSORB_REQUEST );

			int iCnt = vMineList.size();
			kPacket << iCnt;
			for( i=0; i < iCnt; ++i )
				kPacket << vMineList[i];

			iCnt = vItemList.size();
			kPacket << iCnt;
			for( i=0; i < iCnt; ++i )
				kPacket << vItemList[i];

			iCnt = vStructList.size();
			kPacket << iCnt;
			for( i=0; i < iCnt; ++i )
				kPacket << vStructList[i];

			kPacket << pChar->GetCharName();
			kPacket << GetName();
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		int iMineCnt, iFieldCnt, iStructCnt;
		int i = 0;

		iMineCnt = vMineList.size();
		for( i=0; i < iMineCnt; ++i )
		{
			ioWeaponManager::WeaponIterator kMine = g_WeaponMgr.GetMineWeaponIterator();
			while( kMine.HasMoreElements() )
			{
				ioMineWeapon *pMine = ToMineWeapon( kMine.Next() );
				if( !pMine || !pMine->IsLive() )
					continue;

				if( vMineList[i] == pMine->GetWeaponIndex() )
				{
					pMine->SetWeaponDead();
				}
			}
		}

		iFieldCnt = vItemList.size();
		for( i=0; i < iFieldCnt; ++i )
		{
			pStage->HidingFieldItem( vItemList[i] );
		}

		iStructCnt = vStructList2.size();
		for( i=0; i < iStructCnt; ++i )
		{
			ioPushStruct *pPush = g_PushStructListMgr.GetPushStruct( vStructList2[i] );

			if( pPush )
			{
				pPush->StructDie( true );
			}
		}

		int iTotalCnt = iMineCnt + iFieldCnt + iStructCnt;

		ApplyAbsorbInfo( pChar, iTotalCnt );
	}
}

void ioAbsorbSkill::ApplyAbsorbInfo( ioBaseChar *pChar, int iTotalCnt )
{
	float fCurRecoveryGauge = iTotalCnt * m_fAbsorbGauge;

	for( int iSlot=0; iSlot < 4; ++iSlot )
	{
		ioItem *pItem = pChar->GetEquipedItem( iSlot );
		ioSkill *pSkill = pChar->GetEquipedSkill( iSlot );

		if( !pItem || !pSkill ) continue;
		if( pItem == GetOwnerItem() ) continue;

		float fCurGauge = pItem->GetCurSkillGuage();
		fCurGauge += fCurRecoveryGauge;
		pItem->SetCurSkillGauge( fCurGauge );
	}

	SetEndState( pChar );
}


