#include "StdAfx.h"
#include "ioShadowBoxerDummy.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"
#include "ioZoneEffectWeapon.h"

ioShadowBoxerDummy::ioShadowBoxerDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_OwnerState = CS_DELAY;
	m_OwnerOutBoxerChargeState = ioOutBoxerItem::OBCS_NONE;
	m_OwnerJumpState = JS_NONE;

	m_vOwnerPosition = ioMath::VEC3_ZERO;
	m_vDummyPosition = ioMath::VEC3_ZERO;
	m_qtOwnerRot = ioMath::QUAT_IDENTITY;

	m_pAniCollisionBoxGrp = NULL;

	m_iCurNormalAttackComboCount = -1;
	m_iCurMovingCount = 0;
	m_iCurSecondStepDefenseCount = 0;
	m_iCurExtraDashAttack = -1;

	m_bOwnerInvisibleState = false;
}

ioShadowBoxerDummy::~ioShadowBoxerDummy(void)
{
	SAFEDELETE( m_pAniCollisionBoxGrp );
}

void ioShadowBoxerDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_vDummyOffset.x = rkLoader.LoadFloat( "start_offset_x", 0.0f );
	m_vDummyOffset.y = rkLoader.LoadFloat( "start_offset_y", 0.0f );
	m_vDummyOffset.z = rkLoader.LoadFloat( "start_offset_z", 0.0f );

	int iMaxCombo = rkLoader.LoadInt_e( "max_combo", 0 );
	m_NormalAttackList.clear();
	for( int i = 0;i < iMaxCombo ;i++)
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "normal_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_NormalAttackList.push_back( kAttribute );
	}


	iMaxCombo = rkLoader.LoadInt_e( "max_dash_combo", 0 );
	m_DashAttakList.clear();
	for( int i = 0;i < iMaxCombo ;i++)
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "dash_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_DashAttakList.push_back( kAttribute );
	}


	LoadAttackAttribute( "jump_attack", m_JumpAttack, rkLoader );

	LoadAttackAttribute( "boxer_attack", m_BoxerAttack, rkLoader );

	LoadAttackAttribute( "second_step_attack", m_SecondStepAttack, rkLoader );

	LoadAttackAttribute( "counter_attack", m_CounterAttack, rkLoader );

	rkLoader.LoadString_e( "default_collision_mesh", "", szBuf, MAX_PATH );

	SAFEDELETE( m_pAniCollisionBoxGrp );
	m_pAniCollisionBoxGrp = g_CollisionBoxGrpMgr.CreateAniCollisionGrp( szBuf );

}

void ioShadowBoxerDummy::ClearStateData()
{
	m_iCurNormalAttackComboCount = -1;
	m_OwnerOutBoxerChargeState = ioOutBoxerItem::OBCS_NONE;
	m_OwnerJumpState = JS_NONE;
	m_vAttackFireTimeList.clear();
	m_iCurExtraDashAttack = -1;
}

void ioShadowBoxerDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	CheckLive();

	switch( m_State )
	{
	case DCS_START:
		ProcessStartState();
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec, dwPreTime );
		break;
	}

	CheckShadowPosition();
	CheckHide();
	/*
	ioEntityGroup *pGrp = GetOwner()->GetGroup();
	bool bLoopAni = false;
	for ( int i=0 ; i<pGrp->GetCurPlayingLoopCnt() ; ++i )
	{
		ioAnimation *pLoopAnimation = pGrp->GetLoopInPlayByOrder( i );
		if ( pLoopAnimation && pLoopAnimation->GetState() == ioAnimation::AS_PLAY && m_szLoopAni != pLoopAnimation->GetName() )
		{
			m_szLoopAni = pLoopAnimation->GetName();
			float fRate = pLoopAnimation->GetTimeFactor();
			SetLoopAni( m_szLoopAni, FLOAT1/fRate );
			m_szActionAni.Clear();
			bLoopAni = true;
			break;
		}
	}

	for ( int i=0 ; i<pGrp->GetCurPlayingLoopCnt() ; ++i )
	{
		ioAnimation *pActionAnimation = pGrp->GetActionInPlayByOrder( i );
		if ( pActionAnimation && pActionAnimation->GetState() == ioAnimation::AS_PLAY && m_szActionAni != pActionAnimation->GetName() )
		{
			m_szActionAni = pActionAnimation->GetName();
			float fRate = pActionAnimation->GetTimeFactor();
			SetActionAni( m_szActionAni, FLOAT1/fRate );
			m_szLoopAni.Clear();
		}
	}
	*/
}

void ioShadowBoxerDummy::CheckLive()
{
	ioBaseChar* pOwner = GetOwner();
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwLifeTime <= 0 || m_dwStartTime + m_dwLifeTime > dwCurTime )
		return;

	if ( m_OwnerState == CS_ATTACK )
		return;

	ioOutBoxerItem *pOutBoxerItem = ToOutBoxerItem( pOwner->GetWeapon() );
	if ( pOutBoxerItem )
		return;

	m_State = DCS_DIE;

	SendDieState( true );
}

void ioShadowBoxerDummy::CheckShadowPosition()
{
	ioBaseChar* pOwner = GetOwner();
	if ( !pOwner )
		return;

	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	if ( m_vOwnerPosition != vOwnerPos || m_qtOwnerRot != qtRot )
	{
		m_qtOwnerRot = qtRot;
		m_vOwnerPosition = vOwnerPos;
		m_vDummyPosition = m_vOwnerPosition + m_qtOwnerRot * m_vDummyOffset;
		SetWorldPosition( m_vDummyPosition );
		SetWorldOrientation( m_qtOwnerRot );
	}
}

void ioShadowBoxerDummy::CheckHide()
{
	ioBaseChar* pOwner = GetOwner();
	if ( !pOwner )
		return;

	//뉴클리어 솔져 건
	if ( m_iCurAlphaRate != pOwner->GetCurrentAlphaRate() )
	{
		m_iCurAlphaRate = pOwner->GetCurrentAlphaRate();
		SetAlphaRateDirect( m_iCurAlphaRate );
		SetAttachEffectAlphaRateDirect( m_iCurAlphaRate, true );
	}

	//일반 hide check
	if ( m_bOwnerInvisibleState != pOwner->IsCanInvisible() )
	{
		m_bOwnerInvisibleState = pOwner->IsCanInvisible();
		m_pGroup->SetVisible( !m_bOwnerInvisibleState );
	}
}

void ioShadowBoxerDummy::ProcessStartState()
{
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		ioBaseChar *pOwner = GetOwner();
		if ( !pOwner )
			return;

		ChangeNoneAttackAni( pOwner, pOwner->GetState() );

		SetDelayState( false );
	}
}

void ioShadowBoxerDummy::ProcessDelayState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;
		
	if ( m_OwnerState != pOwner->GetState() )
	{
		m_OwnerState = pOwner->GetState();

		ClearStateData();

		ChangeNoneAttackAni( pOwner, m_OwnerState );
		ChangeAttackAni( pOwner, m_OwnerState );
	}

	if ( m_OwnerState == CS_ATTACK )
		CheckOutBoxerAttackState( pOwner, dwPreTime );
	else if ( m_OwnerState == CS_JUMP )
		CheckJumpState( pOwner, dwPreTime );
	else if ( m_OwnerState == CS_DASH_ATTACK )
		CheckDashAttackState( pOwner, dwPreTime );
}

void ioShadowBoxerDummy::ChangeNoneAttackAni( ioBaseChar *pOwner, CharState OwnerState )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	switch( m_OwnerState )
	{
	case CS_DELAY:
		{
			ioHashString saAniName = pGrp->GetAnimationName( pOwner->GetDelayAnimationIdx() );
			SetLoopAni( saAniName, FLOAT1 );
		}
		break;
	case CS_RUN:
		{
			ioHashString saAniName = pGrp->GetAnimationName( pOwner->GetRunAnimationIdx() );
			SetLoopAni( saAniName, FLOAT1 );
		}
		break;
	case CS_DASH:
		{
			ioHashString saAniName = pGrp->GetAnimationName( pOwner->GetDashRunAnimationIdx() );
			SetLoopAni( saAniName, FLOAT1/pOwner->GetRunDashAniRate() );
		}
		break;
	case CS_DEFENSE:
		{
			ioHashString saAniName = pGrp->GetAnimationName( pOwner->GetDefenseAnimationIdx() );
			SetLoopAni( saAniName, FLOAT1 );
		}
		break;
	//다른 프로세스에서 처리
	case CS_JUMP:
	case CS_ATTACK:
	//ChangeAttackAni()에서 처리
	case CS_COUNTER_ATTACK:
		break;
	default:
		{
			ioHashString saAniName = pGrp->GetAnimationName( pOwner->GetDelayAnimationIdx() );
			SetLoopAni( saAniName, FLOAT1 );
		}
		break;
	}
}

void ioShadowBoxerDummy::ChangeAttackAni( ioBaseChar *pOwner, CharState OwnerState )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	switch( m_OwnerState )
	{
	case CS_COUNTER_ATTACK:
		SetAttackAni( m_CounterAttack );
		break;
	}
}

void ioShadowBoxerDummy::CheckOutBoxerAttackState( ioBaseChar *pOwner, DWORD dwPreTime )
{
	ioOutBoxerItem *pOutBoxerItem = ToOutBoxerItem( pOwner->GetWeapon() );
	if ( !pOutBoxerItem )
		return;

	if ( m_OwnerOutBoxerChargeState != pOutBoxerItem->GetOutBoxerChargeState() )
	{
		m_OwnerOutBoxerChargeState = pOutBoxerItem->GetOutBoxerChargeState();
		ioHashString szAni;
		float fRate;
		m_vAttackFireTimeList.clear();
		switch( m_OwnerOutBoxerChargeState )
		{
		case ioOutBoxerItem::OBCS_MOVE:
			{
				pOutBoxerItem->GetAnimationNameByState( m_OwnerOutBoxerChargeState, szAni, fRate );
				SetActionAni( szAni, fRate );
				m_iCurMovingCount = pOutBoxerItem->GetMoveingCount();
				
			}
		break;
		case ioOutBoxerItem::OBCS_FIRST_STEP:
		case ioOutBoxerItem::OBCS_SECOND_STEP:
			{
				pOutBoxerItem->GetAnimationNameByState( m_OwnerOutBoxerChargeState, szAni, fRate );
				SetLoopAni( szAni, fRate );
			}
			break;
		case ioOutBoxerItem::OBCS_SECOND_STEP_DEFENSE:
			{
				pOutBoxerItem->GetAnimationNameByState( m_OwnerOutBoxerChargeState, szAni, fRate );
				SetLoopAni( szAni, fRate );
				m_iCurSecondStepDefenseCount = pOutBoxerItem->GetSecondStepDefenseCount();
			}
			break;
		case ioOutBoxerItem::OBCS_BOXER_ATTACK:
			SetAttackAni( m_BoxerAttack );
			break;
		case ioOutBoxerItem::OBCS_SECOND_STEP_ATTACK:
			SetAttackAni( m_SecondStepAttack );
			break;
		}
	}

	if ( m_OwnerOutBoxerChargeState == ioOutBoxerItem::OBCS_NORMAL_ATTACK )
	{
		if ( m_iCurNormalAttackComboCount != pOutBoxerItem->GetNormalComboCount() )
		{
			m_iCurNormalAttackComboCount = pOutBoxerItem->GetNormalComboCount();
			if( COMPARE( m_iCurNormalAttackComboCount, 0, (int)m_NormalAttackList.size() ) )
				SetAttackAni( m_NormalAttackList[m_iCurNormalAttackComboCount] );
		}
	}
	else if ( m_OwnerOutBoxerChargeState == ioOutBoxerItem::OBCS_MOVE )
	{
		if ( m_iCurMovingCount != pOutBoxerItem->GetMoveingCount() )
		{
			m_iCurMovingCount = pOutBoxerItem->GetMoveingCount();
			ioHashString szAni;
			float fRate;
			pOutBoxerItem->GetAnimationNameByState( m_OwnerOutBoxerChargeState, szAni, fRate );
			SetActionAni( szAni, fRate );
		}
	}
	else if ( m_OwnerOutBoxerChargeState == ioOutBoxerItem::OBCS_SECOND_STEP_DEFENSE )
	{
		if ( m_iCurSecondStepDefenseCount != pOutBoxerItem->GetSecondStepDefenseCount() )
		{
			m_iCurSecondStepDefenseCount = pOutBoxerItem->GetSecondStepDefenseCount();
			ioHashString szAni;
			float fRate;
			pOutBoxerItem->GetAnimationNameByState( m_OwnerOutBoxerChargeState, szAni, fRate );
			SetLoopAni( szAni, fRate );
		}
	}

	CheckAttackTime( pOwner, dwPreTime );
}

void ioShadowBoxerDummy::SetActionAni( const ioHashString& szAniName, float fRate )
{
	m_pGroup->ClearAllLoopAni( FLOAT100, true );
	m_pGroup->ClearAllActionAni( FLOAT100, true );

	float fTimeRate = fRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	m_pGroup->SetActionAni( szAniName, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioShadowBoxerDummy::SetLoopAni( const ioHashString& szAniName, float fRate )
{
	m_pGroup->ClearAllLoopAni( FLOAT100, true );
	m_pGroup->ClearAllActionAni( FLOAT100, true );

	float fTimeRate = fRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	m_pGroup->SetLoopAni( szAniName, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioShadowBoxerDummy::SetAttackAni( const AttackAttribute& CurAttackAttribute )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = m_pGroup->GetAnimationIdx( CurAttackAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = CurAttackAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		DWORD dwPreDelay = CurAttackAttribute.m_dwPreDelay;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = CurAttackAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + CurAttackAttribute.m_dwPreDelay;

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

		//캐릭터를 따라 다니므로 force 및 jump 세팅이 불필요
		/*
		SetReservedSliding( CurAttackAttribute.m_vForceInfoList, iAniID, fAniRate, CurAttackAttribute.m_dwPreDelay );
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
		//
		*/
	}
}

void ioShadowBoxerDummy::CheckAttackTime( ioBaseChar *pOwner, DWORD dwPreTime )
{
	if ( !pOwner || m_vAttackFireTimeList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vCurPos = GetMidPositionByRate();
	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );
	FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
	while( iter != m_vAttackFireTimeList.end() )
	{
		const FireTime &rkFire = *iter;

		DWORD dwFireTime = rkFire.dwStart;
		if( COMPARE( dwFireTime, dwPreTime, dwCurTime ) )
		{
			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				ioWeapon *pWeapon = NULL;
				pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
				
				if( pWeapon )
				{
					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					if( pZone )
					{
						ioBaseChar *pChar = pZone->GetOwner();
						if( pChar )
						{
							pZone->SetCollisionDir( vCurDir );
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
}

void ioShadowBoxerDummy::CheckJumpState( ioBaseChar *pOwner, DWORD dwPreTime )
{
	if ( m_OwnerJumpState != pOwner->GetJumpState() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( !pGrp )
			return;
		m_OwnerJumpState = pOwner->GetJumpState();
		m_vAttackFireTimeList.clear();
		switch( m_OwnerJumpState )
		{
		case JS_JUMPPING:
			{
				ioHashString saAniName = pGrp->GetAnimationName( pOwner->GetJumppingAnimationIdx() );
				SetActionAni( saAniName, FLOAT1 );
			}
			break;
		case JS_JUMP_ATTACK:
			{
				SetAttackAni( m_JumpAttack );
			}
			break;
		case JS_LANDING:
			{
				ioHashString saAniName = pGrp->GetAnimationName( pOwner->GetJumpEndAnimationIdx() );
				SetActionAni( saAniName, FLOAT1/pOwner->GetJumpEndAnimationRate() );
			}
		}
	}

	CheckAttackTime( pOwner, dwPreTime );
}

void ioShadowBoxerDummy::CheckDashAttackState( ioBaseChar *pOwner, DWORD dwPreTime )
{
	if ( m_iCurExtraDashAttack != pOwner->GetCurExtraDashAttackCombo() )
	{
		m_iCurExtraDashAttack = pOwner->GetCurExtraDashAttackCombo();
		if( COMPARE( m_iCurExtraDashAttack, 0, (int)m_DashAttakList.size() ) )
			SetAttackAni( m_DashAttakList[m_iCurExtraDashAttack] );
	}

	CheckAttackTime( pOwner, dwPreTime );
}

ioOrientBox ioShadowBoxerDummy::GetAttackCollisionBox( AniCollisionType eType /*= ACT_NONE*/, bool bNoScale/*=false */ )
{
	AniCollisionBox *pColBox = NULL;
	float fBoxScaleRate = FLOAT1;
	float fBoxMoveOffset = 0.0f;

	ioWeaponItem *pWeapon = NULL;
	if( GetOwner()->GetEquipSlot() )
	{
		if( GetOwner()->GetObject() )
			pWeapon = GetOwner()->GetObject();
		else
			pWeapon = GetOwner()->GetEquipSlot()->GetWeapon();
	}

	if( pWeapon && eType != ACT_SPECIAL )
	{
		ioAniCollisionBoxGrp *pBoxGrp = pWeapon->GetAniCollisionBoxGrp();
		if( pBoxGrp && pBoxGrp->HasCollisionBox( eType ) )
		{
			pColBox = pBoxGrp->GetCollisionBox( eType );
			fBoxScaleRate  = pWeapon->GetCollisionBoxScale();
			fBoxMoveOffset = pWeapon->GetCollisionBoxOffset();
		}
	}

	if( !pColBox && eType == ACT_SPECIAL )
	{
		if( m_pSpecialBox && m_pSpecialBox->HasCollisionBox( eType ) )
		{
			pColBox = m_pSpecialBox->GetCollisionBox( eType );
			fBoxMoveOffset = 0.0f;
		}
	}

	if( !pColBox && m_pAniCollisionBoxGrp && m_pAniCollisionBoxGrp->HasCollisionBox( eType ) )
	{
		pColBox = m_pAniCollisionBoxGrp->GetCollisionBox( eType );
		fBoxMoveOffset = 0.0f;
	}

	ioOrientBox kBox;
	if( pColBox )
	{
		ioNode *pNode = m_pGroup->GetParentNode();
		ioBiped *pBiped = m_pGroup->GetBiped( pColBox->m_LinkBone );
		if( pNode && pBiped )
		{
			kBox = pColBox->m_Box;

			if( fBoxMoveOffset != 0.0f )
			{
				kBox.SetCenter( kBox.GetCenter() - kBox.GetAxis(2) * fBoxMoveOffset );
			}

			D3DXMATRIX matWorld;
			D3DXMatrixMultiply( &matWorld, pBiped->GetBipedSpaceFullXform(), pNode->GetFullTransform() );
			kBox.Transform( &matWorld );
		}
	}
	else
	{
		kBox = GetWorldCollisionBox();

		float fExtentsX = kBox.GetExtents( 0 );
		float fExtentsY = kBox.GetExtents( 1 );
		float fExtentsZ = kBox.GetExtents( 2 );

		float fZLength = 3.0f;

		D3DXVECTOR3 vDiff = kBox.GetAxis(0) * fExtentsX * ( fZLength + FLOAT1 );
		vDiff += kBox.GetAxis(1) * fExtentsY * FLOAT05;
		kBox.SetCenter( kBox.GetCenter() + vDiff );

		kBox.SetExtents( 0, fExtentsX * 2.0f );
		kBox.SetExtents( 1, fExtentsY * FLOAT05 );
		kBox.SetExtents( 2, fExtentsZ );
	}

	return kBox;
}

void ioShadowBoxerDummy::UpdateBound() const
{
	UpdateWorldAxisBox();
}

void ioShadowBoxerDummy::UpdateWorldAxisBox() const
{
	ioBiped *pBiped = m_pGroup->GetBiped( "Bip01" );
	if( m_OwnerState == CS_JUMP || !pBiped )
	{
		ioPlayEntity::UpdateBound();
		return;
	}

	m_WorldColBox = GetCollisionBox();
	m_WorldColBox.SetCenter( ioMath::VEC3_ZERO );

	D3DXVECTOR3 vTranslate;
	vTranslate = GetWorldOrientation() * pBiped->GetDerivedPosition() + GetWorldPosition();

	D3DXQUATERNION qtRotate;
	D3DXQuaternionMultiply( &qtRotate,
		&pBiped->GetDerivedOrientation(),
		&GetWorldOrientation() );

	D3DXVECTOR3 vScale = GetWorldScale();
	m_WorldColBox.Transform( vTranslate, vScale, qtRotate );
	m_WorldAxisBox.SetByOrientBox( m_WorldColBox );

	D3DXVECTOR3 vMin, vMax;
	vMin = m_WorldAxisBox.GetMinPos();
	vMax = m_WorldAxisBox.GetMaxPos();

	float m_fAxisCollisionBoxRate = FLOAT08;
	D3DXVECTOR3 vDiff = vMax - vMin;
	vDiff.y = 0.0f;
	vDiff *= ( FLOAT1 - m_fAxisCollisionBoxRate ) * FLOAT05;
	vMax -= vDiff;
	vMin += vDiff;
	m_WorldAxisBox.SetMinMaxPos( vMin, vMax );

	ioOrientBox kDefColBox = GetCollisionBox();
	kDefColBox.Transform( GetWorldPosition(),
		GetWorldScale(),
		GetWorldOrientation() );

	ioAxisAlignBox kDefAxisBox;
	kDefAxisBox.SetByOrientBox( kDefColBox );
	m_WorldAxisBox.Merge( kDefAxisBox );
}
