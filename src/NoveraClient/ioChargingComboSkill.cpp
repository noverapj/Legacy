#include "StdAfx.h"
#include "ioChargingComboSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"


//기본 생성자
ioChargingComboSkill::ioChargingComboSkill()
{
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;

	m_dwCurSkillProtectTime = 0;

	m_bSetAttackFire = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_InputDirKey = ioUserKeyInput::DKI_NONE;

	// 캐릭터 이동
	m_bSetChargingMoveAni = false;

	// 액션 콤보 
	m_bUsedEndOtherAttack	= false;
	m_iCurChargingCombo		= 0;
	m_nCurEndCombo			= 0;
	m_bUsedComboList		= false;
}

//복사 생성자 ( Clone 에서 쓰임)
ioChargingComboSkill::ioChargingComboSkill( const ioChargingComboSkill &rhs )
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
	m_bSetRotation( rhs.m_bSetRotation ),
	// 캐릭터 이동 
	m_ChargingMoveAnimation( rhs.m_ChargingMoveAnimation ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	// 액션 콤보 
	m_vChargingComboList( rhs.m_vChargingComboList ),
    m_bAutoComboAction( rhs.m_bAutoComboAction ),
	m_bUseEndOtherAttack( rhs.m_bUseEndOtherAttack ),
    m_bUseEndCombo( rhs.m_bUseEndCombo ),
	m_nMaxEndCombo( rhs.m_nMaxEndCombo ),
	m_bUseChangeDirection( rhs.m_bUseChangeDirection ),
	m_EndOtherAttack( rhs.m_EndOtherAttack ),
	m_bNoGravityCombo( rhs.m_bNoGravityCombo ),
	m_DummyCharList( rhs.m_DummyCharList ),
	m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	m_DummyCharEffect( rhs.m_DummyCharEffect )
{	
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;

	m_dwCurSkillProtectTime = 0;

	m_bSetAttackFire = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_InputDirKey = ioUserKeyInput::DKI_NONE;
	m_dwStartChargeTime = 0;

	// 캐릭터 이동
	m_bSetChargingMoveAni = false;

	// 액션 콤보
	m_bUsedEndOtherAttack	= false;
	m_iCurChargingCombo		= 0;
	m_nCurEndCombo			= 0;
	m_bUsedComboList		= false;
}

//소멸자
ioChargingComboSkill::~ioChargingComboSkill()
{	
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();

	int iChargingComboListSize = m_vChargingComboList.size();
	for(int i = 0; i < iChargingComboListSize; i++)
	{
		m_vChargingComboList[i].m_vComboList.clear();
	}
	m_vChargingComboList.clear();
}

//ini 파일 읽기
void ioChargingComboSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	//추가 데이터 읽기
	LoadChargeProperty(rkLoader);
	
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

//충전 데이터 읽기
void ioChargingComboSkill::LoadChargeProperty( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH];

	//충전 애니메이션 이름
	rkLoader.LoadString_e( "gathering_ani", "", szBuf, MAX_PATH );
	m_ChargeMotionName = szBuf;
	//애니메이션 빠르기
	m_fChargeAnimationTimeRate = rkLoader.LoadFloat_e("gathering_ani_rate",1.0f);

	//충전 공격 단계 얻기
	int iChargeCnt = (int)rkLoader.LoadInt_e( "gathering_charge_attack_cnt", 0 );
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
	m_dwMaxChargeTime = (DWORD)rkLoader.LoadInt_e( "max_gather_time", 0 );
	m_dwChargeTic = (DWORD)rkLoader.LoadInt_e( "gather_tic", 0 );
	m_bSetFirstAttack = rkLoader.LoadBool_e( "set_first_attack", 0);
	m_bSetRotation = rkLoader.LoadBool_e( "set_charging_rotation", 0);


	// 캐릭터 이동
	rkLoader.LoadString_e( "attack_move_ani", "", szBuf, MAX_PATH );
	m_ChargingMoveAnimation = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	// 액션 콤보
	int iChargingComboListSize = m_vChargingComboList.size();
	for(int i = 0; i < iChargingComboListSize; i++)
	{
		m_vChargingComboList[i].m_vComboList.clear();
	}
	m_vChargingComboList.clear();	

	for( int i = 0; i < iChargeCnt; i++ )
	{
		ChargingComboList kChargingComboList;
		m_vChargingComboList.push_back( kChargingComboList );

		wsprintf_e( szKey, "charging_combo%d_linear_combo_cnt", i+1 );
		int iComboCount = rkLoader.LoadInt( szKey, 0 );

		for( int c = 0; c < iComboCount; c++ )
		{
			AttackAttribute kComboAttack;
			wsprintf_e( szKey, "charging_combo%d_linear_combo%d", i+1, c+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );
			m_vChargingComboList[i].m_vComboList.push_back( kComboAttack );

		}
	}
	LoadAttackAttribute_e( "end_other_attack", m_EndOtherAttack, rkLoader );

	m_bAutoComboAction		= rkLoader.LoadBool_e( "auto_combo_action", false );
	m_bUseEndOtherAttack	= rkLoader.LoadBool_e( "use_end_other_attack", false );
	m_bUseChangeDirection	= rkLoader.LoadBool_e( "use_change_direction", true );
	m_bUseEndCombo			= rkLoader.LoadBool_e( "use_end_combo", false );
	m_nMaxEndCombo			= (int)rkLoader.LoadInt_e( "max_end_combo", 0 );
	m_bNoGravityCombo		= rkLoader.LoadBool_e( "no_gravity_combo", false );

	rkLoader.LoadString_e( "dummy_char_effect", "", szBuf, MAX_PATH );
	m_DummyCharEffect		= szBuf;

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );
}

//스킬 복사 생성 후 반환
ioSkill* ioChargingComboSkill::Clone()
{
	return new ioChargingComboSkill( *this );
}

//스킬 시작 부분 설정
bool ioChargingComboSkill::OnSkillStart( ioBaseChar *pChar )
{	
	m_dwEnableReserveTime = 0;
	m_dwFireMotionEndTime = 0;

	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_InputDirKey = ioUserKeyInput::DKI_NONE;

	InitExtraAniJump();

	m_bSetAttackFire = false;

	// 캐릭터 이동
	m_bSetChargingMoveAni = false;
	pChar->SetAttackMoveEnable( false );

	// 액션 콤보
	m_bUsedEndOtherAttack	= false;
	m_iCurChargingCombo		= 0;
	m_nCurEndCombo			= 0;
	m_bUsedComboList		= false;	

	m_bSetHandMesh = false;

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
void ioChargingComboSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

			// 액션 도중에 콤보 상태
			CheckChargingCombo( pChar );
		}
		break;

	case SS_COMBO:
		{
			CheckChargingCombo( pChar );
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
void ioChargingComboSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();
	
	m_bSetAttackFire = false;

	pChar->SetSkillMoveEnable( false, false );

	// 액션 콤보
	m_bUsedEndOtherAttack	= false;
	m_iCurChargingCombo		= 0;
	m_bUsedComboList		= false;
	m_dwEnableReserveTime	= 0;
	
	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
	}
}

//메쉬 변경 설정
bool ioChargingComboSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

//무기 속성 적용
void ioChargingComboSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
void ioChargingComboSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{	
	if( !pChar )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{	
	case SS_CHANGEMOVE:
		{
			int inputDirkey;
			rkPacket >> inputDirkey;
			m_InputDirKey = (ioUserKeyInput::DirKeyInput)inputDirkey;
			////회전 방향 변경
			//int nInputDir;
			//rkPacket >> nInputDir;

			//m_InputDirKey = (ioUserKeyInput::DirKeyInput)nInputDir;
			//pChar->SetTargetRotToDirKey( m_InputDirKey );
			// 캐릭터 이동
			SyncChargingMoveState( pChar );
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

	case SS_COMBO:
		{
			rkPacket >> m_iCurChargingCombo;

			int iCurKey;
			rkPacket >> iCurKey;
			pChar->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iCurKey );

			bool bOther;
			rkPacket >> bOther;

			SetNextChargingCombo( pChar, bOther );

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

			if( IsUseAutoTargetValue() )
			{
				pChar->ApplyAutoTargetInfo( rkPacket );
			}
		}
		break;
	}
}

//스킬 종료 모션이 가능한가
bool ioChargingComboSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END )
		return true;

	return false;
}

//스킬 사용시 무적 상태에 대한 처리
bool ioChargingComboSkill::IsProtected( int iDefenseBreakType ) const
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
bool ioChargingComboSkill::IsEnableReserve() const
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
bool ioChargingComboSkill::IsJumpState() const
{	
	return false;
}

//스킬이 떠있는 상태인가
bool ioChargingComboSkill::IsNoDropState() const
{	
	return false;
}

//패킷 (위치를 갱신) 을 보낼 수 있는 상태인가
bool ioChargingComboSkill::IsCanSendControl() const
{	
	if( m_bSetAttackFire )
		return false;

	return true;
}

//대쉬 확인이 가능한가
bool ioChargingComboSkill::IsEnableDashCheck( ioBaseChar *pChar )
{		
	return false;
}

//스킬 종료 상태인가
bool ioChargingComboSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

//공격 완료 상태 (점프 스킬 사용 후 상태 변경) 
bool ioChargingComboSkill::IsAttackEndState() const
{
	return IsEndState();
}

void ioChargingComboSkill::AttackSkillFire( ioBaseChar *pChar )
{
//	if ( m_bSetFirstAttack )
//	{
		ioAttackSkill::AttackSkillFire( pChar );
//	}
//	else 
//	{
		if( !m_bReduceGauge )
		{
			pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
			m_bReduceGauge = true;
		}
//	}

	if( !m_szChargeAreaWeapon.IsEmpty() )
	{
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vPos = pChar->GetWorldPosition() + vDir * m_fChargeAreaWeaponOffset;
		g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(), "", GetName(), m_szChargeAreaWeapon, 
									vPos, pChar->GetTargetRot(), ioAreaWeapon::CT_NORMAL );
	}
	m_bSetAttackFire = true;	
	m_SkillState = SS_CHARGING;

	// 캐릭터 이동
	pChar->SetSkillMoveEnable( false, false );
}
//추가한 함수/////////////////////////////////////////////////////////

//충전 반복 애니메이션 설정
void ioChargingComboSkill::SetChargeLoopAnimation( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_ChargeMotionName );
	if( iAniID == -1 )	return;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1, 0.0f );

	// 캐릭터 이동
	pChar->SetSkillMoveEnable( true, true );
}

//충전 상태일때 캐릭터 회전
void ioChargingComboSkill::SetCharacterMove( ioBaseChar *pChar )
{
	if ( !m_bSetRotation )
		return;

	//방향키 입력이 있다면
	//if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	//if( pChar->IsSettedDirection() )
	//{
		//현재 입력된 방향키 확인하고 변경
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	
		//입력된 방향키와 현재 캐릭터의 방향이 다르다면
		if ( m_InputDirKey != eNewDirKey )
		{
			m_InputDirKey = eNewDirKey;

			//입력된 방향으로 회전
			//pChar->SetTargetRotToDirKey( m_InputDirKey );

			// 캐릭터 이동
			CheckChargingMoveState( pChar );

			if ( pChar->IsNeedSendNetwork() )
			{
				//패킷전송
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SS_CHANGEMOVE;
				kPacket << (int)m_InputDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	//}
}

//충전 발사 상태 확인
void ioChargingComboSkill::CheckChargeShotState( ioBaseChar *pChar )
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

	//캐릭터 이동
	SetCharacterMove( pChar );
	
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
void ioChargingComboSkill::ActionChargeShot( ioBaseChar *pChar )
{
	if ( !pChar || m_bIsChargeShotInit )
		return;

	// 캐릭터 이동
	pChar->SetSkillMoveEnable( false, false );

	// 무적 버프 해제  2020-06-11
	//pChar->RemoveAllBuff();
	ioHashString szSkillStartBuff = GetOwnerSkillStartBuff();
	if( !szSkillStartBuff.IsEmpty() )
	{
		pChar->RemoveBuff( szSkillStartBuff );
	}


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

	// 2020-05-21
	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

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

void ioChargingComboSkill::CheckChargingMoveState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ChargingMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ChargeMotionName );
			if( pAni )
			{
				dwPlayTime = pAni->GetCurTime();
			}

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = true;
		}
	}
	else
	{
		if( m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_ChargeMotionName );
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ChargingMoveAnimation );
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = false;
		}
	}
}

void ioChargingComboSkill::SyncChargingMoveState( ioBaseChar* pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	pOwner->SetNetworkKeyInput( m_InputDirKey );

	if( m_InputDirKey == ioUserKeyInput::DKI_NONE )
	{
		iAniID = pGrp->GetAnimationIdx(m_ChargeMotionName);
		if( iAniID == -1 )
			return;

		DWORD dwPlayTime = 0;
		ioAnimation *pAni = pGrp->GetLoopInPlay( m_ChargingMoveAnimation );
		if( pAni )
		{
			dwPlayTime = pAni->GetCurTime();
		}

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		float fCurRate = FLOAT1;
		if( m_fGatherAniRate > 0.0f )
			fCurRate = m_fGatherAniRate;

		pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	}
	else
	{
		iAniID = pGrp->GetAnimationIdx( m_ChargingMoveAnimation );
		if( iAniID == -1 )
			return;

		DWORD dwPlayTime = 0;
		ioAnimation *pAni = pGrp->GetLoopInPlay( m_ChargeMotionName );
		if( pAni )
			dwPlayTime = pAni->GetCurTime();

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		float fCurRate = FLOAT1;
		if( m_fGatherAniRate > 0.0f )
			fCurRate = m_fGatherAniRate;

		pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	}
}

void ioChargingComboSkill::CheckChargingCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	
	if( m_bUsedEndOtherAttack && !m_bUseEndCombo )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	const AttributeList &rkComboList = m_vChargingComboList[m_nChargeGrade].m_vComboList;

	if( !COMPARE( m_iCurChargingCombo, 0, (int)rkComboList.size() ) || 
		m_bUseEndCombo && !COMPARE( m_nCurEndCombo, 0, m_nMaxEndCombo ) )
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
						SetNextChargingCombo( pChar, true );
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
				SetNextChargingCombo( pChar, false );
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
					SetNextChargingCombo( pChar, false );
					return;
				}
				else if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
				{				
					m_bUsedEndOtherAttack = true;
					SetNextChargingCombo( pChar, true );
					return;
				}
			}
		}
	}
}


void ioChargingComboSkill::SetNextChargingCombo( ioBaseChar *pChar, bool bOther )
{
	if( !pChar ) return;

	m_dwEnableReserveTime = 0;
	pChar->ClearSkillReserveKey();

	const AttributeList &rkComboList = m_vChargingComboList[m_nChargeGrade].m_vComboList;

	int iMaxCombo = rkComboList.size();
	if( !COMPARE( m_iCurChargingCombo, 0, iMaxCombo ) )
		return;

	m_SkillState = SS_COMBO;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() && m_bUseChangeDirection )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = rkComboList[m_iCurChargingCombo];

	if( bOther )
		kAttack = m_EndOtherAttack;
	
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
		if( m_iCurChargingCombo == 0 )
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurChargingCombo,
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

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		kPacket << m_iCurChargingCombo;
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
		
	if ( !m_bUseEndCombo )
	{
		m_iCurChargingCombo++;
	}
	else
	{
		//엔드콤보사용시
		if ( bOther )
			m_nCurEndCombo++;
		else
			m_iCurChargingCombo++;
	}
}