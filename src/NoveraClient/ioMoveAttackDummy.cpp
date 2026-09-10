
#include "stdafx.h"

#include "ioMoveAttackDummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "BuffDefine.h"

ioMoveAttackDummy::ioMoveAttackDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_szTargetName.Clear();
}

ioMoveAttackDummy::~ioMoveAttackDummy()
{
}

void ioMoveAttackDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "";

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", 0.0f );
	m_fAimMoveSpeed = rkLoader.LoadFloat_e( "aim_move_speed", 0.0f );
	m_fZeroAimMoveSpeed = rkLoader.LoadFloat_e( "zero_aim_move_speed", 0.0f );

	m_iMaxMoveCount = rkLoader.LoadInt_e( "move_count", 0 );

	m_AimInfo[AT_AIM].m_dwAimTime = rkLoader.LoadInt_e( "aim_time", 0 );
	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimInfo[AT_AIM].m_AimEffect = szBuf;

	m_AimInfo[AT_ZERO_AIM].m_dwAimTime = rkLoader.LoadInt_e( "zero_aim_time", 0 );
	rkLoader.LoadString_e( "zero_aim_effect", "", szBuf, MAX_PATH );
	m_AimInfo[AT_ZERO_AIM].m_AimEffect = szBuf;

	m_bReduceGaugeOnUse = rkLoader.LoadBool_e( "reduce_gauge_on_use", false );
	m_fReduceGauge = rkLoader.LoadFloat_e( "reduce_gauge", 0.0f );

	_ENCSTR("aim_weapon", szEncAimWeapon);
	_ENCSTR("zero_aim_weapon", szEncZeroAimWeapon);
	LoadWeaponInfo( szEncAimWeapon, m_AimWeapon, rkLoader );
	LoadWeaponInfo( szEncZeroAimWeapon, m_ZeroAimWeapon, rkLoader );
}

void ioMoveAttackDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	}

	if( !CheckOwnerBuff() )
	{
		SetDieState( true );
	}
}

void ioMoveAttackDummy::ProcessStartState( float fTimePerSec )
{
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		m_iCurMoveCount = 0;
		SetDelayState();
		return;
	}
}

void ioMoveAttackDummy::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	FindTarget();
	CheckAimTime();
	UpdateAimMark();

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return;

	D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << SYNC_ATTACK_KEY;
			kPacket << vTargetPos;
			kPacket << static_cast<int>( m_CurAimType );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		SetMoveState( vTargetPos );
	}
}

void ioMoveAttackDummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	float fMoveSpeed = 0.0f;
	if( m_CurAimType == AT_AIM )
		fMoveSpeed = m_fAimMoveSpeed;
	else
		fMoveSpeed = m_fZeroAimMoveSpeed;
	float fMoveValue = fTimePerSec * fMoveSpeed;
	vPos += m_vAttackDir * fMoveValue;
	m_fCheckMoveRange -= fMoveValue;

	if( m_fCheckMoveRange <= 0.0f )
	{
		CreateWeapon();

		m_iCurMoveCount++;
		if( m_iCurMoveCount >= m_iMaxMoveCount )
		{
			SetDieState( true );
		}
		else
		{
			SetDelayState();
		}
		return;
	}

	SetWorldPosition( vPos );
}

void ioMoveAttackDummy::SetDelayState()
{
	if( m_State == DCS_DELAY )
		return;

	m_State = DCS_DELAY;

	SetAimInit();

	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = FLOAT1;
			if( m_fDelayAniRate > 0.0f )
				fAniRate = m_fDelayAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}
}

void ioMoveAttackDummy::SetMoveState( D3DXVECTOR3 vTargetPos )
{
	if( m_State == DCS_MOVE )
		return;

	m_State = DCS_MOVE;

	DestroyAimMark();

	D3DXVECTOR3 vDiff = vTargetPos - GetWorldPosition();
	D3DXVec3Normalize( &m_vAttackDir, &vDiff );
	m_fCheckMoveRange = D3DXVec3Length( &vDiff );

	D3DXQUATERNION qtRot;
	qtRot = Help::ConvertDirToQuaternion( m_vAttackDir );
	SetWorldOrientation( qtRot );

	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = FLOAT1;
			if( m_fMoveAniRate > 0.0f )
				fAniRate = m_fMoveAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioBuff *pBuff = pOwner->GetUseActiveCountInBuff();
		ioCreateDummyCharBuff *pCreateDummyCharBuff = ToCreateDummyCharBuff( pBuff );
		if( pCreateDummyCharBuff )
		{
			pCreateDummyCharBuff->CheckActiveCount();

			if( m_bReduceGaugeOnUse )
			{
				for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
				{
					ioSkill *pSkill = pOwner->GetEquipedSkill(i);
					if( pSkill && pSkill->GetName() == pCreateDummyCharBuff->GetSkillName() )
					{
						ioItem *pItem = pSkill->GetOwnerItem();
						if( pItem && pItem->HasSkillGauge() )
							pItem->ReduceSkillGauge( m_fReduceGauge );
					}
				}
			}
		}
	}
}

void ioMoveAttackDummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SYNC_ATTACK_KEY:
		{
			D3DXVECTOR3 vTargetPos;
			rkPacket >> vTargetPos;

			int iCurAimType;
			rkPacket >> iCurAimType;
			m_CurAimType = (AimType)iCurAimType;

			SetMoveState( vTargetPos );
		}
		break;
	case SYNC_CHANGE_TARGET:
		{
			rkPacket >> m_szTargetName;
			if( m_szTargetName.IsEmpty() )
			{
				SetAimInit();
				DestroyAimMark();
			}
			else
			{
				SetAimInit();
				CreateAimMark( pStage );
			}
		}
		break;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioMoveAttackDummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	DestroyAimMark();
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

void ioMoveAttackDummy::FindTarget()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fPrevDistSq = m_fAimRange*m_fAimRange;

	m_vTargetList.clear();

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	int iSize = rkTargetList.size();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = GetWorldPosition() - pTarget->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq < fPrevDistSq )	// Enemy
		{
			BaseCharDistanceSq TargetInfo;
			TargetInfo.m_Name = pTarget->GetCharName();
			TargetInfo.m_fDistanceSq = fDistSq;
			m_vTargetList.push_back( TargetInfo );
		}
	}

	std::sort( m_vTargetList.begin(), m_vTargetList.end(), BaseCharDistanceSqSort() );
	std::reverse( m_vTargetList.begin(), m_vTargetList.end() );

	if( m_szTargetName.IsEmpty() )
	{
		if( !m_vTargetList.empty() )
		{
			m_szTargetName = m_vTargetList[0].m_Name;
			SetAimInit();
			CreateAimMark( pStage );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << SYNC_CHANGE_TARGET;
				kPacket << m_szTargetName;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		int iCurTarget = -1;
		int iSize = m_vTargetList.size();
		for( int i=0; i<iSize; ++i )
		{
			if( m_vTargetList[i].m_Name == m_szTargetName )
				iCurTarget = i;
		}

		if( iCurTarget == -1 )
		{
			SetAimInit();
			m_szTargetName.Clear();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << SYNC_CHANGE_TARGET;
				kPacket << "";
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
		else
		{
			bool bTargetChange = false;
			if( pOwner->IsJumpKey() )
			{
				bTargetChange = true;
				iCurTarget++;
				if( iCurTarget >= iSize )
					iCurTarget = 0;
			}
			else if( pOwner->IsDefenseKey() )
			{
				bTargetChange = true;
				iCurTarget--;
				if( iCurTarget < 0 )
					iCurTarget = iSize-1;
			}

			if( bTargetChange )
			{
				m_szTargetName = m_vTargetList[iCurTarget].m_Name;
				SetAimInit();
				CreateAimMark( pStage );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
					kPacket << m_OwnerName;
					kPacket << m_iCreateIndex;
					kPacket << SYNC_CHANGE_TARGET;
					kPacket << m_szTargetName;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
}

void ioMoveAttackDummy::SetAimInit()
{
	DestroyAimMark();

	m_CurAimType = AT_AIM;
	if( !m_szTargetName.IsEmpty() )
		m_dwAimCheckTime = FRAMEGETTIME();
	else
		m_dwAimCheckTime = 0;
}

void ioMoveAttackDummy::CheckAimTime()
{
	if( m_dwAimCheckTime > 0 &&
		m_dwAimCheckTime + m_AimInfo[m_CurAimType].m_dwAimTime < FRAMEGETTIME() )
	{
		m_dwAimCheckTime = FRAMEGETTIME();

		if( m_CurAimType == AT_AIM )
		{
			m_CurAimType = AT_ZERO_AIM;
		}
		else
		{
			m_CurAimType = AT_AIM;
		}
	}
}

void ioMoveAttackDummy::CreateAimMark( ioPlayStage *pStage )
{
	m_dwAimCheckTime = FRAMEGETTIME();

	if( m_AimInfo[AT_AIM].m_dwAimEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_AimInfo[AT_AIM].m_dwAimEffectID );
		m_AimInfo[AT_AIM].m_dwAimEffectID = -1;
	}

	if( m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID );
		m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID = -1;
	}
}

void ioMoveAttackDummy::UpdateAimMark()
{
	ioBaseChar *pChar = GetOwner();
	if( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( !pTarget || m_dwAimCheckTime == 0 )
	{
		m_szTargetName.Clear();
		SetAimInit();
		return;
	}

	if( m_dwAimCheckTime > 0 )
	{
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		if( m_CurAimType == AT_AIM )
		{
			if( m_AimInfo[AT_AIM].m_dwAimEffectID == -1 )
			{			
				ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_AimInfo[AT_AIM].m_AimEffect, vPos, vScale );
				if( pMapEffect )
					m_AimInfo[AT_AIM].m_dwAimEffectID = pMapEffect->GetUniqueID(); 
			}
			else if( m_AimInfo[AT_AIM].m_dwAimEffectID != -1 )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( m_AimInfo[AT_AIM].m_dwAimEffectID );
				if( pMapEffect )
					pMapEffect->SetWorldPosition( vPos );
			}

			if( m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID != -1 )
			{
				pStage->DestroyMapEffect( m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID );
				m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID = -1;
			}
		}
		else
		{
			if( m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID == -1 )
			{
				ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_AimInfo[AT_ZERO_AIM].m_AimEffect, vPos, vScale );
				if( pMapEffect )
					m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID = pMapEffect->GetUniqueID(); 
			}
			else if( m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID != -1 )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( m_AimInfo[AT_ZERO_AIM].m_dwAimEffectID );
				if( pMapEffect )
					pMapEffect->SetWorldPosition( vPos );
			}

			if( m_AimInfo[AT_AIM].m_dwAimEffectID != -1 )
			{
				pStage->DestroyMapEffect( m_AimInfo[AT_AIM].m_dwAimEffectID );
				m_AimInfo[AT_AIM].m_dwAimEffectID = -1;
			}
		}
	}
}

void ioMoveAttackDummy::DestroyAimMark()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	for( int i=AT_AIM; i<AT_ZERO_AIM+1; ++i )
	{
		if( m_AimInfo[i].m_dwAimEffectID != -1 )
		{
			pStage->DestroyMapEffect( m_AimInfo[i].m_dwAimEffectID );
			m_AimInfo[i].m_dwAimEffectID = -1;
		}
	}
}

bool ioMoveAttackDummy::CheckOwnerBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( pOwner->HasBuff(BT_CREATE_DUMMY_CHAR) )
		return true;

	return false;
}

void ioMoveAttackDummy::CreateWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	WeaponInfo kInfo;
	kInfo.Init();

	if( m_CurAimType == AT_AIM )
		kInfo = m_AimWeapon;
	else
		kInfo = m_ZeroAimWeapon;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx  = kInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = kInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni   = kInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration  = kInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = kInfo.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
	kFireTime.m_iExtraValue = m_iCreateIndex;

	if( kFireTime.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
	{
		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, m_vAttackDir, "" );
		if( pWeapon )
		{
			ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
			if( pZone )
			{
				pZone->SetCollisionDir( m_vAttackDir );
				pZone->SetZoneValue( true, false );
			}
		}
	}
}