
#include "stdafx.h"

#include "ioDummyChain.h"
#include "ioAttachedDummyBuff2.h"
#include "ioZoneEffectWeapon.h"

ioDummyChain::ioDummyChain( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_bSetUpKey	= false;
	m_bSetDownKey = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_ChainActionState = CAS_Start;
	m_bTargetRot = false;
	m_bLeftRot = false;

	m_dwMotionEndTime = 0;
	m_dwReserveKeyTime = 0;

	m_dwCheckLiveTime = 0;
	m_dwCheckChargeTime = 0;
	m_szCurAttachEffect.Clear();
	m_bIgnoreKeyInput = false;
}

ioDummyChain::~ioDummyChain()
{
}

void ioDummyChain::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	
	rkLoader.LoadString_e( "delay_ani", "", szBuf, MAX_PATH );
	m_szDelayAni = szBuf;
	m_fDelayAniRate = rkLoader.LoadFloat_e( "delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "move_ani", "", szBuf, MAX_PATH );
	m_szMoveAni = szBuf;
	m_fMoveAniRate = rkLoader.LoadFloat_e( "move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "action_ani", "", szBuf, MAX_PATH );
	m_szActionAni = szBuf;
	m_fActionAniRate = rkLoader.LoadFloat_e( "action_ani_rate", FLOAT1 );

	m_dwRotateSpeed = rkLoader.LoadInt_e( "rotate_speed", 0 );

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
	m_dwChargeTime = rkLoader.LoadInt_e( "charge_time", 0 );
	m_nTargetMaxCnt = rkLoader.LoadInt_e( "target_max_cnt", 0 );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	m_ChainWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "chain_weapon_index", 0 );
	m_ChainWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "chain_weapon_resistance", 0 );

	rkLoader.LoadString_e( "chain_weapon_wound_ani", "", szBuf, MAX_PATH );
	m_ChainWeaponInfo.m_WoundedAnimation = szBuf;

	m_ChainWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "chain_weapon_wound_duration", 0 );
	m_ChainWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "chain_weapon_wound_loop_ani", false );

	m_AttachWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "attach_weapon_index", 0 );
	m_AttachWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "attach_weapon_resistance", 0 );

	rkLoader.LoadString_e( "attach_weapon_wound_ani", "", szBuf, MAX_PATH );
	m_AttachWeaponInfo.m_WoundedAnimation = szBuf;

	m_AttachWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "attach_weapon_wound_duration", 0 );
	m_AttachWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attach_weapon_wound_loop_ani", false );

	rkLoader.LoadString_e( "attach_buff", "", szBuf, MAX_PATH );
	m_szAttachBuff = szBuf;

	m_bSetCircleMoveState = rkLoader.LoadBool_e( "set_circle_move_state", false );
	m_bDontCheckRangeDie = rkLoader.LoadBool_e( "dont_check_range_die", false );
}

void ioDummyChain::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
	{
		SetDieState( true );
		return;
	}

	if ( !m_bDontCheckRangeDie )
	{
		D3DXVECTOR3 vDiff = GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );
		if( m_fControlOutRange > 0.0f && m_fControlOutRange * m_fControlOutRange < fDistanceSq )
		{
			SetDieState( true );
			return;
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckLiveTime > 0 && m_dwCheckLiveTime + m_dwLiveTime < dwCurTime )
	{
		SetEndState( pOwner );
		return;
	}

	switch( m_ChainActionState )
	{
	case CAS_Start:
		{
			if( !m_DummyEffect.IsEmpty() )
			{
				m_pGroup->AttachEffect( m_DummyEffect, NULL );
				m_bMoveEffectCreated = true;
			}

			D3DXVECTOR3 vNewPos = GetWorldPosition();
			vNewPos.y += m_fHeightOffset;
			SetWorldPosition( vNewPos );

			if ( !m_bSetCircleMoveState )
			{
				m_dwCheckLiveTime = dwCurTime;
				SetState( m_szMoveAni, m_fMoveAniRate, CAS_Move );
			}
			else
				SetCircleMoveState( pOwner );
		}
		break;
	case CAS_Move:
		ProcessMoveState( pOwner, fTimePerSec );
		break;
	case CAS_Check_Charge:
		ProcessCheckChargeState( pOwner );
		break;
	case CAS_Action:
		ProcessActionState( pOwner );
		break;
	case CAS_Circle_Move:
		ProcessCircleMoveState( pOwner, fTimePerSec );
		break;
	}
}

void ioDummyChain::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	if( !pStage ) 
		return;	
	
	int iType;
	rkPacket >> iType;	

	switch ( iType )
	{
	case CAS_Move:
		SetMoveState( pOwner );
		break;
	case CAS_Change_Rise_Key:
		{
			rkPacket >> m_bSetUpKey;
			rkPacket >> m_bSetDownKey;
			
			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;

			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( qtNewRot );
		}
		break;
	case CAS_Change_Dir_Key:
		{
			int nDir;
			rkPacket >> nDir;
			rkPacket >> m_bLeftRot;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nDir;
			m_bTargetRot = false;
			
			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;

			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( qtNewRot );
		}
		break;
	case CAS_Check_Charge:
		{
			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;

			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( qtNewRot );

			SetCheckChargeState( pOwner );
		}
		break;
	case CAS_Action:
		{
			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;

			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( qtNewRot );

			SetActionState( pOwner );
		}
		break;
	case CAS_Create_Weapon:
		{
			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;

			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( qtNewRot );

			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;

			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			int nTargetCnt;
			rkPacket >> nTargetCnt;

			dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
			DWORD dwWeaponIndex = dwWeaponBaseIndex;

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();
			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;
			kFireTime.iAttributeIdx = m_ChainWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = m_ChainWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni = m_ChainWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = m_ChainWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = m_ChainWeaponInfo.m_bWoundedLoopAni;

			D3DXVECTOR3 vPos = GetMidPositionByRate();
			D3DXVECTOR3 vDir = GetMoveDir();
			D3DXVec3Normalize( &vDir, &vDir );

			if ( nTargetCnt > 0 )
			{
				for ( int i=0; i<nTargetCnt; i++ )
				{					
					D3DXVECTOR3 vTargetDir;
					rkPacket >> vTargetDir;
					D3DXVec3Normalize( &vTargetDir, &vTargetDir );

					kFireTime.dwWeaponIdx = dwWeaponIndex++;

					ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
					if ( pWeapon )
						pWeapon->SetMoveDir( vTargetDir );
				}
			}
			else
			{
				kFireTime.dwWeaponIdx = dwWeaponIndex++;
				pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			}

			kFireTime.iAttributeIdx  = m_AttachWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = m_AttachWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni   = m_AttachWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = m_AttachWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = m_AttachWeaponInfo.m_bWoundedLoopAni;
			kFireTime.dwWeaponIdx = dwWeaponIndex++;

			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if( pWeapon )
			{
				ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
				if( pZone )
				{
					pZone->SetCollisionDir( vDir );
					pZone->SetZoneValue( true, false );
				}
			}
		}
		break;
	case CAS_Teleport:
		SetTeleportState( pOwner );
		break;
	case CAS_End:
		SetDieState( true );
		break;
	}
}

bool ioDummyChain::IsActivateWeapon()
{
	return true;
}

ApplyWeaponExp ioDummyChain::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	m_LastAttacker = pWeapon->GetOwnerName();

	float fDamage = pWeapon->GetDamage( this ) * m_fDamageRate;
	float fPushPower = pWeapon->GetDefensePushPower( fDamage );

	if( !m_bNoDamage && !m_bPiercingProtected )
		m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 )
		bContactType = true;

	bool bStructWeapon = false;
	if( pWeapon->CheckWeaponByStruct() || pWeapon->CheckWeaponByDummyChar() )
		bStructWeapon = true;

	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);

	if( bContactType && !bStructWeapon && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
			&& !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iShakeCamera = pWeapon->GetShakeCamera();
	ActionStopType eActionStop = pWeapon->GetActionStopType();

	if( eActionStop == AST_BOTH || eActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
		pWeapon->SetActionStopDuration( ioBaseChar::CalcActionStopDelay( fDamage, false ),
			ioBaseChar::GetActionStopLooseRate( false ) );
	}

	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		SetDieState( true, true );
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	return AWE_EXPLOSION;
}

void ioDummyChain::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
}

void ioDummyChain::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	if ( !m_szCurAttachEffect.IsEmpty() )
		m_pGroup->EndEffect( m_szCurAttachEffect, true );
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioDummyChain::IsNeedCheckMapCollision() const
{
	return !m_bDisableMapCollision;
}

int ioDummyChain::DontMoveEntityLevel() const
{
	return 1;
}

void ioDummyChain::SetState( ioHashString szAniName, float fAniRate, ChainActionState eState )
{
	if ( !m_pGroup || szAniName.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( szAniName );
	if( iAniID == -1 )
		return;
	
	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_ChainActionState = eState;
}

void ioDummyChain::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	SetState( m_szMoveAni, m_fMoveAniRate, CAS_Move );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << CAS_Move;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChain::SetCheckChargeState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_dwCheckChargeTime = FRAMEGETTIME();
	SetState( m_szDelayAni, m_fDelayAniRate, CAS_Check_Charge );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << CAS_Check_Charge;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChain::SetActionState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( !m_pGroup )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( m_szActionAni );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_fActionAniRate;

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + m_pGroup->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwReserveKeyTime = dwCurTime + m_pGroup->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_ChainActionState = CAS_Action;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << CAS_Action;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChain::SetTeleportState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	const BaseCharList vTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=vTargetList.begin(); iter!=vTargetList.end(); ++iter )
	{
		ioBaseChar *pTargetChar = *iter;
		if( pTargetChar != pOwner )
		{
			if ( pOwner->GetTeam() == pTargetChar->GetTeam() )
				continue;

			ioBuff *pBuff = pTargetChar->GetBuff( m_szAttachBuff, pOwner->GetCharName() );
			ioAttachedDummyBuff2 *pAttachedDummyBuff2 = ToAttachedDummyBuff2( pBuff );
			if ( pAttachedDummyBuff2 )
				pAttachedDummyBuff2->SetTeleportState();
		}
	}

	SetDieState( true );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << CAS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChain::SetEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	SetDieState( true );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << CAS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChain::ProcessMoveState( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	if ( !CheckAttackKeyInput( pOwner ) )
	{
		CheckDirKeyInput( pOwner );
		CheckRiseKeyInput( pOwner );

		CheckMove( pOwner, fTimePerSec );
		CheckRise( pOwner, fTimePerSec );
	}
	else
		SetCheckChargeState( pOwner );
}

void ioDummyChain::ProcessCheckChargeState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( pOwner->IsAttackKeyRelease() )
	{
		SetActionState( pOwner );
		return;
	}

	if( m_dwCheckChargeTime + m_dwChargeTime <= FRAMEGETTIME() )
		SetTeleportState( pOwner );
}

void ioDummyChain::ProcessActionState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
		SetMoveState( pOwner );
	else
	{
		if ( m_dwReserveKeyTime > 0 && m_dwReserveKeyTime < dwCurTime )
			CreateWeapon( pOwner );
	}
}

bool ioDummyChain::CheckAttackKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( pOwner->IsAttackKey() )
		return true;

	return false;
}

bool ioDummyChain::CheckDirKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return false;
	
	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_bTargetRot = false;
			m_bLeftRot = false;

			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vDir = GetMoveDir();
			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				D3DXQUATERNION qtCurRot = GetWorldOrientation();
				float fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fCurYaw = ioMath::ArrangeHead( fCurYaw );

				float fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << CAS_Change_Dir_Key;
				kPacket << m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << GetWorldPosition();
				kPacket << GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return true;
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << CAS_Change_Dir_Key;
				kPacket << m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << GetWorldPosition();
				kPacket << GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return true;
		}
	}

	return false;
}

void ioDummyChain::CheckRiseKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	bool bSend = false;
	if( pOwner->IsJumpKeyDown() )
	{
		if( !m_bSetUpKey )
			bSend = true;

		m_bSetUpKey	= true;
		m_bSetDownKey = false;
	}
	else if( pOwner->IsDefenseKeyDown() )
	{
		if( !m_bSetDownKey )
			bSend = true;

		m_bSetUpKey	= false;
		m_bSetDownKey = true;
	}
	else
	{
		if( m_bSetUpKey || m_bSetDownKey )
			bSend = true;

		m_bSetUpKey	= false;
		m_bSetDownKey = false;
	}
	
	if ( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << CAS_Change_Rise_Key;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChain::CheckCircleDirKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;
	
	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_CurDirKey = eNewDirKey;

			float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );
			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
			SetWorldOrientation( qtRotate );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << CAS_Change_Dir_Key;
				kPacket << m_CurDirKey;
				kPacket << false;
				kPacket << GetWorldPosition();
				kPacket << GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << CAS_Change_Dir_Key;
				kPacket << m_CurDirKey;
				kPacket << false;
				kPacket << GetWorldPosition();
				kPacket << GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioDummyChain::CheckMove( ioBaseChar *pOwner, float fTimePerSec )
{
	if( !pOwner )	
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	CheckDir( pOwner );

	D3DXVECTOR3 vTotalMove = GetMoveDir() * m_fMoveSpeed * fTimePerSec;	
	Translate( vTotalMove );

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	SetWorldPosition( vCurPos );
}

void ioDummyChain::CheckDir( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_dwRotateSpeed <= 0 || m_bTargetRot || m_CurDirKey == ioUserKeyInput::DKI_NONE ) 
		return;
	
	float fRate = g_FrameTimer.GetCurFrameGap() / m_dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = GetWorldOrientation();
	float fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fAngleGap ) < fRotateAngle )
	{
		m_bTargetRot = true;
		SetWorldOrientation( qtRotate );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	D3DXVECTOR3 vDir = qtAngle * GetMoveDir();
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );
	SetWorldOrientation( qtAngle );
}

void ioDummyChain::CheckRise( ioBaseChar *pOwner, float fTimePerSec )
{	
	if ( !m_bSetUpKey && !m_bSetDownKey )
		return;

	if( !pOwner )	
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	float fUpDownSpeed = m_fUpDownSpeed * fTimePerSec;
	D3DXVECTOR3 vCurPos = GetWorldPosition();	

	if( m_bSetUpKey )
		vCurPos.y += fUpDownSpeed;

	if( m_bSetDownKey )
		vCurPos.y -= fUpDownSpeed;

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	SetWorldPosition( vCurPos );
}

D3DXVECTOR3 ioDummyChain::GetMoveDir()
{
	return GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
}

void ioDummyChain::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	m_dwReserveKeyTime = 0;

	FindTarget( pOwner );
	
	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwWeaponIndex = dwWeaponBaseIndex;
	
	D3DXVECTOR3 vPos = GetMidPositionByRate();
	D3DXVECTOR3 vDir = GetMoveDir();
	D3DXVec3Normalize( &vDir, &vDir );

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
	kFireTime.m_iExtraValue = m_iCreateIndex;
	kFireTime.iAttributeIdx = m_ChainWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_ChainWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni = m_ChainWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_ChainWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_ChainWeaponInfo.m_bWoundedLoopAni;

	std::vector<D3DXVECTOR3> FireDirList;
	
	int nTargetCnt = (int)m_SortList.size();
	if ( nTargetCnt > 0 )
	{
		for ( int i=0; i<nTargetCnt; i++ )
		{
			kFireTime.dwWeaponIdx = dwWeaponIndex++;

			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( pWeapon )
			{
				ioBaseChar *pTarget = pOwner->GetBaseChar( m_SortList[i].szCharName );
				if ( pTarget )
				{		
					vDir = pTarget->GetMidPositionByRate() - vPos;
					D3DXVec3Normalize( &vDir, &vDir );
					pWeapon->SetMoveDir( vDir );
				}
				FireDirList.push_back( vDir );
			}
		}		
	}
	else
	{
		kFireTime.dwWeaponIdx = dwWeaponIndex++;
		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}

	kFireTime.iAttributeIdx  = m_AttachWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_AttachWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni   = m_AttachWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration  = m_AttachWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_AttachWeaponInfo.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = dwWeaponIndex++;
		
	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if( pWeapon )
	{
		ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
		if( pZone )
		{
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << CAS_Create_Weapon;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		kPacket << dwWeaponBaseIndex;
		kPacket << nTargetCnt;
		for ( int i=0; i<nTargetCnt; i++ )
			kPacket << FireDirList[i];
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioDummyChain::FindTarget( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;
	
	SortTargetList( pOwner, pStage->GetBaseCharList() );
	return true;
}

void ioDummyChain::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList )
{
	if( !pOwner )	
		return;

	if ( TargetList.empty() )
		return;

	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.clear();

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = GetWorldPosition();

	SortList TempSortList;
	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pTargetChar = *iter;
		if( pTargetChar != pOwner )
		{
			if ( pOwner->GetTeam() == pTargetChar->GetTeam() )
				continue;

			if ( !IsEnableTarget( pTargetChar ) )
				continue;

			vDiff = pTargetChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );
			if ( fCurDistSq > m_fTargetRange * m_fTargetRange )
				continue;

			DistInfo sDist;
			sDist.szCharName = pTargetChar->GetCharName();
			sDist.fDistSq = fCurDistSq;
			TempSortList.push_back( sDist );
		}
	}

	std::sort( TempSortList.begin(), TempSortList.end() );

	if ( m_nTargetMaxCnt > 0 )
	{
		int nTargetCnt = 0;
		for ( int i=0; i<(int)TempSortList.size(); i++ )
		{
			if ( nTargetCnt >= m_nTargetMaxCnt )
				break;

			m_SortList.push_back( TempSortList[i] );
			nTargetCnt++;
		}
	}
	else
		m_SortList = TempSortList;
}

bool ioDummyChain::IsEnableTarget( ioBaseChar *pTarget )
{
	if ( !pTarget || pTarget->HasHideBuff() )
		return false;
	
	CharState eTargetState = pTarget->GetState();
	switch( eTargetState )
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
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return false;
	case CS_BLOW_WOUNDED:
		{
			if ( !pTarget->IsFloatingState() )
				return false;
		}
	case CS_BOUND_BLOW_WOUNDED:
		{
			if ( !pTarget->IsFloatingState() )
				return false;
		}
	case CS_ETC_STATE:
		{
			if ( pTarget->IsApplyDownState(false) )
				return false;
		}
	case CS_USING_SKILL:
		{
			if ( pTarget->IsApplyDownState(false) )
				return false;
		}
	}

	return true;
}

void ioDummyChain::SetCircleMoveState( ioBaseChar *pOwner )
{
	m_ChainActionState = CAS_Circle_Move;
}

void ioDummyChain::ProcessCircleMoveState( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_bIgnoreKeyInput )
		return;

	CheckCircleDirKeyInput( pOwner );

	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		D3DXVECTOR3 vCurPos = GetWorldPosition();
		D3DXVECTOR3 vTotalMove = GetMoveDir() * m_fMoveSpeed * fTimePerSec;
		D3DXVECTOR3 vNextPos = vCurPos + vTotalMove;
		float fHeight = pStage->GetMapHeightNoneEntity( vNextPos.x, vNextPos.z );
		if ( fHeight != 0.0f )
			vNextPos.y = fHeight;
		D3DXVECTOR3 vDiff = vNextPos - pOwner->GetWorldPosition();
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );
		if( m_fControlOutRange * m_fControlOutRange < fDistanceSq )
			return;
		SetWorldPosition( vNextPos );
	}
}

void ioDummyChain::SetAttachEffect( const ioHashString& szEffectName )
{
	if ( !m_szCurAttachEffect.IsEmpty() )
		m_pGroup->EndEffect( m_szCurAttachEffect, true );

	m_pGroup->AttachEffect( szEffectName, NULL );
	m_szCurAttachEffect = szEffectName;
}