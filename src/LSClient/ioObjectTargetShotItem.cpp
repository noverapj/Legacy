#include "StdAfx.h"
#include "ioObjectTargetShotItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioObjectTargetShotItem::ioObjectTargetShotItem(void)
{
}

ioObjectTargetShotItem::ioObjectTargetShotItem( const ioObjectTargetShotItem &rhs )
: ioObjectItem( rhs ),
  m_NormalAttribute( rhs.m_NormalAttribute ),
  m_ExtendAttribute( rhs.m_ExtendAttribute ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetAngle( rhs.m_fTargetAngle ),
  m_fTargetRangeJump( rhs.m_fTargetRangeJump ),
  m_fTargetAngleJump( rhs.m_fTargetAngleJump ),
  m_iMaxTargetCount( rhs.m_iMaxTargetCount ),
  m_TargetWoundType( rhs.m_TargetWoundType ),
  m_TargetWoundTypeJump( rhs.m_TargetWoundTypeJump ),
  m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
  m_dwProtectTime( rhs.m_dwProtectTime )
{
	m_dwAttackStartTime = 0;
}

ioObjectTargetShotItem::~ioObjectTargetShotItem(void)
{
}

void ioObjectTargetShotItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	// attack
	LoadAttackAttribute_e( "normal_attack", m_NormalAttribute, rkLoader );

	// extend attack
	LoadAttackAttribute_e( "extend_attack", m_ExtendAttribute, rkLoader );

	m_iMaxTargetCount = rkLoader.LoadInt_e( "max_target_count", 0 );

	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRangeJump = rkLoader.LoadFloat_e( "target_range_jump", 0.0f );
	m_fTargetAngleJump = rkLoader.LoadFloat_e( "target_angle_jump", 0.0f );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", 0 );
	m_TargetWoundTypeJump = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type_jump", 0 );

	m_dwEnableKeyTime = rkLoader.LoadInt_e( "key_enable_time", 0 );

	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );
}

ioItem* ioObjectTargetShotItem::Clone()
{
	return new ioObjectTargetShotItem( *this );
}

void ioObjectTargetShotItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );
	
	m_AttackState = AS_NORMAL;
	m_dwAttackStartTime = 0;
	m_CurTargetName.Clear();
	m_dwEquipTime = FRAMEGETTIME();
}

ioObjectItem::ObjectSubType ioObjectTargetShotItem::GetObjectSubType() const
{
	return OST_TARGET_SHOT;
}

void ioObjectTargetShotItem::CheckTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	BaseCharDistanceSqList vTargetList;
	vTargetList.clear();

	int iTargetCount = 0;

	float fPrevDistSq = 0.0f;
	if( pOwner->GetState() == CS_JUMP )
		fPrevDistSq = m_fTargetRangeJump * m_fTargetRangeJump;
	else
		fPrevDistSq = m_fTargetRange * m_fTargetRange;

	ioBaseChar *pTargetChar = pOwner;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )	continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		if( pOwner->GetState() == CS_JUMP && !pTarget->CheckTargetWoundType( m_TargetWoundTypeJump ) )
			continue;

		if( pOwner->GetState() != CS_JUMP && !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )
		{
			D3DXVECTOR3 vDir = vDiff;
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSearchDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSearchDir, &vSearchDir );

			float fHalfCosine = 0.0f;

			if( pOwner->GetState() == CS_JUMP )
				fHalfCosine = cosf( DEGtoRAD( m_fTargetAngleJump ) * FLOAT05 );
			else
				fHalfCosine = cosf( DEGtoRAD( m_fTargetAngle ) * FLOAT05 );

			if( D3DXVec3Dot( &vSearchDir, &vDir ) >= fHalfCosine )
			{
				BaseCharDistanceSq kInfo;
				kInfo.m_Name = pTarget->GetCharName();
				kInfo.m_fDistanceSq = fDistSq;
				vTargetList.push_back( kInfo );

				iTargetCount++;
			}
		}
	}

	if( !vTargetList.empty() )
		std::sort( vTargetList.begin(), vTargetList.end(), BaseCharDistanceSqSort2() );

	int iSize = vTargetList.size();
	
	if ( iSize >= m_iMaxTargetCount )
	{
		ClearAllTarget( pOwner );
		m_vTargetInfoList.reserve( m_iMaxTargetCount );
		for( int i = 0; i < m_iMaxTargetCount; ++i )
		{
			m_vTargetInfoList.push_back( vTargetList[i] );			
		}
	}
	else
	{
		// 1. 중복된 대상은 미리 삭제한다.
		auto new_it = vTargetList.begin();
		for( ; new_it != vTargetList.end(); ++new_it )
		{
			auto it = m_vTargetInfoList.begin();
			while( it != m_vTargetInfoList.end() )
			{
				if( (*new_it).m_Name == (*it).m_Name )
				{
					it = m_vTargetInfoList.erase( it );
				}
				else
				{
					++it;
				}
			}
		}

		// 2. 추가로 들어올 대상을 위한 자리를 확보한다.
		int iOverCount = m_vTargetInfoList.size() + vTargetList.size();
		if( iOverCount > m_iMaxTargetCount )
		{
			int iBlank = iOverCount - m_iMaxTargetCount;
			auto it = m_vTargetInfoList.begin();
			while( iBlank > 0 )
			{
				it = m_vTargetInfoList.erase( it );
				iBlank--;
			}
		}
		
		// 3. 추가로 들어올 대상을 m_vTargetInfo 리스트에 넣는다.
		int iCnt = vTargetList.size();
		for( int i=0; i<iCnt; ++i )
		{
			m_vTargetInfoList.push_back( vTargetList[i] );
		}
	}

}

void ioObjectTargetShotItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioTraceWeaponType2 *pTrace = ToTraceWeaponType2( pWeapon );
	if( pTrace && !m_CurTargetName.IsEmpty() )
	{
		pTrace->SetTraceTarget( m_CurTargetName );
	}
}

void ioObjectTargetShotItem::ClearAllTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	m_vTargetInfoList.clear();
}

void ioObjectTargetShotItem::CheckFire( ioBaseChar *pOwner, bool bCharged )
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCurTargetCnt = m_vTargetInfoList.size();

	int iMaxFireCnt = iCurTargetCnt;
	if( iMaxFireCnt == 0 )
		iMaxFireCnt = 1;

	WeaponInfo rkInfo;
	if ( bCharged )
	{
		if( m_ExtendAttribute.m_vWeaponInfoList.empty() )
			return;
		rkInfo = m_ExtendAttribute.m_vWeaponInfoList.front();
	}
	else
	{
		if( m_NormalAttribute.m_vWeaponInfoList.empty() )
			return;

		rkInfo = m_NormalAttribute.m_vWeaponInfoList.front();
	}
		
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

	int iCurFireCnt = 0;
	DWORD dwWeaponIndex = rkInfo.m_iWeaponIdx;

	// target fire
	int iSize = m_vTargetInfoList.size();
	for( int i=0; i < iSize; ++i )
	{
		m_CurTargetName = m_vTargetInfoList[i].m_Name;
		
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurTargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			D3DXVec3Normalize( & vDir, &vDir );

			FireTime kFireTime;
			kFireTime.dwStart = dwCurTime;

			kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = dwWeaponIndex;
			if( pOwner->GetState() == CS_JUMP )
				kFireTime.eFireTimeType = FTT_JUMP_ATTACK;
			if( pOwner->GetState() != CS_JUMP )
				kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

			pOwner->ExtendFireExplicit( kFireTime,
										vPos,
										vDir,
										GetName() );

			iCurFireCnt++;
			dwWeaponIndex++;
		}
	}

	// none target fire
	if( iMaxFireCnt > 0 && iCurFireCnt < iMaxFireCnt )
	{
		m_CurTargetName.Clear();
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = dwWeaponIndex;
		if( pOwner->GetState() == CS_JUMP )
			kFireTime.eFireTimeType = FTT_JUMP_ATTACK;
		if( pOwner->GetState() != CS_JUMP )
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime,
									vPos,
									vDir,
									GetName() );
	}

}

bool ioObjectTargetShotItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioObjectTargetShotItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	return false;
}

void ioObjectTargetShotItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwAttackStartTime;

	if( (m_dwAttackStartTime == 0) || (m_dwAttackStartTime > 0 && dwGapTime > m_dwEnableKeyTime ) )
	{
		if( pOwner->IsAttackKey() )
		{
			m_AttackState = AS_CHARGING;
			m_dwChargingTime = dwCurTime;
		}
	}
}

void ioObjectTargetShotItem::OnCharging( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKeyDown() )
	{
		if ( m_dwChargingTime + m_dwChargeTime <= dwCurTime )
		{
			CheckTargetAttack( pOwner, true, true );
			m_AttackState = AS_NORMAL;
			m_dwAttackStartTime = dwCurTime;
		}
	}
	else
	{
		CheckTargetAttack( pOwner, false, true );
		m_AttackState = AS_NORMAL;
		m_dwAttackStartTime = dwCurTime;
	}
}

void ioObjectTargetShotItem::ProcessRunAttack( ioBaseChar *pOwner )
{
	switch( m_AttackState )
	{
	case AS_NORMAL:
		{
			if ( pOwner->IsNeedProcess() )
				ChangeToAttackFire( pOwner );
		}
		break;
	case AS_CHARGING:
		{
			if ( pOwner->IsNeedProcess() )
				OnCharging( pOwner );
		}
		break;
	}
}

void ioObjectTargetShotItem::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap )
{
	switch( m_AttackState )
	{
	case AS_NORMAL:
		{
			if ( pOwner->IsNeedProcess() )
				ChangeToAttackFire( pOwner );
		}
		break;
	case AS_CHARGING:
		{
			if ( pOwner->IsNeedProcess() )
				OnCharging( pOwner );
		}
		break;
	}
}

bool ioObjectTargetShotItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwEquipTime + m_dwProtectTime > FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

void ioObjectTargetShotItem::CheckTargetAttack( ioBaseChar *pOwner, bool bCharged, bool bSend/*= false*/ )
{
	CheckTarget( pOwner );
	CheckFire( pOwner, bCharged );
	
	ClearAllTarget( pOwner );
	if ( bSend )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << bCharged;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	
}

void ioObjectTargetShotItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int bCharged;
	rkPacket >> bCharged;

	if ( bCharged )
	{
		CheckTargetAttack( pOwner, true );
	}
	else
	{
		CheckTargetAttack( pOwner, false );
	}
}

bool ioObjectTargetShotItem::IsNormalAttackStop()
{
	return false;
}