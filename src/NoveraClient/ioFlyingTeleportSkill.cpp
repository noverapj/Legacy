

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioFlyingTeleportSkill.h"

#include "WeaponDefine.h"

ioFlyingTeleportSkill::ioFlyingTeleportSkill()
{
	m_dwFireStartTime	= 0;
	m_dwAttackEndTime	= 0;
	m_dwMapEffectID		= -1;
	m_bSettedInvisibleState	= false;
	
	m_bSetUpKey		= false;
	m_bSetDownKey	= false;
}

ioFlyingTeleportSkill::ioFlyingTeleportSkill( const ioFlyingTeleportSkill &rhs )
: ioNormalSkill( rhs ),
  m_AttackAttribute( rhs.m_AttackAttribute ),
  m_AirAttackAttribute( rhs.m_AirAttackAttribute ),
  m_fTeleportRange( rhs.m_fTeleportRange ),
  m_fBackRange( rhs.m_fBackRange ),
  m_fHeightRange( rhs.m_fHeightRange ),
  m_OwnerBuffList( rhs.m_OwnerBuffList ),
  m_bDisableGrowth( rhs.m_bDisableGrowth ),
  m_fGuagePerTic( rhs.m_fGuagePerTic ),
  m_dwTicTime( rhs.m_dwTicTime ),
  m_szSkillAttackEffect( rhs.m_szSkillAttackEffect ),
  m_szSkillAttackEffectAir( rhs.m_szSkillAttackEffectAir )
{
	m_dwFireStartTime	= 0;
	m_dwAttackEndTime	= 0;
	m_dwMapEffectID		= -1;
	m_bSettedInvisibleState	= false;
	
	m_bSetUpKey		= false;
	m_bSetDownKey	= false;
}

ioFlyingTeleportSkill::~ioFlyingTeleportSkill()
{
}

void ioFlyingTeleportSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	LoadAttackAttribute_e( "attack", m_AttackAttribute, rkLoader );
	LoadAttackAttribute_e( "attack_air", m_AirAttackAttribute, rkLoader );

	m_fTeleportRange = rkLoader.LoadFloat_e( "teleport_range", 0.0f );
	m_fBackRange = rkLoader.LoadFloat_e( "back_move_range", 0.0f );
	m_fHeightRange = rkLoader.LoadFloat_e( "back_move_height_range", 0.0f );

	LoadBuffList( rkLoader );

	// Skill Gauge
	m_bDisableGrowth	= rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic		= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime			= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	// skill Effect
	rkLoader.LoadString_e( "skill_attack_effect", "", szBuf, MAX_PATH );
	m_szSkillAttackEffect = szBuf;
	rkLoader.LoadString_e( "skill_attack_effect_air", "", szBuf, MAX_PATH );
	m_szSkillAttackEffectAir = szBuf;
}

ioSkill* ioFlyingTeleportSkill::Clone()
{
	return new ioFlyingTeleportSkill( *this );
}

bool ioFlyingTeleportSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_AttackState = AS_NONE;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	// hand mesh를 predelay 시점에 하기에...
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	pChar->AttachEffect( m_SkillEffectName );

	m_dwFireStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	m_bSettedInvisibleState	= false;
	
	m_bSetUpKey		= false;
	m_bSetDownKey	= false;

	// 스킬 게이지
	m_dwCheckTime			= FRAMEGETTIME();
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	// 성장관련 요소 추가.
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

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
	return true;
}

void ioFlyingTeleportSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetNormalSkill( pChar );
				SetInvisibleState( pChar, true );
			}
		}
		break;
	case SS_LOOP:
		{
			if( iTickGap >= iCurTicTime )
			{
				m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

				if( fCurTicGauge != 0.0f )
				{
					if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
					{
						SetEndState( pChar );
						return;
					}
				}
			}

			if( m_dwMapEffectID != -1 )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwMapEffectID );
				if( pMapEffect )
				{
					pMapEffect->SetWorldPosition( pChar->GetMidPositionByRate() );
					pMapEffect->SetWorldOrientation( pChar->GetWorldOrientation() );
				}
			}

			if( pChar->IsNeedProcess() )
			{
                CheckKeyInputChange( pChar );
				if( pChar->IsAttackKey() )
				{
					SetTeleport( pStage, pChar );
					SetNormalFire( pChar );
					SetInvisibleState( pChar, false );
				}
			}

			ProcessMove( pChar );
		}
		break;
	case SS_ACTION:
		{
			if( m_dwMotionEndTime <= dwCurTime )
				SetEndState( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioFlyingTeleportSkill::OnSkillEnd( ioBaseChar *pChar )
{
	m_dwFireStartTime = 0;

	SetInvisibleState( pChar, false );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	ioNormalSkill::OnSkillEnd( pChar );
}

void ioFlyingTeleportSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		ioBaseChar *pOwner = pWeapon->GetOwner();
		D3DXVECTOR3 vDir = pOwner->GetMoveDir();
		if( pOwner )
		{
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( false, true );
		}
	}
}

void ioFlyingTeleportSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_KEY_CHANGE:
		{
			rkPacket >> m_bSetUpKey >> m_bSetDownKey;
			D3DXVECTOR3	vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
		}
		break;
	case SSC_TELEPORT:
		{
			D3DXVECTOR3 vTargetPos;
			D3DXQUATERNION qtTargetOrientation;

			rkPacket >> vTargetPos >> qtTargetOrientation;

			if( pOwner )
			{
				pOwner->SetWorldPosition( vTargetPos );
				pOwner->SetTargetRotAndMoveDirChange( qtTargetOrientation );
			}

			SetNormalFire( pOwner );
			SetInvisibleState( pOwner, false );
		}
		break;
	}
}

bool ioFlyingTeleportSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_LOOP:
	case SS_ACTION:
		return false;
	case SS_END:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			return true;
		return false;
	}

	return true;
}

bool ioFlyingTeleportSkill::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		if( m_AttackState == AS_LAND )
			return false;
		else
			return true;
	}

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioFlyingTeleportSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_ACTION:
	case SS_END:
		if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
			return false;

		if( m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

void ioFlyingTeleportSkill::LoadBuffList( ioINILoader &rkLoader )
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
}

void ioFlyingTeleportSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() )	return;

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i<iCnt; ++i)
	{
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
	}
}

void ioFlyingTeleportSkill::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() )	return;
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i<iCnt; ++i)
	{
		pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
	}
}

void ioFlyingTeleportSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_SkillState = SS_LOOP;
}

void ioFlyingTeleportSkill::SetNormalFire( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	AttackAttribute vAttackAttribute;
	vAttackAttribute.Init();

	if( m_AttackState == AS_NONE )
	{
		float fCharHeight = pChar->GetWorldPosition().y;
		float fBottomHeight = pChar->GetBottomHeight();
		float fValue = fCharHeight - fBottomHeight;
		if( fValue - m_fHeightRange > FLOAT1 )
			m_AttackState = AS_FLOAT;
		else
			m_AttackState = AS_LAND;
	}

	switch( m_AttackState )
	{
	case AS_NONE:
		return;
		break;
	case AS_LAND:
		vAttackAttribute = m_AttackAttribute;
		break;
	case AS_FLOAT:
		vAttackAttribute = m_AirAttackAttribute;
		break;
	}

	int iCurSkillAniID = pGrp->GetAnimationIdx( vAttackAttribute.m_AttackAnimation );
	if( iCurSkillAniID == -1 )
		return;
	
	float fTimeRate = vAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = vAttackAttribute.m_dwPreDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, 
										iCurSkillAniID,
										vAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwFireMotionEndTime = dwCurTime + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID );

	pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem()->GetName() ) );
}

void ioFlyingTeleportSkill::SetEndState( ioBaseChar *pChar )
{
	//SetSKillEndJumpState() 를 사용하기 위해 약간의 여유시간은 둔다.
	m_dwMotionEndTime = FRAMEGETTIME() + 100;
	m_SkillState = SS_END;
}

void ioFlyingTeleportSkill::SetTeleport( ioPlayStage *pStage, ioBaseChar *pChar )
{
	if( !pStage )	return;
	if( !pChar )	return;

	if( !pChar->IsNeedProcess() )
		return;

	D3DXVECTOR3 vScale = pChar->GetWorldScale();
	pChar->CreateMapEffectBySkill( m_SkillEffectName, pChar->GetWorldPosition(), vScale );

	float fPrevDistSq = m_fTeleportRange;
	ioBaseChar *pTargetChar = pChar;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )
			continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;


		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 	
			continue;
				
		if( pTarget->IsPrisonerMode() )
			continue;

		if( pTarget->IsApplyDownState(false) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )	// Enemy
		{
			pTargetChar = pTarget;
			fPrevDistSq = fDistSq;
			if( pTarget->IsFloating() )
				m_AttackState = AS_FLOAT;
			else
				m_AttackState = AS_LAND;
		}
	}

	if( pChar && pTargetChar )
	{
		D3DXVECTOR3 vOffSet( 0.0f, m_fHeightRange, 0.0f );
		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition() + vOffSet;
		D3DXQUATERNION qtTargetOrientation = pTargetChar->GetWorldOrientation();	
		D3DXVECTOR3 vNewPos = vTargetPos - ( ( qtTargetOrientation * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) ) * m_fBackRange );

		pChar->SetWorldPosition( vNewPos );
		pChar->SetTargetRotAndMoveDirChange( qtTargetOrientation );

		if( pStage )
			pStage->SetCameraToOwnerExplicit( pChar );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_TELEPORT;
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetTargetRot();
			//kPacket << pChar->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioFlyingTeleportSkill::SetInvisibleState( ioBaseChar *pChar, bool bSetVisible )
{
	if( !pChar )	return;

	if( bSetVisible && !m_bSettedInvisibleState )
	{
		pChar->SetInvisibleState( true, true );
		pChar->SetSkillMoveEnable( true, true );

		AddOwnerBuff( pChar );

		D3DXVECTOR3 vScale = pChar->GetWorldScale();
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_SkillMapEffectName, vPos, vScale );
		if( pMapEffect )
		{
			m_dwMapEffectID = pMapEffect->GetUniqueID();
		}

		m_bSettedInvisibleState = true;
	}
	else if( m_bSettedInvisibleState )
	{
		pChar->SetInvisibleState( false, true );
		ioHelmetItem *pHelmet = ToHelmetItem( pChar->GetEquipedItem( ES_HELMET ) );
		if( pHelmet )
		{
			ioHelmetItem::InvisibleFaceType eType = pHelmet->GetInvisibleFaceType();

			switch( eType )
			{
			case ioHelmetItem::IFT_ALL:
				pChar->ShowFaceMesh( false );
				break;
			case ioHelmetItem::IFT_MALE:
				if( pChar->IsMale() )
					pChar->ShowFaceMesh( false );
				break;
			case ioHelmetItem::IFT_FEMALE:
				if( !pChar->IsMale() )
					pChar->ShowFaceMesh( false );
				break;
			}
		}

		if( m_bUseItemMesh )
			pChar->ShowWeaponItemMesh( false );

		pChar->SetSkillMoveEnable( false, true );

		D3DXVECTOR3 vScale = pChar->GetWorldScale();
		if( m_AttackState == AS_LAND )
			pChar->CreateMapEffectBySkill( m_szSkillAttackEffect, pChar->GetWorldPosition(), vScale );
		else if( m_AttackState == AS_FLOAT )
			pChar->CreateMapEffectBySkill( m_szSkillAttackEffectAir, pChar->GetWorldPosition(), vScale );

		RemoveOwnerBuff( pChar );

		pChar->SetCurMoveSpeed( 0.0f );

		if( m_dwMapEffectID != -1 )
		{
			pChar->DestroyMapEffect( m_dwMapEffectID );
			m_dwMapEffectID = -1;
		}
		
		m_bSettedInvisibleState = false;
	}
}

void ioFlyingTeleportSkill::CheckKeyInputChange( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( !pChar->IsNeedProcess() )	return;

	bool bChange = false;
	
	if( pChar->IsJumpKeyDown() )
	{
		if( !m_bSetUpKey )	bChange = true;
		m_bSetUpKey		= true;
		m_bSetDownKey	= false;
	}
	else if( pChar->IsDefenseKeyDown() )
	{
		if( !m_bSetDownKey )	bChange = true;
		m_bSetUpKey		= false;
		m_bSetDownKey	= true;
	}
	else
	{
		if( m_bSetUpKey || m_bSetDownKey )	bChange = true;
		m_bSetUpKey		= false;
		m_bSetDownKey	= false;
	}

	if( bChange && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_KEY_CHANGE;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFlyingTeleportSkill::ProcessMove( ioBaseChar *pChar )
{
	float fSpeed = pChar->GetMaxSpeed();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	fSpeed *= fTimePerSec;

	if( m_bSetUpKey )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		vPos.y += fSpeed;
		pChar->SetWorldPosition( vPos );
	}
	else if( m_bSetDownKey )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		vPos.y -= fSpeed;
		pChar->SetWorldPosition( vPos );
	}
}