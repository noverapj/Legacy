#include "StdAfx.h"
#include "ioChargingFireSkill7.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"


//기본 생성자
ioChargingFireSkill7::ioChargingFireSkill7()
{
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;

	m_dwCurSkillProtectTime = 0;

	m_bSetAttackFire = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_InputDirKey = ioUserKeyInput::DKI_NONE;
}

//복사 생성자 ( Clone 에서 쓰임)
ioChargingFireSkill7::ioChargingFireSkill7( const ioChargingFireSkill7 &rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_vChargeShotList( rhs.m_vChargeShotList ),
	m_szChargeAreaWeapon( rhs.m_szChargeAreaWeapon ),
	m_fChargeAreaWeaponOffset( rhs.m_fChargeAreaWeaponOffset ),
	m_dwMaxChargeTime( rhs.m_dwMaxChargeTime ),
	m_dwChargeTic( rhs.m_dwChargeTic ),
	m_ChargeMotionName( rhs.m_ChargeMotionName ),
	m_fChargeAnimationTimeRate( rhs.m_fChargeAnimationTimeRate ),
	m_bSetFirstAttack( rhs.m_bSetFirstAttack ),
	m_bSetRotation( rhs.m_bSetRotation )
{	
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;

	m_dwCurSkillProtectTime = 0;

	m_bSetAttackFire = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_InputDirKey = ioUserKeyInput::DKI_NONE;
	m_dwStartChargeTime = 0;
}

//소멸자
ioChargingFireSkill7::~ioChargingFireSkill7()
{	
}

//ini 파일 읽기
void ioChargingFireSkill7::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	//추가 데이터 읽기
	LoadChargeProperty(rkLoader);
	
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

//충전 데이터 읽기
void ioChargingFireSkill7::LoadChargeProperty( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH];

	//충전 애니메이션 이름
	rkLoader.LoadString_e( "gathering_ani", "", szBuf, MAX_PATH );
	m_ChargeMotionName = szBuf;
	//애니메이션 빠르기
	m_fChargeAnimationTimeRate = rkLoader.LoadFloat_e("gathering_ani_rate",1.0f);

	//충전 공격 단계 얻기
	int iChargeCnt = rkLoader.LoadInt_e( "gathering_charge_attack_cnt", 0 );
	m_vChargeShotList.clear();
	//단계가 있다면 크기 설정 후 데이터 넣기
	if( iChargeCnt > 0 )
	{
		m_vChargeShotList.reserve( iChargeCnt );

		for( int i=0; i < iChargeCnt; ++i )
		{
			AttackAttribute kChargeShot;

			wsprintf_e( szKey, "gathering_charge_attack_combo%d", i+1 );
			LoadAttackAttribute( szKey, kChargeShot, rkLoader );

			m_vChargeShotList.push_back( kChargeShot );
		}
	}
	rkLoader.LoadString_e( "gathering_charge_attack_area_weapon", "", szBuf, MAX_PATH );
	m_szChargeAreaWeapon = szBuf;
	m_fChargeAreaWeaponOffset = rkLoader.LoadFloat_e( "gathering_charge_attack_area_weapon_offset", 1.0f );

	//최대 충전 시간, 충전 단계의 차이값, 충전상태확인 시간
	m_dwMaxChargeTime = rkLoader.LoadInt_e( "max_gather_time", 0 );
	m_dwChargeTic = rkLoader.LoadInt_e( "gather_tic", 0 );
	m_bSetFirstAttack = rkLoader.LoadBool_e( "set_first_attack", 0);
	m_bSetRotation = rkLoader.LoadBool_e( "set_charging_rotation", 0);
}

//스킬 복사 생성 후 반환
ioSkill* ioChargingFireSkill7::Clone()
{
	return new ioChargingFireSkill7( *this );
}

//스킬 시작 부분 설정
bool ioChargingFireSkill7::OnSkillStart( ioBaseChar *pChar )
{	
	m_dwEnableReserveTime = 0;
	m_dwFireMotionEndTime = 0;

	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_InputDirKey = ioUserKeyInput::DKI_NONE;

	InitExtraAniJump();

	m_bSetAttackFire = false;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	if( !m_SkillSoundName.IsEmpty() && pChar )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		m_SkillSoundID = g_SoundMgr.PlaySound( m_SkillSoundName, pGrp->GetParentSceneNode() );
	}

	m_dwCurSkillProtectTime = 0;
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
	
	//충전 시작 시간, 충전 단계, 충전 초기화 상태
	m_dwStartChargeTime = 0;
	m_nChargeGrade = 0;
	m_bIsChargeStateInit = false;
	m_bIsChargeShotInit = false;
	
	return true;
}

//스킬 발동 중 설정 (Loop 상태)
void ioChargingFireSkill7::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_CHARGING:
		{		
			if ( m_bSetFirstAttack && m_dwMotionEndTime > dwCurTime )
				return;

			//키를 누르는 중인지, 떼었는지 확인하여 단계 설정 및 상태 변경, 반복 애니메이션
			CheckChargeShotState( pChar );
		}
		break;
	case SS_CHARGESHOT:
		{
			//무기 생성 및 애니메이션 동작
			ActionChargeShot( pChar );
		}
		break;
	}
	
	//스킬 상태가 액션 중, 콤보 중, 충전 발사 중에 모션이 끝나게 된다면 종료 상태로 변경
	if ( m_dwMotionEndTime <= dwCurTime && m_bIsChargeShotInit )
	{
		//종료 상태로 변경
		m_SkillState = SS_END;
	}
}

//스킬 종료 부분 설정
void ioChargingFireSkill7::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();
	
	m_bSetAttackFire = false;
	
	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
	}
}

//메쉬 변경 설정
bool ioChargingFireSkill7::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

//무기 속성 적용
void ioChargingFireSkill7::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
	SetChargingRateForWeapon( pWeapon ); 
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if ( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

//스킬 정보 추가 적용 (패킷을 받아 캐릭터의 상태를 처리, 이동, 회전, 특수 기능 동기화)
void ioChargingFireSkill7::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{	
	if( !pChar )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{	
	case SS_CHANGEROT:
		{
			//회전 방향 변경
			int nInputDir;
			rkPacket >> nInputDir;

			m_InputDirKey = (ioUserKeyInput::DirKeyInput)nInputDir;
			pChar->SetTargetRotToDirKey( m_InputDirKey );
		}
		break;
	case SS_CHARGESHOT:
		{
			//스킬 상태 변경 및 단계 설정
			int nChargeGrade;
			rkPacket >> nChargeGrade;

			m_SkillState = (SkillState)iState;
			m_nChargeGrade = nChargeGrade;
		}
		break;
	}
}

//스킬 종료 모션이 가능한가
bool ioChargingFireSkill7::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END )
		return true;

	return false;
}

//스킬 사용시 무적 상태에 대한 처리
bool ioChargingFireSkill7::IsProtected( int iDefenseBreakType ) const
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
//예약 가능한가
bool ioChargingFireSkill7::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}

//스킬이 점프 상태인가
bool ioChargingFireSkill7::IsJumpState() const
{	
	return false;
}

//스킬이 떠있는 상태인가
bool ioChargingFireSkill7::IsNoDropState() const
{	
	return false;
}

//패킷 (위치를 갱신) 을 보낼 수 있는 상태인가
bool ioChargingFireSkill7::IsCanSendControl() const
{	
	if( m_bSetAttackFire )
		return false;

	return true;
}

//대쉬 확인이 가능한가
bool ioChargingFireSkill7::IsEnableDashCheck( ioBaseChar *pChar )
{		
	return false;
}

//스킬 종료 상태인가
bool ioChargingFireSkill7::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

//공격 완료 상태 (점프 스킬 사용 후 상태 변경) 
bool ioChargingFireSkill7::IsAttackEndState() const
{
	return IsEndState();
}

void ioChargingFireSkill7::AttackSkillFire( ioBaseChar *pChar )
{
	if ( m_bSetFirstAttack )
	{
		ioAttackSkill::AttackSkillFire( pChar );
	}
	else 
	{
		if( !m_bReduceGauge )
		{
			pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
			m_bReduceGauge = true;
		}
	}
	if( !m_szChargeAreaWeapon.IsEmpty() )
	{
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vPos = pChar->GetWorldPosition() + vDir * m_fChargeAreaWeaponOffset;
		g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(), "", GetName(), m_szChargeAreaWeapon, 
									vPos, pChar->GetTargetRot(), ioAreaWeapon::CT_NORMAL );
	}
	m_bSetAttackFire = true;	
	m_SkillState = SS_CHARGING;
}
//추가한 함수/////////////////////////////////////////////////////////

//충전 반복 애니메이션 설정
void ioChargingFireSkill7::SetChargeLoopAnimation( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_ChargeMotionName );
	if( iAniID == -1 )	return;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1, 0.0f );
}

//충전 상태일때 캐릭터 회전
void ioChargingFireSkill7::SetCharacterRot( ioBaseChar *pChar )
{
	if ( !m_bSetRotation )
		return;

	//방향키 입력이 있다면
	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		//현재 입력된 방향키 확인하고 변경
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	
		//입력된 방향키와 현재 캐릭터의 방향이 다르다면
		if ( m_InputDirKey != eNewDirKey )
		{
			m_InputDirKey = eNewDirKey;

			//입력된 방향으로 회전
			pChar->SetTargetRotToDirKey( m_InputDirKey );

			if ( pChar->IsNeedSendNetwork() )
			{
				//패킷전송
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SS_CHANGEROT;
				kPacket << (int)m_InputDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

//충전 발사 상태 확인
void ioChargingFireSkill7::CheckChargeShotState( ioBaseChar *pChar )
{
	if ( !pChar ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	//충전 초기화 (처음 한번만 실행)
	if ( !m_bIsChargeStateInit )
	{
		//충전 반복 애니메이션 설정
		SetChargeLoopAnimation( pChar );
		//충전 시작 시간 변경
		m_dwStartChargeTime = dwCurTime;

		m_bIsChargeStateInit = true;
	}

	//캐릭터 회전 확인
	SetCharacterRot( pChar );
	
	if( pChar->IsNeedProcess() )
	{
		//최대 충전 시간이 지났으면서 키가 눌려있다면 최대 단계 설정 후 발사 (최대 시간 경과 후 자동 발사 상태로 변경)
		if ( m_dwStartChargeTime + m_dwMaxChargeTime <= dwCurTime && pChar->IsAttackKeyDown() )
		{
			m_nChargeGrade = (int)m_vChargeShotList.size() - 1;
			m_SkillState = SS_CHARGESHOT;			
		}
		//키가 떼어졌다면 시간에 따른 단계 설정 후 상태 변경
		else if ( pChar->IsAttackKeyRelease() || !pChar->IsAttackKeyDown() )
		{
			m_nChargeGrade = (int)( ( dwCurTime - m_dwStartChargeTime ) / m_dwChargeTic );
			if( (int)m_vChargeShotList.size() <= m_nChargeGrade )
				m_nChargeGrade = (int)m_vChargeShotList.size() - 1;
			else if( m_nChargeGrade < 0 )
				m_nChargeGrade = 0;
			m_SkillState = SS_CHARGESHOT;
		}

		//충전 발사 상태면
		if ( m_SkillState == SS_CHARGESHOT && pChar->IsNeedSendNetwork() )
		{
			//패킷전송
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << m_SkillState;
			kPacket << m_nChargeGrade;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//충전 발사 
void ioChargingFireSkill7::ActionChargeShot( ioBaseChar *pChar )
{
	if ( !pChar || m_bIsChargeShotInit )
		return;

	m_bIsChargeShotInit = true;

	ioEntityGroup *pGrp = pChar->GetGroup();

	//단계의 애니메이션 값 읽기
	int iAniID = pGrp->GetAnimationIdx( m_vChargeShotList[m_nChargeGrade].m_AttackAnimation );
	if( iAniID == -1 )	return;

	//빠르기 설정, 딜레이 설정
	float fTimeRate  = m_vChargeShotList[m_nChargeGrade].m_fAttackAniRate;
	DWORD dwPreDelay = m_vChargeShotList[m_nChargeGrade].m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	//모션 시작 시간 설정
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_vChargeShotList[m_nChargeGrade].m_dwEndDelay;

	//종료 애니메이션이 없다면
	if( m_vChargeShotList[m_nChargeGrade].m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	//있다면
	else
	{
		//기본 애니메이션 설정
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		//끝나는 시간
		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = m_vChargeShotList[m_nChargeGrade].m_fAttackEndAniRate;

		int iEndAniID = pGrp->GetAnimationIdx( m_vChargeShotList[m_nChargeGrade].m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;

		//종료 애니메이션 설정
		pGrp->SetActionAni( iEndAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/fEndTimeRate, fWaitTime, 0.0f, true );		
	}

	pChar->SetReservedSliding( m_vChargeShotList[m_nChargeGrade].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	//무기 생성 인덱스 넣기
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	//개수만큼 해당 무기 생성 (방향은 현재 캐릭터가 보는 방향)
	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											m_vChargeShotList[m_nChargeGrade].m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}
}