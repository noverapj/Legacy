#include "stdafx.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioSizeComboDash.h"
#include "ItemDefine.h"


ioSizeComboDash::ioSizeComboDash()
{
	m_DashState					= CDS_NONE;
	m_dwChargingStartTime		= 0;
	m_dwFireTime				= 0;
	m_iCurComboCnt				= 0;
	m_dwCancelCheckTime			= 0;
	m_dwActionEndTime			= 0;
	m_dwPreReserveEnableTime	= 0;
	m_bSetCombo					= false;
	m_fCurChargeRate			= 0.0f;
	m_fInputEventAnimationRate	= FLOAT1;

	m_szInputEventAnimation.Clear();

	// D~ 크기가 커지면서 상대를 내려친다.
	m_bScale					= false;
	m_vBeforeScale				= D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

	// +D~ : 크기가 작아지면서 상대의 우치로 이동하며 공격
	m_dwExtendChargingStartTime	= 0;
	m_bExtendChargingAttack		= false;

	// +D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	m_vMoveDir					= ioMath::VEC3_ZERO;
	m_fMoveAmt					= 0.0f;
	m_fSlideSpeed				= 0.0f;
	m_fTargetRange				= 0.0f;
	m_bAimedBall				= false;
	m_AimedTarget.Clear();

	// 이펙트
	m_dwScaleEffectID			= -1;
	m_dwExtendScaleEffectID		= -1;

	// 일정 시간 이후에 이동
	m_dwExtendChargingMoveCoolTime	= 0;
}

ioSizeComboDash::ioSizeComboDash( const ioSizeComboDash &rhs )
: ioExtendDash( rhs ),
m_ExtendAttackList( rhs.m_ExtendAttackList ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_szGatheringAni( rhs.m_szGatheringAni ),
m_fChargingSpeedRate( rhs.m_fChargingSpeedRate ),
m_ChargingAniList( rhs.m_ChargingAniList ),
m_fScaleValue( rhs.m_fScaleValue ),
m_fExtendScaleValue( rhs.m_fExtendScaleValue ),
m_ExtendChargingAniList( rhs.m_ExtendChargingAniList ),		// +D,+D~
m_dwExtendChargingTime( rhs.m_dwExtendChargingTime ),
m_ExtendChargeAttackList( rhs.m_ExtendChargeAttackList ),	// +D~, +-->, +D
m_fSlideTaregtCheckRange( rhs.m_fSlideTaregtCheckRange ),	// +D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
m_fSlideAngle( rhs.m_fSlideAngle ),
m_fSlideMoveRange( rhs.m_fSlideMoveRange ),
m_fChargeSlideSpeed( rhs.m_fChargeSlideSpeed ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_fSlideEndJumpEnableHeight( rhs.m_fSlideEndJumpEnableHeight ),
m_fSlideEndJumpAmt( rhs.m_fSlideEndJumpAmt ),
m_szScaleEffectName( rhs.m_szScaleEffectName ),				// 이펙트
m_szExtendScaleEffectName( rhs.m_szExtendScaleEffectName ),
m_szScaleEffectCreateSound( rhs.m_szScaleEffectCreateSound ),
m_dwExtendSlideMoveCoolTime( rhs.m_dwExtendSlideMoveCoolTime )
{
	m_DashState					= CDS_NONE;
	m_dwChargingStartTime		= 0;
	m_dwFireTime				= 0;
	m_iCurComboCnt				= 0;
	m_dwCancelCheckTime			= 0;
	m_dwActionEndTime			= 0;
	m_dwPreReserveEnableTime	= 0;
	m_bSetCombo					= false;
	m_fCurChargeRate			= 0.0f;
	m_fInputEventAnimationRate	= FLOAT1;

	m_szInputEventAnimation.Clear();

	// D~ 크기가 커지면서 상대를 내려친다.
	m_bScale					= false;
	m_vBeforeScale				= D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

	// +D~ : 크기가 작아지면서 상대의 우치로 이동하며 공격
	m_dwExtendChargingStartTime	= 0;
	m_bExtendChargingAttack		= false;

	// +D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	m_vMoveDir					= ioMath::VEC3_ZERO;
	m_fMoveAmt					= 0.0f;
	m_fSlideSpeed				= 0.0f;
	m_fTargetRange				= 0.0f;
	m_bAimedBall				= false;
	m_AimedTarget.Clear();

	// 이펙트
	m_dwScaleEffectID			= -1;
	m_dwExtendScaleEffectID		= -1;

	// 일정 시간 이후에 이동
	m_dwExtendChargingMoveCoolTime	= 0;
}

ioExtendDash* ioSizeComboDash::Clone()
{
	return new ioSizeComboDash( *this );
}

ioSizeComboDash::~ioSizeComboDash()
{
}

DashType ioSizeComboDash::GetType() const
{
	return DT_SIZE_COMBO;
}

void ioSizeComboDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	// D~
	m_dwChargingTime		= rkLoader.LoadInt_e( "dash_charge_time", 0 );	
	rkLoader.LoadString_e( "gathering_dash_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;

	// 차지 동작
	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_dash_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_dash_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ChargingAniList.push_back( szBuf );
	}

	// D~ 크기가 커지면서 상대를 내려친다.
	m_fScaleValue = rkLoader.LoadFloat_e( "extend_dash_scale_value", 0.0f );
	m_fExtendScaleValue = rkLoader.LoadFloat_e( "dash_extend_dash_scale_value", 0.0f );
	

	// Extend combo 
	// D~ : 크기가 커지면서 상대를 내려침
	// --> : 전방으로 돌진하면서 팔로 침
	// +D : 상대를 올려침 
	int iMax = rkLoader.LoadInt_e( "extend_dash_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "extend_dash_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtendAttackList.push_back( eAttribute );
	}

	// +D~ : 크기가 작아지면서 상대의 우치로 이동하며 공격 ( 여기서 분기 )
	m_dwExtendChargingTime = rkLoader.LoadInt_e( "dash_extend_charge_time", 0 );

	// Extend combo
	// +D~ : 크기가 작아지면서 상대의 위치로 이동하며 공격 
	// +--> : 주먹을 휘두르면서 전방으로 이동
	// +D : 상대를 띄움
	iMax = rkLoader.LoadInt_e( "dash_extend_charge__max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;
		wsprintf_e( szKey, "dash_extend_charge_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );
		m_ExtendChargeAttackList.push_back( eAttribute );
	}

	// D~ 크기가 작아지며 전방 위치로 이동하며 공격
	m_fSlideMoveRange = rkLoader.LoadFloat_e( "dash_extend_jump_slide_move_range", 0.0f );
	m_fSlideTaregtCheckRange = rkLoader.LoadFloat_e( "dash_extend_jump_slide_traget_check_range", 0.0f );
	m_fSlideAngle = rkLoader.LoadFloat_e( "dash_extend_jump_slide_angle", 0.0f );
	m_fChargeSlideSpeed = rkLoader.LoadFloat_e( "dash_extend_charging_slide_speed", 0 );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	// +D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	m_fSlideEndJumpEnableHeight = rkLoader.LoadFloat_e( "dash_extend_slide_end_jump_enable_height", 0.0f );
	m_fSlideEndJumpAmt = rkLoader.LoadFloat_e( "dash_extend_slide_end_jump_amt", 0.0f );

	// 이펙트
	rkLoader.LoadString_e( "scale_effect_name", "", szBuf, MAX_PATH );	// 커질때
	m_szScaleEffectName = szBuf;
	rkLoader.LoadString_e( "extend_scale_effect_name", "", szBuf, MAX_PATH );	// 작아질때
	m_szExtendScaleEffectName = szBuf;

	rkLoader.LoadString_e( "scale_effect_create_sound", "", szBuf, MAX_PATH );
	m_szScaleEffectCreateSound = szBuf;

	// 일정 시간 이후에 이동
	m_dwExtendSlideMoveCoolTime = rkLoader.LoadInt_e( "dash_extend_slide_move_cool_time", 0 );

}

bool ioSizeComboDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return false;	}

	m_bSetCombo					= false;
	m_iCurComboCnt				= 0;
	m_dwChargingStartTime		= 0;
	m_dwActionEndTime			= 0;
	m_dwPreReserveEnableTime	= 0;
	m_DashState					= CDS_NONE;
	m_dwCancelCheckTime			= 0;
	m_fCurChargeRate			= 0.0f;

	pOwner->ClearDirDoubleClick();

	// +D~ : 크기가 작아지면서 상대의 위치로 이동하며 공격
	m_dwExtendChargingStartTime	= 0;

	// +D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	m_fMoveAmt					= 0.0f;
	m_fSlideSpeed				= m_fChargeSlideSpeed;	

	pOwner->NormalAttackOver( false );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );

	ChangeToCharging( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_NONE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioSizeComboDash::ExtendDashEnd( ioBaseChar *pOwner )
{
	m_DashState					= CDS_NONE;
	m_bSetCombo					= false;
	m_iCurComboCnt				= 0;
	m_dwChargingStartTime		= 0;
	m_bScale					= false;

	m_dwActionEndTime			= 0;
	m_dwReserveEnableTime		= 0;
	m_dwPreReserveEnableTime	= 0;

	m_iSkillInput				= -1;
	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;	
	m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;	
	
	pOwner->ClearDirDoubleClick();

	if( pOwner )
	{
		pOwner->InitExtendAttackTagInfo();
	}

	// 크기를 원래 상태로
	pOwner->SetWorldScale( m_vBeforeScale );

	// 이펙트
	pOwner->EndEffect( m_dwScaleEffectID );
	pOwner->EndEffect( m_dwExtendScaleEffectID );
	m_dwScaleEffectID			= -1;
	m_dwExtendScaleEffectID		= -1;

	// +D~ : 크기가 작아지면서 상대의 우치로 이동하며 공격
	m_dwExtendChargingStartTime	= 0;
	m_bExtendChargingAttack		= false;

	// +D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	m_fMoveAmt					= 0.0f;
}

void ioSizeComboDash::ChangeToCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_ChargingAniList.empty() )
		return;

	ioHashString szCurChargingAni;

	int iChargingCnt = m_ChargingAniList.size();
	int iCurIndex = m_iCurComboCnt;

	if( m_bSetCombo )
	{
		iCurIndex = m_iCurComboCnt+1;
	}

	if( COMPARE( iCurIndex, 0, iChargingCnt ) )
	{
		szCurChargingAni = m_ChargingAniList[iCurIndex];
	}
	else
	{
		szCurChargingAni = m_ChargingAniList[0];
	}

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( szCurChargingAni, FLOAT100 );

	m_dwChargingStartTime = FRAMEGETTIME();

	// +D~ : 크기가 작아지면서 상대의 우치로 이동하며 공격
	m_dwExtendChargingStartTime	= 0;
	m_bExtendChargingAttack		= false;

	m_dwActionEndTime			= 0;
	m_dwReserveEnableTime		= 0;
	m_dwPreReserveEnableTime	= 0;

	m_iSkillInput				= -1;
	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;	
	m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;	

	m_DashState					= CDS_CHARGE;
}

void ioSizeComboDash::ProcessExtendCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwExtendChargingStartTime > 0 && m_dwExtendChargingStartTime+m_dwExtendChargingTime < dwCurTime )
		{	
			// 게이지 검사
			if( CheckEnableExtendComboState(pOwner) )
			{
				// +--> 공격 후 +D~ 으로 2020-10-19
				if( m_iCurComboCnt == 1 )
				{				
					m_iCurComboCnt			= 0;
					m_bScale				= false;
					//m_bExtendChargingAttack = true;
					m_fMoveAmt				= 0.0f;
					m_eSpecialDashCurrDirkey= ioUserKeyInput::DKI_NONE; // 2020-10-19
					m_AimedTarget.Clear();
					SetExtendComboState( pOwner, m_bExtendChargingAttack );
				}
				else if( m_iCurComboCnt == 0 || m_iCurComboCnt == 2 )
				{
					// +D or +--> 공격 이 없는 경우와 +D 공격후에 다음 +D~ 으로 공격 한다. 
					// 크기가 작아지면서 상대의 위치로 이동하면서 공격한다.
					m_iCurComboCnt			= 0;
					m_bScale				= false;
					m_fMoveAmt				= 0.0f;
					m_eSpecialDashCurrDirkey= ioUserKeyInput::DKI_NONE; // 2020-10-19
					m_AimedTarget.Clear();
					if( !m_bExtendChargingAttack )	{	m_bExtendChargingAttack	= true;	}
					else	{	m_bExtendChargingAttack	= false;	}

					SetExtendComboState( pOwner, m_bExtendChargingAttack );

				}
			}
			else
			{
				// 게이지가 없으면 기본 공격
				if( m_bSetCombo )
				{
					SetComboDash( pOwner );
				}
				else 
				{
					ChangeToNormalAttack( pOwner );
				}
			}
		}
	}
	else
	{
		// +--> 했으면 리턴 시킨다.
		if( m_iCurComboCnt > 0 )
		{		
			m_dwExtendChargingStartTime = FRAMEGETTIME();
			// 애니가 끝나면
			DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();
			if( dwEndTime < FRAMEGETTIME() )
			{				
				ProcessKeyReserve( pOwner );
			}
			
			return;
		}

		// +D 2번 애니 공격 
		m_iCurComboCnt = 2;
		SetExtendComboState( pOwner, m_bExtendChargingAttack );
	}
}

void ioSizeComboDash::ChangeToExtendCharging( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	if( m_ExtendChargingAniList.empty() )	{	return;	}

	ioHashString szCurChargingAni;

	int iChargingCnt	= m_ExtendChargingAniList.size();
	int iCurIndex		= m_iCurComboCnt;

	if( m_bSetCombo )
	{
		iCurIndex		= m_iCurComboCnt+1;		
	}

	if( COMPARE( iCurIndex, 0, iChargingCnt ) )
	{
		szCurChargingAni= m_ExtendChargingAniList[iCurIndex];
	}
	else
	{		
		szCurChargingAni= m_ExtendChargingAniList[0];
	}

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( szCurChargingAni, FLOAT100 );

	m_dwChargingStartTime	= FRAMEGETTIME();
	m_dwActionEndTime		= 0;
	m_dwReserveEnableTime	= 0;
	m_dwPreReserveEnableTime= 0;

	m_iSkillInput			= -1;
	m_bEnableDefenseState	= false;
	m_bEnableAttackState	= false;
	m_bEnableJumpState		= false;	
	m_eSpecialDashCurrDirkey= ioUserKeyInput::DKI_NONE;

	m_DashState				= CDS_EXTEND_CHARGE;
	m_iCurComboCnt			= iCurIndex;
}

void ioSizeComboDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_DashState )
	{
	case CDS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case CDS_NORMAL_ATTACK:
	case CDS_COMBO_ATTACK:
		ProcessNormalComboDash( pOwner );
		break;
	case CDS_EXTEND_COMBO:
		ProcessExtendComboDash( pOwner );
		break;
	case CDS_EXTEND_CHARGE:
		ProcessExtendCharging( pOwner );
		break;
	}

	// 크기를 원래 상태로
	if( !m_bScale )
	{
		pOwner->SetWorldScale( m_vBeforeScale );
		pOwner->EndEffect( m_dwScaleEffectID );
		pOwner->EndEffect( m_dwExtendScaleEffectID );
		m_dwScaleEffectID			= -1;
		m_dwExtendScaleEffectID		= -1;
	}

	// 이펙트
	ProcessScaleEffect( pOwner );
}

void ioSizeComboDash::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			// 게이지 검사
			if( CheckEnableExtendComboState( pOwner ) )
			{
				// D~	
				m_AimedTarget.Clear();
				m_iCurComboCnt = 0;
				m_fMoveAmt		= 0.0f;
				SetExtendComboState( pOwner );				
			}
			else
			{
				if( m_bSetCombo )
				{
					SetComboDash( pOwner );
				}
				else 
				{
					ChangeToNormalAttack( pOwner );
				}
			}
			return;
		}
	}
	else
	{
		if( m_bSetCombo )
		{
			SetComboDash( pOwner );
		}
		else 
		{
			ChangeToNormalAttack( pOwner );
		}
		return;
	}
}

bool ioSizeComboDash::CheckEnableExtendComboState( ioBaseChar *pOwner )
{
	if( NULL == pOwner )					{	return false;	}
	if( m_ExtendAttackList.empty() )		{	return false;	}
	if( m_ExtendChargeAttackList.empty() )	{	return false;	}

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioSizeItem* pSizeItem = ToSizeItem( pItem );
	if( pSizeItem )
	{
		// 처음 콤보 시작
		int iCurBullet = pSizeItem->GetCurBullet();
		int iNeedBullet = pSizeItem->GetNeedBullet();

		if( iCurBullet >= iNeedBullet )
		{
			pSizeItem->DecreaseCoolTime();
			return true;
		}
	}

	return false;
}

void ioSizeComboDash::SetExtendComboState( ioBaseChar *pOwner, bool bExtendChargeCombo )
{
	if( !pOwner )
	{
		return;
	}

	int iMaxCnt = 0;
	AttackAttribute rkAttribute;
	if( !bExtendChargeCombo )
	{
		// extend combo D~, +-->, +D
		rkAttribute = m_ExtendAttackList[m_iCurComboCnt];
		iMaxCnt = m_ExtendAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		{
			return;
		}

		// 크기가 커지면서 상대를 내려친다.
		if( !m_bScale )
		{
			m_bScale = true;

			// 이펙트 생성
			CreateScaleEffectChargeEffect( pOwner, bExtendChargeCombo );
			D3DXVECTOR3 vScale = m_vBeforeScale + (m_vBeforeScale * m_fScaleValue);
			pOwner->SetWorldScale( vScale );

			m_bExtendChargingAttack	= false;
		}
	}
	else
	{
		// extend combo +D~, +-->, +D
		rkAttribute = m_ExtendChargeAttackList[m_iCurComboCnt];
		iMaxCnt = m_ExtendChargeAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		{
			return;
		}

		// 크기가 작아지면서 상대의 위치로 이동하며 공격한다.
		if( !m_bScale )
		{
			// 이펙트 생성
			CreateScaleEffectChargeEffect( pOwner, bExtendChargeCombo );

			D3DXVECTOR3 vScale = m_vBeforeScale - (m_vBeforeScale * m_fExtendScaleValue);
			pOwner->SetWorldScale( vScale );
			m_bScale = true;
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
	{
		return;
	}

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );	
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSizeComboDash::SetExtendComboState - %s Animation is Not Exist",
								rkAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = rkAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;

	m_dwActionEndTime				= 0;
	m_dwReserveEnableTime			= 0;
	m_dwPreReserveEnableTime		= 0;
	m_iSkillInput					= -1;
	m_bEnableDefenseState			= false;
	m_bEnableAttackState			= false;
	m_bEnableJumpState				= false;
	m_dwExtendChargingStartTime		= 0;

	// 일정 시간 이후에 이동
	m_dwExtendChargingMoveCoolTime	= 0;

	// D~, +D~ 크기가 작아지면서 상대의 위치로 이동하면서 공격
	if( m_iCurComboCnt == 0 && m_bExtendChargingAttack )
	{
		CheckTarget( pOwner, m_fSlideAngle, m_fSlideTaregtCheckRange, m_fSlideMoveRange, false );
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget )
		{
			pOwner->SetTargetRotToTargetPos( pTarget, true );

			m_vMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
		}
		else
		{
			m_vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
		}
	}


	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pOwner->RefreshFireTimeList( iAniID, rkAttribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + rkAttribute.m_dwEndDelay;

	DWORD dwEndTime = pGrp->GetAnimationEventTime_e( iAniID, "landing" );
	if( dwEndTime == 0 )
	{
		dwEndTime = pGrp->GetAnimationFullTime( iAniID );
	}

	int iEndAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackEndAnimation );
	if( iEndAniID == -1 )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

		CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if( fReserveTime > 0.0f )
		{
			m_dwPreReserveEnableTime = dwCurTime + fReserveTime;
		}

		float fWaitTime = dwEndTime * fTimeRate + dwPreDelay;
		float fEndTimeRate = rkAttribute.m_fAttackEndAniRate;

		pGrp->SetActionAni( iEndAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/fEndTimeRate, fWaitTime, 0.0f, true );

		CheckKeyReserveTime( pOwner, iEndAniID, fEndTimeRate );
		if( m_dwReserveEnableTime > 0 )
		{
			m_dwReserveEnableTime = dwDuration;
		}

		dwDuration += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate + rkAttribute.m_dwEndDelay;
	}

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = CDS_EXTEND_COMBO;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_DashState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_bExtendChargingAttack;
		kPacket << !m_bScale;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSizeComboDash::ProcessExtendComboDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	// 일정 시간 이후에 이동
	if( m_iCurComboCnt == 0 && m_bExtendChargingAttack && m_dwExtendChargingMoveCoolTime == 0 )
	{
		m_dwExtendChargingMoveCoolTime = FRAMEGETTIME();
	}

	//  D~, +D~, --> 주먹을 휘두르면서 전방으로 이동 할때
	if( m_iCurComboCnt ==1 && m_bExtendChargingAttack )
	{
		if( CheckAniDash_Input_Speical_Dash( pOwner ) )
		{
			return;
		}
	}



	// D~, +D~ 크기가 작아지면서 상대의 위치로 이동하면서 공격
	if( m_iCurComboCnt == 0 && m_bExtendChargingAttack && m_dwExtendChargingMoveCoolTime + m_dwExtendSlideMoveCoolTime < dwCurTime)
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
		pOwner->SetCurGravityGapAmt( 0.0f );

		float fMoveSpeed = 0.0f;
		fMoveSpeed = m_fSlideSpeed * g_FrameTimer.GetSecPerFrame();
		m_fMoveAmt += fMoveSpeed;

		// m_fSlideMoveRange 영역까지 이동
		if( m_fMoveAmt < m_fSlideMoveRange )
		{
			D3DXVECTOR3 vTotalMove = m_vMoveDir * fMoveSpeed;		
			ProcessSlideMove( pOwner, vTotalMove );
		}
	}

	if( dwEndTime > dwCurTime )
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			CheckExtendReserve( pOwner );
			CheckAniJump_Input_Jump_S( pOwner );
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

void ioSizeComboDash::ProcessNormalComboDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	// 대시 1타에서 D~ 시 캔슬 D~ 공격이 발생 되게 한다. 
	if( CheckAniDash_Input_Speical_Dash( pOwner ) )
		return;

	if( dwEndTime > dwCurTime )
	{
		if( m_dwReserveEnableEndTime == 0 && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				CheckKeyReserve( pOwner );
			}

			CheckAniJump_Input_Jump_S( pOwner );

			return;
		}
		else if( m_dwReserveEnableEndTime > 0 && m_dwReserveEnableTime > 0 && 
			m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime &&
			m_dwReserveEnableEndTime + pOwner->GetActionStopTotalDelay() >= dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				CheckKeyReserve( pOwner );
			}			
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

void ioSizeComboDash::SetComboDash( ioBaseChar *pOwner, bool bCharge )
{
	if( !pOwner ) return;

	int iMaxCnt = pOwner->GetExtraDashAttackMax();

	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTRA_DASH_ATTACK, m_iCurComboCnt );
	if( !pAttr )
		return;

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	m_szInputEventAnimation = pAttr->m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSizeComboDash::SetComboDash - %s Animation is Not Exist",
								pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	m_dwActionEndTime			= 0;
	m_dwPreReserveEnableTime	= 0;
	m_dwReserveEnableTime		= 0;

	m_iSkillInput				= -1;
	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;
	m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( !bCharge )
	{
		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
	}

	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;
	
	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = CDS_COMBO_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_COMBO_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << bCharge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후
	m_iCurComboCnt++;	
}

void ioSizeComboDash::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;


	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_DASH_ATTACK );;
	if( !pAttr )
		return;

	// 1타에서 D~ 시 캔슬 D~ 공격이 발생 되게 한다. 
	m_szInputEventAnimation = pAttr->m_AttackAnimation;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSizeComboDash::ChangeToNormalAttack - %s Animation is Not Exist",
								pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;
	m_fInputEventAnimationRate = fTimeRate;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, pAttr->m_dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)pAttr->m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = pAttr->m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime= dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, pAttr->m_dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + pAttr->m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );	

	m_bSetCombo = true;
	m_DashState = CDS_NORMAL_ATTACK;

	CheckKeyReserveTime( pOwner, iAniID, pAttr->m_fAttackAniRate );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, pAttr->m_dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_NORMAL_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioSizeComboDash::IsPowerChargingState()
{
	if( m_DashState == CDS_NONE )
	{
		return false;
	}

	return true;
}

void ioSizeComboDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurComboCnt;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, false );

	switch( iState )
	{
		case CDS_NONE:
		{
			StartDash( pOwner );
			break;
		}
		case CDS_CHARGE:
		{
			ChangeToCharging( pOwner );
			break;
		}
		case CDS_NORMAL_ATTACK:
		{
			ChangeToNormalAttack( pOwner );
			break;
		}
		case CDS_EXTEND_COMBO:
		{
			rkPacket >> m_bExtendChargingAttack;
			rkPacket >> m_bScale;
			SetExtendComboState( pOwner, m_bExtendChargingAttack );
			break;
		}
		case CDS_COMBO_ATTACK:
		{
			bool bCharge;
			rkPacket >> bCharge;
			SetComboDash( pOwner, bCharge );
			break;
		}
		case CDS_STATE_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			SetEndState( pOwner );
			break;
		}
		case CDS_CHARGING_TARGET:
		{
			rkPacket >> m_AimedTarget;
			break;
		}
	}
}

void ioSizeComboDash::ProcessKeyReserve( ioBaseChar *pOwner )
{
	// +D~, +-->, +D 중 공중일 경우 상태 종료 시 점핑
	if ( pOwner->IsNeedProcess() )
	{
		SetEndState( pOwner );
		return;
	}

	// 공격키가 눌러져 있으면 공격 상태 처리
	if( pOwner->IsAttackKeyDown() && 
		!pOwner->IsJumpKeyDown() &&
		!pOwner->IsDefenseKeyDown() &&
		!pOwner->IsChatModeState(true) &&
		!pOwner->IsFloating() )
	{	
		ioAttackableItem	*pAttackableItem = pOwner->GetPriorityItem( SPT_ATTACK );
		ioSizeItem* pSizeItem = ToSizeItem( pAttackableItem );
		if( pSizeItem && pSizeItem->IsCanFire( pOwner ) )
		{
			// 대기
			pOwner->ClearAttackFireTimeAndSkill();
			pOwner->ClearReservedSliding();
			pOwner->SetState( CS_DELAY );

			// 원래 크기대로 변경
			pOwner->SetWorldScale( m_vBeforeScale );

			pOwner->EndEffect( m_dwScaleEffectID );
			pOwner->EndEffect( m_dwExtendScaleEffectID );
			m_dwScaleEffectID			= -1;
			m_dwExtendScaleEffectID		= -1;

			return;
		}

		ioObjectItem *pItem = pOwner->GetObject();
		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 필살기 예약되었으면 필살기 발동
	ioUFCItem *pUFCItem = ToUFCItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pUFCItem && pUFCItem->CheckOnHand() && m_iSkillInput > 0 )
	{
		pUFCItem->ChangeToSpecialAttackFire( pOwner, ioUFCItem::SS_CHARGE_A + m_iSkillInput );
		m_iSkillInput = -1;
		return;
	}
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 대기
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetState( CS_DELAY );
	// 원래 크기대로 변경
	pOwner->SetWorldScale( m_vBeforeScale );

	// 이펙트
	pOwner->EndEffect( m_dwScaleEffectID );
	pOwner->EndEffect( m_dwExtendScaleEffectID );
	m_dwScaleEffectID			= -1;
	m_dwExtendScaleEffectID		= -1;
}

void ioSizeComboDash::CheckExtraReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
}

void ioSizeComboDash::CheckExtendReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

		if( pOwner->IsSettedDirection() )
		{
			float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}

		m_dwExtendChargingStartTime	= FRAMEGETTIME();
		m_DashState					= CDS_EXTEND_CHARGE;
	}
	else if( pOwner->IsDirKeyDoubleClick() && !m_bEnableDefenseState && !m_bEnableJumpState && m_eSpecialDashCurrDirkey == ioUserKeyInput::DKI_NONE  ) // 2020-10-29
	{
		m_bEnableJumpState = false;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
		m_eSpecialDashCurrDirkey = pOwner->CheckCurDirKey();

		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
		pOwner->SetWorldOrientation( pOwner->GetTargetRot() );

		// +D 했으면 리턴 시킨다.
		int iChargingCnt = m_ExtendAttackList.size();
		if( !COMPARE( m_iCurComboCnt+1, 0, iChargingCnt ) )
		{
			return;
		}

		// 다음 콤보 +->-> 전방으로 돌진하면서 팔로 침
		m_iCurComboCnt = 1;		
		SetExtendComboState( pOwner, m_bExtendChargingAttack );
	}
}

// 1타에서 D~ 시 캔슬 D~ 공격이 발생 되게 한다.
bool ioSizeComboDash::CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_szInputEventAnimation.IsEmpty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );		
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fInputEventAnimationRate;
		dwInputSpecialDashTime += m_dwActionStartTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioSizeItem* pFrozenItem = ToSizeItem( pItem );
		int nCurBullet = pFrozenItem->GetCurBullet();
		int nNeedBullet = pFrozenItem->GetNeedBullet();
		if( pFrozenItem && pOwner->IsAttackKeyDown() && nCurBullet >= nNeedBullet )
		{
			ChangeToCharging( pOwner );
		}
		return false;
	}

	return false;
}

void ioSizeComboDash::CheckKeyReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKeyDown() && !m_bEnableAttackState && !bReserveSkill )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

		if( pOwner->IsSettedDirection() )
		{
			float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}

		ChangeToCharging( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << CDS_CHARGE;
			kPacket << m_iCurComboCnt;
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
}

bool ioSizeComboDash::CheckAniJump_Input_Jump_S( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_szInputEventAnimation.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fInputEventAnimationRate;

		dwTime += m_dwActionStartTime;
		
		//다른 로직에서 input_jump_s의 기능이 key_reserve를 체크하지 않는데 해당로직에서만 체크하고 있어 버그로 판단하여 수정
		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{				
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

void ioSizeComboDash::CheckTarget( ioBaseChar *pOwner, float fAngle, float fTargetCheckRange, float fRange, bool bSend )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	bool bChange = false;
	ioHashString szTarget;
	m_fTargetRange = fRange;

	ioBaseChar *pTarget = FindTarget( pOwner, fAngle, fTargetCheckRange );

	if( pTarget )
		szTarget = pTarget->GetCharName();

	if ( m_AimedTarget != szTarget )
	{
		bChange = true;
		m_bAimedBall = false;
		m_AimedTarget = szTarget;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_CHARGING_TARGET;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

ioBaseChar *ioSizeComboDash::FindTarget( ioBaseChar *pOwner, float fAngle, float fRange )
{
	if( !pOwner )
		return NULL;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return NULL;

	ioBaseChar *pTargetChar = NULL;
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fPreDistSq = fRange * fRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	BaseCharDistanceSqList kList;
	kList.clear();
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )	continue;

		if( pTarget == pOwner )
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		if ( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq <= fPreDistSq )
		{
			fPreDistSq = fDistSq;

			D3DXVec3Normalize( &vDiff, &vDiff );
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap < fAngle * FLOAT05 )
			{
				pTargetChar = pTarget;
			}
		}
	}

	return pTargetChar;
}

void ioSizeComboDash::ProcessSlideMove( ioBaseChar *pOwner, D3DXVECTOR3 vTotalMove )
{
	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
	{
		float fPreHeight = pOwner->GetBottomHeight();
		pOwner->Translate( vTotalMove );

		float fMoveHeight = pOwner->GetBottomHeight();

		if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
		{
			D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

			float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );

			if( fTopHeight > FLOAT1 )
			{
				vCurPos.y = max( fTopHeight, fPreHeight );
				pOwner->SetWorldPosition( vCurPos );
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();

	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );
}

void ioSizeComboDash::SetEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap < m_fSlideEndJumpEnableHeight )
		pOwner->SetExtendAttackEndJump( 0, FLOAT1 );
	else
		pOwner->SetSKillEndJumpState( m_fSlideEndJumpAmt, false, false, true, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_STATE_END;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioSizeComboDash::CreateScaleEffectChargeEffect( ioBaseChar *pOwner, bool bExtendChargeCombo )
{
	if ( !pOwner )
	{
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
	{
		return;
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vScale = pOwner->GetWorldScale();

	if( !bExtendChargeCombo )
	{
		// 크기가 커지면서 상대를 내려친다.
		if ( m_szScaleEffectName.IsEmpty() )
		{
			return;
		}

		
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szScaleEffectName, vPos, m_vBeforeScale );
		if ( pMapEffect )
		{
			m_dwScaleEffectID =  pMapEffect->GetUniqueID();
		}
	}
	else
	{
		// 크기가 작아지면서 상대의 위치로 이동하며 공격한다.
		if ( m_szExtendScaleEffectName.IsEmpty() )
		{
			return;
		}

		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szExtendScaleEffectName, vPos, m_vBeforeScale );
		if ( pMapEffect )
		{
			m_dwExtendScaleEffectID =  pMapEffect->GetUniqueID();
		}
	}

	if( !m_szScaleEffectCreateSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szScaleEffectCreateSound );
	}
}

void ioSizeComboDash::ProcessScaleEffect( ioBaseChar* pOwner )
{
	if ( !pOwner )
	{
		return;
	}


	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
	{
		return;
	}
	
	if( !m_bExtendChargingAttack )
	{
		// 크기가 커지면서 상대를 내려친다.
		if ( m_dwScaleEffectID == -1)
		{
			return;
		}

		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwScaleEffectID );
		if( pMapEffect )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			pMapEffect->SetWorldPosition( vPos );
		}
	}
	else
	{
		// 크기가 작아지면서 상대의 위치로 이동하며 공격한다.
		if ( m_dwExtendScaleEffectID == -1 )
		{
			return;
		}

		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwExtendScaleEffectID );
		if( pMapEffect )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			pMapEffect->SetWorldPosition( vPos );
		}
	}
}

// CS_FALL
bool ioSizeComboDash::IsNoDropState( ioBaseChar *pOwner )
{
	if( !pOwner )						{	return false;	}
	if( pOwner->GetState() == CS_FALL )	{	return true;	}

	switch( m_DashState )
	{
		case CDS_EXTEND_COMBO:
		case CDS_EXTEND_CHARGE:	// 2020-10-16
		{			
			// 0 : +D~ 크기가 작아지면서 상대의 위치로 이동하며 공격
			// 1 : +D~ 이어서 +--> 주먹을 휘두르면서 전방으로 이동 
			// 2 : +D~ 이어서 +D 상대를 띄움 
			if( ( 0 == m_iCurComboCnt || 1 == m_iCurComboCnt || 2 == m_iCurComboCnt)  && m_bExtendChargingAttack )
			{
				return true;
			}
		}
	}

	return false;
}