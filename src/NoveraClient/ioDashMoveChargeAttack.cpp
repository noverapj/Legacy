
#include "stdafx.h"

#include "ioDashMoveChargeAttack.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioDashMoveChargeAttack::ioDashMoveChargeAttack()
{
	InitData();
}

ioDashMoveChargeAttack::ioDashMoveChargeAttack( const ioDashMoveChargeAttack &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_szAimUpAni( rhs.m_szAimUpAni ),
	  m_szAimDownAni( rhs.m_szAimDownAni ),
	  m_szAimCenterAni( rhs.m_szAimCenterAni ),
	  m_szAimedMarker( rhs.m_szAimedMarker ),
	  m_fAimRange( rhs.m_fAimRange ),
	  m_DashInfo( rhs.m_DashInfo ),
	  m_AttackInfo( rhs.m_AttackInfo ),
	  m_szFireUpAni( rhs.m_szFireUpAni ),
	  m_szFireDownAni( rhs.m_szFireDownAni ),
	  m_fFirstUseGauge( rhs.m_fFirstUseGauge ),
	  m_nMaxAttackCnt( rhs.m_nMaxAttackCnt ),
	  m_SubWeaponDataList( rhs.m_SubWeaponDataList ),
	  m_dwCheckSubWeaponTime( rhs.m_dwCheckSubWeaponTime ),
	  m_szSubWeaponEffect( rhs.m_szSubWeaponEffect ),
	  m_bEnableDownAim( rhs.m_bEnableDownAim )
{
	InitData();
}

ioDashMoveChargeAttack::~ioDashMoveChargeAttack()
{
	m_SubWeaponDataList.clear();
	m_SubWeaponEffectList.clear();
}

void ioDashMoveChargeAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
		
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	//스킬 무적 시간
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString_e( "aim_up_ani", "", szBuf, MAX_PATH );
	m_szAimUpAni = szBuf;
	rkLoader.LoadString_e( "aim_down_ani", "", szBuf, MAX_PATH );
	m_szAimDownAni = szBuf;
	rkLoader.LoadString_e( "aim_center_ani", "", szBuf, MAX_PATH );
	m_szAimCenterAni = szBuf;

	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedMarker = szBuf;

	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", 0.0f );

	LoadAttackAttribute( "dash_info", m_DashInfo, rkLoader );

	LoadAttackAttribute( "attack_info", m_AttackInfo, rkLoader );

	rkLoader.LoadString_e( "attack_fire_up", "", szBuf, MAX_PATH );
	m_szFireUpAni = szBuf;

	rkLoader.LoadString_e( "attack_fire_down", "", szBuf, MAX_PATH );
	m_szFireDownAni = szBuf;

	m_fFirstUseGauge = rkLoader.LoadFloat_e( "first_use_gauge", FLOAT100 );

	m_nMaxAttackCnt = rkLoader.LoadInt_e( "max_attack_cnt", 0 );

	m_SubWeaponDataList.clear();
	int nMaxSubCnt = rkLoader.LoadInt_e( "max_sub_weapon_cnt", 0 );
	for ( int i=0; i<nMaxSubCnt; i++ )
	{
		D3DXVECTOR3 vPos;
		wsprintf_e( szKey, "sub_weapon%d_offset_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_y", i+1 );
		vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szKey, 0.0f );

		WeaponInfo sWeaponInfo;
		wsprintf_e( szKey, "sub_weapon%d_attack_type", i+1 );
		sWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
		wsprintf_e( szKey, "sub_weapon%d_attack_type_resist", i+1 );
		sWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
		wsprintf_e( szKey, "sub_weapon%d_attack_type_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sWeaponInfo.m_WoundedAnimation = szBuf;
		wsprintf_e( szKey, "sub_weapon%d_attack_type_wounded_time", i+1 );
		sWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "sub_weapon%d_attack_type_wounded_loop_ani", i+1 );
		sWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		SubWeaponInfo sSubWeaponInfo;
		sSubWeaponInfo.m_vPos = vPos;
		sSubWeaponInfo.m_WeaponInfo = sWeaponInfo;

		m_SubWeaponDataList.push_back( sSubWeaponInfo );
	}

	m_dwCheckSubWeaponTime = (DWORD)rkLoader.LoadInt_e( "check_sub_weapon_time", 0 );

	rkLoader.LoadString_e( "sub_weapon_effect", "", szBuf, MAX_PATH );
	m_szSubWeaponEffect = szBuf;

	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );
}

ioSkill* ioDashMoveChargeAttack::Clone()
{
	return new ioDashMoveChargeAttack( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDashMoveChargeAttack::OnSkillStart( ioBaseChar *pChar )
{	
	InitData();

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
		
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bUseActiveCnt = true;

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	
	
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, m_fFirstUseGauge );
		m_bReduceGauge = true;
	}

	return true;
}

void ioDashMoveChargeAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				
				SetAimState( pChar );
			}
		}
		break;
	case SS_AIM:
		{
			GetTargetList( pChar, pStage );
			SetTargetRotation( pChar );
			UpdateMaker( pChar, pStage );

			if ( CheckInputChangeTarget( pChar ) )
				return;

			if ( CheckInputDash( pChar ) )
			{
				SetDashState( pChar, pStage );
				return;
			}

			if ( m_bSetReserveAttack || CheckInputAttack( pChar ) )
			{
				SetAttackState( pChar, pStage );
				return;
			}
		}
		break;
	case SS_DASH:
		{
			CheckReserveAttack( pChar );

			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
				SetAimState( pChar );
		}
		break;
	case SS_ATTACK:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				if ( m_nAttackCnt < m_nMaxAttackCnt )
					SetAimState( pChar );
				else
					SetEndState( pChar );
				return;
			}

			if ( m_dwSubWeaponFireTime > 0 && m_dwSubWeaponFireTime < dwCurTime )
			{
				CreateWeapon( pChar, pStage );
				CreateSubWeapon( pChar, pStage );
				DestroySubWeaponEffect( pChar, pStage );
				m_dwSubWeaponFireTime = 0;
			}
		}
	}

	UpdateSubWeaponEffect( pChar, pStage );
}

void ioDashMoveChargeAttack::OnSkillEnd( ioBaseChar *pChar )
{
	m_bUseActiveCnt = false;
	pChar->ClearReservedSliding();

	ioPlayStage *pStage = pChar->GetCreator();
	if ( pStage )
	{
		DestroyEffect( pChar, pStage, m_dwMakerID );
		DestroySubWeaponEffect( pChar, pStage );
	}

	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDashMoveChargeAttack::IsProtected( int iDefenseBreakType ) const
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
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioDashMoveChargeAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if ( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioDashMoveChargeAttack::IsEnableDashCheck( ioBaseChar *pChar )
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDashMoveChargeAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );	
}

void ioDashMoveChargeAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case SS_TARGET:
		{
			rkPacket >> m_szAimedTarget;
		}
		break;
	case SS_DASH:
		{
			int nCurDirKey;
			rkPacket >> nCurDirKey;

			ioUserKeyInput::DirKeyInput NewDirKey = (ioUserKeyInput::DirKeyInput)nCurDirKey;
			pChar->SetTargetRotToDirKey( NewDirKey );

			SetDashState( pChar, pStage );
		}
		break;
	case SS_ATTACK:
		{
			rkPacket >> m_nAttackCnt;
			SetAttackState( pChar, pStage );
		}
		break;
	case SS_RESERVE_ATTACK:
		{
			rkPacket >> m_bSetReserveAttack;
			rkPacket >> m_nAttackCnt;
		}
		break;
	case SS_CREATE_SUB_WEAPON_EFFECT:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			DWORD dwEffectID = CreateEffect( pChar, pStage, m_szSubWeaponEffect, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
			m_SubWeaponEffectList.push_back( dwEffectID );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDashMoveChargeAttack::InitData()
{
	m_SkillState = SS_NONE;	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_szAimedTarget.Clear();
	m_dwMakerID = -1;
	m_bChangeTarget = false;
	m_vAimDir = ioMath::VEC3_ZERO;
	m_bSetSubWeapon = false;
	m_nAttackCnt = 0;
	m_bSetReserveAttack = false;
	m_bUseActiveCnt = false;
	m_SubWeaponEffectList.clear();
	m_dwStartSubWeaponTime = 0;
	m_nCurSubWeaponCnt = 0;
	m_dwSubWeaponFireTime = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioDashMoveChargeAttack::CreateEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pChar || !pStage || szEffect.IsEmpty() )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioDashMoveChargeAttack::DestroyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pChar || !pStage || dwEffectID == -1 )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDashMoveChargeAttack::GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	m_vTargetInfoList.clear();

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget ) 
			continue;

		if( pTarget == pChar )
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )	
			continue;

		if( pTarget->HasHideBuff() )
			continue;

		if ( !m_bEnableDownAim )
		{
			if ( pTarget->IsApplyDownState() )
				continue;
		}

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 
			continue;


		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		tagBaseCharDistanceSq kInfo;
		kInfo.m_Name        = pTarget->GetCharName();
		kInfo.m_fDistanceSq = fDistSq;

		float fEnableDistSq = m_fAimRange * m_fAimRange;
		if( fDistSq < fEnableDistSq )	// Enemy
		{
			m_vTargetInfoList.push_back(kInfo);
		}
	}
	std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), BaseCharDistanceSqSort2() );	

	bool bSendPacket = false;
	if ( m_szAimedTarget.IsEmpty() )
	{
		if( !m_vTargetInfoList.empty() )
			m_szAimedTarget = m_vTargetInfoList[0].m_Name;
		
		if ( !m_szAimedTarget.IsEmpty() )
			bSendPacket = true;
	}
	else if ( !m_vTargetInfoList.empty() )
	{
		int nCurTarget = -1;

		int nCnt = m_vTargetInfoList.size();
		for( int i=0; i<nCnt; ++i )
		{
			if( m_szAimedTarget == m_vTargetInfoList[i].m_Name )
				nCurTarget = i;
		}

		if ( nCurTarget != -1 )
		{
			if( m_bChangeTarget )
			{
				nCurTarget++;
				m_bChangeTarget = false;

				if( nCurTarget >= nCnt )
					nCurTarget = 0;
			}

			if( m_szAimedTarget != m_vTargetInfoList[nCurTarget].m_Name )
			{
				m_szAimedTarget = m_vTargetInfoList[nCurTarget].m_Name;
				bSendPacket = true;
			}
		}
		else if ( !m_szAimedTarget.IsEmpty() )
		{
			m_szAimedTarget.Clear();
			bSendPacket = true;
			m_bChangeTarget = false;
		}

	}
	else
	{
		if ( !m_szAimedTarget.IsEmpty() )
		{
			m_szAimedTarget.Clear();
			bSendPacket = true;
			m_bChangeTarget = false;
		}
	}

	if ( pChar->IsNeedSendNetwork() && bSendPacket )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_TARGET;
		kPacket << m_szAimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDashMoveChargeAttack::SetTargetRotation( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	ioAnimation *pUp = pGrp->GetLoopInPlay( m_szAimUpAni );
	ioAnimation *pCenter = pGrp->GetLoopInPlay( m_szAimCenterAni );
	ioAnimation *pDown = pGrp->GetLoopInPlay( m_szAimDownAni );

	if( !pUp || !pCenter || !pDown )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	D3DXVECTOR3 vAimedDir;
	if( pTarget )
	{
		vAimedDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		pChar->SetTargetRotToDir( vAimedDir, true );		
	}
	else
		vAimedDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	m_vAimDir = vAimedDir;

	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )
	{
		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else
	{		
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}
}

float ioDashMoveChargeAttack::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}

void ioDashMoveChargeAttack::UpdateMaker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + m_vAimDir * m_fAimRange;

	if ( !m_szAimedTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szAimedTarget );
		if ( pTarget )
			vPos = pTarget->GetMidPositionByRate();
	}

	if ( m_dwMakerID == -1 )
		m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );

	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwMakerID );
	if ( pEffect )		
		pEffect->SetWorldPosition( vPos );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDashMoveChargeAttack::SetAimState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllLoopAni( 0.f, true );
		pGrp->ClearAllActionAni( 0.f, true );

		pGrp->SetLoopAni( m_szAimUpAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_szAimDownAni, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_szAimCenterAni, 0.0f, FLOAT1 );
	}

	m_SkillState = SS_AIM;
}

void ioDashMoveChargeAttack::SetDashState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	DestroyEffect( pChar, pStage, m_dwMakerID );
		
	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_DashInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	//빠르기 설정, 딜레이 설정
	float fTimeRate  = m_DashInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	//모션 시작 시간 설정
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;
	
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	//캐릭터 충돌 해제
	pChar->CheckCharColSkipSkillTime( nAniID, fTimeRate, dwPreDelay );

	//슬라이딩 설정
	pChar->SetReservedSliding( m_DashInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	m_SkillState = SS_DASH;
}

void ioDashMoveChargeAttack::SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;
	
	DestroyEffect( pChar, pStage, m_dwMakerID );
	m_bSetReserveAttack = false;
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_AttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	//빠르기 설정, 딜레이 설정
	float fTimeRate  = m_AttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pChar->ClearReservedSliding();
	//모션 시작 시간 설정
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwSubWeaponFireTime = dwCurTime;

	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwSubWeaponFireTime = pGrp->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;

	SetAttackAni( pGrp, fTimeRate, dwPreDelay );

	//캐릭터 충돌 해제
	pChar->CheckCharColSkipSkillTime( nAniID, fTimeRate, dwPreDelay );

	//슬라이딩 설정
	pChar->SetReservedSliding( m_AttackInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );
	
	m_SkillState = SS_ATTACK;
}

void ioDashMoveChargeAttack::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_END;
}

void ioDashMoveChargeAttack::SetAttackAni( ioEntityGroup *pGrp, float fTimeRate, DWORD dwPreDelay )
{
	if ( !pGrp )
		return;

	float fAniWeight = GetAimedUpDownWeight( m_vAimDir, 85.0f );
	if ( m_vAimDir.y >= 0 )
	{
		bool bAniFx1 = false;
		bool bAniFx2 = true;

		if( fAniWeight > FLOAT05 )
		{
			bAniFx1 = true;
			bAniFx2 = false;
		}
		pGrp->SetActionAni( m_szFireUpAni, FLOAT100, FLOAT100, fAniWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true, bAniFx1 );
		pGrp->SetActionAni( m_AttackInfo.m_AttackAnimation, FLOAT100, FLOAT100, FLOAT1 - fAniWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true, bAniFx2 );
	}
	else
	{
		bool bAniFx1 = true;
		bool bAniFx2 = false;

		if( fAniWeight > FLOAT05 )
		{
			bAniFx1 = false;
			bAniFx2 = true;
		}
		pGrp->SetActionAni( m_AttackInfo.m_AttackAnimation, FLOAT100, FLOAT100, FLOAT1 - fAniWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true, bAniFx1 );
		pGrp->SetActionAni( m_szFireDownAni, FLOAT100, FLOAT100, fAniWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true, bAniFx2 );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDashMoveChargeAttack::CheckInputDash( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( pChar->IsDirKeyDoubleClick() )
	{
		pChar->ClearDirDoubleClick();

		ioUserKeyInput::DirKeyInput CurKey = pChar->CheckCurDirKey();
		pChar->SetTargetRotToDirKey( CurKey );

		if ( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_DASH;
			kPacket << (int)CurKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}

	return false;
}

bool ioDashMoveChargeAttack::CheckInputChangeTarget( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	ioUserKeyInput::ActionKeyInput CurActonKey = pChar->GetCurActionKey();
	if ( pChar->IsDefenseKey() || CurActonKey == ioUserKeyInput::AKI_XC )
	{
		m_bChangeTarget = true;
		return true;
	}

	return false;
}

bool ioDashMoveChargeAttack::CheckInputAttack( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() || m_nAttackCnt >= m_nMaxAttackCnt )
		return false;

	if ( pChar->IsAttackKeyDown() )
	{
		if ( !m_bSetSubWeapon )
		{
			m_bSetSubWeapon = true;
			m_dwStartSubWeaponTime = FRAMEGETTIME();
		}
		return false;
	}
	else if ( pChar->IsAttackKeyRelease() )
	{
		m_bSetSubWeapon = false;
		m_nAttackCnt++;
		if ( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_ATTACK;
			kPacket << m_nAttackCnt;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}
	return false;
}

bool ioDashMoveChargeAttack::CheckReserveAttack( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( pChar->IsAttackKeyDown() )
	{
		if ( !m_bSetSubWeapon )
		{
			m_bSetSubWeapon = true;
			m_dwStartSubWeaponTime = FRAMEGETTIME();
		}
		return false;
	}
	else if ( pChar->IsAttackKeyRelease() )
	{
		m_bSetSubWeapon = false;
		m_bSetReserveAttack = true;
		m_nAttackCnt++;
		if ( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_RESERVE_ATTACK;
			kPacket << m_bSetReserveAttack;
			kPacket << m_nAttackCnt;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDashMoveChargeAttack::CreateSubWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage || m_SubWeaponDataList.empty() )
		return;

	if ( !m_bSetSubWeapon || m_nCurSubWeaponCnt >= (int)m_SubWeaponDataList.size() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwStartSubWeaponTime + m_dwCheckSubWeaponTime < dwCurTime )
	{
		m_dwStartSubWeaponTime = dwCurTime;

		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + pChar->GetWorldOrientation() * m_SubWeaponDataList[m_nCurSubWeaponCnt].m_vPos;
		DWORD dwEffectID = CreateEffect( pChar, pStage, m_szSubWeaponEffect, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
		m_SubWeaponEffectList.push_back( dwEffectID );
		m_nCurSubWeaponCnt++;

		if ( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_CREATE_SUB_WEAPON_EFFECT;
			kPacket << vPos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDashMoveChargeAttack::DestroySubWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage || m_SubWeaponEffectList.empty() )
		return;

	for ( int i=0; i<(int)m_SubWeaponEffectList.size(); i++ )
	{
		DWORD dwEffectID = m_SubWeaponEffectList[i];
		DestroyEffect( pChar, pStage, dwEffectID );
	}
	m_SubWeaponEffectList.clear();
	m_nCurSubWeaponCnt = 0;
}

void ioDashMoveChargeAttack::UpdateSubWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage || m_SubWeaponDataList.empty() )
		return;

	CreateSubWeaponEffect( pChar, pStage );

	int nEffectCnt = (int)m_SubWeaponEffectList.size();
	if ( nEffectCnt > 0 && nEffectCnt <= (int)m_SubWeaponDataList.size() )
	{
		for ( int i=0; i<nEffectCnt; i++ )
		{
			DWORD dwEffectID = m_SubWeaponEffectList[i];
			ioMapEffect *pSubWeaponEffect = pStage->FindMapEffect( dwEffectID );
			if ( pSubWeaponEffect )
			{
				D3DXQUATERNION qtRot = pChar->GetWorldOrientation();
				D3DXVECTOR3 vNewPos = pChar->GetMidPositionByRate() + pChar->GetWorldOrientation() * m_SubWeaponDataList[i].m_vPos;
				pSubWeaponEffect->SetWorldPosition( vNewPos );
				pSubWeaponEffect->SetWorldOrientation( qtRot );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDashMoveChargeAttack::CreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	if ( m_AttackInfo.m_vWeaponInfoList.empty() )
		return;

	WeaponInfo sWeaponInfo = m_AttackInfo.m_vWeaponInfoList[0];

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex;
	kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);

	ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon && !m_szAimedTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szAimedTarget );
		if ( pTarget )
		{		
			D3DXVECTOR3 vFireDir =  pTarget->GetMidPositionByRate() - pWeapon->GetPosition();
			D3DXVec3Normalize( &vFireDir, &vFireDir );
			pWeapon->SetMoveDir( vFireDir );
		}
	}
}

void ioDashMoveChargeAttack::CreateSubWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	int nEffectCnt = (int)m_SubWeaponEffectList.size();
	if ( nEffectCnt > 0 && nEffectCnt <= (int)m_SubWeaponDataList.size() )
	{
		for ( int i=0; i<nEffectCnt; i++ )
		{
			WeaponInfo sWeaponInfo = m_SubWeaponDataList[i].m_WeaponInfo;

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex++;
			kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + pChar->GetWorldOrientation() * m_SubWeaponDataList[i].m_vPos;
			D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
			D3DXVec3Normalize( &vDir, &vDir );
			ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( !m_szAimedTarget.IsEmpty() )
			{
				ioTraceWeapon *pTraceWeapon = ToTraceWeapon( pWeapon );
				if ( pTraceWeapon )
					pTraceWeapon->SetTraceTarget( m_szAimedTarget );
			}
		}
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDashMoveChargeAttack::IsUseActiveCnt() const
{
	return m_bUseActiveCnt;
}

int ioDashMoveChargeAttack::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	return m_nMaxAttackCnt;
}

int ioDashMoveChargeAttack::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	return m_nAttackCnt;
}