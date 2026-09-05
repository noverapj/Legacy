#include "StdAfx.h"
#include "ioTargetCreateWeaponBuff4.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioTargetCreateWeaponBuff4::ioTargetCreateWeaponBuff4(void)
{
	Init();
}

ioTargetCreateWeaponBuff4::ioTargetCreateWeaponBuff4( const ioTargetCreateWeaponBuff4 &rhs ):
ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bDisableDropState( rhs.m_bDisableDropState ),
m_vSkipBuffList( rhs.m_vSkipBuffList),
m_fTargetRange( rhs.m_fTargetRange ),
m_fTargetAngle( rhs.m_fTargetAngle ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_CheckWoundedTarget( rhs.m_CheckWoundedTarget ),
m_iMaxAttackCount( rhs.m_iMaxAttackCount ),
m_iMaxTargetCount( rhs.m_iMaxTargetCount ),
m_dwTargetAttackTicTime( rhs.m_dwTargetAttackTicTime ),
m_TargetAttackWeapon( rhs.m_TargetAttackWeapon ),
m_TargetAttackOffset( rhs.m_TargetAttackOffset ),
m_dwDelayDuration( rhs.m_dwDelayDuration )
{
	Init();
}

ioTargetCreateWeaponBuff4::~ioTargetCreateWeaponBuff4(void)
{
}

void ioTargetCreateWeaponBuff4::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropState = rkLoader.LoadBool_e( "disable_drop_state", false );

	int iCnt = rkLoader.LoadInt_e( "skip_buff_cnt", 0 );
	for (int i=0; i<iCnt; ++i)
	{
		ioHashString szBuffName;
		wsprintf_e( szKey, "skip_buff%d_name",i+1);
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szBuffName = szBuf;
		if ( !szBuffName.IsEmpty() )
			m_vSkipBuffList.push_back( szBuffName );
		else
			LOG.PrintTimeAndLog( 0, "%s - %s is no name", __FUNCTION__, szKey );
	}

	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_CheckWoundedTarget = rkLoader.LoadBool_e( "check_wounded_target", false );
	m_iMaxAttackCount = rkLoader.LoadInt_e( "max_attack_count", 0 );
	m_iMaxTargetCount = rkLoader.LoadInt_e( "max_target_count", 0 );
	m_dwTargetAttackTicTime =(DWORD) rkLoader.LoadInt_e( "target_attack_tic_time", 0 );
	m_dwDelayDuration = (DWORD)rkLoader.LoadInt_e( "delay_time", 0 );
	LoadWeaponInfo( "target_attack", m_TargetAttackWeapon, rkLoader );
	m_TargetAttackOffset.x = rkLoader.LoadFloat_e( "target_attack_type_offset_x", 0.0f );
	m_TargetAttackOffset.y = rkLoader.LoadFloat_e( "target_attack_type_offset_y", 0.0f );
	m_TargetAttackOffset.z = rkLoader.LoadFloat_e( "target_attack_type_offset_z", 0.0f );

	
}

ioBuff* ioTargetCreateWeaponBuff4::Clone()
{
	return new ioTargetCreateWeaponBuff4( *this );
}

void ioTargetCreateWeaponBuff4::Init()
{
	m_BuffState = TBS_NONE;
	m_iCurAttackCount = 0;
	m_dwDelayEndTime = 0;
	m_dwTargetAttackStartTime = 0;
}

void ioTargetCreateWeaponBuff4::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	Init();
	SetDelayState();
}

void ioTargetCreateWeaponBuff4::ProcessBuff( float fTimePerSec )
{
	if ( CheckOwnerState() || CheckCreatorState() )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	switch( m_BuffState )
	{
	case TBS_DELAY:
		if ( m_dwDelayEndTime > 0 && m_dwDelayEndTime < FRAMEGETTIME() )
		{
			m_dwDelayEndTime = 0;
			SetCreateTicWeaponState();
			return;
		}
		break;
	case TBS_CREATE_TIC_WEAPON:
		{
			if ( m_iCurAttackCount >= m_iMaxAttackCount )
			{
				SetReserveEndBuff();
				return;
			}

			if ( m_pOwner && m_pOwner->IsNeedProcess() )
			{
				if ( m_dwTargetAttackStartTime && m_dwTargetAttackStartTime < FRAMEGETTIME() )
				{
					m_dwTargetAttackStartTime = FRAMEGETTIME() + m_dwTargetAttackTicTime;
					CreateTargetAttackWeapon();
				}
			}
		}
	}
}

void ioTargetCreateWeaponBuff4::EndBuff()
{
	ioBuff::EndBuff();
}

bool ioTargetCreateWeaponBuff4::CheckOwnerState()
{
	if( !m_pOwner )
		return true;

	if( !m_bDisableDownState )
	{
		CharState eState = m_pOwner->GetState();
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}

	if ( !m_bDisableDropState )
	{
		CharState eState = m_pOwner->GetState();
		if( eState == CS_DROP_ZONE_DOWN )
		{
			SetReserveEndBuff();
			return true;
		}
	}

	if ( IsLive() )
		return false;
	return true;
}

bool ioTargetCreateWeaponBuff4::CheckCreatorState()
{
	if( !m_pOwner )
		return true;

	//생성자와 걸린사람이 같다면 체크할필요 없으므로
	if ( GetCreatorName() == m_pOwner->GetCharName() )
		return false;

	ioBaseChar *pCreater = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreater )
		return true;

	return false;
}

void ioTargetCreateWeaponBuff4::SetDelayState()
{
	m_BuffState = TBS_DELAY;
	m_dwDelayEndTime = FRAMEGETTIME() + m_dwDelayDuration;
}

void ioTargetCreateWeaponBuff4::SetCreateTicWeaponState()
{
	m_BuffState = TBS_CREATE_TIC_WEAPON;
	if ( m_dwTargetAttackTicTime > 0 )
		m_dwTargetAttackStartTime = FRAMEGETTIME() + m_dwTargetAttackTicTime;
	
}

void ioTargetCreateWeaponBuff4::CreateTargetAttackWeapon()
{
	if ( !m_pOwner )
		return;
	ioBaseChar *pCreater = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreater )
		return;

	FindTargetList();
	for ( int i=0 ; i<(int)m_vTargetNameList.size() ; ++i )
	{
		if ( i >= m_iMaxTargetCount )
			break;

		CreateWeapon( m_pOwner, pCreater, m_vTargetNameList[i] );
	}

	int iTargetSize = m_vTargetNameList.size();
	if ( iTargetSize > 0 && m_pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << TBS_WEAPON_FIRE;
		kPacket << iTargetSize;
		for ( int i=0; i<iTargetSize ; ++i )
			kPacket << m_vTargetNameList[i];
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iCurAttackCount++;
}

void ioTargetCreateWeaponBuff4::FindTargetList()
{
	m_vTargetNameList.clear();
	if ( !m_pOwner )
		return;
	ioPlayStage *pStage = m_pOwner->GetCreator();
	if ( !pStage )
		return;

	ioBaseChar *pCreater = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreater )
		return;

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = m_pOwner->GetMidPositionByRate();
	D3DXQUATERNION qtRot = m_pOwner->GetWorldOrientation();

	float fHalfAngle = m_fTargetAngle * FLOAT05;
	fHalfAngle = ioMath::ArrangeHead(fHalfAngle);
	float fHalfValue = cosf( DEGtoRAD(fHalfAngle) );

	float fSensingRangeSq = m_fTargetRange * m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;

		if( !pChar ) 
			continue;

		if ( CheckTargetState( pCreater, pChar ) )
			continue;

		if ( CheckSkipBuff( pChar ) )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetMidPositionByRate();
		// 각도 체크
		if( m_fTargetAngle != 0.0f )
		{
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDiff2 = vTargetPos - vCurPos;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크

		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );
		if( fDiffRangeSq >= fSensingRangeSq )
			continue;

		m_vTargetNameList.push_back( pChar->GetCharName() );
	}
}

bool ioTargetCreateWeaponBuff4::CheckTargetState( ioBaseChar *pChar, ioBaseChar *pTarget )
{
	if ( !pChar || !pTarget || !m_pOwner )
		return true;

	if( pTarget->GetTeam() == pChar->GetTeam() ) 
		return true;

	//Creator의 이름 비교
	if( pTarget->GetCharName() == pChar->GetCharName() ) 
		return true;

	//Owner의 이름 비교
	if( pTarget->GetCharName() == m_pOwner->GetCharName() ) 
		return true;

	if( pTarget->IsSystemState() )
		return true;

	if( !pTarget->IsEnableTarget() ) 	
		return true;

	// 쓰러짐 관련 체크
	if( m_CheckWoundedTarget && !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
		return true;

	if( pTarget->IsCanNoWound() )
		return true;
	
	return false;
}

bool ioTargetCreateWeaponBuff4::CheckSkipBuff( ioBaseChar *pTarget )
{
	if ( !pTarget )
		return true;

	ioBaseChar *pCreater = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreater )
		return true;

	for ( int i=0; i<(int)m_vSkipBuffList.size() ; ++i  )
	{
		const ioHashString& szBuffName = m_vSkipBuffList[i];
		if ( pTarget->HasBuff( m_CreateChar, szBuffName ) )
			return true;
	}

	return false;
}

void ioTargetCreateWeaponBuff4::CreateWeapon( ioBaseChar *pChar, ioBaseChar *pCreator, ioHashString szTargetName )
{
	if ( !pChar || !pCreator || szTargetName.IsEmpty() )
		return;

	if ( m_TargetAttackWeapon.m_iWeaponIdx == NO_WEAPON_ATTRIBUTE )
		return;

	ioBaseChar* pTarget = pChar->GetBaseChar( szTargetName );
	if ( !pTarget )
		return;

	DWORD dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_TargetAttackWeapon.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_TargetAttackWeapon.m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_TargetAttackWeapon.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_TargetAttackWeapon.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_TargetAttackWeapon.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
	kFireTime.eFireTimeType = FTT_BUFF_ATTACK2;
	D3DXVECTOR3 vPosOffset = pChar->GetWorldOrientation() * m_TargetAttackOffset;
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + vPosOffset;

	D3DXVECTOR3 vCurDir = pTarget->GetMidPositionByRate() - vPos;
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	//Creator가 생성
	pCreator->ExtendFireExplicit( kFireTime, vPos, vCurDir, "" );
}

void ioTargetCreateWeaponBuff4::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case TBS_WEAPON_FIRE:
		{
			int iTargetSize;
			rkPacket >> iTargetSize;
			for ( int i=0; i<iTargetSize ; ++i )
			{
				if ( i >= m_iMaxTargetCount )
					break;
				ioHashString szTargetName;
				rkPacket >> szTargetName;
				CreateWeapon( m_pOwner, pCreator, szTargetName );
			}
			m_iCurAttackCount++;
		}
		break;
	}
}
