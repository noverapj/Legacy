
#include "stdafx.h"
#include "ioOwnerControlDummy6.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"

#include "WeaponDefine.h"

#include "ioRocketlauncherItem.h"

ioOwnerControlDummy6::ioOwnerControlDummy6( ioEntityGroup *pGrp, ioPlayMode *pMode ):
ioDummyChar( pGrp, pMode )
{
	m_bCreatedDummyBySkill = true;
	
	m_vAttachWeaponTypeList.clear();
	m_CollisionUserList.clear();
	m_iCurAttackLevel = 0;
	m_fEnemyDistance = 0.0f;

	m_bFirstCollision		= true;
	m_dwDummyCreateTime		= FRAMEGETTIME();
}

ioOwnerControlDummy6::~ioOwnerControlDummy6()
{
	m_vAttachWeaponTypeList.clear();
	m_CollisionUserList.clear();
}

void ioOwnerControlDummy6::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	m_bDestroyDummyByAttackKey = rkLoader.LoadBool_e( "destroy_dummy_by_attack_key", true );
	
	m_dwAfterLeavingDummy_Interval	= (DWORD)rkLoader.LoadInt_e( "interval_after_leaving_dummy", 0 );
	m_dwDelete_Interval				= (DWORD)rkLoader.LoadInt_e( "delete_interval_in_list", 0 );
	m_dwNextLevel1_Interval			= (DWORD)rkLoader.LoadInt_e( "interval_next_level_1", 0 );
	m_dwNextLevel2_Interval			= (DWORD)rkLoader.LoadInt_e( "interval_next_level_2", 0 );
	m_dwNextLevel3_Interval			= (DWORD)rkLoader.LoadInt_e( "interval_next_level_3", 0 );

	m_iMaxAttackLevel				= rkLoader.LoadInt_e( "max_attack_level", 0 );

	m_fCollisionRange				= rkLoader.LoadFloat_e( "collision_range", 0.0f );

	m_fEnemyCharYPosPlusVal			= rkLoader.LoadFloat_e( "enemy_y_pos_plus", 0.0f );
}

void ioOwnerControlDummy6::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	float fSecPerTime = fTimePerSec * FLOAT1000;
	m_pGroup->Update( fSecPerTime );

	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;

	ProcessState( fTimePerSec, dwPreTime );

	DropZoneCheck();
	CheckDestroyRange();

	if( CheckOwnerDestroyState() )
	{
		SendDieState( true );
	}

	ioPlayStage* pPlayStage = pOwner->GetCreator();
	const BaseCharList &rkTargetList = pPlayStage->GetBaseCharList();
	for( int i = 0; i < (int)rkTargetList.size(); i++ )
	{
		if( pOwner != rkTargetList[ i ] && pOwner->GetTeam() != rkTargetList[ i ]->GetTeam() )
			CheckCollisionChar( rkTargetList[ i ] );
	}

	ioCollisionUserList::iterator iter = m_CollisionUserList.begin();
	bool bExist = false;
	
	bool bExistEnemy = false;
	ioRocketLauncherItem* pRocketLauncherItem2 = ToRocketLauncherItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	for( ; iter != m_CollisionUserList.end(); iter++ )
	{
		DWORD dwInterval = dwCurTime - iter->m_dwMaintenanceTime;
		// 아래 체크하는 것보다 일정 텀이 더 지나면 삭제
		if( dwInterval > m_dwDelete_Interval || iter->m_bDelete )
		{
			iter = m_CollisionUserList.erase(iter);
			continue;
		}

		if( iter == m_CollisionUserList.end() )
			continue;

		bExistEnemy = true;
		if( iter->m_iCurAttackLevel <= m_iMaxAttackLevel )
		{
			ioRocketLauncherItem* pRocketLauncherItem1 = ToRocketLauncherItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pRocketLauncherItem1 )
			{
				int iCheckBullet = iter->m_iCurAttackLevel * 4;
				if( iter->m_iCurAttackLevel == DCAS_THIRD_LEVEL )
					iCheckBullet += 4;

				ioBaseChar* pTargetChar = pPlayStage->GetBaseChar( iter->m_UserName );
				if( !pPlayStage || !pTargetChar )
					continue;

				if( pTargetChar->HasHideBuff() || pTargetChar->GetState() == CS_DIE || pTargetChar->GetState() == CS_LOADING )
				{
					iter->m_bDelete = true;
					m_iCurAttackLevel = (int)DCAS_NONE;
					pRocketLauncherItem1->DeleteSpecialAttackAim( pOwner, pTargetChar );
					continue;
				}

				// 리스트 요소가 1개 이상일때 적들의 거리 비교
				bool bChangeTargetByDistance = false;
				if( m_bFirstCollision )
				{
					if( m_CollisionUserList.size() > 1 )
					{
						D3DXVECTOR3 vDiff = pTargetChar->GetWorldPosition() - pOwner->GetWorldPosition();
						float fDistance = D3DXVec3Length( &vDiff );
						if( m_fEnemyDistance <= 0.0f )
						{
							m_fEnemyDistance = fDistance;
							m_iCurAttackLevel = (int)DCAS_NONE;
							continue;
						}
						else
						{
							if( m_fEnemyDistance > fDistance )
							{
								if( iter->m_UserName != pRocketLauncherItem1->GetDummyAimTarget() )
								{
									m_fEnemyDistance		= fDistance;
									bChangeTargetByDistance = true;
								}
							}
							else if( m_fEnemyDistance < fDistance )
							{
								if( iter->m_UserName == pRocketLauncherItem1->GetDummyAimTarget() )
								{
									m_iCurAttackLevel = (int)DCAS_NONE;
									pRocketLauncherItem1->DeleteSpecialAttackAim( pOwner, pTargetChar );
								}
								continue;
							}
						}
					}
					else
					{
						if( m_fEnemyDistance != 0.0f )
						{
							m_fEnemyDistance = 0.0f;

							if( iter->m_UserName != pRocketLauncherItem1->GetDummyAimTarget() )
							{
								iter->m_iCurAttackLevel			= (int)DCAS_NONE;
								iter->m_dwMaintenanceTime		= dwCurTime;
								bChangeTargetByDistance			= true;
								m_iCurAttackLevel				= (int)DCAS_NONE;
							}
						}
					}
				}
				else
				{
					ioHashString stTargetName = pRocketLauncherItem1->GetDummyAimTarget();
					if( !stTargetName.IsEmpty() && iter->m_UserName != stTargetName )
					{
						iter->m_bDelete = true;
						continue;
					}
				}
				
				if( pRocketLauncherItem1->GetCurBullet() >= iCheckBullet && ( m_iCurAttackLevel != iter->m_iCurAttackLevel || bChangeTargetByDistance ) )
				{
					if( m_iCurAttackLevel == DCAS_NONE )
						pRocketLauncherItem1->AttachDummyAimEffect( pOwner );
					m_iCurAttackLevel = iter->m_iCurAttackLevel;
					pRocketLauncherItem1->SetDummyAimTarget( pOwner, m_iCurAttackLevel, iter->m_UserName );
				}
			}
		}
	}

	// 충돌 체크 유저 리스트에 아무것도 없고 현재 1단계 이상이면 1단계로 변경
	if( ( m_CollisionUserList.empty() || !bExistEnemy ) && m_iCurAttackLevel > (int)DCAS_NONE )
	{
		m_iCurAttackLevel = (int)DCAS_NONE;
		if( pRocketLauncherItem2 )
			pRocketLauncherItem2->DeleteSpecialAttackAim( pOwner );
	}

	// 제한 이동에 따라서 제한된 위치가 컨베이어 벨트, 엘리베이터 등으로 달라질수 있어서 위치 체크
	CalibrateDummyPosition( pOwner );

	DWORD dwInterval = dwCurTime - m_dwDummyCreateTime;
	// 이 더미 생성 후 0.1초 간 여러 적 체크.. 그 이후로는 적 1명만 체크
	if( dwInterval > 100 && m_bFirstCollision )
		m_bFirstCollision = false;
}

void ioOwnerControlDummy6::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState();
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimerPerSec );
		break;
	case DCS_NONE_DELAY:
		ProcessNoneDelayState( fTimerPerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimerPerSec );
		break;
	case DCS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimerPerSec, dwPreTime );
		break;
	case DCS_CONTROL_ATTACK:
		ProcessChargeOwnerControl( fTimerPerSec );
		break;
	case DCS_DIE:
		ProcessDieState( fTimerPerSec );
		break;
	case DCS_TRACE:
		ProcessTraceState( fTimerPerSec, dwPreTime );
		break;
	case DCS_TRACE_TARGET_ATTACK:
		ProcessTraceTargetAttack( fTimerPerSec, dwPreTime );
		break;
	case DCS_LOOP_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessLoopAttackState( fTimerPerSec, dwPreTime );
		break;
	case DCS_GROUND_PIERCING_ATTACK:		
		ProcessPiercingState( fTimerPerSec );		
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	ProcessPos( fTimerPerSec );
	ProcessEffectUpdate( fTimerPerSec );
}

void ioOwnerControlDummy6::ProcessStartState()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_bStartAttchWeapon )
		SetAttachWeapon();

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioOwnerControlDummy6::ProcessDelayState( float fTimePerSec )
{
	ProcessDelayOwnerControl( fTimePerSec );
}

void ioOwnerControlDummy6::ProcessMoveState( float fTimePerSec )
{
	ProcessMoveOwnerControl( fTimePerSec );
}

void ioOwnerControlDummy6::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioDummyChar::ProcessAttackState( fTimePerSec, dwPreTime );
}

void ioOwnerControlDummy6::ProcessPos( float fTimePerSec )
{
	ioDummyChar::ProcessPos( fTimePerSec );
}

int ioOwnerControlDummy6::DontMoveEntityLevel() const
{
	if( m_bMountEnable )
		return 1;
	return 3;
}

bool ioOwnerControlDummy6::IsMountingAvailable() const
{
	return ioDummyChar::IsMountingAvailable();
}

bool ioOwnerControlDummy6::IsNowMountEnable() const
{
	return ioDummyChar::IsNowMountEnable();
}

void ioOwnerControlDummy6::DropZoneCheck()
{
	//ioDummyChar::DropZoneCheck();
}

bool ioOwnerControlDummy6::IsMountAvailable() const
{
	return ioDummyChar::IsMountAvailable();
}


void ioOwnerControlDummy6::SetDelayState( bool bNet )
{
	m_State = DCS_DELAY;
	m_iReserveAttackCode = 0;

	m_dwDelayStartTime = FRAMEGETTIME();

	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;

	m_iCurExtraAniJump = 0;
	m_vExtraAniJumpList.clear();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;
	
	CheckChargeWeaponDead();

	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			float fAniRate = FLOAT1;
			if( m_fDelayAniRate > 0.0f )
				fAniRate = m_fDelayAniRate;

			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}
}

void ioOwnerControlDummy6::SetMoveState()
{
	if( m_State == DCS_DIE )
		return;

	m_State = DCS_MOVE;

	CheckChargeWeaponDead();

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

	SetAttachWeapon();
}

bool ioOwnerControlDummy6::CheckOwnerDestroyState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( m_bDiableOwnerWoundDestroy )
	{
		return false;
	}

	if( !pOwner->IsCurUsingSkill( m_ControlSkillName ) && m_bCreatedDummyBySkill )
	{
		m_ControlSkillName.Clear();
		return true;
	}

	// 용병 캐릭터 상태가 CS_ATTACK이 아니면 현재 이 더미 삭제
	ioRocketLauncherItem* pRocketLauncherItem = ToRocketLauncherItem( GetOwner()->GetPriorityItem( SPT_ATTACK ) );
	if( pRocketLauncherItem )
	{
		if( pOwner->GetState() != CS_ATTACK )
			return true;
	}

	return false;
}

bool ioOwnerControlDummy6::CheckAttackKeyControl()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return false;

	if( !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bAttackToDestroyDummy )
	{
		if( pOwner->IsAttackKey() && m_bDestroyDummyByAttackKey )
		{
			SetDieState( true );
			return true;
		}
	}
	else if( m_dwAttackKeyCheckTime == 0 )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			m_dwAttackKeyCheckTime = dwCurTime;
			return false;
		}
	}
	else
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			// 수평공격 발생.
			m_iReserveAttackCode = m_iControlNormalAttack;
			SetAttackState();

			return true;
		}
		else if( pOwner->IsAttackKeyDown() && m_dwAttackKeyCheckTime+m_dwAttackChargeTime < dwCurTime )
		{
			// 수직공격 발생.
			SetControlAttackState( true );
			return true;
		}
	}

	return false;
}

void ioOwnerControlDummy6::ProcessDelayOwnerControl( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		bool bChange = false;

		// 방향변화 체크
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			bChange = true;
			m_CurDirKey = eNewDirKey;
		}

		// Up 체크
		if( !bChange && !m_bSetDownKey )
		{
			if( pOwner->IsJumpKeyDown() )
			{
				bChange = true;
				m_bSetUpKey = true;
			}
		}

		// Down 체크
		if( !bChange && !m_bSetUpKey && !m_bAutoDescent )
		{
			if( pOwner->IsDefenseKeyDown() )
			{
				bChange = true;
				m_bSetDownKey = true;
			}
		}

		if( bChange )
		{
			SetMoveState();
		}

		// 공격 체크
		if( !bChange && CheckAttackKeyControl() )
			return;

		SetAttachWeapon();
	}

	if( m_bAutoDescent && !m_bSetUpKey )
		SetMoveState();
}

void ioOwnerControlDummy6::ProcessMoveOwnerControl( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;
	if( m_State == DCS_DIE ) return;

	if( pOwner->IsNeedProcess() &&
		!pOwner->IsSettedDirection() && !pOwner->IsJumpKeyDown() && ( !pOwner->IsDefenseKeyDown() && !m_bAutoDescent ) )
	{
		SetDelayState( true );
		return;
	}

	// 공격 체크
	if( CheckAttackKeyControl() )
		return;

	CheckKeyInputChange();
	CheckControlMove( fTimePerSec );
}

void ioOwnerControlDummy6::ProcessChargeOwnerControl( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			m_dwAttackKeyCheckTime = 0;
			SetDelayState( true );
			return;
		}
		else if( m_dwAttackChargeRepeatTime != 0 && pOwner->IsAttackKeyDown() && m_dwAttackKeyCheckTime+m_dwAttackChargeRepeatTime < dwCurTime )
		{
			SetControlAttackState( false );
		}
	}

	CheckKeyInputChange();
	CheckControlMove( fTimePerSec );
}

void ioOwnerControlDummy6::CalibrateDummyPosition( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	D3DXVECTOR3 vDiff = ( vCurPos ) - pOwner->GetWorldPosition();
	float fDistanceSq = D3DXVec3LengthSq( &vDiff );

	if( m_fControlOutRange * m_fControlOutRange < fDistanceSq )
	{
		if( GetType() == DCT_OWNER_CONTROL6 )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			vDiff *= m_fControlOutRange;

			float fCurHeight = vCurPos.y;
			D3DXVECTOR3 vDummyPos1 = pOwner->GetWorldPosition() + vDiff;
			vDummyPos1.y = fCurHeight;
			SetWorldPosition( vDummyPos1 );
		}
	}
	else if ( m_fControlMinRange * m_fControlMinRange > fDistanceSq )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= m_fControlMinRange;

		float fCurHeight = vCurPos.y;
		D3DXVECTOR3 vDummyPos2 = pOwner->GetWorldPosition() + vDiff;
		vDummyPos2.y = fCurHeight;
		SetWorldPosition( vDummyPos2 );
	}
}

void ioOwnerControlDummy6::RemoveAttachWeaponType( int iWeaponTypeIdx )
{
	for ( IntVec::size_type i=0; i < m_vAttachWeaponTypeList.size(); ) 
	{
		if ( iWeaponTypeIdx == m_vAttachWeaponTypeList[i] )
			m_vAttachWeaponTypeList.erase(m_vAttachWeaponTypeList.begin() + i);
		else 
			i++;
	}
}

void ioOwnerControlDummy6::CheckCollisionChar( ioBaseChar *pCollisionChar )
{
	if( !pCollisionChar )
		return;

	if( GetOwner() == pCollisionChar )
		return;

	if( pCollisionChar->HasHideBuff() || pCollisionChar->GetState() == CS_DIE || pCollisionChar->GetState() == CS_LOADING )
		return;

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	D3DXVECTOR3 vEnemyPos = pCollisionChar->GetWorldPosition();
	if( m_fEnemyCharYPosPlusVal > 0.0f )
		vEnemyPos.y += m_fEnemyCharYPosPlusVal;
	D3DXVECTOR3 vDiff = vCurPos - vEnemyPos;
	float fDistanceSq = D3DXVec3LengthSq( &vDiff );

	if ( m_fCollisionRange * m_fCollisionRange > fDistanceSq )
	{
		ioCollisionUserList::iterator iter = m_CollisionUserList.begin();
		ioCollisionUserList::iterator iter2;
		DWORD dwCurTime = FRAMEGETTIME();
		bool bExist = false;
		for( ; iter != m_CollisionUserList.end(); iter++ )
		{
			if( pCollisionChar->GetUserIndex() == iter->m_dwUserindex && pCollisionChar->GetCharName() == iter->m_UserName )
			{
				bExist = true;
				iter2 = iter;
			}
		}
		if( bExist )
		{
			// 유지 시간 갱신
			iter2->m_dwMaintenanceTime = dwCurTime;
			// 충돌시간 체크
			DWORD dwInterval = dwCurTime - iter2->m_dwCollisionTime;
			//if( iter2->m_iCurAttackLevel == 0 && dwInterval > m_dwNextLevel1_Interval )
			if( CheckAttackLevel( iter2->m_iCurAttackLevel, dwInterval ) )
			{
				iter2->m_dwCollisionTime = dwCurTime;
				if( iter2->m_iCurAttackLevel < m_iMaxAttackLevel )
					iter2->m_iCurAttackLevel++;
			}
		}
		else
		{
			if( !m_bFirstCollision && (int)m_CollisionUserList.size() > 1 )
				return;

			CollisionUserInfo info;
			info.m_dwUserindex = pCollisionChar->GetUserIndex();
			info.m_dwCollisionTime = dwCurTime;
			info.m_dwMaintenanceTime = dwCurTime;
			info.m_iCurAttackLevel = (int)DCAS_NONE;
			info.m_UserName = pCollisionChar->GetCharName();
			m_CollisionUserList.push_back( info );
		}
	}
}

bool ioOwnerControlDummy6::CheckAttackLevel( int iCollCharAttLv, DWORD dwInterval )
{
	if( iCollCharAttLv == 0 && dwInterval > m_dwNextLevel1_Interval )
		return true;
	else if( iCollCharAttLv == 1 && dwInterval > m_dwNextLevel2_Interval )
		return true;
	else if( iCollCharAttLv == 2 && dwInterval > m_dwNextLevel3_Interval )
		return true;

	return false;
}


ioHashString ioOwnerControlDummy6::GetTargetName()
{
	ioHashString szTargetName;
	szTargetName.Clear();
	ioCollisionUserList::iterator iter = m_CollisionUserList.begin();
	int iHighestLevel = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	for( ; iter != m_CollisionUserList.end(); iter++ )
	{
		DWORD dwInterval = dwCurTime - iter->m_dwMaintenanceTime;
		// 일정 텀이 지나면 체크하지 않음
		if( dwInterval > m_dwAfterLeavingDummy_Interval )
			continue;
		if( iter->m_iCurAttackLevel > iHighestLevel && iter->m_iCurAttackLevel <= m_iMaxAttackLevel )
		{
			iHighestLevel = iter->m_iCurAttackLevel;
			szTargetName = iter->m_UserName;
		}
	}
	
	return szTargetName;
}

int ioOwnerControlDummy6::GetCurAttackLevel()
{
	return m_iCurAttackLevel;
}