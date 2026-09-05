#include "StdAfx.h"
#include "ioCountGatherAttackBuff.h"
#include "ioNpcChar.h"

#include "ioLimitActionBuff.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioCountGatherAttackBuff::ioCountGatherAttackBuff(void)
{
	ClearData();
}

ioCountGatherAttackBuff::ioCountGatherAttackBuff( const ioCountGatherAttackBuff &rhs ):
ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bDisableDropState( rhs.m_bDisableDropState ),
m_HitAttackWeaponList( rhs.m_HitAttackWeaponList ),
m_vHitAttackOffsetList( rhs.m_vHitAttackOffsetList ),
m_szHitAttackBuff( rhs.m_szHitAttackBuff ),
m_CountEndAttackList( rhs.m_CountEndAttackList ),
m_iMinEndAttackNumber( rhs.m_iMinEndAttackNumber ),
m_dwSwitchTime( rhs.m_dwSwitchTime )
{
	ClearData();
}

ioCountGatherAttackBuff::~ioCountGatherAttackBuff(void)
{
}

ioBuff* ioCountGatherAttackBuff::Clone()
{
	return new ioCountGatherAttackBuff( *this );
}

void ioCountGatherAttackBuff::ClearData()
{
	m_iCurActiveCount = 0;
	m_bAlreadyCreate = false;
	m_iItemSlotIndex = -1;
	m_dwEnableSwitchTime = 0;
	m_bReserveEndAttackFire = false;
	m_bSendedEndAttack = false;
}

void ioCountGatherAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropState = rkLoader.LoadBool_e( "disable_drop_state", false );
	m_dwSwitchTime = (DWORD)rkLoader.LoadInt_e( "switch_time", 0 );
	
	LoadHitWeaponList( rkLoader );

	LoadEndWeaponList( rkLoader );
	
}

void ioCountGatherAttackBuff::LoadHitWeaponList( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	int iAttackTypeCnt = rkLoader.LoadInt( "hit_attack_type_count", 0 );
	m_HitAttackWeaponList.clear();
	m_HitAttackWeaponList.reserve( iAttackTypeCnt );
	m_vHitAttackOffsetList.reserve( iAttackTypeCnt );
	m_vHitAttackOffsetList.clear();
	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponFireInfo kInfo;
		D3DXVECTOR3 vOffset;

		wsprintf_e( szKey, "hit_attack_type%d_time", i+1 );
		kInfo.m_dwWeaponFireTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "hit_attack_type%d_angle", i+1 );
		kInfo.m_fWeaponFireAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "hit_attack_type%d", i+1 );
		kInfo.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "hit_attack_type%d_resist", i+1 );
		kInfo.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "hit_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "hit_attack_type%d_wounded_time", i+1 );
		kInfo.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "hit_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "hit_attack_type%d_offset_x", i+1 );
		vOffset.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "hit_attack_type%d_offset_y", i+1 );
		vOffset.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "hit_attack_type%d_offset_z", i+1 );
		vOffset.z = rkLoader.LoadFloat( szKey, 0.0f );

		m_HitAttackWeaponList.push_back( kInfo );
		m_vHitAttackOffsetList.push_back( vOffset );
	}

	rkLoader.LoadString_e( "hit_attack_buff", "", szBuf, MAX_PATH );
	m_szHitAttackBuff = szBuf;
}

void ioCountGatherAttackBuff::LoadEndWeaponList( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_iMinEndAttackNumber = rkLoader.LoadInt( "min_end_attack_number", 0 );
	for ( int i=0 ; i<m_iMaxActiveCount ; ++i )
	{
		wsprintf_e( szKey, "end_attack%d_start_count", i+1 );
		int iCount = rkLoader.LoadInt( szKey, -1 );

		if( iCount == -1 )
			continue;

		CountEndAttack EndAttack;
		EndAttack.m_iCount = iCount;

		wsprintf_e( szKey, "end_attack%d_type_count", i+1 );
		int iEndAttackTypeCnt = rkLoader.LoadInt( szKey, 0 );
		for( int j=0 ; j<iEndAttackTypeCnt ; j++ )
		{
			WeaponInfo kInfo;

			wsprintf_e( szKey, "end_attack%d_type%d", i+1, j+1 );
			kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "end_attack%d_type%d_resist", i+1, j+1 );
			kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "end_attack%d_type%d_wounded", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szKey, "end_attack%d_type%d_wounded_time", i+1, j+1 );
			kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "end_attack%d_type%d_wounded_loop_ani", i+1, j+1 );
			kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			EndAttack.m_EndAttackList.push_back( kInfo );
		}
		m_CountEndAttackList.push_back( EndAttack );
	}
	std::sort( m_CountEndAttackList.begin(), m_CountEndAttackList.end(), CountEndAttackSort() );
}

void ioCountGatherAttackBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	
	ClearData();

	if ( m_dwSwitchTime > 0 )
		m_dwEnableSwitchTime = m_dwBuffStartTime + m_dwSwitchTime;

	if ( GetOwnerItem() )
	{
		int iSlot = GetOwnerItem()->GetType() - 1;
		const CHARACTER& rkChar = g_MyInfo.GetCharacter();
		m_iItemSlotIndex =rkChar.m_extra_item[iSlot];
	}
}


bool ioCountGatherAttackBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	ClearData();

	if ( m_dwSwitchTime > 0 )
		m_dwEnableSwitchTime = m_dwBuffStartTime + m_dwSwitchTime;

	if ( GetOwnerItem() )
	{
		int iSlot = GetOwnerItem()->GetType() - 1;
		const CHARACTER& rkChar = g_MyInfo.GetCharacter();
		m_iItemSlotIndex =rkChar.m_extra_item[iSlot];
	}

	return true;
}

void ioCountGatherAttackBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		{
			if ( m_pOwner->IsNeedProcess() )
			{
				DWORD dwCurTime = FRAMEGETTIME();
				float fCurTicGauge = GetCurTicGauge();
				int iTickGap = GetCurTicGap();
				int iCurTicTime = GetCurTicTime();
				if( iTickGap >= iCurTicTime )
				{
					m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

					if( fCurTicGauge != 0.0f )
					{
						if( m_pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, fCurTicGauge ) <= 0.0f )
						{
							SetReserveEndAttack();
							break;
						}
					}
				}
			}

			CheckCallingProcess();
		}
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				break;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				SetReserveEndBuff();
				break;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	CheckEnableKeepBuff( m_pOwner );

	//Last Attack
	if ( m_pOwner->IsNeedProcess() )
	{
		if ( IsReserveEndattack() )
			CreateEndWeapon( m_pOwner );

		if ( m_bWaitSelfRemove && !IsLive() && !m_bSendedEndAttack )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << m_pOwner ->GetCharName();
			kPacket << GetName();
			kPacket << static_cast<BYTE>( BSS_END );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	
}

bool ioCountGatherAttackBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner )
		return false;

	if( m_bDisableDownState )
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

	if ( m_bDisableDropState )
	{
		CharState eState = m_pOwner->GetState();
		if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
	}

	if ( IsLive() )
		return false;
	return true;
}

void ioCountGatherAttackBuff::IncreaseHitCount()
{
	m_iCurActiveCount++;
	m_iCurActiveCount = min( m_iMaxActiveCount, m_iCurActiveCount );
}

void ioCountGatherAttackBuff::CheckHitCount( ioBaseChar* pAttacker, ioBaseChar* pWounder )
{
	if ( !IsLive() )
		return;
	if ( !pAttacker || !pWounder )
		return;

	ioPlayStage *pStage = pWounder->GetCreator();
	if ( !pStage )
		return;

	if ( EnableHitAttack( pAttacker, pWounder ) )
		CreateHitAttackWeapon( pAttacker, pWounder, pStage );
	else
		SendIncreaseHitCount( pAttacker, pWounder, pStage );
}

void ioCountGatherAttackBuff::SendIncreaseHitCount( ioBaseChar* pAttacker, ioBaseChar* pWounder, ioPlayStage *pStage )
{
	if ( !pAttacker || !pWounder || !pStage )
		return;

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if ( pWounder->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pAttacker->GetCharName();
			kPacket << GetName();
			kPacket << static_cast<BYTE>(BSS_INCREASE_HIT_COUNT);
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
		IncreaseHitCount();
}

void ioCountGatherAttackBuff::CreateHitAttackWeapon( ioBaseChar* pAttacker, ioBaseChar* pWounder, ioPlayStage *pStage )
{
	if ( !pAttacker || !pWounder || !pStage )
		return;

	ioHashString szItemName;
	if ( GetOwnerItem() )
		szItemName = GetOwnerItem()->GetName();

	m_dwWeaponBaseIndex = pAttacker->IncreaseWeaponIndexBase();
	//적 위치+ weapon offset 에서 웨폰 생성
	D3DXVECTOR3 vCurPos = pWounder->GetMidPositionByRate();
	//방향은 현재 공격자 방향
	D3DXQUATERNION qtCurRot = pAttacker->GetWorldOrientation();
	for ( int i=0; i<(int)m_HitAttackWeaponList.size() ; ++i )
	{
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME() + m_HitAttackWeaponList[i].m_dwWeaponFireTime;
		kFireTime.iAttributeIdx = m_HitAttackWeaponList[i].m_WeaponInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = m_HitAttackWeaponList[i].m_WeaponInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = m_HitAttackWeaponList[i].m_WeaponInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = m_HitAttackWeaponList[i].m_WeaponInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = m_HitAttackWeaponList[i].m_WeaponInfo.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex++;
		kFireTime.eFireTimeType = FTT_BUFF_ATTACK2;

		float fAngle = m_HitAttackWeaponList[i].m_fWeaponFireAngle;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationAxis( &qtAngle, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );

		D3DXVECTOR3 vCurDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vCurDir = qtAngle * vCurDir;
		D3DXVec3Normalize( &vCurDir, &vCurDir );
		
		D3DXQUATERNION qtMultiply;
		D3DXQuaternionMultiply( &qtMultiply, &qtCurRot, &qtAngle );
		D3DXVECTOR3 vFirePos = vCurPos;
		vFirePos += qtMultiply * m_vHitAttackOffsetList[i];
		pAttacker->ExtendFireExplicit( kFireTime, vFirePos, vCurDir, szItemName );
	}

	if ( !m_szHitAttackBuff.IsEmpty() )
		pWounder->AddNewBuff( m_szHitAttackBuff, pAttacker->GetCharName(), szItemName, NULL );

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if ( pWounder->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pAttacker->GetCharName();
			kPacket << GetName();
			kPacket << static_cast<BYTE>(BSS_CREAT_HIT_ATTACK_WEAPON);
			kPacket << pWounder->GetCharName();
			kPacket << vCurPos;
			kPacket << qtCurRot;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		if ( ToNpcChar( pWounder ) )
			IncreaseHitCount();
	}
	else
		IncreaseHitCount();
}

bool ioCountGatherAttackBuff::EnableHitAttack( ioBaseChar* pAttacker, ioBaseChar* pWounder )
{
	if ( !pAttacker || !pWounder )
		return false;

	if ( pWounder->GetState() == CS_BLOW_WOUNDED && !pWounder->IsFloatingState() )
		return false;

	if ( pWounder->HasBuff( m_szHitAttackBuff ) )
		return false;

	return true;
}

void ioCountGatherAttackBuff::CreateEndWeapon( ioBaseChar* pOwner )
{
	if ( m_bAlreadyCreate )
		return;

	m_bAlreadyCreate = true;

	if ( m_iMinEndAttackNumber >= m_iCurActiveCount )
		return;
	
	ioHashString szItemName;
	if ( GetOwnerItem() )
		szItemName = GetOwnerItem()->GetName();

	int iIndex = -1;
	for ( int i=0 ; i<(int)m_CountEndAttackList.size() ; ++i )
	{
		if ( m_CountEndAttackList[i].m_iCount == m_iCurActiveCount )
		{
			iIndex = i;
			break;
		}
		else if( m_CountEndAttackList[i].m_iCount > m_iCurActiveCount )
		{
			iIndex = max( i-1, 0 );
			break;
		}
	}

	if ( iIndex != -1 )
	{
		m_bSendedEndAttack = true;
		WeaponInfoList	  EndAttackList = m_CountEndAttackList[iIndex].m_EndAttackList;
		m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

		D3DXVECTOR3 vCurDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vCurDir, &vCurDir );
		D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();

		for ( int i=0; i<(int)EndAttackList.size() ; ++i )
		{
			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();
			kFireTime.iAttributeIdx = EndAttackList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = EndAttackList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni  = EndAttackList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = EndAttackList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = EndAttackList[i].m_bWoundedLoopAni;
			kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex++;
			kFireTime.eFireTimeType = FTT_BUFF_ATTACK2;

			
			pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, szItemName );
		}

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << static_cast<BYTE>(BSS_END_ATTACK);
			kPacket << m_iCurActiveCount;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioCountGatherAttackBuff::CheckEnableKeepBuff( ioBaseChar* pOwner )
{
	if( CheckEnableDelayState() )
		return;

	if( CheckOwnerStateCheck() )
		return;

	if( !IsRightOwnerItem() )
	{
		SetReserveEndBuff();
		return;
	}

	if ( m_bWaitSelfRemove )
	{
		//Check Key Input
		if( pOwner->IsNeedProcess() && m_dwEnableSwitchTime > 0 && m_dwEnableSwitchTime < FRAMEGETTIME() )
		{
			int iSlot = GetOwnerItem()->GetType() - 1;
			if( m_pOwner->GetSkillKeyInput() == iSlot )
			{
				SetReserveEndAttack();
				return;
			}
		}

		//Check Item Equip
		if ( m_iItemSlotIndex != - 1 && GetOwnerItem() )
		{
			int iSlot = GetOwnerItem()->GetType() - 1;
			const CHARACTER& rkChar = g_MyInfo.GetCharacter();
			int iSlotIndex = -1;
			iSlotIndex =rkChar.m_extra_item[iSlot];

			if ( iSlotIndex != m_iItemSlotIndex )
			{
				SetReserveEndBuff();
				return;
			}
		}
	}
}

void ioCountGatherAttackBuff::SetReserveEndAttack()
{
	SetReserveEndBuff();
	m_bReserveEndAttackFire = true;
}

void ioCountGatherAttackBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	BYTE btState;
	rkPacket >> btState;

	switch( btState )
	{
	case BSS_INCREASE_HIT_COUNT:
		IncreaseHitCount();
		break;
	case BSS_CREAT_HIT_ATTACK_WEAPON:
		{
			ioHashString szItemName;
			if ( GetOwnerItem() )
				szItemName = GetOwnerItem()->GetName();

			m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
			ioHashString szWounder;
			rkPacket >> szWounder;
			ioBaseChar *pWounder = pOwner->GetBaseChar( szWounder );
			if ( !pWounder )
				return;

			D3DXVECTOR3 vCurPos;
			D3DXQUATERNION qtCurRot;
			rkPacket >> vCurPos;
			rkPacket >> qtCurRot;
			for ( int i=0; i<(int)m_HitAttackWeaponList.size() ; ++i )
			{
				FireTime kFireTime;
				kFireTime.dwStart = FRAMEGETTIME() + m_HitAttackWeaponList[i].m_dwWeaponFireTime;
				kFireTime.iAttributeIdx = m_HitAttackWeaponList[i].m_WeaponInfo.m_iWeaponIdx;
				kFireTime.iResistanceIdx = m_HitAttackWeaponList[i].m_WeaponInfo.m_iResistanceIdx;
				kFireTime.szWoundedAni  = m_HitAttackWeaponList[i].m_WeaponInfo.m_WoundedAnimation;
				kFireTime.dwWoundedAniDuration = m_HitAttackWeaponList[i].m_WeaponInfo.m_dwWoundedDuration;
				kFireTime.bLoopWoundedAni = m_HitAttackWeaponList[i].m_WeaponInfo.m_bWoundedLoopAni;
				kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex++;
				kFireTime.eFireTimeType = FTT_BUFF_ATTACK2;

				float fAngle = m_HitAttackWeaponList[i].m_fWeaponFireAngle;
				D3DXQUATERNION qtAngle;
				D3DXQuaternionRotationAxis( &qtAngle, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );

				D3DXVECTOR3 vCurDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				vCurDir = qtAngle * vCurDir;
				D3DXVec3Normalize( &vCurDir, &vCurDir );

				D3DXQUATERNION qtMultiply;
				D3DXQuaternionMultiply( &qtMultiply, &qtCurRot, &qtAngle );
				D3DXVECTOR3 vFirePos = vCurPos;
				vFirePos += qtMultiply * m_vHitAttackOffsetList[i];
				pOwner->ExtendFireExplicit( kFireTime, vFirePos, vCurDir, szItemName );
			}
			if ( !m_szHitAttackBuff.IsEmpty() )
				pWounder->AddNewBuff( m_szHitAttackBuff, pOwner->GetCharName(), szItemName, NULL );

			IncreaseHitCount();
		}
		break;
	case BSS_END_ATTACK:
		{
			SetReserveEndBuff();
			rkPacket >> m_iCurActiveCount;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			CreateEndWeapon( pOwner );
		}
		break;
	case BSS_END:
		SetReserveEndBuff();
		break;
	}
}

void ioCountGatherAttackBuff::EndBuff()
{
	ioBuff::EndBuff();
}
