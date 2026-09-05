
#include "stdafx.h"

#include "ioOwnerControlDummy2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


#include "WeaponDefine.h"
#include "SkillDefine.h"

ioOwnerControlDummy2::ioOwnerControlDummy2( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_OwnerControlState = OCS_CONTROL;
	m_dwLostControlTime = 0;

	m_bSetUpKey   = false;
	m_bSetDownKey = false;
}

ioOwnerControlDummy2::~ioOwnerControlDummy2()
{
}

void ioOwnerControlDummy2::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "", szKey[MAX_PATH] = "";

	m_fRotateSpeed = rkLoader.LoadFloat_e( "owner_control_rotate_speed", 0.0f );
	m_dwLostControlMoveTime = (DWORD)rkLoader.LoadInt_e( "lost_control_move_time", 0 );
}

void ioOwnerControlDummy2::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	}

	ProcessPos( fTimePerSec );
}

void ioOwnerControlDummy2::ProcessPos( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
		return;
	}

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			D3DXVECTOR3 vPos = GetWorldPosition();
			float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, this, true );
			float fHeightGap = GetWorldPosition().y - fMapHeight;
			if( fMapHeight > 0.0f && m_fMinHeight > fHeightGap )
			{
				D3DXVECTOR3 vPos = GetWorldPosition();
				vPos.y = fMapHeight + m_fMinHeight;
				SetWorldPosition( vPos );
			}
		}
	}
}

void ioOwnerControlDummy2::ProcessStartState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		m_OwnerControlState = OCS_CONTROL;
		m_dwLostControlTime = 0;

		m_bSetUpKey = false;
		m_bSetDownKey = false;
		SetMoveState();
		return;
	}
}

void ioOwnerControlDummy2::ProcessMoveState( float fTimePerSec )
{
	// 공격 체크
	CheckAttackKeyControl();
	CheckKeyInputChange();
	CheckControlMove( fTimePerSec );

	if( m_dwLostControlTime > 0 && m_dwLostControlTime + m_dwLostControlMoveTime < FRAMEGETTIME() )
	{
		SetDieState( true );
		return;
	}

	int iCnt = m_vAttachWeaponIndexList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pOwner = GetOwner();
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_vAttachWeaponIndexList[i] );
		if( !pWeapon )
		{
			SetDieState( true );
		}
	}
}

void ioOwnerControlDummy2::SetMoveState()
{
	m_State = DCS_MOVE;

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

void ioOwnerControlDummy2::SetLostControl()
{
	m_OwnerControlState = OCS_LOST_CONTROL;
	m_dwLostControlTime = FRAMEGETTIME();

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioEquipSlot *pSlot = pOwner->GetEquipSlot();
		if( pSlot )
		{
			ioControlDummySkill *pSkill1 = ToControlDummySkill( pSlot->GetEquipedSkill( m_ControlSkillName ) );
			if( pSkill1 )
			{
				pSkill1->SetSkillEndState( pOwner );
			}
			ioControlDummySkill2 *pSkill2 = ToControlDummySkill2( pSlot->GetEquipedSkill( m_ControlSkillName ) );
			if( pSkill2 )
			{
				pSkill2->SetSkillEndState( pOwner );
			}
			ioControlDummySkill3 *pSkill3 = ToControlDummySkill3( pSlot->GetEquipedSkill( m_ControlSkillName ) );
			if( pSkill3 )
			{
				pSkill3->SetSkillEndState( pOwner, pOwner->GetCreator() );
			}
		}
	}
}

void ioOwnerControlDummy2::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SYNC_ATTACK_KEY:
		SetLostControl();
		break;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioOwnerControlDummy2::CheckAttackKeyControl()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;
	
	if( m_OwnerControlState == OCS_LOST_CONTROL )
		return;

	if( pOwner->IsAttackKey() )
	{
		SetLostControl();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << SYNC_ATTACK_KEY;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioOwnerControlDummy2::CheckKeyInputChange()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_OwnerControlState != OCS_CONTROL )
		return;

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
		if( m_bSetUpKey && !pOwner->IsJumpKeyDown() )
		{
			bChange = true;
			m_bSetUpKey = false;
		}
		else if( !m_bSetUpKey && pOwner->IsJumpKeyDown() )
		{
			bChange = true;
			m_bSetUpKey = true;
		}
	}

	// Down 체크
	if( !bChange && !m_bSetUpKey )
	{
		if( m_bSetDownKey && !pOwner->IsDefenseKeyDown() )
		{
			bChange = true;
			m_bSetDownKey = false;
		}
		else if( !m_bSetDownKey && pOwner->IsDefenseKeyDown() )
		{
			bChange = true;
			m_bSetDownKey = true;
		}
	}

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_KEY_CHANGE;
		kPacket << (int)m_CurDirKey;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		kPacket << false;			// setmove...
		kPacket << GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOwnerControlDummy2::CheckControlMove( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, this, true );
	float fHeightAmt = 0.0f;

	// 수직이동
	float fUpDownSpeed = m_fUpDownSpeed * fTimePerSec;
	float fHeightGap = ( GetWorldPosition().y + fUpDownSpeed ) - fMapHeight;

	// 위로
	if( m_bSetUpKey )
	{
		// 상승 최대치에 대한 보정.		
		if( fMapHeight > 0.0f && fHeightGap < m_fMaxHeight )
		{
			fHeightAmt += fUpDownSpeed;
		}
	}

	// 아래로
	if( m_bSetDownKey )
	{
		if( fMapHeight > 0.0f && fHeightGap > m_fMinHeight )
		{
			fHeightAmt -= fUpDownSpeed;
		}
	}

	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
 		float fFrameGap = g_FrameTimer.GetCurFrameGap();
 		float fRate = fFrameGap / m_fRotateSpeed;
 		float fRotateAngle = 360.0f * fRate;

		// 목표에 도달체크 및 처리
		float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		D3DXQUATERNION qtCurRot = GetWorldOrientation();

		float fCurYaw, fTargetYaw;
		fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
		fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

		fCurYaw = ioMath::ArrangeHead( fCurYaw );
		fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

		float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
		
		if( fabs( fRotAngle ) < fRotateAngle )
		{
			SetWorldOrientationAndDir( qtRotate );
		}
		else
		{
			if( fRotAngle < 0.0f )
				fRotateAngle *= -1;

			fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

			D3DXQUATERNION qtAngle;
			D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

			D3DXVECTOR3 vDir = qtAngle * m_vAttackDir;
			ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

			SetWorldOrientationAndDir( qtAngle );
		}
	}

	m_fGravityAmt = 0.0f;

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	vCurPos.y += fHeightAmt;
	
	float fMoveValue = fTimePerSec * m_fMoveSpeed;
	vCurPos += m_vAttackDir * fMoveValue;

	ioOrientBox kBox = GetWorldCollisionBox();
	float fDownHeight = vCurPos.y - kBox.GetExtents(1);
	if( fDownHeight < fMapHeight )
	{
		vCurPos.y = fMapHeight + kBox.GetExtents(1);
	}

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	SetWorldPosition( vCurPos );
}