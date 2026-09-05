#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioExtendJump.h"

ioExtendJump::ioExtendJump()
{
	m_bUseExtendJump = false;
	m_bUseJumpDashAndLandDash = false;
	m_JumpDashDownKey = ioUserKeyInput::DKI_NONE;

	// air_jump
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	m_dwReserveEnableTime = 0;
}

ioExtendJump::ioExtendJump( const ioExtendJump &rhs )
: m_fJumpNeedGauge( rhs.m_fJumpNeedGauge ),
  m_fJumpExtraGauge( rhs.m_fJumpExtraGauge ),
  m_fJumpSpeedRate( rhs.m_fJumpSpeedRate ),
  m_bEnableLandAfterState( rhs.m_bEnableLandAfterState ),
  m_LandAfterFailAni( rhs.m_LandAfterFailAni ),
  m_fLandAfterFailAniRate( rhs.m_fLandAfterFailAniRate ),
  m_LandAfterSuccessAni( rhs.m_LandAfterSuccessAni ),
  m_fLandAfterSuccessAniRate( rhs.m_fLandAfterSuccessAniRate ),
  m_JumpGrapplingInfo( rhs.m_JumpGrapplingInfo ),
  m_JumpGrapplingWoundInfo( rhs.m_JumpGrapplingWoundInfo ),
  m_fAirForcePower( rhs.m_fAirForcePower ),
  m_fAirJumpPower( rhs.m_fAirJumpPower ),
  m_bUseJumpDashAndLandDash( rhs.m_bUseJumpDashAndLandDash ),
  m_bEnableDashEndJump( rhs.m_bEnableDashEndJump ),
  m_fDashEndJumpPower( rhs.m_fDashEndJumpPower ),
  m_bEnableLandAfterAttackByKeyDown( rhs.m_bEnableLandAfterAttackByKeyDown )
{
	m_bUseExtendJump = false;

	// air_jump
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	m_dwReserveEnableTime = 0;
}

ioExtendJump::~ioExtendJump()
{
}

void ioExtendJump::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_fJumpNeedGauge = rkLoader.LoadFloat_e( "jump_need_gauge", 0.0f );
	m_fJumpExtraGauge = rkLoader.LoadFloat_e( "jump_extra_gauge", 0.0f );

	m_fJumpSpeedRate = rkLoader.LoadFloat_e( "jump_speed_rate", FLOAT1 );

	m_bEnableLandAfterState = rkLoader.LoadBool_e( "enable_land_after_state", false );

	rkLoader.LoadString_e( "land_after_fail_ani", "", szBuf, MAX_PATH );
	m_LandAfterFailAni = szBuf;
	m_fLandAfterFailAniRate = rkLoader.LoadFloat_e( "land_after_fail_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "land_after_success_ani", "", szBuf, MAX_PATH );
	m_LandAfterSuccessAni = szBuf;
	m_fLandAfterSuccessAniRate = rkLoader.LoadFloat_e( "land_after_success_ani_rate", FLOAT1 );

	//
	m_JumpGrapplingInfo.Init();

	rkLoader.LoadString_e( "jump_grappling_ani", "", szBuf, MAX_PATH );
	m_JumpGrapplingInfo.m_JumpGrapplingAni = szBuf;
	m_JumpGrapplingInfo.m_fJumpGrapplingAniRate = rkLoader.LoadFloat_e( "jump_grappling_ani_rate", FLOAT1 );

	m_JumpGrapplingInfo.m_fJumpGrapplingMoveSpeed = rkLoader.LoadFloat_e( "jump_grappling_move_speed", 0.0f );
	m_JumpGrapplingInfo.m_dwJumpGrapplingRotateTime = (DWORD)rkLoader.LoadInt_e( "jump_grappling_rotate_speed", 0 );

	LoadAttackAttribute_e( "jump_grappling_attack", m_JumpGrapplingInfo.m_JumpGrapplingAttack, rkLoader );

	//
	m_JumpGrapplingWoundInfo.Init();

	rkLoader.LoadString_e( "jump_grappling_wound_ani", "", szBuf, MAX_PATH );
	m_JumpGrapplingWoundInfo.m_WoundedAni = szBuf;
	m_JumpGrapplingWoundInfo.m_fWoundedAniRate = rkLoader.LoadFloat_e( "jump_grappling_wound_ani_rate", FLOAT1 );

	m_JumpGrapplingWoundInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "jump_grappling_wound_reverse_rotate", false );
	m_JumpGrapplingWoundInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "jump_grappling_wound_enable_col", GCT_NONE );

	m_JumpGrapplingWoundInfo.m_fOffSet = rkLoader.LoadFloat_e( "jump_grappling_wound_offset", 0.0f );
	m_JumpGrapplingWoundInfo.m_fHeightRate = rkLoader.LoadFloat_e( "jump_grappling_wound_height_rate", FLOAT05 );

	m_JumpGrapplingWoundInfo.m_dwJumpGrapplingRotateTime = (DWORD)rkLoader.LoadInt_e( "jump_grappling_rotate_speed", 0 );

	int iBuffCnt = rkLoader.LoadInt_e( "jump_grappling_cancel_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_JumpGrapplingWoundInfo.m_CancelBuffList.push_back( szBuf );
	}

	// air_jump
	m_fAirJumpPower = rkLoader.LoadFloat_e( "jump_air_power", 0.0f );
	m_fAirForcePower = rkLoader.LoadFloat_e( "jump_air_force_power", 0.0f );

	m_bUseJumpDashAndLandDash = rkLoader.LoadBool_e( "jump_use_jumpdash_landdash", false );
		
	m_bEnableDashEndJump = rkLoader.LoadBool_e( "endable_dash_end_jump", false );
	m_fDashEndJumpPower = rkLoader.LoadFloat_e( "dash_end_jump_power", 0.0f );

	m_bEnableLandAfterAttackByKeyDown = rkLoader.LoadBool_e( "enable_land_after_attack_by_key_down", false );
}

void ioExtendJump::InitJump()
{
}

void ioExtendJump::SetJumpState()
{
	m_bUseExtendJump = false;
}

void ioExtendJump::ProcessJumpReady( ioBaseChar *pOwner )
{
}

void ioExtendJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
}

void ioExtendJump::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate )
{
}

void ioExtendJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
}

void ioExtendJump::ProcessLanding( ioBaseChar *pOwner )
{

}

void ioExtendJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
}

bool ioExtendJump::ProcessAimJump( ioBaseChar *pOwner, float fHeightGap )
{
	return false;
}

void ioExtendJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
}

void ioExtendJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
}

ioHashString ioExtendJump::GetJumpAniDefenseWounded()
{
	return ioHashString();
}

ioHashString ioExtendJump::GetJumpAniDefenseCriticalWounded()
{
	return ioHashString();
}

DWORD ioExtendJump::GetChargingTime( ioBaseChar *pChar )
{
	return 0;
}

bool ioExtendJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	return true;
}

bool ioExtendJump::IsCanJumpAttack( ioBaseChar *pOwner )
{
	return true;
}

float ioExtendJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	return m_fJumpSpeedRate;
}

void ioExtendJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
}

bool ioExtendJump::IsEnableLandAfterState()
{
	return m_bEnableLandAfterState;
}

const ioHashString& ioExtendJump::GetLandAfterAni( bool bSuccess )
{
	if( bSuccess )
		return m_LandAfterSuccessAni;

	return m_LandAfterFailAni;
}

float ioExtendJump::GetLandAfterAniRate( bool bSuccess )
{
	if( bSuccess )
		return m_fLandAfterSuccessAniRate;

	return m_fLandAfterFailAniRate;
}

bool ioExtendJump::IsEnableJumpLandEndDash()
{
	return true;
}

bool ioExtendJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return false;
}

void ioExtendJump::OnEndExtendJump( ioBaseChar *pOwner )
{
}

void ioExtendJump::ClearAirJump()
{
	m_vAirJumpTimeList.erase( m_vAirJumpTimeList.begin(), m_vAirJumpTimeList.end() );
	m_iCurAirJump = 0;
}

void ioExtendJump::SetAirJump( ioBaseChar *pOwner, int iAniID, float fTimeRate )
{
	ClearAirJump();

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	pGrp->GetAnimationEventTimeList_e( iAniID, "air_jump", m_vAirJumpTimeList );
	std::list< float >::iterator iter = m_vAirJumpTimeList.begin();
	while( iter != m_vAirJumpTimeList.end() )
	{
		(*iter) *= fTimeRate;
		(*iter) += dwCurTime;
		iter++;
	}

	// For AirJumpPower
	m_fAirJumpPowerRate = 0.0f;

	float fJumpPowerAmt = pOwner->GetCurJumpPowerAmt();
	if( fJumpPowerAmt > 0.0f )
	{
		m_fAirJumpPowerRate = fabs( fJumpPowerAmt - pOwner->GetGravityAmt() ) / fJumpPowerAmt;
		m_fAirJumpPowerRate = max( 0.0f, min( FLOAT1, m_fAirJumpPowerRate ) );
		m_fAirJumpPowerRate = FLOAT1 - m_fAirJumpPowerRate;
	}
}

void ioExtendJump::CheckAirJump( ioBaseChar *pOwner )
{
	int iMaxCnt = m_vAirJumpTimeList.size();

	if( m_iCurAirJump < iMaxCnt )
	{
		DWORD dwCurAirJumpTime = 0;
		std::list< float >::iterator iter = m_vAirJumpTimeList.begin();

		for( int i=0; iter != m_vAirJumpTimeList.end(); i++ )
		{
			if( m_iCurAirJump == i )
			{
				dwCurAirJumpTime = (DWORD)(*iter);
				break;
			}

			++iter;
		}

		if( dwCurAirJumpTime > 0 && dwCurAirJumpTime < FRAMEGETTIME() )
		{
			if( m_fAirJumpPower >= 0.0f )
			{
				pOwner->SetAirJumpPower( m_fAirJumpPower*m_fAirJumpPowerRate );

				D3DXVECTOR3 vAttDir =  pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
				if( m_fAirForcePower < 0.0f )
					pOwner->SetForcePower( -vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				else
					pOwner->SetForcePower( vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
			}

			m_iCurAirJump++;
		}
	}
}

bool ioExtendJump::IsEnableJumpDashAndLandDash()
{
	return m_bUseJumpDashAndLandDash;
}

const ioUserKeyInput::DirKeyInput& ioExtendJump::GetLandDashKey() const 
{ 
	return m_JumpDashDownKey; 
}

float ioExtendJump::CheckKeyReserveTime( ioBaseChar *pOwner, int iAniID, float fTimeRate, DWORD dwPredelay )
{
	if( !pOwner )
		return 0.0f;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return 0.0f;

	DWORD dwCurTime = FRAMEGETTIME();

	float fEventTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fEventTime > 0 )
	{
		m_dwReserveEnableTime = dwCurTime + fEventTime + dwPredelay;
	}

	return fEventTime;
}

void ioExtendJump::CheckKeyReserve( ioBaseChar *pOwner )
{
}


bool ioExtendJump::CheckCanUseBuffAirDash()
{
	return true;
}


void ioExtendJump::SetNextJumpItemState( ioBaseChar *pOwner, ioBaseChar *pWounder, bool bDefense, ChangeNextItemType type, ioWeapon* pWeapon, float fDamage )
{

}

bool ioExtendJump::IsBuffFlyChange( ioBaseChar* pOwner )
{
	return true;
}

bool ioExtendJump::CheckUseExtendJump( ioBaseChar* pOwner )
{
	return m_bUseExtendJump;
}

void ioExtendJump::SetExtraJump( ioBaseChar *pOwner )
{
}

void ioExtendJump::ProcessExtraJump( ioBaseChar *pOwner )
{
}


void ioExtendJump::InitAniRotate()
{
	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();
}

void ioExtendJump::SetAniRotate( ioBaseChar *pChar, int iAniID, float fAniRate )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	InitAniRotate();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fAniRate;

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
}

void ioExtendJump::CheckAniRotate( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_vAniRotateInfoList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwAniRotateTime > 0 && m_dwAniRotateTime < dwCurTime )
	{
		if( m_fAniRotateAngle > 0.0f )
		{
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(m_fAniRotateAngle) );
			D3DXVECTOR3 vMoveDir = qtRot * pChar->GetMoveDir();

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

			pChar->SetTargetRotAndMoveDirChange( qtTargetRot );
		}

		m_iCurAniRotate++;

		int iSize = m_vAniRotateInfoList.size();
		if( COMPARE(m_iCurAniRotate, 0, iSize) )
		{
			m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
			m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
		}
		else
		{
			m_dwAniRotateTime = 0;
			m_fAniRotateAngle = 0.0f;

		}
	}
}

bool ioExtendJump::IsNoProcessMove()
{
	return false;
}

bool ioExtendJump::IsEnableLandAfterAttackByKeyDown()
{
	return m_bEnableLandAfterAttackByKeyDown;
}
