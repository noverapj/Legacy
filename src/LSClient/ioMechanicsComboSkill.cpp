

#include "stdafx.h"

#include "ioMechanicsComboSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMechanicsComboSkill::ioMechanicsComboSkill()
{
	m_bSetHandMesh = false;
	m_bUsedEndOtherAttack = false;

	m_iCurLinearCombo = 0;
	m_dwNextComboTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bBallTarget = false;
	m_ComboTargetName.Clear();
	m_dwSkillMoveEndTime = 0;
	
	m_vTargetPos = ioMath::VEC3_ZERO;
	m_vTargetDir = ioMath::VEC3_ZERO;
}

ioMechanicsComboSkill::ioMechanicsComboSkill( const ioMechanicsComboSkill &rhs )
: ioAttackSkill( rhs ),
 m_vComboList( rhs.m_vComboList ),
 m_vJumpComboList( rhs.m_vJumpComboList ),
 m_vDownComboList( rhs.m_vDownComboList ),
 m_vShotInfoList( rhs.m_vShotInfoList ),
 m_vJumpShotInfoList( rhs.m_vJumpShotInfoList ),
 m_vDownShotInfoList( rhs.m_vDownShotInfoList ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_bAutoComboAction( rhs.m_bAutoComboAction ),
 m_ProtectCheckType( rhs.m_ProtectCheckType ),
 m_ComboAttackType( rhs.m_ComboAttackType ),
 m_bUseEndOtherAttack( rhs.m_bUseEndOtherAttack ),
 m_EndOtherAttack( rhs.m_EndOtherAttack ),
 m_JumpEndOtherAttack( rhs.m_JumpEndOtherAttack ),
 m_DownEndOtherAttack( rhs.m_DownEndOtherAttack ),
 m_bNoGravityCombo( rhs.m_bNoGravityCombo ),
 m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
 m_DummyCharList( rhs.m_DummyCharList ),
 m_DummyCharEffect( rhs.m_DummyCharEffect ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_fTargetAngle( rhs.m_fTargetAngle ),
 m_fTargetRange( rhs.m_fTargetRange ),
 m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
 m_fTargetDownHeight( rhs.m_fTargetDownHeight ),
 m_fAttMoveSpeed( rhs.m_fAttMoveSpeed ),
 m_fAttMoveRange( rhs.m_fAttMoveRange ),
 m_fAttMoveRate( rhs.m_fAttMoveRate )
{
	m_bSetHandMesh = false;
	m_bUsedEndOtherAttack = false;

	m_iCurLinearCombo = 0;
	m_dwNextComboTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bBallTarget = false;
	m_ComboTargetName.Clear();
	m_dwSkillMoveEndTime = 0;

	m_vTargetPos = ioMath::VEC3_ZERO;
	m_vTargetDir = ioMath::VEC3_ZERO;
}

ioMechanicsComboSkill::~ioMechanicsComboSkill()
{
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();
}

void ioMechanicsComboSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_ProtectCheckType = PCT_TIME;
	m_ComboAttackType = (ComboAttackType)rkLoader.LoadInt_e( "combo_attack_type", CAT_NORMAL );

	//
	int iComboCnt = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_vComboList.clear();
	m_vShotInfoList.clear();

	if( iComboCnt > 0 )
	{
		m_vComboList.reserve( iComboCnt );
		m_vShotInfoList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboList.push_back( kComboAttack );

			ShotInfo kInfo;

			wsprintf_e( szKey, "shot_info_up_ani%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_ShotUpAni = szBuf;

			wsprintf_e( szKey, "shot_info_down_ani%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_ShotDownAni = szBuf;

			wsprintf_e( szKey, "shot_info_effect%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_ShotEffect = szBuf;

			m_vShotInfoList.push_back( kInfo );
		}
	}

	int iJumpComboCnt = rkLoader.LoadInt_e( "jump_linear_combo_cnt", 0 );
	m_vJumpComboList.clear();
	m_vJumpShotInfoList.clear();

	if( iJumpComboCnt > 0 )
	{
		m_vJumpComboList.reserve( iJumpComboCnt );
		m_vJumpShotInfoList.reserve( iJumpComboCnt );

		for( int i=0; i < iJumpComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "jump_linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vJumpComboList.push_back( kComboAttack );

			ShotInfo kInfo;

			wsprintf_e( szKey, "jump_shot_info_up_ani%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_ShotUpAni = szBuf;

			wsprintf_e( szKey, "jump_shot_info_down_ani%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_ShotDownAni = szBuf;

			wsprintf_e( szKey, "jump_shot_info_effect%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_ShotEffect = szBuf;

			m_vJumpShotInfoList.push_back( kInfo );
		}
	}

	int iDownComboCnt = rkLoader.LoadInt_e( "down_linear_combo_cnt", 0 );
	m_vDownComboList.clear();
	m_vDownShotInfoList.clear();

	if( iDownComboCnt > 0 )
	{
		m_vDownComboList.reserve( iDownComboCnt );
		m_vDownShotInfoList.reserve( iDownComboCnt );

		for( int i=0; i < iDownComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "down_linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vDownComboList.push_back( kComboAttack );

			ShotInfo kInfo;

			wsprintf_e( szKey, "down_shot_info_up_ani%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_ShotUpAni = szBuf;

			wsprintf_e( szKey, "down_shot_info_down_ani%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_ShotDownAni = szBuf;

			wsprintf_e( szKey, "down_shot_info_effect%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_ShotEffect = szBuf;

			m_vDownShotInfoList.push_back( kInfo );
		}
	}

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_bAutoComboAction = rkLoader.LoadBool_e( "auto_combo_action", false );

	//
	m_bUseEndOtherAttack = rkLoader.LoadBool_e( "use_end_other_attack", false );

	LoadAttackAttribute_e( "end_other_attack", m_EndOtherAttack, rkLoader );

	LoadAttackAttribute_e( "jump_end_other_attack", m_JumpEndOtherAttack, rkLoader );

	LoadAttackAttribute_e( "down_end_other_attack", m_DownEndOtherAttack, rkLoader );

	//
	m_bNoGravityCombo = rkLoader.LoadBool_e( "no_gravity_combo", false );

	//
	rkLoader.LoadString_e( "dummy_char_effect", "", szBuf, MAX_PATH );
	m_DummyCharEffect = szBuf;

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );


	//
	m_ComboAttackType = (ComboAttackType)rkLoader.LoadInt_e( "combo_attack_type", CAT_NORMAL );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetDownHeight = rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	m_fAttMoveSpeed = rkLoader.LoadFloat_e( "attack_move_speed", 0.0f );
	m_fAttMoveRange = rkLoader.LoadFloat_e( "attack_move_range", 0.0f );
	m_fAttMoveRate = rkLoader.LoadFloat_e( "attack_move_rate", 0.0f );
}

ioSkill* ioMechanicsComboSkill::Clone()
{
	return new ioMechanicsComboSkill( *this );
}

bool ioMechanicsComboSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_iCurLinearCombo = 0;
	m_dwNextComboTime = 0;
	m_dwEnableReserveTime = 0;
	m_dwFireMotionEndTime = 0;

	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	InitExtraAniJump();

	m_bUsedEndOtherAttack = false;
	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	if( !m_SkillSoundName.IsEmpty() && pChar )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		m_SkillSoundID = g_SoundMgr.PlaySound( m_SkillSoundName, pGrp->GetParentSceneNode() );
	}

	m_dwCurSkillProtectTime = 0;
	
	m_bBallTarget = false;
	m_ComboTargetName.Clear();
	m_dwSkillMoveEndTime = 0;

	m_vTargetPos = ioMath::VEC3_ZERO;
	m_vTargetDir = ioMath::VEC3_ZERO;

	return true;
}

void ioMechanicsComboSkill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_MOVE;

	int iAniID = pGrp->GetAnimationIdx( GetSkillMotionName() );
	float fTimeRate = GetSkillMotionRate();

	pChar->AttachEffect( m_SkillEffectName );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = dwCurTime;

	//
	D3DXVECTOR3 vCharPos = pChar->GetMidPositionByRate();
	m_dwSkillMoveEndTime = dwCurTime;

	switch( m_ComboAttackType )
	{
	case CAT_NORMAL:
		{
			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			D3DXVECTOR3 vDiff = m_vTargetPos - vCharPos;
			float fTargetRange = D3DXVec3Length( &vDiff ) * m_fAttMoveRate;
			D3DXVec3Normalize( &m_vTargetDir, &vDiff );
			
			DWORD dwMoveTime = 0;
			if( m_fAttMoveSpeed > 0.0f )
				dwMoveTime = (DWORD)(fTargetRange / m_fAttMoveSpeed * FLOAT1000);

			m_dwSkillMoveEndTime += dwMoveTime;
		}
		break;
	case CAT_SHOT:
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

			if( m_vTargetPos != ioMath::VEC3_ZERO )
			{
				D3DXVECTOR3 vDiff = m_vTargetPos - vCharPos;
				D3DXVec3Normalize( &m_vTargetDir, &vDiff );
			}
			else
			{
				m_vTargetDir = ioMath::VEC3_ZERO;
			}

			m_dwSkillMoveEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
		}
		break;
	}

	//
	if( m_vTargetDir == ioMath::VEC3_ZERO )
	{
		m_vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &m_vTargetDir, &m_vTargetDir );
	}

	D3DXVECTOR3 vRotDir = m_vTargetDir;
	vRotDir.y = 0.0f;
	D3DXVec3Normalize( &vRotDir, &vRotDir );

	D3DXQUATERNION qtNewRot;
	ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
	pChar->SetTargetRotToRotate( qtNewRot, true );

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

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate );

	//
	//pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, 0 );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	SetSkillActionState( pChar );
	m_bSetAttackFire = true;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = dwCurTime + m_dwSkillProtectTime;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_TARGET;
		kPacket << m_vTargetPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMechanicsComboSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioMechanicsComboSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	switch( m_SkillState )
	{
	case SS_NONE:
		if( pChar && pChar->IsNeedProcess() && m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;

			CheckFindTarget( pChar );
			AttackSkillFire( pChar );
			EndPreDelayEffect( pChar );
		}
		break;
	case SS_MOVE:
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );

		if( m_dwSkillMoveEndTime < dwCurTime )
		{
			m_SkillState = SS_COMBO;
			m_dwEnableReserveTime = dwCurTime;
			
			SetNextLinearCombo( pChar, false, false );
		}
		else if( m_ComboAttackType == CAT_NORMAL )
		{
			ProcessMoveState( pChar );
		}
		break;
	case SS_COMBO:
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
		CheckLinearCombo( pChar );
		break;
	}
}

void ioMechanicsComboSkill::ProcessMoveState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fAttMoveSpeed > 0.0f )
		{
			float fFinalMoveAmt = m_fAttMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
			{
				vMoveAmt = m_vTargetDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			float fPreHeight = pChar->GetBottomHeight();
			pChar->Translate( vTotalMove );

			float fMoveHeight = pChar->GetBottomHeight();
			D3DXVECTOR3 vCurPos = pChar->GetWorldPosition();
			float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pChar, false, pChar->GetMidHeightByRate( 1.8f ) );

			bool bChange = false;
			if( vCurPos.y < fMoveHeight )
			{
				bChange = true;
				vCurPos.y = fMoveHeight;
			}

			if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
			{
				if( fTopHeight > FLOAT1 )
				{
					bChange = true;
					vCurPos.y = max( fTopHeight, fPreHeight );
				}
			}

			if( bChange )
				pChar->SetWorldPosition( vCurPos );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );
}

void ioMechanicsComboSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	if( GetOwnerItem() )
	{
		GetOwnerItem()->ClearCurChargeRateForSkill();
	}

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;
	m_bUsedEndOtherAttack = false;
	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	m_iCurLinearCombo = 0;
	m_dwEnableReserveTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	pChar->SetSkillMoveEnable( false, false );

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
		//g_SoundMgr.StopSound( m_SkillSoundName, 0 );
	}
}

bool ioMechanicsComboSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMechanicsComboSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon )
		pWeapon->SetMoveDir( m_vTargetDir );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			if( pChar )
			{
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( false, true );
			}
		}
	}
}

void ioMechanicsComboSkill::SetNextLinearCombo( ioBaseChar *pChar, bool bOther, bool bSendNet )
{
	switch( m_ComboAttackType )
	{
	case CAT_NORMAL:
		SetNormalCombo( pChar, bOther, bSendNet );
		break;
	case CAT_SHOT:
		SetShotCombo( pChar, bOther, bSendNet );
		break;
	}
}

void ioMechanicsComboSkill::SetNormalCombo( ioBaseChar *pChar, bool bOther, bool bSendNet )
{
	if( !pChar ) return;

	m_dwEnableReserveTime = 0;

	const AttributeList &rkComboList = GetComboList();

	int iMaxCombo = rkComboList.size();
	if( !COMPARE( m_iCurLinearCombo, 0, iMaxCombo ) )
		return;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	/*
	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}
	*/

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = rkComboList[m_iCurLinearCombo];

	if( bOther )
		kAttack = GetEndAttribute();

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

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

	if( m_bRefreshFireTimeList )
	{
		if( m_iCurLinearCombo == 0 )
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
											iAniID,
											kAttack.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

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

	//
	if( !m_bUsedComboList )
		g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	if( pStage && pChar->IsNeedProcess() && !m_bUsedComboList && !m_DummyCharList.empty() )
	{
		D3DXVECTOR3 vCreatePos = pChar->GetWorldPosition();

		if( !m_DummyCharEffect.IsEmpty() )
			pChar->CreateMapEffectBySkill( m_DummyCharEffect, vCreatePos );

		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSideDir;
		D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSideDir, &vSideDir );

		int iSize = m_DummyCharList.size();
		for( int i=0; i < iSize; ++i )
		{
			ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
			float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
			float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
			float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

			D3DXVECTOR3 vTargetPos = vCreatePos + (fOffset * vDir);
			vTargetPos += fSideOffset * vSideDir;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
														   pChar->GetCharName(),
														   vTargetPos,
														   fStartAngle,
														   0, true );

			if( pDummy )
			{
				pDummy->SetControlSkillName( GetName() );

				DummyCharCreateInfo kInfo;
				kInfo.m_DummyCharName = szDummyCharName;
				kInfo.m_iDummyCharIdx = iCurIndex;
				kInfo.m_fDummyCharStartAngle = fStartAngle;
				kInfo.m_vTargetPos = vTargetPos;

				vCreateList.push_back( kInfo );
			}
		}
	}

	m_bUsedComboList = true;

	int iDummyCnt = vCreateList.size();

	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_COMBO;
		kPacket << m_iCurLinearCombo;
		kPacket << (int)m_CurDirKey;
		kPacket << bOther;

		kPacket << iDummyCnt;
		for( int i=0; i < iDummyCnt; ++i )
		{
			kPacket << vCreateList[i].m_DummyCharName;
			kPacket << vCreateList[i].m_iDummyCharIdx;
			kPacket << vCreateList[i].m_fDummyCharStartAngle;
			kPacket << vCreateList[i].m_vTargetPos;
		}

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}

void ioMechanicsComboSkill::SetShotCombo( ioBaseChar *pChar, bool bOther, bool bSendNet )
{
	m_dwEnableReserveTime = 0;

	const AttributeList &rkComboList = GetComboList();
	const ShotInfoList &rkShotList = GetShotInfoList();

	int iMaxCombo = rkComboList.size();
	if( !COMPARE( m_iCurLinearCombo, 0, iMaxCombo ) )
		return;

	bool bShotInfoValidate = false;
	if( COMPARE( m_iCurLinearCombo, 0, (int)rkShotList.size() ) )
		bShotInfoValidate = true;

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = rkComboList[m_iCurLinearCombo];

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

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

	if( m_bRefreshFireTimeList )
	{
	if( m_iCurLinearCombo == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
											iAniID,
											kAttack.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

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

	if( bShotInfoValidate )
	{
		float fWeight = GetTargetUpDownWeight( 89.0f );
		if( m_vTargetDir.y > 0.0f )
		{
			int iUpAniID = pGrp->GetAnimationIdx( rkShotList[m_iCurLinearCombo].m_ShotUpAni );

			pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1-fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pGrp->SetActionAni( iUpAniID, 0.0f, 0.0f, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}
		else if( m_vTargetDir.y < 0.0f )
		{
			int iDownAniID = pGrp->GetAnimationIdx( rkShotList[m_iCurLinearCombo].m_ShotDownAni );

			pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1-fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pGrp->SetActionAni( iDownAniID, 0.0f, 0.0f, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}
		else
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}

		ioHashString szCurEffect = rkShotList[m_iCurLinearCombo].m_ShotEffect;
		pChar->CreateMapEffectWithDir( szCurEffect, m_vTargetDir );
	}
	else
	{
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	if( !m_bUsedComboList )
		g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	if( pStage && pChar->IsNeedProcess() && !m_bUsedComboList && !m_DummyCharList.empty() )
	{
		D3DXVECTOR3 vCreatePos = pChar->GetWorldPosition();

		if( !m_DummyCharEffect.IsEmpty() )
			pChar->CreateMapEffectBySkill( m_DummyCharEffect, vCreatePos );

		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSideDir;
		D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSideDir, &vSideDir );

		int iSize = m_DummyCharList.size();
		for( int i=0; i < iSize; ++i )
		{
			ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
			float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
			float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
			float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

			D3DXVECTOR3 vTargetPos = vCreatePos + (fOffset * vDir);
			vTargetPos += fSideOffset * vSideDir;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
														   pChar->GetCharName(),
														   vTargetPos,
														   fStartAngle,
														   0, true );

			if( pDummy )
			{
				pDummy->SetControlSkillName( GetName() );

				DummyCharCreateInfo kInfo;
				kInfo.m_DummyCharName = szDummyCharName;
				kInfo.m_iDummyCharIdx = iCurIndex;
				kInfo.m_fDummyCharStartAngle = fStartAngle;
				kInfo.m_vTargetPos = vTargetPos;

				vCreateList.push_back( kInfo );
			}
		}
	}

	m_bUsedComboList = true;

	int iDummyCnt = vCreateList.size();

	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_COMBO;
		kPacket << m_iCurLinearCombo;
		kPacket << (int)m_CurDirKey;
		kPacket << bOther;

		kPacket << iDummyCnt;
		for( int i=0; i < iDummyCnt; ++i )
		{
			kPacket << vCreateList[i].m_DummyCharName;
			kPacket << vCreateList[i].m_iDummyCharIdx;
			kPacket << vCreateList[i].m_fDummyCharStartAngle;
			kPacket << vCreateList[i].m_vTargetPos;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}

void ioMechanicsComboSkill::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_bUsedEndOtherAttack )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	const AttributeList &rkComboList = GetComboList();

	if( !COMPARE( m_iCurLinearCombo, 0, (int)rkComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bAutoComboAction )
	{
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < dwCurTime )
		{
			if( pChar->IsNeedProcess() )
			{
				bool bReserveSkill = false;
				int iSkillNum = pChar->GetSkillKeyInput();
				if( iSkillNum >= 0 )
				{
					bReserveSkill = true;
				}

				if( !bReserveSkill )
				{
					if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
					{
						pChar->SetSkillMoveEnable( false, true );

						m_bUsedEndOtherAttack = true;
						SetNextLinearCombo( pChar, true, true );
						return;
					}
				}
			}
		}
		
		if( m_dwMotionEndTime < dwCurTime )
		{
			pChar->SetSkillMoveEnable( false, true );

			if( pChar->IsNeedProcess() )
			{
				SetNextLinearCombo( pChar, false, true );
			}
		}

		return;
	}

	if( m_dwEnableReserveTime == 0 )
		return;

	if( m_dwEnableReserveTime < dwCurTime )
	{
		pChar->SetSkillMoveEnable( false, true );

		if( pChar->IsNeedProcess() )
		{
			bool bReserveSkill = false;
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				bReserveSkill = true;
			}

			if( !bReserveSkill )
			{
				if( pChar->IsAttackKey() )
				{
					SetNextLinearCombo( pChar, false, true );
					return;
				}
				else if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
				{				
					m_bUsedEndOtherAttack = true;
					SetNextLinearCombo( pChar, true, true );
					return;
				}
			}
		}
	}
}

void ioMechanicsComboSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SST_COMBO:
		{
			rkPacket >> m_iCurLinearCombo;

			int iCurKey;
			rkPacket >> iCurKey;

			if( m_ComboAttackType == CAT_NORMAL )
				pChar->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iCurKey );

			bool bOther;
			rkPacket >> bOther;

			SetNextLinearCombo( pChar, bOther, false );

			int iDummyCnt;
			rkPacket >> iDummyCnt;

			for( int i=0; i < iDummyCnt; ++i )
			{
				int iCurIndex;
				ioHashString szDummyCharName;
				float fStartAngle;
				D3DXVECTOR3 vTargetPos;

				rkPacket >> szDummyCharName >> iCurIndex;
				rkPacket >> fStartAngle;
				rkPacket >> vTargetPos;

				ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
															   pChar->GetCharName(),
															   vTargetPos,
															   fStartAngle,
															   0, false );

				if( pDummy )
					pDummy->SetControlSkillName( GetName() );
			}

			if( m_ComboAttackType == CAT_NORMAL && IsUseAutoTargetValue() )
			{
				pChar->ApplyAutoTargetInfo( rkPacket );
			}
		}
		break;
	case SST_TARGET:
		{
			rkPacket >> m_vTargetPos;

			m_dwPreDelayStartTime = 0;

			AttackSkillFire( pChar );
			EndPreDelayEffect( pChar );
		}
		break;
	}
}

bool ioMechanicsComboSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_MOVE:
		return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioMechanicsComboSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioMechanicsComboSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		switch( m_ProtectCheckType )
		{
		case PCT_TIME:
			if( m_dwCurSkillProtectTime > dwCurTime )
				return true;
			break;
		case PCT_ANI:
			if( m_iCurLinearCombo == 0 &&
				m_dwFireMotionEndTime > 0 &&
				m_dwFireMotionEndTime > dwCurTime )
			{
				return true;
			}
			break;
		}
	}

	return false;
}

const AttributeList& ioMechanicsComboSkill::GetComboList()
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_vComboList;
	case SUT_DOWN:
		if( !m_vDownComboList.empty() )
			return m_vDownComboList;
		return m_vComboList;
	case SUT_JUMP:
		if( !m_vJumpComboList.empty() )
			return m_vJumpComboList;
		return m_vComboList;
	}

	return m_vComboList;
}

const ShotInfoList& ioMechanicsComboSkill::GetShotInfoList()
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_vShotInfoList;
	case SUT_DOWN:
		if( !m_vDownShotInfoList.empty() )
			return m_vDownShotInfoList;
		return m_vShotInfoList;
	case SUT_JUMP:
		if( !m_vJumpShotInfoList.empty() )
			return m_vJumpShotInfoList;
		return m_vShotInfoList;
	}

	return m_vShotInfoList;
}

AttackAttribute ioMechanicsComboSkill::GetEndAttribute()
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_EndOtherAttack;
	case SUT_DOWN:
		if( !m_DownEndOtherAttack.m_AttackAnimation.IsEmpty() )
			return m_DownEndOtherAttack;
		return m_EndOtherAttack;
	case SUT_JUMP:
		if( !m_JumpEndOtherAttack.m_AttackAnimation.IsEmpty() )
			return m_JumpEndOtherAttack;
		return m_EndOtherAttack;
	}

	return m_EndOtherAttack;
}

bool ioMechanicsComboSkill::IsJumpState() const
{
	if( !m_bNoGravityCombo )
		return false;

	return true;
}

bool ioMechanicsComboSkill::IsNoDropState() const
{
	if( !m_bNoGravityCombo )
		return false;

	return true;
}

bool ioMechanicsComboSkill::IsCanSendControl() const
{
	if( m_bSetAttackFire )
		return false;

	return true;
}

void ioMechanicsComboSkill::CheckFindTarget( ioBaseChar *pChar )
{
	m_bBallTarget = false;
	m_ComboTargetName.Clear();
	m_vTargetPos = ioMath::VEC3_ZERO;

	if( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return;

	bool bNeedFind = true;

	ioMechanicsItem *pMechanics = ToMechanicsItem( GetOwnerItem() );
	ioDestroyerItem *pDestroyer = ToDestroyerItem( GetOwnerItem() );
	if( pMechanics )
	{
		m_bBallTarget = pMechanics->IsBallTarget();
		m_ComboTargetName = pMechanics->GetMechanicsTargetSkill();
	}
	else if( pDestroyer )
	{
		m_bBallTarget = pDestroyer->IsBallTarget();
		m_ComboTargetName = pDestroyer->GetDestroyerTargetSkill();
	}

	if( m_bBallTarget || !m_ComboTargetName.IsEmpty() )
	{
		if( !CheckTargetValidate( pChar ) )
			bNeedFind = true;
		else
			bNeedFind = false;
	}

	if( bNeedFind )
	{
		if( pChar->GetExperienceMode() == EMS_NONE )
		{
			D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
			D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

			float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
			float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
			float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

			const BaseCharList &rkCharList = pStage->GetBaseCharList();
			BaseCharList::const_iterator iter;

			ioHashString szTarget;
			float fCurGap = -FLOAT1;
			for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
			{
				ioBaseChar *pTarget = *iter;
				if( !pTarget ) continue;

				if( pTarget->GetTeam() == pChar->GetTeam() ) continue;
				if( pTarget->GetCharName() == pChar->GetCharName() ) continue;
				if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

				if( !IsEnableTargetState( pTarget ) )
					continue;

				D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
				vDiff = vTargetPos - vCurPos;
				float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

				// 각도 체크
				if( fCurAngle != 0.0f )
				{
					D3DXQUATERNION qtRot = pChar->GetTargetRot();
					D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					D3DXVec3Normalize( &vDir, &vDir );
					D3DXVECTOR3 vDiff2 = vDiff;
					vDiff2.y = 0.0f;
					D3DXVec3Normalize( &vDiff2, &vDiff2 );

					float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
					if( fCurValue < fHalfValue )
						continue;
				}

				// 높이 체크
				float fHeightGap = vTargetPos.y - vCurPos.y;
				if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
					continue;

				if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
					continue;

				// 거리 체크
				if( fDiffRangeSq < fTargetRangeSq )
				{
					if( fCurGap == -FLOAT1 )
					{
						fCurGap = fDiffRangeSq;
						szTarget = pTarget->GetCharName();
					}
					else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
					{
						fCurGap = fDiffRangeSq;
						szTarget = pTarget->GetCharName();
					}
				}
			}

			if( !szTarget.IsEmpty() )
			{
				m_ComboTargetName = szTarget;
			}
		}

		if( CompareTarget(pChar) )
		{
			m_bBallTarget = true;
			m_ComboTargetName.Clear();
		}
	}

	if( !m_bBallTarget && m_ComboTargetName.IsEmpty() )
	{
		D3DXVECTOR3 vCharPos = pChar->GetMidPositionByRate();
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_ComboAttackType == CAT_NORMAL )
			m_vTargetPos = vCharPos + (m_fAttMoveRange * vDir);
		else
			m_vTargetPos = ioMath::VEC3_ZERO;
	}
	else if( m_bBallTarget )
	{
		ioBall *pBall = pChar->FindBall();
		if( pBall )
		{
			m_vTargetPos = pBall->GetMidPositionByRate();
		}
	}
	else if( !m_ComboTargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_ComboTargetName );
		if( pTarget )
		{
			m_vTargetPos = pTarget->GetMidPositionByRate();
		}
	}
}

bool ioMechanicsComboSkill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() )
		return false;

	if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
		return false;

	return true;
}

bool ioMechanicsComboSkill::CompareTarget( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pChar->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
	vDiff = vTargetPos - vCurPos;
	float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

	// 각도 체크
	if( fCurAngle != 0.0f )
	{
		D3DXQUATERNION qtRot = pChar->GetTargetRot();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vDiff2 = vDiff;
		vDiff2.y = 0.0f;
		D3DXVec3Normalize( &vDiff2, &vDiff2 );

		float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
		if( fCurValue < fHalfValue )
			return false;
	}

	// 높이 체크
	float fHeightGap = vTargetPos.y - vCurPos.y;
	if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

bool ioMechanicsComboSkill::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	if( m_bBallTarget )
	{
		ioBall *pBall = pOwner->FindBall();
		if( !pBall ) return false;

		D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_ComboTargetName );
		if( !pTarget )
			return false;

		if( !IsEnableTargetState( pTarget ) )
			return false;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			return false;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}

float ioMechanicsComboSkill::GetTargetUpDownWeight( float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( m_vTargetDir.x, 0.0f, m_vTargetDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &m_vTargetDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}


