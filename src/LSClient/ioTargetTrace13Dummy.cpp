#include "StdAfx.h"
#include "ioTargetTrace13Dummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"

ioTargetTrace13Dummy::ioTargetTrace13Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_dwNextFireTime = FRAMEGETTIME();
	m_dwCreateFireTime = FRAMEGETTIME();
	m_nAttackAttributeIndex = 0;

	m_dwAttack1FiredTime = 0;
	m_dwAttack2FiredTime = 0;
	m_dwAttack3FiredTime = 0;
	
	m_szFindTarget.Clear();
}


ioTargetTrace13Dummy::~ioTargetTrace13Dummy(void)
{
}

void ioTargetTrace13Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	//char szKey[MAX_PATH];
	char szBuf[MAX_PATH];
	m_fAddOffSetHeight = rkLoader.LoadFloat_e( "add_offset_height", FLOAT100 );

	m_dwAttack1FireGapTime = (DWORD)rkLoader.LoadInt_e( "attack1_fire_gap_time", 30000 );
	m_dwAttack2FireGapTime = (DWORD)rkLoader.LoadInt_e( "attack2_fire_gap_time", 30000 );
	m_dwAttack3FireGapTime = (DWORD)rkLoader.LoadInt_e( "attack3_fire_gap_time", 30000 );

	m_fFind_1_Angle = rkLoader.LoadFloat_e( "attack1_find_angle", 0.f );
	m_fFind_1_Range_min = rkLoader.LoadFloat_e( "attack1_find_range_min", 0.f );
	m_fFind_1_Range_max = rkLoader.LoadFloat_e( "attack1_find_range_max", 0.f );
	m_fFind_1_Height_min = rkLoader.LoadFloat_e( "attack1_find_height_min", 0.f );
	m_fFind_1_Height_max = rkLoader.LoadFloat_e( "attack1_find_height_max", 0.f );

	m_fWeapon_1_LookOffSet = rkLoader.LoadFloat_e( "attack_1_weapon_look_offset", 0.f );
	m_fWeapon_1_height_OffSet = rkLoader.LoadFloat_e( "attack_1_weapon_height_offset", 0.f );
	m_fWeapon_1_Side_OffSet = rkLoader.LoadFloat_e( "attack_1_weapon_side_offset", 0.f );

	m_fTarget_1_ShootOffHeight = rkLoader.LoadFloat_e( "attack_1_target_shoot_off_height", 0.f );

	//////////////////////////////////////////////////////////////////////////

	m_fFind_2_Angle = rkLoader.LoadFloat_e( "attack2_find_angle", 0.f );
	m_fFind_2_Range_min = rkLoader.LoadFloat_e( "attack2_find_range_min", 0.f );
	m_fFind_2_Range_max = rkLoader.LoadFloat_e( "attack2_find_range_max", 0.f );
	m_fFind_2_Height_min = rkLoader.LoadFloat_e( "attack2_find_height_min", 0.f );
	m_fFind_2_Height_max = rkLoader.LoadFloat_e( "attack2_find_height_max", 0.f );

	m_fWeapon_2_LookOffSet = rkLoader.LoadFloat_e( "attack_2_weapon_look_offset", 0.f );
	m_fWeapon_2_height_OffSet = rkLoader.LoadFloat_e( "attack_2_weapon_height_offset", 0.f );
	m_fWeapon_2_Side_OffSet = rkLoader.LoadFloat_e( "attack_2_weapon_side_offset", 0.f );

	m_fTarget_2_ShootOffHeight = rkLoader.LoadFloat_e( "attack_2_target_shoot_off_height", 0.f );

	//////////////////////////////////////////////////////////////////////////

	m_fFind_3_Angle = rkLoader.LoadFloat_e( "attack3_find_angle", 0.f );
	m_fFind_3_Range_min = rkLoader.LoadFloat_e( "attack3_find_range_min", 0.f );
	m_fFind_3_Range_max = rkLoader.LoadFloat_e( "attack3_find_range_max", 0.f );
	m_fFind_3_Height_min = rkLoader.LoadFloat_e( "attack3_find_height_min", 0.f );
	m_fFind_3_Height_max = rkLoader.LoadFloat_e( "attack3_find_height_max", 0.f );

	m_fWeapon_3_LookOffSet = rkLoader.LoadFloat_e( "attack_3_weapon_look_offset", 0.f );
	m_fWeapon_3_height_OffSet = rkLoader.LoadFloat_e( "attack_3_weapon_height_offset", 0.f );
	m_fWeapon_3_Side_OffSet = rkLoader.LoadFloat_e( "attack_3_weapon_side_offset", 0.f );

	m_fTarget_3_ShootOffHeight = rkLoader.LoadFloat_e( "attack_3_target_shoot_off_height", 0.f );

	//////////////////////////////////////////////////////////////////////////

	//웨폰
	int nFire1Cnt = (DWORD)rkLoader.LoadInt_e( "attack_1_fire_cnt", 0 );
	int i = 0;
	for( i = 0; i < nFire1Cnt; i++ )
	{
		wsprintf_e( szBuf, "attack_1_%d_weapon", i+1 );

		DummyTT13AttackInfo info;
		info.m_dwWeaponIndex = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "attack_1_%d_fire_gap", i+1 );
		info.m_dwFireTime    = rkLoader.LoadInt( szBuf, 0 );

		m_vAttackFire1Info.push_back( info );
	}

	int nFire2Cnt = (DWORD)rkLoader.LoadInt_e( "attack_2_fire_cnt", 0 );
	for( i = 0; i < nFire2Cnt; i++ )
	{
		wsprintf_e( szBuf, "attack_2_%d_weapon", i+1 );

		DummyTT13AttackInfo info;
		info.m_dwWeaponIndex = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "attack_2_%d_fire_gap", i+1 );
		info.m_dwFireTime    = rkLoader.LoadInt( szBuf, 0 );

		m_vAttackFire2Info.push_back( info );
	}

	int nFire3Cnt = (DWORD)rkLoader.LoadInt_e( "attack_3_fire_cnt", 0 );
	for( i = 0; i < nFire3Cnt; i++ )
	{
		wsprintf_e( szBuf, "attack_3_%d_weapon", i+1 );

		DummyTT13AttackInfo info;
		info.m_dwWeaponIndex = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "attack_3_%d_fire_gap", i+1 );
		info.m_dwFireTime    = rkLoader.LoadInt( szBuf, 0 );

		m_vAttackFire3Info.push_back( info );
	}


	m_fFollowLength = rkLoader.LoadFloat( "follow_length", FLOAT1 );
}


void ioTargetTrace13Dummy::InitDummyCharInfo(	int iIndex, DWORD dwCreateGapTime, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale, float fStartAngle, bool bNetWork )
{
	__super::InitDummyCharInfo( iIndex, dwCreateGapTime, vPos, vScale, fStartAngle, bNetWork );

	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
	{
		D3DXVECTOR3 vWorldPos = pOwner->GetWorldPosition();
		vWorldPos.y += m_fAddOffSetHeight;

		SetWorldPosition( vWorldPos );
	}
}


void ioTargetTrace13Dummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
	{
		SetDieState( false );
		return;
	}

	switch( m_State )
	{
	case DCS_START:
		OnProcessStartState();
		break;
	case DCS_DELAY:
		//여기서 타겟을 찾고 공격을 한다.
		if( pOwner->IsNeedProcess() )
			ProcessDelayState( fTimerPerSec );

		break;
	}


	if( pOwner->IsNeedProcess() && m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
	{
		SendDieState( true );
		return;
	}

	ProcessEffectUpdate( fTimerPerSec );

	//ProcessPosition( pOwner );
	if( m_vAttackFireReserveInfo.size() == 0 )
		FollowOwner( pOwner, fTimerPerSec );

	ProcessWeaponCall( pOwner, fTimerPerSec );
}


void ioTargetTrace13Dummy::ProcessWeaponCall( ioBaseChar *pOwner, float fTimePerSec )
{
	int iWeaonBaseIndex = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	for( DummyTT13AttackInfoList::const_iterator iter = m_vAttackFireReserveInfo.begin(); iter != m_vAttackFireReserveInfo.end(); )
	{
		const DummyTT13AttackInfo& Info = *iter;
		if( Info.m_dwFireTime < dwCurTime )
		{
			if( CheckFindedTarget( pOwner, Info.m_fRangeMin, Info.m_fRangeMax, Info.m_fAngle, Info.m_fHeightMin, Info.m_fHeightMax, Info.m_stTargetName ) )
			{
				D3DXVECTOR3 vFireDir;
				ioBaseChar *pTarget = pOwner->GetBaseChar( Info.m_stTargetName );
				if( pTarget )
				{
					if( iWeaonBaseIndex == 0 )
					{
						pOwner->IncreaseWeaponIndexBase();
						iWeaonBaseIndex = pOwner->GetWeaponIndexBase();
					}
					else
						iWeaonBaseIndex++;

					vFireDir = pTarget->GetMidPositionByRate() - GetMidPositionByRate();
					D3DXVec3Normalize( &vFireDir, &vFireDir );
					ioWeapon* pWeapon = CreateWeapon( pOwner, vFireDir, iWeaonBaseIndex, Info );

					if( pWeapon	)
					{
						D3DXVECTOR3 vLookDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
						D3DXVec3Normalize( &vLookDir, &vLookDir );
						D3DXVECTOR3 vSide;
						D3DXVec3Cross( &vSide, &vLookDir, &ioMath::UNIT_Y );
						D3DXVec3Normalize( &vSide, &vSide );

						D3DXVECTOR3 vAddVector = (vLookDir * Info.m_fWeaponLookOffSet) + (vSide * Info.m_fWeaponSideOffSet) + D3DXVECTOR3( 0.f, Info.m_fWeaponHeighOffSet, 0.f );
						D3DXVECTOR3 vPos = GetWorldPosition() + vAddVector;
						pWeapon->SetPosition( vPos );

						vFireDir = (pTarget->GetMidPositionByRate() + D3DXVECTOR3( 0.f, Info.m_fTargetShootOffHeight, 0.f) ) - vPos;
						D3DXVec3Normalize( &vFireDir, &vFireDir );
						pWeapon->SetMoveDir( vFireDir );
					}
				}
			}

			iter = m_vAttackFireReserveInfo.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}


bool ioTargetTrace13Dummy::CheckFindedTarget( ioBaseChar *pOwner, float fMinLength, float fMaxLength, float fAngle, float fCheckHeightGapMin, float fCheckHeightGapMax, const ioHashString& stTargetName )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( stTargetName );
	if( !pTarget )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( fAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq_max = fMaxLength * fMaxLength;
	float fTargetRangeSq_min = fMinLength * fMinLength;

	if( !IsEnableTargetState( pTarget ) )
		return false;

	if( pTarget->IsCanNoWound() )
		return false;

	if( m_bCheckWounedState && !IsTargetWoundedState( pTarget ) )
		return false;

	auto SkipBuffIter = m_vSkipBuffList.begin();
	for ( ; SkipBuffIter != m_vSkipBuffList.end(); ++SkipBuffIter )
	{
		ioHashString& szBuffName = *SkipBuffIter;
		if ( pTarget->HasBuff( szBuffName ) )
		{				
			return false;
		}
	}

	D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
	vDiff = vCurPos - vTargetPos;
	float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

	// 각도 체크
	if( fCurAngle != 0.0f )
	{
		D3DXQUATERNION qtRot = GetWorldOrientation();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		D3DXVECTOR3 vDiff2 = vTargetPos - GetWorldPosition();
		vDiff2.y = 0.0f;
		D3DXVec3Normalize( &vDiff2, &vDiff2 );

		float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
		if( fCurValue < fHalfValue )
			return false;;
	}

	//높이
	float fHeight = pStage->GetMapHeightOnlyTerrain( vTargetPos.x, vTargetPos.z );
	float fTerrainHeightGap = fabs( pTarget->GetWorldPosition().y - fHeight );
	if( fTerrainHeightGap < fCheckHeightGapMin || fTerrainHeightGap > fCheckHeightGapMax)
		return false;

	//거리
	if( fDiffRangeSq < fTargetRangeSq_min || fDiffRangeSq > fTargetRangeSq_max )
		return false;

	return true;
}


void ioTargetTrace13Dummy::ProcessPosition( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	vPos.y += m_fAddOffSetHeight;

	SetWorldPosition( vPos );
	SetWorldOrientation( pOwner->GetTargetRot() );
}


ioWeapon* ioTargetTrace13Dummy::CreateWeapon( ioBaseChar *pOwner, const D3DXVECTOR3& vFireDir, DWORD dwWeaponBaseIndex, const DummyTT13AttackInfo& Info )
{
	if ( !pOwner )
		return NULL;

	FireTime kFireTime;
	kFireTime.dwStart				= FRAMEGETTIME();
	kFireTime.iAttributeIdx			= Info.m_dwWeaponIndex;
	kFireTime.iResistanceIdx		= 0;
	kFireTime.szWoundedAni			= "";
	kFireTime.dwWoundedAniDuration	= 0;
	kFireTime.bLoopWoundedAni		= false;
	kFireTime.dwWeaponIdx			= dwWeaponBaseIndex;
	kFireTime.eFireTimeType			= FTT_DUMMY_CHAR_ATTACK;

	return pOwner->ExtendFireExplicit( kFireTime, GetWorldPosition(), vFireDir, "" );
}


void ioTargetTrace13Dummy::OnProcessStartState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );

		ioBaseChar *pOwner = GetOwner();
		if ( !pOwner )
			return;
	}
}

void ioTargetTrace13Dummy::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;

	if( (m_dwAttack1FiredTime + m_dwAttack1FireGapTime) < dwCurTime )
	{
		if( FindTarget( pOwner, m_fFind_1_Range_min, m_fFind_1_Range_max, m_fFind_1_Angle, m_fFind_1_Height_min, m_fFind_1_Height_max ) )
		{
			DummyTT13AttackInfo info;
			for( int i = 0; i < (int)m_vAttackFire1Info.size(); i++ )
			{
				info = m_vAttackFire1Info[i];
				info.m_dwFireTime += dwCurTime;

				info.m_stTargetName = m_szFindTarget;
				info.m_fAngle = m_fFind_1_Angle;
				info.m_fHeightMax = m_fFind_1_Height_max;
				info.m_fHeightMin = m_fFind_1_Height_min;
				info.m_fRangeMax = m_fFind_1_Range_max;
				info.m_fRangeMin = m_fFind_1_Range_min;

				info.m_fWeaponLookOffSet = m_fWeapon_1_LookOffSet;
				info.m_fWeaponHeighOffSet = m_fWeapon_1_height_OffSet;
				info.m_fWeaponSideOffSet = m_fWeapon_1_Side_OffSet;

				info.m_fTargetShootOffHeight = m_fTarget_1_ShootOffHeight;

				m_vAttackFireReserveInfo.push_back( info );
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DUMMY_CHAR_STRIDER_SD_SYNC;
				kPacket	<< 1;
				kPacket << m_szFindTarget;
				kPacket << GetWorldPosition();
				kPacket << GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			m_dwAttack1FiredTime = dwCurTime;
		}
	}

	if( (m_dwAttack2FiredTime + m_dwAttack2FireGapTime) < dwCurTime )
	{
		if( FindTarget( pOwner, m_fFind_2_Range_min, m_fFind_2_Range_max, m_fFind_2_Angle, m_fFind_2_Height_min, m_fFind_2_Height_max ) )
		{
			DummyTT13AttackInfo info;
			for( int i = 0; i < (int)m_vAttackFire2Info.size(); i++ )
			{
				info = m_vAttackFire2Info[i];
				info.m_dwFireTime += dwCurTime;
				
				info.m_stTargetName = m_szFindTarget;
				info.m_fAngle = m_fFind_2_Angle;
				info.m_fHeightMax = m_fFind_2_Height_max;
				info.m_fHeightMin = m_fFind_2_Height_min;
				info.m_fRangeMax = m_fFind_2_Range_max;
				info.m_fRangeMin = m_fFind_2_Range_min;

				info.m_fWeaponLookOffSet = m_fWeapon_2_LookOffSet;
				info.m_fWeaponHeighOffSet = m_fWeapon_2_height_OffSet;
				info.m_fWeaponSideOffSet = m_fWeapon_2_Side_OffSet;

				info.m_fTargetShootOffHeight = m_fTarget_2_ShootOffHeight;

				m_vAttackFireReserveInfo.push_back( info );
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DUMMY_CHAR_STRIDER_SD_SYNC;
				kPacket	<< 2;
				kPacket << m_szFindTarget;
				kPacket << GetWorldPosition();
				kPacket << GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			m_dwAttack2FiredTime = dwCurTime;
		}	
	}

	if( (m_dwAttack3FiredTime + m_dwAttack3FireGapTime) < dwCurTime )
	{
		if( FindTarget( pOwner, m_fFind_3_Range_min, m_fFind_3_Range_max, m_fFind_3_Angle, m_fFind_3_Height_min, m_fFind_3_Height_max ) )
		{
			DummyTT13AttackInfo info;
			for( int i = 0; i < (int)m_vAttackFire3Info.size(); i++ )
			{
				info = m_vAttackFire3Info[i];
				info.m_dwFireTime += dwCurTime;
				
				info.m_stTargetName = m_szFindTarget;
				info.m_fAngle = m_fFind_3_Angle;
				info.m_fHeightMax = m_fFind_3_Height_max;
				info.m_fHeightMin = m_fFind_3_Height_min;
				info.m_fRangeMax = m_fFind_3_Range_max;
				info.m_fRangeMin = m_fFind_3_Range_min;

				info.m_fWeaponLookOffSet = m_fWeapon_3_LookOffSet;
				info.m_fWeaponHeighOffSet = m_fWeapon_3_height_OffSet;
				info.m_fWeaponSideOffSet = m_fWeapon_3_Side_OffSet;

				info.m_fTargetShootOffHeight = m_fTarget_3_ShootOffHeight;

				m_vAttackFireReserveInfo.push_back( info );
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DUMMY_CHAR_STRIDER_SD_SYNC;
				kPacket	<< 3;
				kPacket << m_szFindTarget;
				kPacket << GetWorldPosition();
				kPacket << GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			m_dwAttack3FiredTime = dwCurTime;
		}
	}
}


void ioTargetTrace13Dummy::SetOffsetPosAndDir( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	D3DXQUATERNION qtOwnerRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vOnwerPos = pOwner->GetWorldPosition();

	D3DXVECTOR3 vDir = qtOwnerRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	m_vOffsetPos = vOnwerPos;
	m_vOffsetPos.y += m_fAddOffSetHeight;

	m_vMoveDir = m_vOffsetPos - GetWorldPosition();
}


void ioTargetTrace13Dummy::FollowOwner( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	SetOffsetPosAndDir( pOwner );

	if( D3DXVec3Length( &m_vMoveDir ) > m_fFollowLength )
	{
		D3DXVECTOR3 vMoveDir = ioMath::VEC3_ZERO;
		D3DXVec3Normalize( &vMoveDir, &m_vMoveDir);

		Translate( vMoveDir * m_fMoveSpeed * fTimePerSec );

		D3DXVECTOR3 vRot = vMoveDir;
		D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vRot );
		SetWorldOrientation( qtRot );
	}
	else
	{
		D3DXQUATERNION qtOwnerRot = pOwner->GetWorldOrientation();
		SetWorldOrientation( qtOwnerRot );
	}
}


bool ioTargetTrace13Dummy::FindTarget( ioBaseChar *pOwner, float fMinLength, float fMaxLength, float fAngle, float fCheckHeightGapMin, float fCheckHeightGapMax )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return false;

	m_szFindTarget.Clear();

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( fAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq_max = fMaxLength * fMaxLength;
	float fTargetRangeSq_min = fMinLength * fMinLength;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) 
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) 
			continue;
		if( pTarget->GetCharName() == GetOwnerName() ) 
			continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) 
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();//pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		float fHeight = pStage->GetMapHeightOnlyTerrain( vTargetPos.x, vTargetPos.z );
		float fTerrainHeightGap = fabs( pTarget->GetWorldPosition().y - fHeight );
		if( fTerrainHeightGap < fCheckHeightGapMin || fTerrainHeightGap > fCheckHeightGapMax)
			continue;

		// 거리 체크
		if( fDiffRangeSq > fTargetRangeSq_min && fDiffRangeSq < fTargetRangeSq_max )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				m_szFindTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				m_szFindTarget = pTarget->GetCharName();
			}
		}
	}

	if( !m_szFindTarget.IsEmpty() )
		return true;

	return false;
}


void ioTargetTrace13Dummy::OnProcessTraceState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_TraceState )
	{
	case DCTS_NONE:
		break;
	case DCTS_SENSING:
		OnProcessTraceSensing( fTimePerSec );
		break;
	case DCTS_TRACING:
		OnProcessTraceTracing( fTimePerSec );
		break;
	case DCTS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		OnProcessTraceAttack( fTimePerSec, dwPreTime );
		break;
	case DCTS_TRACING_WAIT:
		ProcessTraceTracingWait( fTimePerSec, dwPreTime );
		break;
	case DCTS_TARGET_JUMP:
		OnProcessTraceJumpAttack( pStage, fTimePerSec );
		break;
	}

	//CheckTraceAttack();
}

void ioTargetTrace13Dummy::OnProcessTraceSensing( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_TargetTraceName.IsEmpty() )
		m_TargetTraceName.Clear();

	ioHashString szTarget;
	szTarget = GetSearchTarget( pStage, false );

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_TargetTraceName = szTarget;
			SetTracingState( pStage, true );
		}
	}
}


void ioTargetTrace13Dummy::CheckTarget( ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( m_TargetTraceName.IsEmpty() ) return;

	ioHashString szTarget = m_TargetTraceName;
	bool bLost = false;
	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );
	if( pTarget )
	{
		CharState eState = pTarget->GetState();
		switch( eState )
		{
		case CS_DIE:
		case CS_DROP_ZONE_DOWN:
		case CS_READY:
		case CS_VIEW:
		case CS_OBSERVER:
		case CS_LOADING:
		case CS_FISHING:
		case CS_STARTMOTION:
		case CS_ENDMOTION:
		case CS_WARP_STATE:
		case CS_EXCAVATING:
		case CS_ROULETTE:
			m_TargetTraceName.Clear();
			break;
		}

		if( !pTarget->IsNeedProcess() )
			return;

		if( !m_TargetTraceName.IsEmpty() && pTarget->HasHideBuff() )
			m_TargetTraceName.Clear();

		if( !m_TargetTraceName.IsEmpty() && pTarget->IsChatModeState( false ) )
			m_TargetTraceName.Clear();

		if( !m_TargetTraceName.IsEmpty() )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			vPrePos.y = 0.f;

			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate();
			vDiff.y = 0.f;
			vDiff = vDiff - vPrePos;

			//float fRangeSq = D3DXVec3Length( & vDiff );
			float fRange = D3DXVec3Length( & vDiff );
			if( fRange >= m_fTraceRange )
				m_TargetTraceName.Clear();
		}
	}
	else
	{
		m_TargetTraceName.Clear();
	}

	if( m_TargetTraceName.IsEmpty() )
		bLost = true;

	if( bLost )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DUMMY_CHAR_TRACE;
			kPacket << m_TargetTraceName;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTargetTrace13Dummy::OnProcessTraceTracing( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();
	ioBaseChar *pTarget = NULL;
	if( m_dwTracingEndTime > dwCurTime )
	{
		CheckTarget( pStage );
		FindTraceTarget( pStage );

		pTarget = pStage->GetBaseChar( m_TargetTraceName );		
		if( !IsTargetWoundedState( pTarget ) )
		{
			SetTraceWaitState();
			return;
		}

		if( pTarget )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			vPrePos.y = 0;
			D3DXVECTOR3 vDiff;
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			vTargetPos.y = 0.f;

			vDiff = vTargetPos - vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			if( fRangeSq < m_fTraceRange * m_fTraceRange )
			{
				D3DXVECTOR3 vTargetDir = vDiff;
				vTargetDir.y = 0;
				D3DXVec3Normalize( &vTargetDir, &vTargetDir );

				if( m_dwRotateSpeed > 0 )
				{
					D3DXQUATERNION	qtRot;
					ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );

					float fAngle = D3DXVec3Dot( &vTargetDir, &m_vTracingMoveDir );

					float fAngleGap = RADtoDEG( acosf(fAngle) );
					float fRate = (fTimePerSec*1000) / m_dwRotateSpeed;
					float fRotateAngle = 360.0f * fRate;

					if( fAngleGap < fRotateAngle )
					{
						m_vTracingMoveDir = vTargetDir;
					}
					else
					{
						D3DXQUATERNION qtRotate;
						ioMath::CalcDirectionQuaternion( qtRotate, -m_vTracingMoveDir );

						bool bLeftRot = false;
						if( D3DXVec3Dot( &vTargetDir, &m_vTracingMoveDir ) > -FLOAT1 )
						{
							float fCurYaw, fTargetYaw;
							fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
							fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

							fCurYaw = ioMath::ArrangeHead( fCurYaw );
							fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

							float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
							if( fRotAngle < 0.0f )
								bLeftRot = true;
						}

						if( bLeftRot )
							fRotateAngle *= -1;

						fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

						D3DXQUATERNION	qtNewRot;
						D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
						m_vTracingMoveDir = qtNewRot * m_vTracingMoveDir;
					}
				}
				else
				{
					m_vTracingMoveDir = vTargetDir;
				}
			}
		}
		else
		{
			m_TargetTraceName.Clear();
		}
	}
	else
	{
		SetTraceState( DCTS_SENSING );
		return;
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	//ProcessTraceMoving( pStage, fMoveDist, fTimePerSec );
	if( pTarget )
	{
		float fLength = 0.f;
		D3DXVECTOR3 vSrc = GetWorldPosition();
		D3DXVECTOR3 vDest = pTarget->GetWorldPosition();
		
		vSrc.y = 0.f;
		vDest.y = 0.f;

		D3DXVECTOR3 vDiffLength = vDest - vSrc;
		fLength = D3DXVec3Length( &vDiffLength );
		if( fLength < fMoveDist )
		{
			vDest.y = GetWorldPosition().y;
			SetWorldPosition( vDest );
		}
		else
			ProcessTraceMovingNoneTerrain( pStage, fMoveDist, fTimePerSec );
	}
	
}

void ioTargetTrace13Dummy::OnProcessTraceAttack( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	DWORD dwLastFireTime = dwPreTime;
	FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
	while( iter != m_vAttackFireTimeList.end() )
	{
		const FireTime &rkFire = *iter;

		DWORD dwFireTime = rkFire.dwStart;
		if( COMPARE( dwFireTime, dwPreTime, dwCurTime ) )
		{
			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				ioWeapon* pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
				ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
				if( pZone )
				{
					pZone->SetCollisionDir( vCurDir );
					pZone->SetZoneValue( true, false );
				}
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
	{
		m_LastAttacker = m_OwnerName;
		SetDelayState( false );
		return;
	}
}

void ioTargetTrace13Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	pStage->RemoveSummonDummyInfo( m_iCreateIndex, false );

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

ioHashString ioTargetTrace13Dummy::GetSearchTarget( ioPlayStage *pStage, bool bSecond )
{
	ioHashString szTarget;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return szTarget;
	if( !pStage )
		return szTarget;

	float fSensingRangeSq = 0.0f;
	if( bSecond )
		fSensingRangeSq = m_fSensingRange2 * m_fSensingRange2;
	else
		fSensingRangeSq = m_fSensingRange * m_fSensingRange;

	if( fSensingRangeSq == 0.0f )
		return szTarget;

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	vCurPos.y = 0.f;

	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;

		if( !pChar ) continue;
		if( pChar->GetTeam() == pOwner->GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( !IsEnableTargetState( pChar ) )
			continue;

		if( pChar->IsCanNoWound() )
			continue;

		if( m_bCheckWounedState && !IsTargetWoundedState( pChar ) )
			continue;

		bool bPass = false;

		auto SkipBuffIter = m_vSkipBuffList.begin();
		for ( ; SkipBuffIter != m_vSkipBuffList.end(); ++SkipBuffIter )
		{
			ioHashString& szBuffName = *SkipBuffIter;
			if ( pChar->HasBuff( szBuffName ) )
			{				
				bPass = true;
				break;
			}
		}

		if( bPass )
			continue;

		bPass = false;
		for( size_t i = 0; i < m_vTargetedChar.size(); i++ )
		{
			const ioHashString& stTargetedName = m_vTargetedChar[i];
			if( pChar->GetCharName() == stTargetedName )
			{
				bPass = true;
				break;
			}
		}

		if( bPass )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition();
		vTargetPos.y = 0.f;

		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( m_fSensingAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2 = vTargetPos - GetWorldPosition();
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fSensingRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
		}
	}

	return szTarget;
}


void ioTargetTrace13Dummy::OnProcessTraceJumpAttack( ioPlayStage *pStage, float fTimePerSec )
{
	if ( !pStage )
		return;
	m_fSensingRange = JUMP_RANGE * (float)m_fJumpAttackRate;
	ioHashString szTargetName = GetSearchTarget( pStage, false );
	FindAttackTheTarget( szTargetName );
	SetJumpAttackState();
}


void ioTargetTrace13Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage )
		return;	

	DummyTT13AttackInfo info;
	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtTargetRot;

	int ePacketType;
	rkPacket >> ePacketType;
	if( ePacketType != DUMMY_CHAR_STRIDER_SD_SYNC )
	{
		ioDummyChar::ApplyDummyCharSyncByType( ePacketType, rkPacket, pStage );
		return;
	}

	int eType;
	rkPacket >> eType;
	rkPacket >> m_szFindTarget;
	rkPacket >> vPos;
	rkPacket >> qtTargetRot;

	SetWorldPosition( vPos );
	SetWorldOrientation( qtTargetRot );

	switch( eType )
	{
	case 1:
		{
			for( int i = 0; i < (int)m_vAttackFire1Info.size(); i++ )
			{
				info = m_vAttackFire1Info[i];
				info.m_dwFireTime += dwCurTime;

				info.m_stTargetName = m_szFindTarget;
				info.m_fAngle = m_fFind_1_Angle;
				info.m_fHeightMax = m_fFind_1_Height_max;
				info.m_fHeightMin = m_fFind_1_Height_min;
				info.m_fRangeMax = m_fFind_1_Range_max;
				info.m_fRangeMin = m_fFind_1_Range_min;
				
				info.m_fWeaponLookOffSet = m_fWeapon_1_LookOffSet;
				info.m_fWeaponHeighOffSet = m_fWeapon_1_height_OffSet;
				info.m_fWeaponSideOffSet = m_fWeapon_1_Side_OffSet;

				m_vAttackFireReserveInfo.push_back( info );
			}
		}
		break;
	case 2:
		for( int i = 0; i < (int)m_vAttackFire2Info.size(); i++ )
		{
			info = m_vAttackFire2Info[i];
			info.m_dwFireTime += dwCurTime;

			info.m_stTargetName = m_szFindTarget;
			info.m_fAngle = m_fFind_2_Angle;
			info.m_fHeightMax = m_fFind_2_Height_max;
			info.m_fHeightMin = m_fFind_2_Height_min;
			info.m_fRangeMax = m_fFind_2_Range_max;
			info.m_fRangeMin = m_fFind_2_Range_min;

			info.m_fWeaponLookOffSet = m_fWeapon_2_LookOffSet;
			info.m_fWeaponHeighOffSet = m_fWeapon_2_height_OffSet;
			info.m_fWeaponSideOffSet = m_fWeapon_2_Side_OffSet;

			m_vAttackFireReserveInfo.push_back( info );
		}
		break;
	case 3:
		for( int i = 0; i < (int)m_vAttackFire3Info.size(); i++ )
		{
			info = m_vAttackFire3Info[i];
			info.m_dwFireTime += dwCurTime;

			info.m_stTargetName = m_szFindTarget;
			info.m_fAngle = m_fFind_3_Angle;
			info.m_fHeightMax = m_fFind_3_Height_max;
			info.m_fHeightMin = m_fFind_3_Height_min;
			info.m_fRangeMax = m_fFind_3_Range_max;
			info.m_fRangeMin = m_fFind_3_Range_min;

			info.m_fWeaponLookOffSet = m_fWeapon_3_LookOffSet;
			info.m_fWeaponHeighOffSet = m_fWeapon_3_height_OffSet;
			info.m_fWeaponSideOffSet = m_fWeapon_3_Side_OffSet;

			m_vAttackFireReserveInfo.push_back( info );
		}
		break;
	}

	/*if( eType == DUMMY_CHAR_STRIDER_SYNC )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> m_nAttackAttributeIndex;
		rkPacket >> vPos;
		SetWorldPosition( vPos );
		SetNextAttack();
	}
	else*/
		//ApplyDummyCharSyncByType( eType, rkPacket, pStage );

	/*if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket	<< 1;
		kPacket << m_szFindTarget;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}