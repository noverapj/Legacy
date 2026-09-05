

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioGlideSkill.h"

ioGlideSkill::ioGlideSkill()
{
	ClearData();
}

ioGlideSkill::ioGlideSkill( const ioGlideSkill &rhs )
: ioCountSkill( rhs ),
  m_NormalGlideInfo( rhs.m_NormalGlideInfo ),
  m_EndAnimation( rhs.m_EndAnimation ),
  m_fEndAniRate( rhs.m_fEndAniRate ),
  m_JumpGlideInfo( rhs.m_JumpGlideInfo ),
  m_dwRotateSpeed( rhs.m_dwRotateSpeed ),
  m_GuideEffect( rhs.m_GuideEffect )
{
	ClearData();
}

ioGlideSkill::~ioGlideSkill()
{
	InitExtraAniJump();
}

void ioGlideSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );

	LoadGlideInfo( "normal", m_NormalGlideInfo, rkLoader );
	LoadGlideInfo( "jump", m_JumpGlideInfo, rkLoader );


	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;

	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );

	rkLoader.LoadString_e( "guide_effect", "", szBuf, MAX_PATH );
	m_GuideEffect = szBuf;
}

void ioGlideSkill::ClearData()
{
	SetSkillState( SS_NONE );

	m_fMoveRange = 0;
	m_dwMotionEndTime = 0;
	m_dwEnableReserveTime = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_fCurAngle = 0.0f;
	m_fCurMaxAngle = 0.0f;
	m_fCurMinAngle = 0.0f;

	m_dwGuideEffectID = -1;

	m_bAngleUp = true;

	m_bSetHandMesh = false;
}

void ioGlideSkill::LoadGlideInfo( const char *szType, GlideInfo &rkInfo, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH] = "", szBuf[MAX_PATH] = "";

	wsprintf_e( szKey, "%s_glide_animation", szType );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	rkInfo.m_Animation = szBuf;

	wsprintf_e( szKey, "%s_glide_ani_rate", szType );
	rkInfo.m_fAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

	wsprintf_e( szKey, "%s_glide_delay_animation", szType );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	rkInfo.m_DelayAnimation = szBuf;

	wsprintf_e( szKey, "%s_glide_delay_duration", szType );
	rkInfo.m_dwDelayDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_glide_delay_ani_rate", szType );
	rkInfo.m_fDelayAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

	wsprintf_e( szKey, "%s_glide_charging_duration", szType );
	rkInfo.m_dwChargingDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_glide_default_angle", szType );
	rkInfo.m_fDefaultAngle = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_glide_angle_min", szType );
	rkInfo.m_fAngleMin = rkLoader.LoadFloat( szKey, 0.0f );
	
	wsprintf_e( szKey, "%s_glide_angle_max", szType );
	rkInfo.m_fAngleMax = rkLoader.LoadFloat( szKey, 0.0f );
	
	wsprintf_e( szKey, "%s_glide_range", szType );
	rkInfo.m_fRange = rkLoader.LoadFloat( szKey, 0.0f );
	
	wsprintf_e( szKey, "%s_glide_speed", szType );
	rkInfo.m_fSpeed = rkLoader.LoadFloat( szKey, 0.0f );
	
	wsprintf_e( szKey, "%s_glide_angle_moving_speed", szType );
	rkInfo.m_fAngleMovingSpeed = rkLoader.LoadFloat( szKey, 0.0f );
}

ioSkill* ioGlideSkill::Clone()
{
	return new ioGlideSkill( *this );
}

bool ioGlideSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		ioHashString szSkillMotion = GetSkillMotionName();

		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	return true;
}

bool ioGlideSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioCountSkill::OnSkillStart(pChar) )
		return false;

	ClearData();

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	return true;
}

void ioGlideSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
	}

	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + GetPreDelayTime() < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;

			EndPreDelayEffect( pChar );
			SetLoopState( pChar );
		}
		break;
	case SS_GLIDE_LOOP:
		ProcessLoopState( pChar );
		break;
	case SS_GLIDE_CHARGING:
		ProcessChargingState( pChar );
		break;
	case SS_GLIDE_MOVING:
		ProcessGlideState( pChar );
		break;
	}
}

void ioGlideSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioCountSkill::OnSkillEnd( pChar );

	pChar->SetSkillMoveEnable( false, false );

	if( m_dwGuideEffectID != -1 )
	{
		pChar->DestroyMapEffect( m_dwGuideEffectID );
		m_dwGuideEffectID = -1;
	}

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	ClearData();
}

bool ioGlideSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	ioHashString szPreDelay = GetPreDelayAniName();

	if( szPreDelay.IsEmpty() )
	{
		m_dwPreDelayStartTime = 0;
		return false;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( szPreDelay, FLOAT100 );

	m_dwPreDelayStartTime = FRAMEGETTIME();
	StartPreDelayEffect( pChar );
	
	return true;
}

void ioGlideSkill::AttackSkillFire( ioBaseChar *pChar )
{
	GlideInfo kInfo = GetGlideInfo();

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetSkillMotionName() );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetSkillMotionRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											GetAttribute().m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											0 );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
	//

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( GetAttribute().m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = GetAttribute().m_fAttackEndAniRate;

		pGrp->SetActionAni( GetAttribute().m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}


	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

void ioGlideSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SC_MOVING:
		{
			rkPacket >> m_vMoveDir;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pChar->SetWorldPosition( vPos );
			SetGlideState( pChar );
		}
		break;
	case SC_KEY:
		{
			int iDirKey;
			//D3DXVECTOR3 vPos;
			rkPacket >> iDirKey;
			rkPacket >> m_bLeftRot;
			//rkPacket >> vPos;

			//m_CurDirKey = dynamic_cast<ioUserKeyInput::DirKeyInput>(iDirKey);
			//pChar->SetWorldPosition( vPos );

			if( iDirKey == ioUserKeyInput::DKI_NONE )
			{
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			}
			else
			{
				m_bTargetRot = false;

				switch( iDirKey )
				{
				case ioUserKeyInput::DKI_UP:
					m_CurDirKey = ioUserKeyInput::DKI_UP;
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTUP;
					break;
				case ioUserKeyInput::DKI_RIGHT:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHT;
					break;
				case ioUserKeyInput::DKI_RIGHTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTDOWN;
					break;
				case ioUserKeyInput::DKI_DOWN:
					m_CurDirKey = ioUserKeyInput::DKI_DOWN;
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTDOWN;
					break;
				case ioUserKeyInput::DKI_LEFT:
					m_CurDirKey = ioUserKeyInput::DKI_LEFT;
					break;
				case ioUserKeyInput::DKI_LEFTUP:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTUP;
					break;
				default:
					m_CurDirKey = ioUserKeyInput::DKI_NONE;
					break;
				}
			}
		}
		break;
	case SC_ATTACK:
		{
			int iDirKey;
			rkPacket >> iDirKey;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			
			m_CurDirKey = static_cast<ioUserKeyInput::DirKeyInput>(iDirKey);
			
			pChar->SetWorldPosition( vPos );

			SetAttackState( pChar );
		}
		break;
	case SC_END:
		{
			SetEndState( pChar );
		}
		break;
	}
}

bool ioGlideSkill::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
		return true;

	if( m_SkillState == SS_GLIDE_END )
		return true;

	return false;
}

bool ioGlideSkill::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_GLIDE_LOOP:
	case SS_GLIDE_CHARGING:
	case SS_GLIDE_MOVING:
	case SS_GLIDE_ATTACK:
		return true;
	}

	return false;
}

bool ioGlideSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_GLIDE_MOVING:
		return false;
	case SS_GLIDE_LOOP:
	case SS_GLIDE_CHARGING:
	case SS_GLIDE_ATTACK:
	case SS_GLIDE_END:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;
	}

	return true;
}

void ioGlideSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_SkillState == SS_GLIDE_LOOP )
		return;

	SetSkillState( SS_GLIDE_LOOP );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	GlideInfo kInfo = GetGlideInfo();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( kInfo.m_DelayAnimation, FLOAT100, FLOAT1, FLOAT1 / kInfo.m_fDelayAniRate );

	m_dwMotionEndTime = FRAMEGETTIME() + kInfo.m_dwDelayDuration;

	if( pChar->IsNeedProcess() )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_GuideEffect, vPos );
		if( pMapEffect )
		{
			m_dwGuideEffectID = pMapEffect->GetUniqueID();
		}
	}

	m_fCurAngle = kInfo.m_fDefaultAngle;

	CheckDir( pChar );
	UpdateGuideEffect( pChar );

	pChar->SetSkillMoveEnable( true, true );
}

void ioGlideSkill::SetChargingState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_SkillState == SS_GLIDE_CHARGING )
		return;

	SetSkillState( SS_GLIDE_CHARGING );

	if( m_SkillUseType == SUT_JUMP )
		m_dwMotionEndTime = FRAMEGETTIME() + m_JumpGlideInfo.m_dwChargingDuration;
	else
		m_dwMotionEndTime = FRAMEGETTIME() + m_NormalGlideInfo.m_dwChargingDuration;
}

void ioGlideSkill::SetGlideState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_SkillState == SS_GLIDE_MOVING )
		return;

	SetSkillState( SS_GLIDE_MOVING );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	GlideInfo kInfo = GetGlideInfo();

	int iAniID = pGrp->GetAnimationIdx( kInfo.m_Animation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/kInfo.m_fAniRate );
	m_dwMotionEndTime = 0;

	pChar->SetSkillMoveEnable( false, false );
	pChar->CheckCharColSkipSkillTime( iAniID, kInfo.m_fAniRate, 0 );

	if( m_dwGuideEffectID != -1 )
	{
		pChar->DestroyMapEffect( m_dwGuideEffectID );
		m_dwGuideEffectID = -1;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SC_MOVING;
		kPacket << m_vMoveDir;
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGlideSkill::SetAttackState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_SkillState == SS_GLIDE_ATTACK )
		return;

	SetSkillState( SS_GLIDE_ATTACK );

	if( pChar->IsNeedProcess() )
		m_CurDirKey = pChar->GetDirKey();

	pChar->SetTargetRotToDirKey( m_CurDirKey );
	AttackSkillFire( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SC_ATTACK;
		kPacket << static_cast<int>(m_CurDirKey);
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGlideSkill::SetEndState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_SkillState == SS_GLIDE_END )
		return;

	SetSkillState( SS_GLIDE_END );
	m_dwMotionEndTime = 0;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iAniID == -1 )	
		return;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * m_fEndAniRate;

	pChar->SetSkillMoveEnable( false, false );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SC_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGlideSkill::ProcessLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	pChar->SetCurMoveSpeed( 0 );

	if( pChar->IsNeedProcess() )
	{
		CheckDir( pChar );
		UpdateGuideEffect( pChar );

		if( m_dwMotionEndTime < FRAMEGETTIME() )
			SetEndState( pChar );
		else if( pChar->IsJumpKey() )
			SetChargingState( pChar );
	}
}

void ioGlideSkill::ProcessChargingState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	pChar->SetCurMoveSpeed( 0 );

	if( pChar->IsNeedProcess() )
	{
		CheckAngle( pChar );
		CheckDir( pChar );
		UpdateGuideEffect( pChar );

		if( m_dwMotionEndTime < FRAMEGETTIME() )
			SetEndState( pChar );
		else if( !pChar->IsJumpKeyDown() )
			SetGlideState( pChar );
	}
}

void ioGlideSkill::ProcessGlideState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return;

	GlideInfo kInfo = GetGlideInfo();
	if( pChar->IsNeedProcess() )
	{
		if( pChar->IsAttackKey() )
		{
			SetAttackState( pChar );
			return;
		}
		else if( m_fMoveRange > 0.0f && m_fMoveRange > kInfo.m_fRange )
		{
			SetEndState( pChar );
			return;
		}

		if( CheckLanding( pChar ) )
		{
			SetEndState( pChar );
			return;
		}
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fMoveAMt = kInfo.m_fSpeed * fTimePerSec;
	m_fMoveRange += fMoveAMt;

	D3DXVECTOR3 vMoveAmt = m_vMoveDir * fMoveAMt;
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos += vMoveAmt;
	pChar->SetWorldPosition( vPos );
	
	CheckKeyInput( pChar );
	ProcessRotate( pChar );
}

void ioGlideSkill::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	if( pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir = pChar->GetMoveDir();
			D3DXQUATERNION qtCurRot = pChar->GetTargetRot();

			m_bTargetRot = false;
			m_bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SC_KEY;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				//kPacket << pChar->GetWorldPosition();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SC_KEY;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				//kPacket << pChar->GetWorldPosition();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioGlideSkill::ProcessRotate( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_dwRotateSpeed <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pChar->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pChar->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	m_vMoveDir = qtAngle * m_vMoveDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pChar->SetTargetRotToRotate( qtAngle, true, false );
}

bool ioGlideSkill::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	if( m_SkillUseType == SUT_NORMAL && m_SkillState == SS_GLIDE_MOVING )
		return false;
		
	return true;
}

void ioGlideSkill::CheckAngle( ioBaseChar* pChar )
{
	if( !pChar )
		return;

	GlideInfo kInfo = GetGlideInfo();

	if( kInfo.m_fAngleMovingSpeed == 0 )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fRate = ( fTimePerSec * 1000 ) / kInfo.m_fAngleMovingSpeed;
	float fValue = 360.0f * fRate;

	float fMaxAngle = kInfo.m_fAngleMax;
	float fMinAngle = kInfo.m_fAngleMin;

	if( m_bAngleUp )
	{
		m_fCurAngle += fValue;

		if( m_fCurAngle > fMaxAngle )
		{
			m_fCurAngle = fMaxAngle;
			m_bAngleUp = false;
		}
	}
	else
	{
		m_fCurAngle -= fValue;

		if( m_fCurAngle < fMinAngle )
		{
			m_fCurAngle = fMinAngle;
			m_bAngleUp = true;
		}
	}
}

void ioGlideSkill::CheckDir( ioBaseChar *pChar )
{	
	if( !pChar )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();

	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vSightDir, &ioMath::UNIT_Y );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fCurAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	m_vMoveDir = qtNewRot * vSightDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
}

void ioGlideSkill::UpdateGuideEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return;

	if( m_dwGuideEffectID != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwGuideEffectID );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( pChar->GetMidPositionByRate() );

			ioEffect *pEffect = pMapEffect->GetEffect();
			if( pEffect )
			{
				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
				pEffect->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}
}

const ioGlideSkill::GlideInfo &ioGlideSkill::GetGlideInfo() const
{
	if( m_SkillUseType == SUT_JUMP )
		return m_JumpGlideInfo;

	return m_NormalGlideInfo;
}