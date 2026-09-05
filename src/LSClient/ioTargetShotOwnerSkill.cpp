

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTargetShotOwnerSkill.h"

#include "WeaponDefine.h"

ioTargetShotOwnerSkill::ioTargetShotOwnerSkill()
{
	ClearData();
}

ioTargetShotOwnerSkill::ioTargetShotOwnerSkill( const ioTargetShotOwnerSkill &rhs )
	: ioNormalSkill( rhs ),
	m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_dwFireDelayTime( rhs.m_dwFireDelayTime ),
	m_szFireSound( rhs.m_szFireSound ),
	m_vAimInfoList( rhs.m_vAimInfoList ),
	m_fAimRange( rhs.m_fAimRange ),
	m_szAimTargetEffect( rhs.m_szAimTargetEffect ),
	m_szZeroAimTargetEffect( rhs.m_szZeroAimTargetEffect ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_bInvisibleCharMesh( rhs.m_bInvisibleCharMesh )
{
	ClearData();
}

ioTargetShotOwnerSkill::~ioTargetShotOwnerSkill()
{
	ClearData();
}

void ioTargetShotOwnerSkill::ClearData()
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwLoopStartTime	= 0;
	m_dwFireStartTime   = 0;
	m_dwAimCheckTime	= 0;
	m_dwZeroAimCheckTime= 0;

	m_szTargetName.Clear();

	m_bZeroAimFire = false;
}

void ioTargetShotOwnerSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwFireDelayTime = rkLoader.LoadInt_e( "fire_delay_time", 0 );

	rkLoader.LoadString_e( "fire_sound", "", szBuf, MAX_PATH );
	m_szFireSound = szBuf;

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	int iCnt = rkLoader.LoadInt_e( "aim_info_count", 0 );
	m_vAimInfoList.clear();
	m_vAimInfoList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		AimInfo kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "aim%d_mark_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_szAimMark = szBuf;
		wsprintf_e( szKey, "Zero_aim%d_mark_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_szZeroAimMark = szBuf;
		wsprintf_e( szKey, "aim%d_time", i+1 );
		kInfo.m_dwAimTime = (DWORD)rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "zero_aim%d_time", i+1 );
		kInfo.m_dwZeroAimTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "aim%d_attack", i+1 );
		LoadWeaponInfo( szKey, kInfo.m_AimWeaponInfo, rkLoader );

		wsprintf_e( szKey, "zero_aim%d_attack", i+1 );
		LoadWeaponInfo( szKey, kInfo.m_ZeroAimWeaponInfo, rkLoader );

		m_vAimInfoList.push_back(kInfo);
	}

	m_fAimRange = rkLoader.LoadFloat_e( "aim_target_range", 0.0f );

	rkLoader.LoadString_e( "aim_target_effect", "", szBuf, MAX_PATH );
	m_szAimTargetEffect = szBuf;
	rkLoader.LoadString_e( "zero_aim_target_effect", "", szBuf, MAX_PATH );
	m_szZeroAimTargetEffect = szBuf;

	m_bInvisibleCharMesh = rkLoader.LoadBool_e( "invisible_char_mesh", false );
}

ioSkill* ioTargetShotOwnerSkill::Clone()
{
	return new ioTargetShotOwnerSkill( *this );
}

bool ioTargetShotOwnerSkill::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_bSkillInJump = false;
	if( m_SkillUseType == SUT_JUMP )
		m_bSkillInJump = true;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwWeaponBaseIndex = 0;
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetNormalSkill( pChar );
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
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	pChar->AttachEffect( m_SkillEffectName );

	CheckSkillDesc( pChar );

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	IORandom random;
	DWORD dwSeed = 0;
	int iRand = 0;
	dwSeed = pChar->GetRandomSeed();
	random.SetRandomSeed( dwSeed );

	int iValue = m_vAimInfoList.size();
	m_iAimType = random.Random( iValue );

	SetSkillNameChatBubble( pChar );

	if( m_bSkillInJump )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
	}

	return true;
}

void ioTargetShotOwnerSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime != 0 && m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetNormalSkill( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckAimTime();
			CheckKeyInput( pChar );
			CheckTarget( pChar, pStage );
			CheckVDir( pChar );
			UpdateAimMark( pChar, pStage );

			ioPlayStage *pStage = pChar->GetCreator();
			if( pStage )
			{
				ioMapEffect *pEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
				if( pEffect )
				{
					pEffect->SetWorldOrientation( pChar->GetWorldOrientation() );
				}
			}

			if( pChar->IsNeedProcess() )
			{
				if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
				{
					SetFireDelayState( pChar );
				}
				else if( pChar->IsAttackKey() )
				{
					SetFireDelayState( pChar );
				}
			}
		}
		break;
	case SS_FIRE_DELAY:
		if( m_dwFireStartTime > 0 && m_dwFireStartTime + m_dwFireDelayTime < FRAMEGETTIME() )
		{
			SetFireState( pChar );
			m_dwFireStartTime = 0;
			return;
		}
		break;
	case SS_FIRE:
		{
			if( m_dwWeaponBaseIndex == 0 )
				SetEndState( pChar );
			else
			{
				ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
				if( !pWeapon )
				{
					SetEndState( pChar );
					return;
				}

				if( !pWeapon->IsLive() )
				{
					SetEndState( pChar );
				}
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioTargetShotOwnerSkill::CheckVDir( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_szTargetName.IsEmpty() )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )	return;

	D3DXVECTOR3 vPos1 = pChar->GetWorldPosition();
	D3DXVECTOR3 vPos2 = pTarget->GetWorldPosition();

	pChar->SetTargetRotToTargetPos( vPos2, true );

	m_vAttackDir = vPos2 - vPos1;
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	float fAngle = 0.0f;
	if( m_vAttackDir != vSightDir )
	{
		float fDotValue = D3DXVec3Dot( &m_vAttackDir, &vSightDir );
		fAngle = RADtoDEG( acosf(fDotValue) );
	}
}

void ioTargetShotOwnerSkill::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	EndPreDelayEffect( pChar );

	EndCameraBuff( pChar );
	pChar->EndEffect( m_SkillEffectName, false );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCurMoveSpeed( 0.0f );

	m_SkillState = SS_NONE;
	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	DestroyAimMark( pChar );

	m_dwWeaponBaseIndex = 0;

	if( m_bInvisibleCharMesh )
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
	}

	if( m_dwSkillMapEffect != -1 )
	{
		pChar->DestroyMapEffect(m_dwSkillMapEffect);
		m_dwSkillMapEffect = -1;
	}
}

bool ioTargetShotOwnerSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	return false;

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_LOOP:
	case SS_FIRE:
		return false;
	case SS_END:
		{
			return true;
		}
	}

	return true;
}

bool ioTargetShotOwnerSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
	{
		return true;
	}

	return false;
}

bool ioTargetShotOwnerSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_END )
	{
		return true;
	}

	return false;
}

void ioTargetShotOwnerSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SNT_CHANGE:
		{
			m_dwAimCheckTime = FRAMEGETTIME();
			m_dwZeroAimCheckTime = 0;
			DestroyAimMark( pChar );

			rkPacket >> m_szTargetName;
		}
		break;
	case SNT_ATTACK:
		{
			rkPacket >> m_vAttackDir;
			rkPacket >> m_bZeroAimFire;
			SetFireDelayState( pChar );
		}
		break;
	}
}

void ioTargetShotOwnerSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	pWeapon->SetMoveDir( m_vAttackDir );

	ioTraceWeapon7	*pTraceWeapon = ToTraceWeapon7( pWeapon );

	if( pTraceWeapon )
	{
		pTraceWeapon->SetTraceTarget( m_szTargetName );
	}
}

void ioTargetShotOwnerSkill::SetFireDelayState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_SkillState = SS_FIRE_DELAY;

	DestroyAimMark( pChar );
	m_dwFireStartTime = FRAMEGETTIME();

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( pTarget )
	{
		if( m_bZeroAimFire )
			pTarget->AttachEffect( m_szZeroAimTargetEffect );
		else
			pTarget->AttachEffect( m_szAimTargetEffect );
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SNT_ATTACK;
		kPacket << m_vAttackDir;
		kPacket << m_bZeroAimFire;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetShotOwnerSkill::SetFireState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	m_SkillState = SS_FIRE;

	WeaponInfo vWeaponInfo;

	if( m_bZeroAimFire )
		vWeaponInfo = m_vAimInfoList[m_iAimType].m_ZeroAimWeaponInfo;
	else
		vWeaponInfo = m_vAimInfoList[m_iAimType].m_AimWeaponInfo;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SkillFireExplicit( vWeaponInfo,
								GetName(),
								m_dwWeaponBaseIndex,
								pChar->GetMidPositionByRate() );

	if( m_dwSkillMapEffect != -1 )
	{
		pChar->DestroyMapEffect(m_dwSkillMapEffect);
		m_dwSkillMapEffect = -1;
	}

	if( !m_szFireSound.IsEmpty() )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
			g_SoundMgr.PlaySound( m_szFireSound, pGrp->GetParentSceneNode() );
	}
}

void ioTargetShotOwnerSkill::SetEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_SkillState = SS_END;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );

		if( fMapHeight > 0.0f && fMapHeight > vPos.y )
			vPos.y = fMapHeight;

		pOwner->SetWorldPosition( vPos );
	}
}

bool ioTargetShotOwnerSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime+m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	if( m_dwFireStartTime > 0 && m_dwFireStartTime + m_dwFireDelayTime > FRAMEGETTIME() )
		return true;

	return false;
}

void ioTargetShotOwnerSkill::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	EndPreDelayEffect( pChar );

	m_SkillState = SS_LOOP;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetLoopAni( m_SkillLoopMotion, 0.0f, FLOAT1 );

	m_dwAimCheckTime = dwCurTime;

	if( m_bInvisibleCharMesh )
		pChar->SetInvisibleState( true, true );

	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pEffect = pStage->CreateMapEffect( m_SkillMapEffectName, vPos, vScale );
		if( pEffect )
		{
			pEffect->SetWorldOrientation( pChar->GetWorldOrientation() );
			m_dwSkillMapEffect = pEffect->GetUniqueID();
		}
	}
}

bool ioTargetShotOwnerSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTargetShotOwnerSkill::CheckAimTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwAimCheckTime > 0 &&
		m_dwAimCheckTime + m_vAimInfoList[m_iAimType].m_dwAimTime < dwCurTime )
	{
		m_dwAimCheckTime = 0;
		m_dwZeroAimCheckTime = dwCurTime;

		if( !m_szTargetName.IsEmpty() )
			m_bZeroAimFire = true;
		else
			m_bZeroAimFire = false;
	}
	else if( m_dwZeroAimCheckTime > 0 &&
		m_dwZeroAimCheckTime + m_vAimInfoList[m_iAimType].m_dwZeroAimTime < dwCurTime )
	{
		m_dwAimCheckTime = dwCurTime;
		m_dwZeroAimCheckTime = 0;
		m_bZeroAimFire = false;
	}
}

void ioTargetShotOwnerSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsJumpKey() )
	{
		m_bJumpKey = true;
		m_bDefenseKey = false;
	}
	else if( pOwner->IsDefenseKey() )
	{
		m_bJumpKey = false;
		m_bDefenseKey = true;
	}
	else
	{
		m_bJumpKey = false;
		m_bDefenseKey = false;
	}
}

void ioTargetShotOwnerSkill::CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )	return;
	if( !pChar )	return;

	if( !pChar->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fAimRange*m_fAimRange;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	m_vTargetInfoList.clear();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )		
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		TargetInfo	kInfo;
		kInfo.m_TargetName = pTarget->GetCharName();
		kInfo.m_fLenth = fDistSq;

		float fEnableDistSq = m_fAimRange*m_fAimRange;
		if( fDistSq < fEnableDistSq )	// Enemy
		{
			m_vTargetInfoList.push_back(kInfo);
		}
	}

	if( m_szTargetName.IsEmpty() )
	{
		int iCnt = m_vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			float fDistSq = m_vTargetInfoList[i].m_fLenth;
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				fPrevDistSq = fDistSq;
				m_szTargetName = m_vTargetInfoList[i].m_TargetName;
			}
		}

		if( !m_szTargetName.IsEmpty() )
		{
			m_dwAimCheckTime = FRAMEGETTIME();
			m_dwZeroAimCheckTime = 0;
			DestroyAimMark( pChar );
			m_bZeroAimFire = false;

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SNT_CHANGE;
				kPacket << m_szTargetName;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else if( !m_vTargetInfoList.empty() )
	{
		int iCurTarget = -1;

		int iCnt = m_vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			if( m_szTargetName == m_vTargetInfoList[i].m_TargetName )
				iCurTarget = i;
		}

		if( iCurTarget != -1 )
		{
			bool bTargetChange = false;
			if( m_bJumpKey )
			{
				iCurTarget++;
				bTargetChange = true;
				if( iCurTarget >= iCnt )
					iCurTarget = 0;
			}
			else if( m_bDefenseKey )
			{
				bTargetChange = true;
				iCurTarget--;
				if( iCurTarget < 0 )
					iCurTarget = iCnt-1;
			}

			m_szTargetName = m_vTargetInfoList[iCurTarget].m_TargetName;

			if( bTargetChange )
			{
				m_dwAimCheckTime = FRAMEGETTIME();
				m_dwZeroAimCheckTime = 0;
				m_bZeroAimFire = false;
				DestroyAimMark( pChar );

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SNT_CHANGE;
					kPacket << m_szTargetName;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		else
			m_szTargetName.Clear();
	}
	else
		m_szTargetName.Clear();
}

void ioTargetShotOwnerSkill::UpdateAimMark( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if( !pChar->IsNeedProcess() )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( !pTarget )
	{
		DestroyAimMark( pChar );
		return;
	}

	if( m_dwAimCheckTime > 0 )
	{
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		if( m_vAimInfoList[m_iAimType].m_dwAimMarkID == -1 )
		{			
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_vAimInfoList[m_iAimType].m_szAimMark, vPos, vScale );
			if( pMapEffect )
				m_vAimInfoList[m_iAimType].m_dwAimMarkID = pMapEffect->GetUniqueID(); 
		}
		else if( m_vAimInfoList[m_iAimType].m_dwAimMarkID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_vAimInfoList[m_iAimType].m_dwAimMarkID );
			if( pMapEffect )
				pMapEffect->SetWorldPosition( vPos );
		}

		if( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID != -1 )
		{
			pStage->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
			m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = -1;
		}
	}
	else if( m_dwZeroAimCheckTime > 0 )
	{
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		if( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID == -1 )
		{
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_vAimInfoList[m_iAimType].m_szZeroAimMark, vPos, vScale );
			if( pMapEffect )
				m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = pMapEffect->GetUniqueID(); 
		}
		else if( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
			if( pMapEffect )
				pMapEffect->SetWorldPosition( vPos );
		}

		if( m_vAimInfoList[m_iAimType].m_dwAimMarkID != -1 )
		{
			pStage->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwAimMarkID );
			m_vAimInfoList[m_iAimType].m_dwAimMarkID = -1;
		}
	}
}

void ioTargetShotOwnerSkill::DestroyAimMark( ioBaseChar *pChar )
{
	if( !pChar )	return;

	pChar->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwAimMarkID );
	m_vAimInfoList[m_iAimType].m_dwAimMarkID = -1;

	pChar->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
	m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = -1;
}

bool ioTargetShotOwnerSkill::IsSkipState() const
{
	if( m_SkillState == SS_FIRE )
		return true;

	else
		return false;
}

void ioTargetShotOwnerSkill::SetPositionOwner( ioBaseChar *pOwner, D3DXVECTOR3 vPos )
{
	if( !pOwner )
		return;

	float fGapHeight = pOwner->GetMidHeightByRate() - pOwner->GetWorldPosition().y;
	vPos.y -= fGapHeight;
	pOwner->SetWorldPosition( vPos );
}


bool ioTargetShotOwnerSkill::IsNoDropState() const
{
	if( m_bSkillInJump )
		return true;

	return false;
}