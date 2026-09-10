
#include "stdafx.h"

#include "ioBildingSkill.h"

#include "ioGrowthUpInfo.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioBildingSkill::ioBildingSkill()
{
	m_BildingState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_bSetHandMesh = false;

	m_bEffectCreated = false;
}

ioBildingSkill::ioBildingSkill( const ioBildingSkill &rhs )
: ioNormalSkill( rhs ),
 m_fOffSet( rhs.m_fOffSet ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_dwLoopTime( rhs.m_dwLoopTime ),
 m_EndAnimation( rhs.m_EndAnimation ),
 m_fEndAniRate( rhs.m_fEndAniRate ),
 m_BildEffect( rhs.m_BildEffect ),
 m_StartHandMesh( rhs.m_StartHandMesh ),
 m_LoopHandMesh( rhs.m_LoopHandMesh ),
 m_EndHandMesh( rhs.m_EndHandMesh ),
 m_iBildStruct( rhs.m_iBildStruct ),
 m_iMaxStructCnt( rhs.m_iMaxStructCnt ),
 m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration )
{
	m_BildingState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_bSetHandMesh = false;

	m_bEffectCreated = false;
}

ioBildingSkill::~ioBildingSkill()
{
}

void ioBildingSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fOffSet = max( rkLoader.LoadFloat_e( "position_offset", 0.0f ), FLOAT1 );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwLoopProtectDuration = rkLoader.LoadInt_e( "loop_protect_duration", 0 );

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_hand_mesh", "", szBuf, MAX_PATH );
	m_StartHandMesh = szBuf;
	rkLoader.LoadString_e( "loop_hand_mesh", "", szBuf, MAX_PATH );
	m_LoopHandMesh = szBuf;
	rkLoader.LoadString_e( "end_hand_mesh", "", szBuf, MAX_PATH );
	m_EndHandMesh = szBuf;

	rkLoader.LoadString_e( "bild_effect", "", szBuf, MAX_PATH );
	m_BildEffect = szBuf;

	m_iBildStruct = rkLoader.LoadInt_e( "bild_struct_num", 0 );
	m_iMaxStructCnt = rkLoader.LoadInt_e( "max_struct_cnt", 0 );
}

ioSkill* ioBildingSkill::Clone()
{
	return new ioBildingSkill( *this );
}

bool ioBildingSkill::IsProtected( int iDefenseBreakType ) const
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
	if( m_BildingState == RS_ACTION )
		return true;

	if( m_BildingState == RS_LOOP )
	{
		if( m_dwLoopProtectDuration == 0 )
			return true;

		if( m_dwProtectTime > dwCurTime )
			return true;
	}
	
	if( m_BildingState == RS_END )
		return false;

	return false;
}

bool ioBildingSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
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

bool ioBildingSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_BildingState != RS_END )
		return false;

	if( m_dwEndAniStartTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioBildingSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->SetSkillProtection();

	m_BildingState = RS_NONE;
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

void ioBildingSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_BildingState )
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
		if( !m_bEffectCreated && !m_BildEffect.IsEmpty() )
		{
			if( m_dwMotionStartTime + 200 < FRAMEGETTIME() )
			{
				D3DXVECTOR3 vCreatePos = GetCreatePosition( pChar, pStage );
				pChar->CreateMapEffectBySkill( m_BildEffect, vCreatePos );
				m_bEffectCreated = true;
			}
		}

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

void ioBildingSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;

	if( !m_StartHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );

	if( !m_LoopHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );

	if( !m_EndHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );

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

void ioBildingSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );
}

void ioBildingSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );
}

void ioBildingSkill::SetActionState( ioBaseChar *pChar )
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
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate - FLOAT100;

	m_BildingState = RS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if(  m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	m_bEffectCreated = false;
}

D3DXVECTOR3 ioBildingSkill::GetCreatePosition( ioBaseChar *pChar, ioPlayStage *pStage )
{
	D3DXVECTOR3 vRayDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vRayOrg = pChar->GetWorldAxisBox().GetCenter();
	vRayOrg.y += pChar->GetWorldAxisBox().GetExtents( 1 );

	D3DXVECTOR3 vCreatePos = vRayOrg + vRayDir * m_fOffSet;

	D3DXVECTOR3 vColPoint;
	ioRay kRay( vRayOrg, vRayDir );

	if( pStage->TestWorldMeshCollision( kRay, &vColPoint ) )
	{
		D3DXVECTOR3 vDiff = vColPoint - vRayOrg;

		if( D3DXVec3Length( &vDiff ) < m_fOffSet )
		{
			vCreatePos = kRay.GetOrigin() + vDiff * 0.55f;
		}
	}

	// 아직 실제 높이를 모르기때문에 일단 캐릭터 높이 기준으로 생성한다.
	vCreatePos.y = pChar->GetWorldPosition().y;

	return vCreatePos;
}

void ioBildingSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioPushStruct *pDelete = g_PushStructListMgr.CheckMaxStructCnt( pChar, m_iBildStruct, m_iMaxStructCnt );
	int iDestroyIndex = 0;

	if( pDelete )
	{
		iDestroyIndex = pDelete->GetPushArrayIdx();
	}

	D3DXVECTOR3 vCreatePos = GetCreatePosition( pChar, pStage );
	vCreatePos.y = 0.0f;	// y값이 지정된경우는 그위치에 생성되고 1.0보다 작으면 높이 계산을 한다.

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			ioHashString szItemName;
			if( GetOwnerItem() )
				szItemName = GetOwnerItem()->GetName();

			SP2Packet kPacket( CTPK_PUSHSTRUCT_CREATE );
			kPacket << iDestroyIndex;
			kPacket << m_iBildStruct;
			kPacket << vCreatePos;
			kPacket << pChar->GetTargetRot();
			kPacket << pChar->GetCharName();
			kPacket << szItemName;
			kPacket << 0;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		// 싱글모드 뿐만아니라 로비에서도 이쪽으로 오기때문에 주의가 필요함
		if( pDelete )
		{
			pDelete->StructDie( true );
		}

		DWORD dwSeed = timeGetTime();
		int iCurIndex = g_PushStructListMgr.GetCurLocalIndex() + 1;
		ioPushStruct *pPush = pStage->CreatePushStruct( m_iBildStruct,
														iCurIndex,
														vCreatePos,
														pChar->GetTargetRot(),
														pChar->GetCharName(),
														0,
														dwSeed,
														0,
														true );

		if( pPush )
		{
			ioHashString szItemName;
			if( GetOwnerItem() )
				szItemName = GetOwnerItem()->GetName();
			
			pPush->SetCreateItem( szItemName );

			float fPreMax = pPush->GetHP().m_fMaxValue;
			float fPreCur = pPush->GetHP().m_fCurValue;
			float fValue = 0.0f;

			ioGrowthUpInfo *pUpInfo = GetGrowthUpInfoByType( GT_PUSHSTRUT_HP_UP, pChar->GetCharName() );
			ioGrowthNormalUpInfo *pNormal = ToNormalUpInfo( pUpInfo );
			if( pNormal )
			{
				fValue = pNormal->GetValue(pStage);
			}

			fPreMax += fValue;
			fPreCur += fValue;

			pPush->SetHP( fPreCur, fPreMax );
		}
	}

	if( m_LoopAnimation.IsEmpty() )
	{
		m_dwLoopStartTime = 0;
		return;
	}

	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_LoopHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_BildingState = RS_LOOP;
}

void ioBildingSkill::SetEndState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iCurSkillAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );
	
	m_dwEndAniStartTime = FRAMEGETTIME();		
	m_dwEndAniEndTime = m_dwEndAniStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fEndAniRate;

	m_BildingState = RS_END;

	if( m_bSetHandMesh )
	{
		if( !m_LoopHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_EndHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

bool ioBildingSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

