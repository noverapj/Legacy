
#include "stdafx.h"

#include "ioDummyHelicopter.h"
#include "ioDummyAttachBuff3.h"

ioDummyHelicopter::ioDummyHelicopter( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_DummyHelicopterState = DHS_Start;
	m_bSetRise = false;
	m_bSetUpKey	= false;
	m_bSetDownKey = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwAttackTime = 0;
}

ioDummyHelicopter::~ioDummyHelicopter()
{
}

void ioDummyHelicopter::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_szAreaWeaponName = szBuf;
	m_vAreaWeaponOffset.x = rkLoader.LoadFloat_e( "area_weapon_offset_x", 0.0f );
	m_vAreaWeaponOffset.y = rkLoader.LoadFloat_e( "area_weapon_offset_y", 0.0f );
	m_vAreaWeaponOffset.z = rkLoader.LoadFloat_e( "area_weapon_offset_z", 0.0f );
	rkLoader.LoadString_e( "area_training_weapon_name", "", szBuf, MAX_PATH );
	m_szAreaTrainingWeaponName = szBuf;

	rkLoader.LoadString_e( "delay_ani_name", "", szBuf, MAX_PATH );
	m_szDelayAniName = szBuf;
	rkLoader.LoadString_e( "move_ani_name", "", szBuf, MAX_PATH );
	m_szMoveAniName = szBuf;
	rkLoader.LoadString_e( "rise_ani_name", "", szBuf, MAX_PATH );
	m_szRiseAniName = szBuf;

	m_nWeaponType = rkLoader.LoadInt_e( "weapon_type", NO_WEAPON_ATTRIBUTE );

	m_vWeaponOffset.x = rkLoader.LoadFloat_e( "weapon_offset_x", 0.0f );
	m_vWeaponOffset.y = rkLoader.LoadFloat_e( "weapon_offset_y", 0.0f );
	m_vWeaponOffset.z = rkLoader.LoadFloat_e( "weapon_offset_z", 0.0f );

	m_dwCheckAttackTime = rkLoader.LoadInt_e( "check_attack_time", 0 );

	m_dwCheckOwnerDelayTime = rkLoader.LoadInt_e( "check_owner_delay", 0 );

	m_bDontCheckDirKey = rkLoader.LoadBool_e( "dont_check_dir_key", false );
	m_fRotateWorldAngle = rkLoader.LoadFloat_e( "rotate_world_angle", 0.0f );

	m_CheckOwnerState = (CharState)rkLoader.LoadInt_e( "check_owner_state", CS_HELICOPTER );
}

void ioDummyHelicopter::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	if ( !CheckOwnerState() )
	{
		SetDieState( true );
	}

	switch( m_DummyHelicopterState )
	{
	case DHS_Start:
		{
			if( !m_DummyEffect.IsEmpty() )
			{
				m_pGroup->AttachEffect( m_DummyEffect, NULL );
				m_bMoveEffectCreated = true;
			}

			if ( m_fRotateWorldAngle != 0.0f )
			{
				D3DXQUATERNION qtRot;
				D3DXQuaternionRotationYawPitchRoll( &qtRot, m_fRotateWorldAngle, 0.0f, 0.0f );
				D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vDir, &vDir );
				vDir.y = 0.0f;
				D3DXVec3Normalize( &vDir, &vDir );
				D3DXQUATERNION qtRot2;
				ioMath::CalcDirectionQuaternion( qtRot2, -vDir );
				SetWorldOrientation( qtRot2 );
			}

			SetCurAreaWeaponName();
			CreateAreaWeapon();
			SetState( m_szDelayAniName, DHS_Delay );
		}
		break;
	case DHS_Delay:
		ProcessDelayState();
		break;
	case DHS_Move:
		ProcessMoveState();
		break;
	case DHS_Rise:
		ProcessRiseState();
		break;
	}

	SetAreaWeaponPos();
	CheckInputKey();
	CheckMove( fTimePerSec );
	CheckRise( fTimePerSec );
}

void ioDummyHelicopter::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) 
		return;	
	
	int iType;
	rkPacket >> iType;	

	switch ( iType )
	{
	case DHS_Delay:
		SetState( m_szDelayAniName, DHS_Delay );
		break;
	case DHS_Move:
		SetState( m_szMoveAniName, DHS_Move );
		break;
	case DHS_Rise:
		SetState( m_szRiseAniName, DHS_Rise );
		break;
	case DHS_Change_Key:
		{
			int nKey;
			rkPacket >> m_bSetUpKey;
			rkPacket >> m_bSetDownKey;
			rkPacket >> nKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nKey;

			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;
			SetWorldPosition( vNewPos );
			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;
			SetWorldOrientation( qtNewRot );

		}
		break;
	case DHS_Create_Weapon:
		{
			DWORD dwBaseWeaponIndex;
			rkPacket >> dwBaseWeaponIndex;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			ioBaseChar *pOwner = GetOwner();
			if ( !pOwner )
				return;

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = m_nWeaponType;
			kFireTime.iResistanceIdx = 0;
			kFireTime.szWoundedAni  = "";
			kFireTime.dwWoundedAniDuration = 0;
			kFireTime.bLoopWoundedAni = false;
			kFireTime.dwWeaponIdx = dwBaseWeaponIndex;
			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( pWeapon )
				pWeapon->SetPosition( vPos );
		}
		break;
	case DHS_Create_Area_Weapon:
		{
			ioBaseChar *pOwner = GetOwner();
			if ( !pOwner )
				return;

			SetCurAreaWeaponName();

			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vPos = GetWorldPosition() + qtRot * m_vAreaWeaponOffset;
			ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_szCurAreaWeaponName, vPos, qtRot, ioAreaWeapon::CT_NORMAL );
		}
		break;
	}
}

bool ioDummyHelicopter::IsActivateWeapon()
{
	return true;
}

ApplyWeaponExp ioDummyHelicopter::ApplyWeapon( ioWeapon *pWeapon )
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

void ioDummyHelicopter::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
}

void ioDummyHelicopter::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ProcessEnd();

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioDummyHelicopter::IsNeedCheckMapCollision() const
{
	return !m_bDisableMapCollision;
}

int ioDummyHelicopter::DontMoveEntityLevel() const
{
	return 1;
}

void ioDummyHelicopter::SetRiseState( bool bRise, ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_bSetRise = bRise;

	if ( bRise )
	{
		SetState( m_szRiseAniName, DHS_Rise );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DHS_Rise;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDummyHelicopter::SetState( ioHashString szAniName, DummyHelicopterState eState )
{
	if ( !m_pGroup || szAniName.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( szAniName );
	if( iAniID == -1 )
		return;
	
	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT100 );

	m_DummyHelicopterState = eState;
}

void ioDummyHelicopter::ProcessDelayState()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if ( pOwner->IsSettedDirection() && !m_bDontCheckDirKey )
	{
		SetState( m_szMoveAniName, DHS_Move );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DHS_Move;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDummyHelicopter::ProcessMoveState()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( !pOwner->IsSettedDirection() )
	{
		SetState( m_szDelayAniName, DHS_Delay );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DHS_Delay;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDummyHelicopter::ProcessRiseState()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner || !pOwner->IsNeedProcess() || m_bSetRise )
		return;

	if ( pOwner->IsSettedDirection() )
	{
		SetState( m_szMoveAniName, DHS_Move );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DHS_Move;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		SetState( m_szDelayAniName, DHS_Delay );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DHS_Delay;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDummyHelicopter::ProcessEnd()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	pOwner->RemoveBuff( m_szCheckBuff, true );

	SetCurAreaWeaponName();

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_szCurAreaWeaponName, pOwner->GetCharName() );
	if ( pAreaWeapon )
		pAreaWeapon->DestroyAreaWeapon( pStage );
}

bool ioDummyHelicopter::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCreateTime + m_dwCheckOwnerDelayTime > dwCurTime )
		return true;

	if ( pOwner->GetState() != m_CheckOwnerState )
		return false;

	return true;
}

void ioDummyHelicopter::CreateWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwBaseWeaponIndex = pOwner->GetWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nWeaponType;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

	D3DXQUATERNION qtRot = GetWorldOrientation();
	D3DXVECTOR3 vPos = GetWorldPosition() + qtRot * m_vWeaponOffset;
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
		pWeapon->SetPosition( vPos );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DHS_Create_Weapon;
		kPacket << dwBaseWeaponIndex;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyHelicopter::SetCurAreaWeaponName()
{
	if ( m_szCurAreaWeaponName.IsEmpty() )
	{
		if( m_pModeParent && ( m_pModeParent->GetModeType() == MT_TRAINING ) )
			m_szCurAreaWeaponName = m_szAreaTrainingWeaponName;
		else
			m_szCurAreaWeaponName = m_szAreaWeaponName;
	}
}

void ioDummyHelicopter::CreateAreaWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;	

	D3DXQUATERNION qtRot = GetWorldOrientation();
	D3DXVECTOR3 vPos = GetWorldPosition() + qtRot * m_vAreaWeaponOffset;
	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_szCurAreaWeaponName, vPos, qtRot, ioAreaWeapon::CT_NORMAL );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DHS_Create_Area_Weapon;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyHelicopter::SetAreaWeaponPos()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	SetCurAreaWeaponName();

	D3DXQUATERNION qtRot = GetWorldOrientation();
	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_szCurAreaWeaponName, pOwner->GetCharName() );
	if ( pAreaWeapon )
	{
		D3DXVECTOR3 vPos = GetWorldPosition() + qtRot * m_vAreaWeaponOffset;
		pAreaWeapon->SetAreaPosition( vPos, pStage );
		pAreaWeapon->SetAreaRotation( qtRot );
	}
}

void ioDummyHelicopter::CheckMove( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	bool bMove = false;
	D3DXVECTOR3 vCurDir = ioMath::VEC3_ZERO;
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		switch( m_CurDirKey )
		{
		case ioUserKeyInput::DKI_UP:
			vCurDir	+= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_DOWN:
			vCurDir	-= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_LEFT:
			vCurDir	-= ioMath::UNIT_X;
			break;
		case ioUserKeyInput::DKI_RIGHT:
			vCurDir	+= ioMath::UNIT_X;
			break;
		case ioUserKeyInput::DKI_LEFTDOWN:
			vCurDir	-= ioMath::UNIT_X;	vCurDir	-= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_LEFTUP:
			vCurDir	-= ioMath::UNIT_X;	vCurDir	+= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_RIGHTDOWN:
			vCurDir	+= ioMath::UNIT_X;	vCurDir	-= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_RIGHTUP:
			vCurDir	+= ioMath::UNIT_X;	vCurDir	+= ioMath::UNIT_Z;
			break;
		};
		bMove = true;
	}
	
	if( !bMove )
		return;
	
	D3DXVec3Normalize( &vCurDir, &vCurDir );
	D3DXVECTOR3 vTotalMove = ioMath::VEC3_ZERO;

	float fMoveSpeed = m_fMoveSpeed * fTimePerSec;
	vTotalMove = vCurDir * fMoveSpeed;	
	
	// 더미 회전
	if( vCurDir != ioMath::VEC3_ZERO )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vCurDir );
		SetWorldOrientation( qtRot );
	}

	bool bCol = false;
	if( pStage->ReCalculateMoveVectorByTerrain( this, &vTotalMove, bCol ) )
		Translate( vTotalMove );

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	SetWorldPosition( vCurPos );
}

void ioDummyHelicopter::CheckRise( float fTimePerSec )
{	
	if ( !m_bSetUpKey && !m_bSetDownKey )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	float fMapHeight = GetBottomHeight();

	float fUpDownSpeed = m_fUpDownSpeed * fTimePerSec;
	D3DXVECTOR3 vCurPos = GetWorldPosition();	

	if( m_bSetUpKey )
		vCurPos.y += fUpDownSpeed;

	if( m_bSetDownKey )
		vCurPos.y -= fUpDownSpeed;

	ioOrientBox kBox = GetWorldCollisionBox();
	float fDownHeight = vCurPos.y - kBox.GetExtents(1);
	if( fDownHeight < fMapHeight )
		vCurPos.y = fMapHeight + kBox.GetExtents(1);

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	SetWorldPosition( vCurPos );
}

void ioDummyHelicopter::CheckInputKey()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	bool bSend = false;
	if( pOwner->IsJumpKeyDown() )
	{
		if( !m_bSetUpKey )
		{
			SetRiseState( true, pOwner );
			bSend = true;
		}

		m_bSetUpKey	= true;
		m_bSetDownKey = false;
	}
	else if( pOwner->IsDefenseKeyDown() )
	{
		if( !m_bSetDownKey )
		{
			SetRiseState( true, pOwner );
			bSend = true;	
		}

		m_bSetUpKey	= false;
		m_bSetDownKey = true;
	}
	else
	{
		if( m_bSetUpKey || m_bSetDownKey )
		{
			SetRiseState( false, pOwner );
			bSend = true;			
		}

		m_bSetUpKey	= false;
		m_bSetDownKey = false;
	}

	if ( !m_bDontCheckDirKey )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_CurDirKey = eNewDirKey;
			bSend = true;	
		}
	}

	if ( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DHS_Change_Key;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		kPacket << (int)m_CurDirKey;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckAttackTime > 0 && m_dwAttackTime + m_dwCheckAttackTime < dwCurTime && pOwner->IsAttackKey() )
	{
		if ( m_dwAttackTime == 0 )
		{
			ioBuff *pBuff = pOwner->GetBuff( m_szCheckBuff );
			ioDummyAttachBuff3 *pDummyAttachBuff3 = ToDummyAttachBuff3( pBuff );
			if ( pDummyAttachBuff3 )
				pDummyAttachBuff3->SetReduceStartGauge();
		}

		m_dwAttackTime = dwCurTime;
		CreateWeapon();
	}
}