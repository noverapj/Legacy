#include "StdAfx.h"
#include "ioWitchDoctoritem.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioTargetTrace18Dummy.h"
#include "ioZoneEffectWeapon.h"

ioWitchDoctoritem::ioWitchDoctoritem(void)
{
	m_iCurCombo = 0;
}

ioWitchDoctoritem::ioWitchDoctoritem( const ioWitchDoctoritem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_ChargeAttack( rhs.m_ChargeAttack ),
m_fDummyCreateEnableRange( rhs.m_fDummyCreateEnableRange ),
m_DummyCharList( rhs.m_DummyCharList ),
m_fDummyTeleportOffset( rhs.m_fDummyTeleportOffset ),
m_fDummyTeleportSideOffset( rhs.m_fDummyTeleportSideOffset )
{
	m_iCurCombo = 0;
}

ioWitchDoctoritem::~ioWitchDoctoritem(void)
{
}

ioWeaponItem::WeaponSubType ioWitchDoctoritem::GetSubType() const
{
	return WST_WITCH_DOCTOR;
}

ioItem* ioWitchDoctoritem::Clone()
{
	return new ioWitchDoctoritem( *this );
}

void ioWitchDoctoritem::ClearData()
{
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
}

void ioWitchDoctoritem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Attack Ready
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(szBuf) );
		}
	}

	//Charge Attack
	m_fDummyCreateEnableRange = rkLoader.LoadInt_e( "dummy_create_enable_range", 0 );
	LoadAttackAttribute_e( "charge_attack", m_ChargeAttack, rkLoader );
	m_DummyCharList.clear();
	ioHashStringVec szList;
	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, szList );

	m_fDummyTeleportOffset = rkLoader.LoadFloat_e( "dummy_teleport_offset", 0.0f );
	m_fDummyTeleportSideOffset = rkLoader.LoadFloat_e( "dummy_teleport_side_offset", 0.0f );
}

void ioWitchDoctoritem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();
	ClearData();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
			pOwner->StopAutoTargetTracking();
	}
}

void ioWitchDoctoritem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_CHARGE_ATTACK:
		ProcessChargeAttack( pOwner );
		break;
	}
}

void ioWitchDoctoritem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToChargeAttackState( pOwner );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioWitchDoctoritem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( CST_NORMAL_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWitchDoctoritem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioWitchDoctoritem::ChangeToChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearData();
	m_iCurCombo++;
	if ( !COMPARE( m_iCurCombo, 0, GetMaxCombo() ) )
		m_iCurCombo = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_ChargeAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	m_ChargeState = CS_CHARGE_ATTACK;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetChargingState( false );

	float fTimeRate = m_ChargeAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_ChargeAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_ChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if ( !m_ChargeAttack.m_vWeaponInfoList.empty() )
	{
		DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
		pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_ChargeAttack.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_dwFireStartTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	
	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_ChargeAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_ChargeAttack.m_AttackAnimation, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	/*
	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}
	

	pOwner->SetAutoTarget( ATT_NORMAL );
	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	*/
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( CST_CHARGE_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWitchDoctoritem::ProcessChargeAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		m_dwMotionEndTime = 0;
		pOwner->SetState( CS_DELAY );
	}

	if ( pOwner->IsNeedProcess() )
	{
		if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
		{
			m_dwFireStartTime = 0;
			CheckDummy( pOwner );
		}
	}
}

void ioWitchDoctoritem::CheckDummy( ioBaseChar *pOwner )
{
	if ( m_DummyCharList.empty() )
		return;

	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	int iDummySize = m_DummyCharList.size();
	int iDummyLevel = -1;
	ioDummyChar *pDummy = NULL;

	for ( int i=0; i<iDummySize ; ++i )
	{
		pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_DummyCharList[i].m_DummyCharName );
		if ( pDummy )
		{
			iDummyLevel = i;
			break;
		}
	}

	//더미가 존재한다면!
	if( pDummy )
	{
		//더미가 진화 가능한 거리인지 체크
		D3DXVECTOR3 vPos = pDummy->GetWorldPosition();
		D3DXVECTOR3 vGap = pOwner->GetWorldPosition() - vPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vGap );
		float fDummyCreateEnableRange = m_fDummyCreateEnableRange * m_fDummyCreateEnableRange;
		if( fDiffRangeSq > fDummyCreateEnableRange )
		{
			//더미가 범위 밖에 있으면 현재 더미를 죽인다!
			ioTargetTrace18Dummy *pTraceDummy = ToTargetTrace18Dummy( pDummy );
			if ( pTraceDummy )
				pTraceDummy->SetDyingState( pOwner, true );
			else
				pDummy->SendDieState( false );
			return;
		}

		//더미가 진화 가능한지 체크
		++iDummyLevel;
		if ( COMPARE( iDummyLevel, 0, iDummySize ) )
		{
			D3DXQUATERNION qtRot = pDummy->GetWorldOrientation();
			//현재 더미는 죽인다.
			pDummy->SendDieState( false );
			//진화가 가능하다면 다음 단계 더미 생성
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
			ioDummyChar *pNewDummy = CreateDummyChar( pOwner, pStage, m_DummyCharList[iDummyLevel].m_DummyCharName, iCurIndex, vPos );
			if ( pNewDummy )
				pNewDummy->SetWorldOrientation( qtRot );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<BYTE>( CST_CREATE_DUMMY );
				kPacket << m_DummyCharList[iDummyLevel].m_DummyCharName;
				kPacket << iCurIndex;
				kPacket << vPos;
				kPacket << 0.0f;
				kPacket << qtRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		else
		{
			//이미 최종 진화 상태 더미라면
			//더미를 죽인다
			ioTargetTrace18Dummy *pTraceDummy = ToTargetTrace18Dummy( pDummy );
			if ( pTraceDummy )
				pTraceDummy->SetDyingState( pOwner, true );
			else
				pDummy->SendDieState( false );
		}
	}
	//더미가 존재하지 않는다면
	else
	{
		DummyCharLoadInfo Info = m_DummyCharList.front();
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vSideDir;
		D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSideDir, &vSideDir );
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + (Info.m_fDummyCharOffset * vDir);
		vPos += Info.m_fDummyCharSideOffset * vSideDir;

		//우선 생성 가능한 지역 체크
		float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
		//드랍존이라면 생성 하지 않는다.
		if ( fHeight <= 0 )
			return;

		//더미 생성!
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		ioDummyChar *pNewDummy = CreateDummyChar( pOwner, pStage, Info.m_DummyCharName, iCurIndex, vPos, Info.m_fDummyCharStartAngle );
		if ( pNewDummy && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<BYTE>( CST_CREATE_DUMMY );
			kPacket << Info.m_DummyCharName;
			kPacket << iCurIndex;
			kPacket << vPos;
			kPacket << Info.m_fDummyCharStartAngle;
			kPacket << pNewDummy->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

ioDummyChar* ioWitchDoctoritem::CreateDummyChar( ioBaseChar *pOwner, ioPlayStage *pStage, ioHashString szDummyCharName, int iDummyIndex, D3DXVECTOR3 vPos, float fStartAngle/* = 0 */ )
{
	if ( m_DummyCharList.empty() )
		return NULL;

	if ( !pOwner || !pStage )
		return NULL;

	ioDummyChar *pNewDummy = pStage->CreateDummyChar( szDummyCharName, iDummyIndex, 
		pOwner->GetCharName(), vPos, fStartAngle, 0 );

	return pNewDummy;
}

void ioWitchDoctoritem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btState;
	rkPacket >> btState;

	if ( btState == CST_NORMAL_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( btState == CST_CHARGE_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ChangeToChargeAttackState( pOwner );
	}
	else if ( btState == CST_CREATE_DUMMY )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if ( !pStage )
			return;
		ioHashString szDummyName;
		int iCurIndex;
		D3DXVECTOR3 vPos;
		float	fStartAngle;
		D3DXQUATERNION qtRot;

		rkPacket >> szDummyName;
		rkPacket >> iCurIndex;
		rkPacket >> vPos;
		rkPacket >> fStartAngle;
		rkPacket >> qtRot;
		ioDummyChar *pNewDummy = CreateDummyChar( pOwner, pStage, szDummyName, iCurIndex, vPos, fStartAngle );
		if ( pNewDummy )
			pNewDummy->SetWorldOrientation( qtRot );
	}
	else if ( btState == CST_CALL_DUMMY )
	{
		ioHashString szDummyName;
		D3DXVECTOR3 vPos;
		D3DXQUATERNION qtRot;
		rkPacket >> szDummyName;
		rkPacket >> vPos;
		rkPacket >> qtRot;

		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, szDummyName );
		if ( pDummy )
		{
			pDummy->SetWorldPosition( vPos );
			pDummy->SetWorldOrientation( qtRot );
		}
	}
	else if ( btState == CST_DUMMY_ATTACK )
	{
		ioHashString szDummyName;
		ioHashString szTargetName;
		rkPacket >> szDummyName;
		rkPacket >> szTargetName;
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, szDummyName );

		ioTargetTrace18Dummy *pTraceDummy = ToTargetTrace18Dummy( pDummy );
		if ( !pTraceDummy )
			return;

		pTraceDummy->TraceOwnerAttackTarget( pOwner, szTargetName );
	}
}

void ioWitchDoctoritem::CallDummyChar( ioBaseChar *pOwner )
{
	if ( m_DummyCharList.empty() )
		return;

	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iDummySize = m_DummyCharList.size();
	int iDummyLevel = -1;
	ioDummyChar *pDummy = NULL;

	for ( int i=0; i<iDummySize ; ++i )
	{
		pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_DummyCharList[i].m_DummyCharName );
		if ( pDummy )
		{
			iDummyLevel = i;
			break;
		}
	}

	//더미가 존재한다면!
	if( !pDummy )
		return;
	ioTargetTrace18Dummy *pTraceDummy = ToTargetTrace18Dummy( pDummy );
	if ( !pTraceDummy )
		return;

	if ( !pTraceDummy->IsEnableTelepoteState() )
		return;

	//텔레포트 가능한 위치인지 체크
	//D3DXVECTOR3 vDummyPos = pTraceDummy->GetWorldPosition();
	D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir.y = 0;
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + (m_fDummyTeleportOffset * vDir);
	vPos += m_fDummyTeleportSideOffset * vSideDir;

	float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if ( fHeight <= 0.0f )
		return;
	
	vPos.y = fHeight;

	pTraceDummy->SetWorldPosition( vPos );
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	pTraceDummy->SetWorldOrientation( qtRot );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( CST_CALL_DUMMY );
		kPacket << m_DummyCharList[iDummyLevel].m_DummyCharName;
		kPacket << vPos;
		kPacket << qtRot;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWitchDoctoritem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if ( fDamage <= 0.0f )
		return;

	if ( !pAttacker || !pWoundChar )
		return;

	int iDummySize = m_DummyCharList.size();
	int iDummyLevel = -1;
	ioDummyChar *pDummy = NULL;

	for ( int i=0; i<iDummySize ; ++i )
	{
		pDummy = g_DummyCharMgr.FindDummyCharToName( pAttacker, m_DummyCharList[i].m_DummyCharName );
		if ( pDummy )
		{
			iDummyLevel = i;
			break;
		}
	}

	//더미가 존재한다면!
	if( !pDummy )
		return;
	ioTargetTrace18Dummy *pTraceDummy = ToTargetTrace18Dummy( pDummy );
	if ( !pTraceDummy )
		return;

	if ( pWoundChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pAttacker, kPacket );
		kPacket << static_cast<BYTE>( CST_DUMMY_ATTACK );
		kPacket << m_DummyCharList[iDummyLevel].m_DummyCharName;
		kPacket << pWoundChar->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( !P2PNetwork::IsNetworkPlaying() || !pWoundChar->GetCreator()->IsNetworkMode() )
		pTraceDummy->TraceOwnerAttackTarget( pAttacker, pWoundChar->GetCharName() );
}

void ioWitchDoctoritem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}

void ioWitchDoctoritem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );
	DestroyAllDummy( pOwner );

}

void ioWitchDoctoritem::DestroyAllDummy( ioBaseChar *pOwner )
{
	if ( m_DummyCharList.empty() )
		return;

	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iDummySize = m_DummyCharList.size();
	int iDummyLevel = -1;
	ioDummyChar *pDummy = NULL;

	for ( int i=0; i<iDummySize ; ++i )
	{
		pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_DummyCharList[i].m_DummyCharName );
		if ( pDummy )
			pDummy->SendDieState( false );
	}
}
