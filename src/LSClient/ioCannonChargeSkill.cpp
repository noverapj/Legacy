

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioCannonChargeSkill.h"

#include "WeaponDefine.h"

ioCannonChargeSkill::ioCannonChargeSkill()
{
	m_State = SS_NONE;
	
	m_bSetHandMesh = false;
	
	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_DwReadyActionEndTime = 0;
	m_fSkillTotalMoveAmount = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vSkillStartPos = ioMath::VEC3_ZERO;

	m_bLeftRot = false;
	m_bSend = true;

}

ioCannonChargeSkill::ioCannonChargeSkill( const ioCannonChargeSkill &rhs )
: ioAttackSkill( rhs ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_SkillPreMotion( rhs.m_SkillPreMotion ),
 m_fSkillPreMotionLate( rhs.m_fSkillPreMotionLate ),
 m_dwOrgLoopDuration( rhs.m_dwOrgLoopDuration ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_fCurMoveSpeed(rhs.m_fCurMoveSpeed ),
 m_fSkillMoveRange( rhs.m_fSkillMoveRange ), 
 m_dwSkillFireRotateTime( rhs.m_dwSkillFireRotateTime ),
 m_dwSkillLoopRotateTime( rhs.m_dwSkillLoopRotateTime ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_fCharTriggerHeight( rhs.m_fCharTriggerHeight ),
 m_OwnerBuffList( rhs.m_OwnerBuffList ),
 m_fMoveProtectedRange( rhs.m_fMoveProtectedRange ),
 m_ReadySkillMapEffect( rhs.m_ReadySkillMapEffect ),
 m_LoopSkillMapEffect( rhs.m_LoopSkillMapEffect ),
 m_FireSkillMapEffect( rhs.m_FireSkillMapEffect ),
 m_CannonFireSound( rhs.m_CannonFireSound )
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_DwReadyActionEndTime = 0;
	m_fSkillTotalMoveAmount = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vSkillStartPos = ioMath::VEC3_ZERO;

	m_bLeftRot = false;
	m_bSend = true;
}

ioCannonChargeSkill::~ioCannonChargeSkill()
{
}

void ioCannonChargeSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	
	rkLoader.LoadString_e( "skill_ready_motion", "", szBuf, MAX_PATH );
	m_SkillPreMotion = szBuf;	
	m_fSkillPreMotionLate = rkLoader.LoadFloat_e( "skill_ready_motion_rate", FLOAT1 );	

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;	
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );	
	m_dwOrgLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	
	m_fCharTriggerHeight = rkLoader.LoadFloat_e( "fire_trigger_height", 0.0f );

	m_fCurMoveSpeed = rkLoader.LoadFloat_e( "fire_move_speed", 0.0f );
	m_dwSkillFireRotateTime = (DWORD)rkLoader.LoadInt_e( "skill_fire_rotate_speed", 0 );
	m_dwSkillLoopRotateTime = (DWORD)rkLoader.LoadInt_e( "skill_loop_rotate_speed", 0 );

	m_fSkillMoveRange = rkLoader.LoadFloat_e( "fire_move_range", FLOAT1 );
	m_fMoveProtectedRange = rkLoader.LoadFloat_e( "fire_move_protected_range", FLOAT1 );

	rkLoader.LoadString_e( "skill_ready_map_effect", "", szBuf, MAX_PATH );
	m_ReadySkillMapEffect = szBuf;

	rkLoader.LoadString_e( "skill_loop_map_effect", "", szBuf, MAX_PATH );
	m_LoopSkillMapEffect = szBuf;

	rkLoader.LoadString_e( "skill_fire_map_effect", "", szBuf, MAX_PATH );
	m_FireSkillMapEffect = szBuf;

	rkLoader.LoadString_e( "waepon_fre_sound", "", szBuf, MAX_PATH );
	m_CannonFireSound = szBuf;

	m_AttackAttribute.Init();
	LoadAttackAttribute_e( "skill_charge_attack", m_AttackAttribute, rkLoader );

	LoadBuffList( rkLoader );
}

void ioCannonChargeSkill::LoadBuffList( ioINILoader &rkLoader )
{
	m_OwnerBuffList.clear();

	int i, iCount;
	ioBuffInfo tBuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

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

ioSkill* ioCannonChargeSkill::Clone()
{
	return new ioCannonChargeSkill( *this );
}

bool ioCannonChargeSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	
	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;	

	m_DwReadyActionEndTime = 0;
	m_fSkillTotalMoveAmount = 0.0f;

	m_State = SS_NONE;

	m_bLeftRot = false;
	m_bSend = true;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vSkillStartPos = ioMath::VEC3_ZERO;

	m_dwReadySkillMapEffect = -1;
	m_dwLoopSkillMapEffect = -1;
	m_dwFireSkillMapEffect = -1;

	m_CannonFireSoundID = 0;

	SetCameraBuff( pChar );

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();
		
	CheckSkillDesc( pChar );
	
	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}
 	
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{			
		SetReadyActionState( pChar, pStage );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioCannonChargeSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_DwReadyActionEndTime = 0;
	m_fSkillTotalMoveAmount = 0.0f;

	m_State = SS_NONE;

	m_bLeftRot = false;
	m_bSend = true;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vSkillStartPos = ioMath::VEC3_ZERO;
	
	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	
	if( !m_CannonFireSound.IsEmpty() )
	{			
		g_SoundMgr.StopSound( m_CannonFireSound, m_CannonFireSoundID );	
	}

	g_WeaponMgr.DestroyReserveZoneWeapon( pChar->GetCharName() );

	if( m_dwReadySkillMapEffect != -1 )
	{
		ioMapEffect* pMapReadyEffect = pStage->FindMapEffect( m_dwReadySkillMapEffect );
		if( pMapReadyEffect )	
		{				
			pStage->DestroyMapEffect(m_dwReadySkillMapEffect);
		}

		m_dwReadySkillMapEffect = -1;
	}	

	if( m_dwLoopSkillMapEffect != -1 )
	{
		ioMapEffect* pMapLoopEffect = pStage->FindMapEffect( m_dwLoopSkillMapEffect );
		if( pMapLoopEffect )	
		{				
			pStage->DestroyMapEffect(m_dwLoopSkillMapEffect);
		}

		m_dwLoopSkillMapEffect = -1;
	}

	if( m_dwFireSkillMapEffect != -1 )
	{
		ioMapEffect* pMapFireEffect = pStage->FindMapEffect( m_dwFireSkillMapEffect );
		if( pMapFireEffect )	
		{				
			pStage->DestroyMapEffect(m_dwFireSkillMapEffect);
		}

		m_dwFireSkillMapEffect = -1;
	}
}

void ioCannonChargeSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE :
		{				
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetReadyActionState( pChar, pStage );			

			}
		}
		break;

	case SS_PRE :
		{	
			UpdateEffect(pChar, pStage);

			if( m_DwReadyActionEndTime > 0 && m_DwReadyActionEndTime < dwCurTime )
			{				
				SetLoopState( pChar, pStage );
				return;
			}	
		}
		break;

	case SS_LOOP :
		{				
			if( COMPARE( dwCurTime, m_dwLoopDurationStart, m_dwLoopDurationEnd ) )
			{	

				CheckLoopState( pChar, pStage );
				return;
			}
			else
				SetEndState(pChar);
		}
		break;

	case SS_FIRE:
		{
			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			if( fTimePerSec == 0.0f )
				fTimePerSec = 0.001f;

			float fMoveAmt = m_fCurMoveSpeed * fTimePerSec;
			m_fSkillTotalMoveAmount += fMoveAmt;
						
			if( m_fSkillTotalMoveAmount < m_fSkillMoveRange )
				CheckFireState( pChar );
			else 
				SetExplosionState(pChar);
		}
		break;
	
	case SS_EXPLOSION:
		{	
			SetEndState(pChar);
		}
		break;
	case SS_END:
		{

		}
		break;
	}
}

bool ioCannonChargeSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{	

	if( !ioAttackSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCannonChargeSkill::SetReadyActionState( ioBaseChar *pChar, ioPlayStage *pStage  )
{
	EndPreDelayEffect( pChar );

	m_State = SS_PRE;	

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillPreMotion );
	if( iCurSkillAniID == -1 )	return;

	float fTimeRate = m_fSkillPreMotionLate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/(fTimeRate), 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();

	m_DwReadyActionEndTime = dwCurTime;
	m_DwReadyActionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
		m_bReduceGauge = true;
	}


	D3DXVECTOR3 vScale = pChar->GetWorldScale();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXQUATERNION qtCharRot;
	ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );	
	
	if( !m_ReadySkillMapEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		m_vSkillStartPos = vPos;

		ioMapEffect* pMapReadyEffect = pStage->CreateMapEffect( m_ReadySkillMapEffect, vPos, vScale );

		if( pMapReadyEffect )
		{	
			m_dwReadySkillMapEffect = pMapReadyEffect->GetUniqueID();			
			pMapReadyEffect->SetWorldOrientation( qtCharRot );
		}
	}
	
}



void ioCannonChargeSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;

	if( m_dwLoopDurationStart == 0 )
	{
		m_dwLoopDurationStart = dwCurTime;
		m_dwLoopDurationEnd = m_dwLoopDurationStart + m_dwOrgLoopDuration;		
	}

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_dwReadySkillMapEffect != -1 )
	{
		ioMapEffect* pMapReadyEffect = pStage->FindMapEffect( m_dwReadySkillMapEffect );
		if( pMapReadyEffect )	
		{				
			pStage->DestroyMapEffect(m_dwReadySkillMapEffect);
		}

		m_dwReadySkillMapEffect = -1;
	}
	
	D3DXVECTOR3 vScale = pChar->GetWorldScale();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXQUATERNION qtCharRot;
	ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );	

	if( !m_LoopSkillMapEffect.IsEmpty() )
	{	
		ioMapEffect* pMapLoopEffect = pStage->CreateMapEffect( m_LoopSkillMapEffect, m_vSkillStartPos, vScale );

		if( pMapLoopEffect )
		{	
			m_dwLoopSkillMapEffect = pMapLoopEffect->GetUniqueID();					
			pMapLoopEffect->SetWorldOrientation( qtCharRot );
		}
	}
	
}

void ioCannonChargeSkill::CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;
	
	CheckKeyInput( pChar );
	ProcessRotate( pChar, m_dwSkillLoopRotateTime );
	
	UpdateEffect(pChar, pStage);
	
	if( pChar->IsNeedProcess() )
	{
		//키입력 여부 검사
		bool bReserveSkill = false;
		int iSkillNum = pChar->GetSkillKeyInput();
		if( iSkillNum >= 0 )
		{
			bReserveSkill = true;
		}
		
		if( pChar->IsAttackKey() || pChar->IsDefenseKey() || pChar->IsJumpKey() && !bReserveSkill )
		{						
			AttackSkillFire( pChar );
		}
	}
}

void ioCannonChargeSkill::AttackSkillFire( ioBaseChar *pChar )
{	
	if( !pChar ) return;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage == NULL )
		return;

	m_State = SS_FIRE;
	ioEntityGroup *pGrp = pChar->GetGroup();
	
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vOrgPos = vPos;
	vPos.y += m_fCharTriggerHeight;
	pChar->SetWorldPosition( vPos );

	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	float fAniRate = m_AttackAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = GetAttribute().m_dwPreDelay * fAniRate;
	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );	
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1, FLOAT1/fAniRate );

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_AttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fAniRate,
		dwPreDelay
	);
	
	if( !m_CannonFireSound.IsEmpty() && pChar )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		m_CannonFireSoundID = g_SoundMgr.PlaySound( m_CannonFireSound, pGrp->GetParentSceneNode() );			
	}
	

	if( m_dwLoopSkillMapEffect != -1 )
	{
		ioMapEffect* pMapLoopEffect = pStage->FindMapEffect( m_dwLoopSkillMapEffect );
		if( pMapLoopEffect )	
		{				
			pStage->DestroyMapEffect(m_dwLoopSkillMapEffect);
			
		}

		m_dwLoopSkillMapEffect = -1;
	}


	D3DXVECTOR3 vScale = pChar->GetWorldScale();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXQUATERNION qtCharRot;
	ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );	

	if( !m_FireSkillMapEffect.IsEmpty() )
	{	
		if( m_dwFireSkillMapEffect == -1 )
		{
			ioMapEffect* pMapFireEffect = pStage->CreateMapEffect( m_FireSkillMapEffect, vOrgPos, vScale );
			if( pMapFireEffect )
			{	
				m_dwFireSkillMapEffect = pMapFireEffect->GetUniqueID();
				pMapFireEffect->SetWorldOrientation( qtCharRot );
			}
		}
	}

	if( pChar->IsNeedProcess() )
	{	
		if( pChar->IsNeedSendNetwork() && m_bSend )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SST_LOOP;
			kPacket << SLT_FIRE;
			kPacket << pChar->GetTargetRot();			
			P2PNetwork::SendToAllPlayingUser( kPacket );

		}else{

			m_bSend = true;
		}
	}
}

void ioCannonChargeSkill::CheckFireState( ioBaseChar *pChar )
{
	if( !CheckWeaponDead( pChar ) )
	{
		SetExplosionState( pChar );
	}

	CheckKeyInput( pChar );
	ProcessRotate( pChar, m_dwSkillFireRotateTime );

	pChar->SetCurMoveSpeed( m_fCurMoveSpeed );

}

void ioCannonChargeSkill::SetExplosionState(ioBaseChar *pChar)
{
	if( !pChar ) return;
	
	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();

	if( pItem )
		szItemName = pItem->GetName();	
	else
		return;
	
	m_State = SS_EXPLOSION;

	g_WeaponMgr.DestroyReserveZoneWeapon( pChar->GetCharName() );

	int iCnt = m_OwnerBuffList.size();	
	for( int i=0; i < iCnt; ++i )
	{			
		ioBuffInfo pInfo = m_OwnerBuffList[i];
		pChar->AddNewBuff( pInfo.m_BuffName,  pChar->GetCharName(), szItemName, this );

	}
}

void ioCannonChargeSkill::SetEndState( ioBaseChar *pChar )
{	
	if( !pChar ) return;
	
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	pChar->SetCurMoveSpeed( 0 );
	m_State = SS_END;

	if( m_dwLoopSkillMapEffect != -1 )
	{
		ioMapEffect* pMapLoopEffect = pStage->FindMapEffect( m_dwLoopSkillMapEffect );
		if( pMapLoopEffect )	
		{				
			pStage->DestroyMapEffect(m_dwLoopSkillMapEffect);
		}

		m_dwLoopSkillMapEffect = -1;
	}

	if( m_dwFireSkillMapEffect != -1 )
	{
		ioMapEffect* pMapFireEffect = pStage->FindMapEffect( m_dwFireSkillMapEffect );
		if( pMapFireEffect )	
		{
			pStage->DestroyMapEffect(m_dwFireSkillMapEffect);
		}

		m_dwFireSkillMapEffect = -1;
	}
}

SkillCharColType ioCannonChargeSkill::IsCharSkipState() const
{	
	switch( m_State )
	{
	case SS_EXPLOSION:
	case SS_FIRE:
			return m_EnableCharCollisionSkip;		
	default:
		return SCCT_NONE;
	}	

	return m_EnableCharCollisionSkip;
}

bool ioCannonChargeSkill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_State )
	{
	case SS_PRE:
		return false;
		break;
	
	case SS_FIRE:
		{
			if( m_fSkillTotalMoveAmount <= m_fMoveProtectedRange )
			{
				return true;
			}

			return false;
		}
		break;
	case SS_LOOP:
	case SS_EXPLOSION:
	case SS_END:		
		return false;
		break;
	}

	return false;
}

void ioCannonChargeSkill::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{	
		m_CurDirKey = eNewDirKey;
		
		if( pChar->IsNeedSendNetwork() )
		{
			if( m_State == SS_LOOP )
			{		
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SST_LOOP;
				kPacket << SLT_LOOP;
				kPacket << pChar->GetTargetRot();				
				kPacket << (int)m_CurDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			
			}
			else if( m_State == SS_FIRE )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SST_FIRE;				
				kPacket << pChar->GetTargetRot();				
				kPacket << (int)m_CurDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
					
			}
		}
	}
}

void ioCannonChargeSkill::ProcessRotate( ioBaseChar *pChar, float fRotateTime )
{
	if( !pChar ) return;
	if( fRotateTime <= 0 ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pChar->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pChar->SetTargetRotToRotate( qtNewRot, true );
}

void ioCannonChargeSkill::UpdateEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{			
	D3DXVECTOR3 m_vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	
	D3DXQUATERNION qtCharRot;
	ioMath::CalcDirectionQuaternion( qtCharRot, -m_vDir );	
	D3DXVec3Normalize( &m_vDir, &m_vDir );

	if(m_dwReadySkillMapEffect  != -1 )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();

		ioMapEffect* pMapReadyEffect = pStage->FindMapEffect( m_dwReadySkillMapEffect );
		if( pMapReadyEffect )
		{
			pMapReadyEffect->SetWorldPosition( vPos );
			pMapReadyEffect->SetWorldOrientation( qtCharRot );
		}
	}

	if( m_dwLoopSkillMapEffect != -1 )
	{

		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
			
		ioMapEffect* pMapLoopEffect = pStage->FindMapEffect( m_dwLoopSkillMapEffect );
		if( pMapLoopEffect )
		{
			pMapLoopEffect->SetWorldPosition( vPos );
			pMapLoopEffect->SetWorldOrientation( qtCharRot );
		}
	}
}

void ioCannonChargeSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{	
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
			
		}
	}
}

void ioCannonChargeSkill::SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock )
{
	if( !pOwner ) return;
	if( !pTarget ) return;
	if( pOwner->GetTeam() == pTarget->GetTeam() ) return;

	if( m_State == SS_FIRE )
	{	
		SetExplosionState( pOwner );
	}
}

void ioCannonChargeSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iCharStateType;
	rkPacket >> iCharStateType;
	
	if( iCharStateType == SST_LOOP )
	{
		int iFireStateType;		
		rkPacket >> iFireStateType;

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		
		switch( iFireStateType )
		{
		case SLT_LOOP :
			{			
				pChar->SetTargetRotToRotate( qtRot, true, false );
		
				int iDir;
				rkPacket >> iDir;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}
			break;

		case SLT_FIRE :
			{			
				pChar->SetTargetRotToRotate( qtRot, true, false );
				m_bSend = false;
				AttackSkillFire( pChar );
			}
			break;
		}
	}
	else if( iCharStateType == SST_FIRE )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pChar->SetTargetRotToRotate( qtRot, true, false );

		int iDir;
		rkPacket >> iDir;
		m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
	}
}


bool ioCannonChargeSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_State )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_FIRE:
	case SS_EXPLOSION:
		return false;
	
	case SS_END:	
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;

		return false;
	}

	return true;
}

bool ioCannonChargeSkill::IsAttackEndState() const
{	
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioCannonChargeSkill::IsNoDropState() const
{	
	if( m_State == SS_FIRE )
		return true;

	return false;
}

bool ioCannonChargeSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioCannonChargeSkill::IsCanSendControl() const
{
	if( m_State == SS_FIRE )
		return false;

	return true;
}

bool ioCannonChargeSkill::CheckWeaponDead( ioBaseChar *pChar )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( ToZoneEffectWeapon( pWeapon ) )
		return true;

	return false;
}




