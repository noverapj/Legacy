
#include "stdafx.h"

#include "ioMamaHahaDummy.h"

#include "ioVoiceChat/Fourier.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "SkillDefine.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

ioMamaHahaDummy::ioMamaHahaDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_MamaHahaState = MHS_NONE;

	m_vShichikapuMoveTargetPos = ioMath::VEC3_ZERO;
	m_vShichikapuMoveDir = ioMath::VEC3_ZERO;

	m_fShichikapuMoveMaxRange = 0.0f;
	m_fShichikapuMoveCurRange = 0.0f;

	m_vMamaHahaWeaponList.clear();
}

ioMamaHahaDummy::~ioMamaHahaDummy()
{
}

void ioMamaHahaDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_vMamaHahaBaseOffSet.x = rkLoader.LoadFloat_e( "mamahaha_base_offset_x", 0.0f );
	m_vMamaHahaBaseOffSet.y = rkLoader.LoadFloat_e( "mamahaha_base_offset_y", 0.0f );
	m_vMamaHahaBaseOffSet.z = rkLoader.LoadFloat_e( "mamahaha_base_offset_z", 0.0f );

	m_vMamaHahaOwnerAttachOffSet.x = rkLoader.LoadFloat_e( "mamahaha_owner_attach_offset_x", 0.0f );
	m_vMamaHahaOwnerAttachOffSet.y = rkLoader.LoadFloat_e( "mamahaha_owner_attach_offset_y", 0.0f );
	m_vMamaHahaOwnerAttachOffSet.z = rkLoader.LoadFloat_e( "mamahaha_owner_attach_offset_z", 0.0f );

	rkLoader.LoadString_e( "mamahaha_owner_attach_ani", "", szBuf, MAX_PATH );
	m_MamaHahaOwnerAttachAni = szBuf;
	m_fMamaHahaOwnerAttachAniRate = rkLoader.LoadFloat_e( "mamahaha_owner_attach_ani_rate", FLOAT1 );

	// shichikapu
	m_fShichikapuMoveSpeed = rkLoader.LoadFloat_e( "shichikapu_move_speed", 0.0f );
	m_fShichikapuMoveAngle = rkLoader.LoadFloat_e( "shichikapu_move_angle", 0.0f );
	m_fShichikapuReturnSpeed = rkLoader.LoadFloat_e( "shichikapu_return_speed", 0.0f );
	m_fShichikapuLimitRange = rkLoader.LoadFloat_e( "shichikapu_limit_range", 0.0f );

	m_dwShichikapuMoveDuration = (DWORD)rkLoader.LoadInt_e( "shichikapu_move_duration", 0 );

	// enemy attach
	rkLoader.LoadString_e( "mamahaha_enemy_attach_ani", "", szBuf, MAX_PATH );
	m_MamaHahaEnemyAttachAni = szBuf;
	m_fMamaHahaEnemyAttachAniRate = rkLoader.LoadFloat_e( "mamahaha_enemy_attach_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "mamahaha_enemy_attach_buff", "", szBuf, MAX_PATH );
	m_MamaHahaEnemyAttachBuff = szBuf;

	m_vMamaHahaEnemyAttachOffSet.x = rkLoader.LoadFloat_e( "mamahaha_enemy_attach_offset_x", 0.0f );
	m_vMamaHahaEnemyAttachOffSet.y = rkLoader.LoadFloat_e( "mamahaha_enemy_attach_offset_y", 0.0f );
	m_vMamaHahaEnemyAttachOffSet.z = rkLoader.LoadFloat_e( "mamahaha_enemy_attach_offset_z", 0.0f );
}

void ioMamaHahaDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
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
	case DCS_MAMAHAHA_STATE:
		ProcessMamaHahaState( fTimePerSec, dwPreTime );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	}

	CheckMamaHahaVisible();
	ProcessPos( fTimePerSec );
	ProcessEffectUpdate( fTimePerSec );
}

bool ioMamaHahaDummy::CheckMamaHahaOwnerVisible()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return false;

	bool bCharVisible = pOwner->IsVisibleChar();

	if( pOwner->IsCanInvisible() )
		bCharVisible = false;

	if( pOwner->HasHideBuff() )
		bCharVisible = false;

	return bCharVisible;
}

void ioMamaHahaDummy::CheckMamaHahaVisible()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	bool bCharVisible = CheckMamaHahaOwnerVisible();
	bool bDummyVisible = m_pGroup->IsVisible();

	switch( m_State )
	{
	case DCS_START:
	case DCS_DELAY:
	case DCS_MOVE:
		if( bCharVisible && !bDummyVisible )
			m_pGroup->SetVisible( true );
		else if( !bCharVisible && bDummyVisible )
			m_pGroup->SetVisible( false );
		break;
	default:
		if( !bDummyVisible )
			m_pGroup->SetVisible( true );
		break;
	}
}

void ioMamaHahaDummy::ProcessPos( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
	}

	ProcessForceMove( fTimePerSec );

	switch( m_State )
	{
	case DCS_START:
	case DCS_DELAY:
	case DCS_MOVE:
		{
			D3DXVECTOR3 vNewOffSet = pOwner->GetTargetRot() * m_vMamaHahaBaseOffSet;
			D3DXVECTOR3 vNewPos = pOwner->GetMidPositionByRate() + vNewOffSet;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( pOwner->GetTargetRot() );
		}
		return;
	case DCS_MAMAHAHA_STATE:
		break;
	default:
		// 나머지는 무시
		return;
	}

	switch( m_MamaHahaState )
	{
	case MHS_OWNER_ATTACH:
		{
			D3DXVECTOR3 vNewOffSet = pOwner->GetTargetRot() * m_vMamaHahaOwnerAttachOffSet;
			D3DXVECTOR3 vNewPos = pOwner->GetMidPositionByRate() + vNewOffSet;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( pOwner->GetTargetRot() );
		}
		return;
	case MHS_FLYING1:
	case MHS_FLYING2:
		{
			bool bEnd = false;
			D3DXVECTOR3 vMove;
			float fMoveSpeed = m_fShichikapuMoveSpeed * fTimePerSec;

			if( m_fShichikapuMoveCurRange+fMoveSpeed < m_fShichikapuMoveMaxRange )
			{
				m_fShichikapuMoveCurRange += fMoveSpeed;
			}
			else
			{
				fMoveSpeed = m_fShichikapuMoveMaxRange - m_fShichikapuMoveCurRange;
				m_fShichikapuMoveCurRange = m_fShichikapuMoveMaxRange;
				bEnd = true;
			}

			vMove = fMoveSpeed * m_vShichikapuMoveDir;
			Translate( vMove );

			if( bEnd )
			{
				SetShichikapuReturn();
			}
		}
		return;
	case MHS_RETURN1:
	case MHS_RETURN2:
		{
			D3DXVECTOR3 vNewOffSet = pOwner->GetTargetRot() * m_vMamaHahaOwnerAttachOffSet;
			D3DXVECTOR3 vNewPos = pOwner->GetMidPositionByRate() + vNewOffSet;

			D3DXVECTOR3 vDir = vNewPos - GetWorldPosition();
			float fGap = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			if( fGap <= 15.0f )
			{
				SetWorldPosition( vNewPos );
				SetWorldOrientation( pOwner->GetTargetRot() );
				
				SetDelayState( false );
			}
			else if( fGap > m_fShichikapuLimitRange )
			{
				SetWorldPosition( vNewPos );
				SetWorldOrientation( pOwner->GetTargetRot() );
				
				SetDelayState( false );
			}
			else
			{
				D3DXVECTOR3 vMove;

				float fMoveSpeed = m_fShichikapuReturnSpeed * fTimePerSec;
				vMove = fMoveSpeed * vDir;

				Translate( vMove );
			}			
		}
		return;
	case MHS_ENEMY_ATTACH:
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_MamaHahaTargetName );
			if( pTarget )
			{
				D3DXQUATERNION qtRot = pTarget->GetTargetRot();
				D3DXVECTOR3 vNewOffSet = qtRot * m_vMamaHahaEnemyAttachOffSet;
				D3DXVECTOR3 vNewPos = pTarget->GetMidPositionByRate() + vNewOffSet;

				SetWorldPosition( vNewPos );
				SetWorldOrientation( qtRot );
			}
		}
		return;
	case MHS_FIRE_ATTACK:
		// 위치변화 없음
		return;
	}

	// 아래는 높이값 처리.
	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vCurPos = vPrePos;
	float fMapHeight = GetBottomHeight();

	DWORD dwGapTime = dwCurTime - m_dwProcessTime;

	do 
	{
		DWORD dwCurGap = min( dwGapTime, 10 );
		if( dwGapTime >= 10 )
		{
			dwGapTime -= 10;
		}
		else
		{
			dwCurGap = dwGapTime;
			dwGapTime = 0;
		}

		float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

		if( m_fCurJumpPowerAmt > 0.0f )
		{
			vCurPos.y += m_fCurJumpPowerAmt * fNewTimePerSec;
		}

		if( vCurPos.y > fMapHeight )
		{
			float fCurRate = m_fGravityRate;
			if( fCurRate <= 0.0f )
				fCurRate = FLOAT1;

			m_fGravityAmt += (m_pModeParent->GetCreator()->GetRoomGravity() * fCurRate )* fNewTimePerSec;
			vCurPos.y -= m_fGravityAmt * fNewTimePerSec;

			if( vCurPos.y <= fMapHeight )
			{
				m_fGravityAmt = 0.0f;
				m_fCurJumpPowerAmt = 0.0f;
			}
		}

	} while( dwGapTime > 0 );

	if( vCurPos.y < fMapHeight )
	{
		vCurPos.y = fMapHeight;
		m_fGravityAmt = 0.0f;
		m_fCurJumpPowerAmt = 0.0f;
	}

	SetWorldPosition( vCurPos );

	m_dwProcessTime = dwCurTime;
}

void ioMamaHahaDummy::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->GetState() == CS_JUMP || pOwner->GetState() == CS_DASH )
	{
		SetMoveState();
		return;
	}

	if( CheckEnableAttackState() )
	{
		SetAttackState();
		return;
	}
}

void ioMamaHahaDummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->GetState() != CS_JUMP && pOwner->GetState() != CS_DASH )
	{
		SetDelayState( false );
		return;
	}
}

void ioMamaHahaDummy::ProcessMamaHahaState( float fTimePerSec, DWORD dwPreTime )
{
	if( CheckEnableAttackState() )
	{
		SetAttackState();
		return;
	}

	switch( m_MamaHahaState )
	{
	case MHS_OWNER_ATTACH:
	case MHS_RETURN1:
	case MHS_RETURN2:
		break;
	case MHS_ENEMY_ATTACH:
		ProcessEnemyAttachState( fTimePerSec );
		break;
	case MHS_FLYING1:
	case MHS_FLYING2:
	case MHS_FIRE_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimePerSec, dwPreTime );
		break;
	}
}

void ioMamaHahaDummy::ProcessEnemyAttachState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_MamaHahaEnemyAttachBuff.IsEmpty() )
	{
		SetShichikapuReturn();
		return;
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_MamaHahaTargetName );
	if( !pTarget || !pTarget->HasBuff( m_MamaHahaEnemyAttachBuff ) )
	{
		SetShichikapuReturn();
		return;
	}
}

void ioMamaHahaDummy::SetMamaHahaOwnerAttach()
{
	switch( m_State )
	{
	case DCS_START:
	case DCS_DELAY:
	case DCS_MOVE:
		SetMamaHahaState( MHS_OWNER_ATTACH );
		break;
	}
}

void ioMamaHahaDummy::ReleaseMamaHahaOwnerAttach()
{
	if( m_State != DCS_MAMAHAHA_STATE )
		return;

	if( m_MamaHahaState == MHS_OWNER_ATTACH )
	{
		SetDelayState( false );
	}
}

void ioMamaHahaDummy::SetMamaHahaState( MamaHahaState eType )
{
	m_State = DCS_MAMAHAHA_STATE;

	switch( eType )
	{
	case MHS_OWNER_ATTACH:
		{
			int iAniID = m_pGroup->GetAnimationIdx( m_MamaHahaOwnerAttachAni );
			if( iAniID != -1 )
			{
				float fAniRate = FLOAT1;
				if( m_fMamaHahaOwnerAttachAniRate > 0.0f )
					fAniRate = m_fMamaHahaOwnerAttachAniRate;

				m_pGroup->ClearAllActionAni( FLOAT10, true );
				m_pGroup->ClearAllLoopAni( FLOAT10, true );

				m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );

				m_MamaHahaState = MHS_OWNER_ATTACH;
			}
		}
		break;
	}
}

bool ioMamaHahaDummy::CheckEnableOwnerAttach()
{
	if( m_State != DCS_MAMAHAHA_STATE )
		return true;

	switch( m_MamaHahaState )
	{
	case MHS_OWNER_ATTACH:
	case MHS_FLYING1:
	case MHS_FLYING2:
	case MHS_ENEMY_ATTACH:
	case MHS_RETURN1:
	case MHS_RETURN2:
	case MHS_FIRE_ATTACK:
		break;
	}

	return false;
}

bool ioMamaHahaDummy::CheckEnableUseCmd( int iType )
{
	if( m_State != DCS_MAMAHAHA_STATE )
		return true;

	switch( m_MamaHahaState )
	{
	case MHS_OWNER_ATTACH:
	case MHS_FLYING1:
	case MHS_FLYING2:
	case MHS_ENEMY_ATTACH:
	case MHS_RETURN2:
		return false;
	case MHS_FIRE_ATTACK:
		return true;
	case MHS_RETURN1:
		if( iType == CUT_SHICHIKAPU_ETU || iType == CUT_SHICHIKAPU_ARM )
			return true;
		break;
	}

	return false;
}

void ioMamaHahaDummy::SetDelayState( bool bNet )
{
	m_MamaHahaState = MHS_NONE;
	ClearMamaHahaWeaponList();
	m_MamaHahaTargetName.Clear();

	ioDummyChar::SetDelayState( bNet );
}

void ioMamaHahaDummy::SetAttackState()
{
	m_vAttackFireTimeList.clear();
	m_dwAttackKeyCheckTime = 0;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	AttackAttribute &rkCurAttribute = iter->second.m_AttackAttribute;
	int iSubType = iter->second.m_iSubUseType;

	switch( iSubType )
	{
	case DSUT_NONE:
		SetNormalAttackState( rkCurAttribute, (DummySubUseType)iSubType );
		break;
	case DSUT_RERA_SHU:
		SetReraShuState( rkCurAttribute, (DummySubUseType)iSubType );
		break;
	case DSUT_SHICHIKAPU_ETU:
		SetShichikapuEtuState( rkCurAttribute, (DummySubUseType)iSubType );
		break;
	case DSUT_SHICHIKAPU_ARM:
		SetShichikapuArmState( rkCurAttribute, (DummySubUseType)iSubType );
		break;
	}

	m_iReserveAttackCode = 0;
}

void ioMamaHahaDummy::SetNormalAttackState( const AttackAttribute &rkCurAttribute, DummySubUseType eSubType )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_State = DCS_MAMAHAHA_STATE;
	m_MamaHahaState = MHS_NONE;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackAniEndTime = dwCurTime;

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = rkCurAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		m_dwAttackAniEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;

		int i=0;
		AniEventConstPtrList::iterator iter;

		std::list< float >::iterator iterEndTime = vFireEndList.begin();
		for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
		{
			float fFireStart = (*iter)->fEventTime;

			FireTime kFireTime;
			kFireTime.dwStart = dwStartTime + fFireStart * fAniRate;

			if( iterEndTime != vFireEndList.end() )
			{
				kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
				++iterEndTime;
			}

			kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

			if( !(*iter)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
				if( vParam.size() >= 1 )
				{
					ioHashString szTypeName = vParam[0].c_str();
					kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

					if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
					{
						kFireTime.szSpecialCollisionBox = vParam[1].c_str();
					}
				}
			}

			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;

			m_vAttackFireTimeList.push_back( kFireTime );
		}

		SetReservedSliding( rkCurAttribute.m_vForceInfoList, iAniID, fAniRate, rkCurAttribute.m_dwPreDelay );
		// 기타 처리

		m_dwExtraAniJumpTime = 0;
		m_fExtraAniJumpPower = 0.0f;
		m_vExtraAniJumpList.clear();

		AniEventConstPtrList vExtraAniJumpList;
		m_pGroup->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

		AniEventConstPtrList::iterator iter_event = vExtraAniJumpList.begin();
		while( iter_event != vExtraAniJumpList.end() )
		{
			ExtraAniJump kInfo;

			kInfo.m_dwTime = (*iter_event)->fEventTime * fAniRate;
			kInfo.m_dwTime += dwCurTime;

			if( !(*iter_event)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter_event)->szSubInfo.c_str(), " " );
				if( vParam.size() == 1 )
				{
					kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
				}
			}

			m_vExtraAniJumpList.push_back(kInfo);

			++iter_event;
		}

		if( !m_vExtraAniJumpList.empty() )
		{
			m_iCurExtraAniJump = 0;
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
	}
}

void ioMamaHahaDummy::SetReraShuState( const AttackAttribute &rkCurAttribute, DummySubUseType eSubType )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_State = DCS_MAMAHAHA_STATE;
	m_MamaHahaState = MHS_FIRE_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackAniEndTime = dwCurTime;

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = rkCurAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		m_dwAttackAniEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;

		int i=0;
		AniEventConstPtrList::iterator iter;

		std::list< float >::iterator iterEndTime = vFireEndList.begin();
		for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
		{
			float fFireStart = (*iter)->fEventTime;

			FireTime kFireTime;
			kFireTime.dwStart = dwStartTime + fFireStart * fAniRate;

			if( iterEndTime != vFireEndList.end() )
			{
				kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
				++iterEndTime;
			}

			kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

			if( !(*iter)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
				if( vParam.size() >= 1 )
				{
					ioHashString szTypeName = vParam[0].c_str();
					kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

					if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
					{
						kFireTime.szSpecialCollisionBox = vParam[1].c_str();
					}
				}
			}

			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;

			m_vAttackFireTimeList.push_back( kFireTime );
		}

		SetReservedSliding( rkCurAttribute.m_vForceInfoList, iAniID, fAniRate, rkCurAttribute.m_dwPreDelay );
		// 기타 처리

		m_dwExtraAniJumpTime = 0;
		m_fExtraAniJumpPower = 0.0f;
		m_vExtraAniJumpList.clear();

		AniEventConstPtrList vExtraAniJumpList;
		m_pGroup->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

		AniEventConstPtrList::iterator iter_event = vExtraAniJumpList.begin();
		while( iter_event != vExtraAniJumpList.end() )
		{
			ExtraAniJump kInfo;

			kInfo.m_dwTime = (*iter_event)->fEventTime * fAniRate;
			kInfo.m_dwTime += dwCurTime;

			if( !(*iter_event)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter_event)->szSubInfo.c_str(), " " );
				if( vParam.size() == 1 )
				{
					kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
				}
			}

			m_vExtraAniJumpList.push_back(kInfo);

			++iter_event;
		}

		if( !m_vExtraAniJumpList.empty() )
		{
			m_iCurExtraAniJump = 0;
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
	}
}

void ioMamaHahaDummy::SetShichikapuEtuState( const AttackAttribute &rkCurAttribute, DummySubUseType eSubType )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_State == DCS_MAMAHAHA_STATE )
	{
		switch( m_MamaHahaState )
		{
		case MHS_OWNER_ATTACH:
		case MHS_FIRE_ATTACK:
		case MHS_FLYING1:
		case MHS_FLYING2:
		case MHS_RETURN2:
		case MHS_ENEMY_ATTACH:
			return;
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = DCS_MAMAHAHA_STATE;

	switch( m_MamaHahaState )
	{
	case MHS_RETURN1:
		{
			D3DXVECTOR3 vGap = m_vShichikapuMoveTargetPos - GetWorldPosition();
			m_fShichikapuMoveMaxRange = D3DXVec3Length( &vGap );
			D3DXVec3Normalize( &vGap, &vGap );

			m_vShichikapuMoveDir = vGap;
		}
		break;
	default:
		{
			m_fShichikapuMoveMaxRange = m_fShichikapuMoveSpeed * m_dwShichikapuMoveDuration / FLOAT1000;

			D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vCurDir, &vCurDir );

			D3DXQUATERNION qtRot;
			float fAngle = ioMath::ArrangeHead( (float)m_fShichikapuMoveAngle );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &ioMath::UNIT_Y, &vCurDir );
			D3DXVec3Normalize( &vAxis, &vAxis );

			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );

			vCurDir = qtRot * vCurDir;
			D3DXVec3Normalize( &vCurDir, &vCurDir );

			m_vShichikapuMoveTargetPos = GetWorldPosition() + (m_fShichikapuMoveMaxRange * vCurDir);
			m_vShichikapuMoveDir = vCurDir;
		}
		break;
	}

	m_fShichikapuMoveCurRange = 0.0f;
	m_MamaHahaState = MHS_FLYING1;

	//
	D3DXVECTOR3 vNewDir = m_vShichikapuMoveDir;
	vNewDir.y = 0.0f;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	D3DXQUATERNION qtNewRot;
	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );

	SetWorldOrientation( qtNewRot );
	//

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = rkCurAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );

		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;

		int i=0;
		AniEventConstPtrList::iterator iter;

		std::list< float >::iterator iterEndTime = vFireEndList.begin();
		for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
		{
			float fFireStart = (*iter)->fEventTime;

			FireTime kFireTime;
			kFireTime.dwStart = dwStartTime + fFireStart * fAniRate;

			if( iterEndTime != vFireEndList.end() )
			{
				kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
				++iterEndTime;
			}

			kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

			m_vMamaHahaWeaponList.push_back( kFireTime.dwWeaponIdx );

			if( !(*iter)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
				if( vParam.size() >= 1 )
				{
					ioHashString szTypeName = vParam[0].c_str();
					kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

					if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
					{
						kFireTime.szSpecialCollisionBox = vParam[1].c_str();
					}
				}
			}

			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;

			m_vAttackFireTimeList.push_back( kFireTime );
		}

		SetReservedSliding( rkCurAttribute.m_vForceInfoList, iAniID, fAniRate, rkCurAttribute.m_dwPreDelay );
		// 기타 처리

		m_dwExtraAniJumpTime = 0;
		m_fExtraAniJumpPower = 0.0f;
		m_vExtraAniJumpList.clear();

		AniEventConstPtrList vExtraAniJumpList;
		m_pGroup->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

		AniEventConstPtrList::iterator iter_event = vExtraAniJumpList.begin();
		while( iter_event != vExtraAniJumpList.end() )
		{
			ExtraAniJump kInfo;

			kInfo.m_dwTime = (*iter_event)->fEventTime * fAniRate;
			kInfo.m_dwTime += dwCurTime;

			if( !(*iter_event)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter_event)->szSubInfo.c_str(), " " );
				if( vParam.size() == 1 )
				{
					kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
				}
			}

			m_vExtraAniJumpList.push_back(kInfo);

			++iter_event;
		}

		if( !m_vExtraAniJumpList.empty() )
		{
			m_iCurExtraAniJump = 0;
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
	}
}

void ioMamaHahaDummy::SetShichikapuArmState( const AttackAttribute &rkCurAttribute, DummySubUseType eSubType )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = DCS_MAMAHAHA_STATE;

	switch( m_MamaHahaState )
	{
	case MHS_RETURN1:
		{
			D3DXVECTOR3 vGap = m_vShichikapuMoveTargetPos - GetWorldPosition();
			m_fShichikapuMoveMaxRange = D3DXVec3Length( &vGap );
			D3DXVec3Normalize( &vGap, &vGap );

			m_vShichikapuMoveDir = vGap;
		}
		break;
	default:
		{
			m_fShichikapuMoveMaxRange = m_fShichikapuMoveSpeed * m_dwShichikapuMoveDuration / FLOAT1000;

			D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vCurDir, &vCurDir );

			D3DXQUATERNION qtRot;
			float fAngle = ioMath::ArrangeHead( (float)m_fShichikapuMoveAngle );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &ioMath::UNIT_Y, &vCurDir );
			D3DXVec3Normalize( &vAxis, &vAxis );

			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );

			vCurDir = qtRot * vCurDir;
			D3DXVec3Normalize( &vCurDir, &vCurDir );

			m_vShichikapuMoveTargetPos = GetWorldPosition() + (m_fShichikapuMoveMaxRange * vCurDir);
			m_vShichikapuMoveDir = vCurDir;
		}
		break;
	}

	m_fShichikapuMoveCurRange = 0.0f;
	m_MamaHahaState = MHS_FLYING2;

	//
	D3DXVECTOR3 vNewDir = m_vShichikapuMoveDir;
	vNewDir.y = 0.0f;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	D3DXQUATERNION qtNewRot;
	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );

	SetWorldOrientation( qtNewRot );
	//

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = rkCurAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		m_dwAttackAniEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;

		int i=0;
		AniEventConstPtrList::iterator iter;

		std::list< float >::iterator iterEndTime = vFireEndList.begin();
		for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
		{
			float fFireStart = (*iter)->fEventTime;

			FireTime kFireTime;
			kFireTime.dwStart = dwStartTime + fFireStart * fAniRate;

			if( iterEndTime != vFireEndList.end() )
			{
				kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
				++iterEndTime;
			}

			kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

			m_vMamaHahaWeaponList.push_back( kFireTime.dwWeaponIdx );

			if( !(*iter)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
				if( vParam.size() >= 1 )
				{
					ioHashString szTypeName = vParam[0].c_str();
					kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

					if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
					{
						kFireTime.szSpecialCollisionBox = vParam[1].c_str();
					}
				}
			}

			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;

			m_vAttackFireTimeList.push_back( kFireTime );
		}

		SetReservedSliding( rkCurAttribute.m_vForceInfoList, iAniID, fAniRate, rkCurAttribute.m_dwPreDelay );
		// 기타 처리

		m_dwExtraAniJumpTime = 0;
		m_fExtraAniJumpPower = 0.0f;
		m_vExtraAniJumpList.clear();

		AniEventConstPtrList vExtraAniJumpList;
		m_pGroup->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

		AniEventConstPtrList::iterator iter_event = vExtraAniJumpList.begin();
		while( iter_event != vExtraAniJumpList.end() )
		{
			ExtraAniJump kInfo;

			kInfo.m_dwTime = (*iter_event)->fEventTime * fAniRate;
			kInfo.m_dwTime += dwCurTime;

			if( !(*iter_event)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter_event)->szSubInfo.c_str(), " " );
				if( vParam.size() == 1 )
				{
					kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
				}
			}

			m_vExtraAniJumpList.push_back(kInfo);

			++iter_event;
		}

		if( !m_vExtraAniJumpList.empty() )
		{
			m_iCurExtraAniJump = 0;
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
	}
}

void ioMamaHahaDummy::SetShichikapuReturn()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_MamaHahaTargetName.Clear();
	ClearMamaHahaWeaponList();

	switch( m_MamaHahaState )
	{
	case MHS_OWNER_ATTACH:
	case MHS_FIRE_ATTACK:
	case MHS_RETURN1:
	case MHS_RETURN2:
		return;
	case MHS_FLYING1:
		m_MamaHahaState = MHS_RETURN1;
		break;
	case MHS_FLYING2:
	case MHS_ENEMY_ATTACH:
		m_MamaHahaState = MHS_RETURN2;
		break;
	}

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

void ioMamaHahaDummy::ClearMamaHahaWeaponList()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	IntVec::iterator iter = m_vMamaHahaWeaponList.begin();
	while( iter != m_vMamaHahaWeaponList.end() )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, (*iter) );
		if( pWeapon )
		{
			pWeapon->SetWeaponDead();
		}

		iter = m_vMamaHahaWeaponList.erase( iter );
	}
}

void ioMamaHahaDummy::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

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
				ioWeapon *pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
				if( pWeapon )
				{
					pWeapon->SetPosition( GetMidPositionByRate() );

					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					if( pZone )
					{
						ioBaseChar *pChar = pZone->GetOwner();
						if( pChar )
						{
							D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
							pZone->SetCollisionDir( vDir );
							pZone->SetZoneValue( true, false );
						}
					}
				}
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	//
	switch( m_MamaHahaState )
	{
	case MHS_OWNER_ATTACH:
	case MHS_RETURN1:
	case MHS_RETURN2:
	case MHS_ENEMY_ATTACH:
	case MHS_FLYING1:
	case MHS_FLYING2:
		break;
	case MHS_FIRE_ATTACK:
		if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
		{
			SetDelayState( false );
			return;
		}
		break;
	}
}

bool ioMamaHahaDummy::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	switch( m_MamaHahaState )
	{
	case MHS_FLYING1:
	case MHS_FLYING2:
		break;
	default:
		return true;
	}

	ioOrientBox kEntityBox;
	kEntityBox.SetByAxisBox( GetWorldAxisBox() );

	if( !pShape->FindIntersection( pShapeWorldMat, kEntityBox, NULL ) )
		return false;

	SetShichikapuReturn();
	return true;
}

void ioMamaHahaDummy::CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense )
{
	ioBaseChar *pTarget = ToBaseChar(pEntity);

	switch( m_MamaHahaState )
	{
	case MHS_FLYING1:
		break;
	case MHS_FLYING2:
		if( pTarget && pTarget->GetTeam() != GetTeam() )
		{
			SetMamaHahaEnemyAttach( pTarget->GetCharName() );
			return;
		}
		break;
	default:
		return;
	}

	SetShichikapuReturn();
}

void ioMamaHahaDummy::SetMamaHahaEnemyAttach( const ioHashString &szName )
{
	m_MamaHahaState = MHS_ENEMY_ATTACH;
	m_MamaHahaTargetName = szName;

	int iAniID = m_pGroup->GetAnimationIdx( m_MamaHahaEnemyAttachAni );
	if( iAniID != -1 )
	{
		float fAniRate = FLOAT1;
		if( m_fMamaHahaEnemyAttachAniRate > 0.0f )
			fAniRate = m_fMamaHahaEnemyAttachAniRate;

		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
	}
}

void ioMamaHahaDummy::ReleaseMamaHahaEnemyAttach()
{
	SetShichikapuReturn();
}





