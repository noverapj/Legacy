

#include "stdafx.h"

#include "ioControlFireSkill4.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

#include "ioBombMissileWeapon.h"

ioControlFireSkill4::ioControlFireSkill4()
{
	Init();
}

ioControlFireSkill4::ioControlFireSkill4( const ioControlFireSkill4 &rhs )
	: ioAttackSkill( rhs ),	
	m_LoopUpAnimation( rhs.m_LoopUpAnimation ),
	m_LoopCenterAnimation( rhs.m_LoopCenterAnimation ),
	m_LoopDownAnimation( rhs.m_LoopDownAnimation ),
	m_dwHorzRotateWeight( rhs.m_dwHorzRotateWeight ),
	m_dwVertRotateWeight( rhs.m_dwVertRotateWeight ),
	m_fVertUpLimitAngle( rhs.m_fVertUpLimitAngle ),
	m_fVertDownLimitAngle( rhs.m_fVertDownLimitAngle ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	Init();
}

ioControlFireSkill4::~ioControlFireSkill4()
{
}

void ioControlFireSkill4::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	
	rkLoader.LoadString_e( "skill_up_loop_motion", "", szBuf, MAX_PATH );
	m_LoopUpAnimation = szBuf;

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_LoopCenterAnimation = szBuf;

	rkLoader.LoadString_e( "skill_down_loop_motion", "", szBuf, MAX_PATH );
	m_LoopDownAnimation = szBuf;

	m_dwHorzRotateWeight	= rkLoader.LoadInt_e( "skill_loop_rotate_horz_weight", 0 );
	m_dwVertRotateWeight	= rkLoader.LoadInt_e( "skill_loop_rotate_vert_weight", 0 );

	m_fVertUpLimitAngle		= rkLoader.LoadFloat_e( "skill_loop_vert_up_limit", 0.0f );
	m_fVertDownLimitAngle	= rkLoader.LoadFloat_e( "skill_loop_vert_down_limit", 0.0f );

	m_dwLoopDuration		= rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwSkillProtectTime	= rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioControlFireSkill4::Clone()
{
	return new ioControlFireSkill4( *this );
}

void ioControlFireSkill4::Init()
{	
	m_SkillState		= SS_PRE;
	m_bSetHandMesh		= false;

	m_dwLoopStartTime	= 0;

	m_vAttackDir		= ioMath::VEC3_ZERO;

	m_fCurrMotionRate	= 0.0f;
	m_CurDirKey			= ioUserKeyInput::DKI_NONE;

	m_WeaponIndexList.clear();
}

bool ioControlFireSkill4::OnSkillStart( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillStart( pOwner );

	Init();	

	SetCameraBuff( pOwner );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( false );
	}

	return true;
}

void ioControlFireSkill4::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pOwner, pStage );
		}
		break;
	case SS_ATTACK:
		{
			CheckActionState( pOwner, pStage );
		}
		break;
	case SS_LOOP:
		{
			CheckLoopState( pOwner, pStage );
		}
	}
}

void ioControlFireSkill4::CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime  = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pOwner );
		SetSkillActionState( pOwner, pStage );
	}
}

void ioControlFireSkill4::SetSkillActionState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_ATTACK;
	AttackSkillFire( pOwner );

	m_vAttackDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
}

void ioControlFireSkill4::CheckActionState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		SetLoopState( pOwner, pStage );		
	}
}

void ioControlFireSkill4::SetLoopState( ioBaseChar* pOwner, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_LoopUpAnimation, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_LoopCenterAnimation, 0.0f, FLOAT1 );
	pGrp->SetLoopAni( m_LoopDownAnimation, 0.0f, 0.0f );

	m_SkillState		= SS_LOOP;
	m_dwLoopStartTime	= FRAMEGETTIME();
}

bool ioControlFireSkill4::CheckDirectionKey( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return false;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey	= eNewDirKey;
		return true;
	}

	return false;
}

void ioControlFireSkill4::CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_dwLoopStartTime + m_dwLoopDuration < FRAMEGETTIME() )
	{
		m_SkillState = SS_END;
	}
	else
	{		
		bool bChnage = false;
		if( CheckDirectionKey( pOwner ) )
			bChnage = true;

		ProcessRotateHorz( pOwner, m_dwHorzRotateWeight, m_CurDirKey, IsLeftRotate( pOwner, m_CurDirKey ) );

		if( pOwner->IsJumpKeyDown() && ProcessRotateVert( pOwner, m_dwVertRotateWeight, UDS_UP ) )
		{
			bChnage = true;
		}
		else if( pOwner->IsDefenseKeyDown() && ProcessRotateVert( pOwner, m_dwVertRotateWeight, UDS_DOWN ) )
		{
			bChnage = true;
		}
		ProcessRotateAnimation( pOwner );
		ProcessWeapon( pOwner );

		if( bChnage && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << static_cast<byte>( SSS_DIR );
			kPacket << pOwner->GetTargetRot();
			kPacket << static_cast<byte>( m_CurDirKey );
			kPacket << m_vAttackDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioControlFireSkill4::IsLeftRotate( ioBaseChar* pOwner, DirKey eNewDirKey )
{
	float fYawD = 0.0f;
	if( eNewDirKey == ioUserKeyInput::DKI_NONE )
		return false;

	fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );	
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir	= qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vDir		= pOwner->GetMoveDir();
	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
	{
		float fCurYaw, fTargetYaw;
		fCurYaw		= RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
		fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

		fCurYaw		= ioMath::ArrangeHead( fCurYaw );
		fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );

		float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
		if( fRotAngle < 0.0f )
			return true;
	}

	return false;
}

bool ioControlFireSkill4::IsLimitAngle( float fLimitAngle, float fCurrAngle )
{
	if( fLimitAngle > 0.0f && fLimitAngle < fCurrAngle )
		return true;

	return false;
}

void ioControlFireSkill4::ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, DirKey eDir, bool bLeft )
{
	if( !pOwner || fRotateTime <= 0 )
		return;

	if( eDir == ioUserKeyInput::DKI_NONE )
		return;

	D3DXVECTOR3 vXZDir = m_vAttackDir;
	vXZDir.y = 0.0f;
	float fXZLength = D3DXVec3Length( &vXZDir );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );	

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw			= ioMath::ArrangeHead( RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) ) );
	fTargetYaw		= ioMath::ArrangeHead( RADtoDEG( ioMath::QuaterToYaw( qtRotate ) ) );	
	float fRotAngle	= ioMath::AngleGap( fCurYaw, fTargetYaw );

	if( fabs( fRotAngle ) < fRotateAngle )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true, true );
		vXZDir = pOwner->GetMoveDir();
		vXZDir = fXZLength * vXZDir;
		vXZDir.y = m_vAttackDir.y;
		D3DXVec3Normalize( &m_vAttackDir, &vXZDir );
	}
	else
	{
		if( bLeft )
			fRotateAngle *= -1;

		fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
		D3DXVECTOR3 vDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

		vDir = qtAngle * vDir;
		ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

		pOwner->SetTargetRotToRotate( qtAngle, true, false );
		vXZDir = pOwner->GetMoveDir();
		vXZDir = fXZLength * vXZDir;
		vXZDir.y = m_vAttackDir.y;
		D3DXVec3Normalize( &m_vAttackDir, &vXZDir );
	}
}

bool ioControlFireSkill4::ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, UpDownState eState )
{
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
	float fRotateAngle = 360.0f * fRate;

	float fAngle = RADtoDEG( acosf( D3DXVec3Dot( &vDir, &m_vAttackDir ) ) );	
	float fLimit = 0.0f;
	if( m_vAttackDir.y > 0.0f )
	{
		fLimit = m_fVertUpLimitAngle;
	}
	else
	{
		fLimit = m_fVertDownLimitAngle;
	}

	D3DXVECTOR3 vAxis;
	D3DXVECTOR3 vYAxis = ioMath::UNIT_Y;	
	D3DXVec3Cross( &vAxis, &m_vAttackDir, &vYAxis );

	if( eState == UDS_UP )
	{
		float fCurrAngle = fRotateAngle;
		if( m_vAttackDir.y < 0.0f )		
			fCurrAngle = -fRotateAngle;		

		if( !IsLimitAngle( fLimit, fAngle + fCurrAngle ) )
		{
			D3DXVec3Normalize( &vAxis, &vAxis );
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fRotateAngle ) );
			m_vAttackDir = qtRot * m_vAttackDir;
			return true;
		}
	}
	else if( eState == UDS_DOWN )
	{
		float fCurrAngle = fRotateAngle;
		if( m_vAttackDir.y > 0.0f )		
			fCurrAngle = -fRotateAngle;	

		if( !IsLimitAngle( fLimit, fAngle + fCurrAngle ) )
		{
			D3DXVec3Normalize( &vAxis, &-vAxis );
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fRotateAngle ) );
			m_vAttackDir = qtRot * m_vAttackDir;
			return true;
		}
	}

	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
	return false;
}

void ioControlFireSkill4::ProcessRotateAnimation( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioAnimation *pUp		= pGrp->GetLoopInPlay( m_LoopUpAnimation );
	ioAnimation *pCenter	= pGrp->GetLoopInPlay( m_LoopCenterAnimation );
	ioAnimation *pDown		= pGrp->GetLoopInPlay( m_LoopDownAnimation );

	if( !pUp || !pCenter || !pDown )
		return;

	D3DXVECTOR3 vYAxis = ioMath::UNIT_Y;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );
	float fGapAngle = RADtoDEG( acosf( D3DXVec3Dot( &vDir, &m_vAttackDir ) ) );

	if( m_vAttackDir.y > 0.0f )
	{
		if( m_fVertUpLimitAngle > 0.0f )
			m_fCurrMotionRate = min( FLOAT1,  fGapAngle / m_fVertUpLimitAngle );
		else
			m_fCurrMotionRate = 0.0f;
	}
	else
	{
		if( m_fVertDownLimitAngle > 0.0f )
			m_fCurrMotionRate = min( FLOAT1, fGapAngle / m_fVertDownLimitAngle );
		else
			m_fCurrMotionRate = 0.0f;
	}

	//센터
	if( m_vAttackDir.y == 0.0f || m_fCurrMotionRate == 0.0f )
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	//업
	else if( m_vAttackDir.y > 0.0f )
	{
		pUp->ForceWeight( m_fCurrMotionRate );
		pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	//다운
	else
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
		pDown->ForceWeight( m_fCurrMotionRate );
	}
}


void ioControlFireSkill4::ProcessWeapon( ioBaseChar *pOwner )
{
	for( int i = 0; i < (int)m_WeaponIndexList.size(); ++i )
	{
		ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( g_WeaponMgr.FindWeapon( pOwner, m_WeaponIndexList[i] ) );
		if( pSimple )
		{
			D3DXVECTOR3 vPos	= pOwner->GetWorldPosition();			
			D3DXVECTOR3 vSide;
			D3DXVec3Cross( &vSide, &m_vAttackDir, &ioMath::UNIT_Y );
			D3DXVec3Normalize( &vSide, &vSide );
			pSimple->SetMoveDir( m_vAttackDir );
		}
	}
}

void ioControlFireSkill4::RemoveWeapon( ioBaseChar *pOwner )
{
	for( IntVec::iterator iter = m_WeaponIndexList.begin(); iter != m_WeaponIndexList.end(); )
	{
		ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( g_WeaponMgr.FindWeapon( pOwner, *iter ) );
		if( pSimple )
		{
			pSimple->SetWeaponDead();			
			iter = m_WeaponIndexList.erase( iter );
		}
		else
		{
			iter++;
		}
	}	
}

void ioControlFireSkill4::OnSkillEnd( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillEnd( pOwner );

	RemoveWeapon( pOwner );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	EndCameraBuff( pOwner );
	Init();
}

bool ioControlFireSkill4::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioControlFireSkill4::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
	if( pSimple )
	{
		RemoveWeapon( pSimple->GetOwner() );
		m_WeaponIndexList.push_back( pSimple->GetWeaponIndex() );
	}
}

void ioControlFireSkill4::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	byte eState;
	rkPacket >> eState;
	
	switch( eState )
	{
	case SSS_DIR:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			if( pOwner )
				pOwner->SetTargetRot( qtRot );

			byte eDir;
			rkPacket >> eDir;
			m_CurDirKey = (DirKey)eDir;

			rkPacket >> m_vAttackDir;
		}
		break;
	}
}

bool ioControlFireSkill4::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioControlFireSkill4::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( 0 < m_dwSkillProtectTime && FRAMEGETTIME() < m_dwSkillStartTime + m_dwSkillProtectTime )
		return true;

	return false;
}