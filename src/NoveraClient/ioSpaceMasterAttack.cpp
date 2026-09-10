

#include "stdafx.h"

#include "ioSpaceMasterAttack.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioDummyBuff.h"

#include "WeaponDefine.h"
#include "FindPredicateImpl.h"

ioSpaceMasterAttack::ioSpaceMasterAttack()
{
	Init();
}

ioSpaceMasterAttack::ioSpaceMasterAttack( const ioSpaceMasterAttack &rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_dwSkillJumpPower( rhs.m_dwSkillJumpPower ),
	m_fJumpAutoTargetRange( rhs.m_fJumpAutoTargetRange ),
	m_fJumpAutoTargetMinAngle( rhs.m_fJumpAutoTargetMinAngle ),
	m_fJumpAutoTargetMaxAngle( rhs.m_fJumpAutoTargetMaxAngle ),
	m_SpaceAttack( rhs.m_SpaceAttack ),
	m_dwCharAttachWeaponIdx( rhs.m_dwCharAttachWeaponIdx ),
	m_fCharAttachWeaponOffsetZ( rhs.m_fCharAttachWeaponOffsetZ ),
	m_fCharAttachWeaponOffsetY( rhs.m_fCharAttachWeaponOffsetY ),
	m_LoopAnimation( rhs.m_LoopAnimation ),
	m_fLoopAnimationRate( rhs.m_fLoopAnimationRate ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_dwCharRotateTime( rhs.m_dwCharRotateTime ),
	m_TargetCatchDelayAnimation( rhs.m_TargetCatchDelayAnimation ),
	m_fTargetCatchDelayAnimationRate( rhs.m_fTargetCatchDelayAnimationRate ),
	m_dwTargetCatchDelayStartDuration( rhs.m_dwTargetCatchDelayStartDuration ),
	m_nTargetThrowMaxCount( rhs.m_nTargetThrowMaxCount ),
	m_fEndTargetThrowMoveRange( rhs.m_fEndTargetThrowMoveRange ),
	m_TargetBuff( rhs.m_TargetBuff ) 
{
	Init();
}

ioSpaceMasterAttack::~ioSpaceMasterAttack()
{
}

void ioSpaceMasterAttack::Init()
{
	m_SkillState					= SS_PRE;
	
	m_dwLoopStartTime				= 0;
	m_dwTargetCatchDelayStartTime	= 0;

	m_dwSelectWeaponIdx				= 0;

	m_nTargetThrowCount				= 1;

	m_vAutoTargetDir				= D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_TargetCharNameList.clear();
}

void ioSpaceMasterAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwSkillProtectTime		= rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_dwSkillJumpPower			= rkLoader.LoadInt_e( "skill_jump_power", 0 );

	//Auto Target
	m_fJumpAutoTargetRange		= rkLoader.LoadFloat_e( "jump_autotarget_range", 300.0f );
	m_fJumpAutoTargetMinAngle	= rkLoader.LoadFloat_e( "jump_autotarget_min_angle", 60.0f );
	m_fJumpAutoTargetMaxAngle	= rkLoader.LoadFloat_e( "jump_autotarget_max_angle", 150.0f );

	//Space
	LoadWeaponInfo( "space_weapon", m_SpaceAttack, rkLoader );

	//Loop
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation				= szBuf;
	m_fLoopAnimationRate		= rkLoader.LoadFloat_e( "loop_animation_rate", FLOAT1 );
	m_dwLoopDuration			= rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwCharRotateTime			= rkLoader.LoadInt_e( "char_rotate_speed", 0 );
	m_dwCharAttachWeaponIdx		= rkLoader.LoadInt_e( "char_attach_weapon", 0 );
	m_fCharAttachWeaponOffsetZ	= rkLoader.LoadFloat_e( "char_attach_weapon_offset_z", 50.0f );
	m_fCharAttachWeaponOffsetY	= rkLoader.LoadFloat_e( "char_attach_weapon_offset_y", 50.0f );

	//Throw
	rkLoader.LoadString_e( "target_catch_delay_animation", "", szBuf, MAX_PATH );
	m_TargetCatchDelayAnimation			= szBuf;
	m_fTargetCatchDelayAnimationRate	= rkLoader.LoadFloat_e( "target_catch_delay_animation_rate", FLOAT1 );
	m_dwTargetCatchDelayStartDuration	= rkLoader.LoadInt_e( "target_catch_delay_duration", 0 );	
	m_nTargetThrowMaxCount				= rkLoader.LoadInt_e( "target_catch_max_count", 2 );

	//End
	m_fEndTargetThrowMoveRange			= rkLoader.LoadFloat_e( "end_targe_thorw_range", 0.0f );

	//Buff
	rkLoader.LoadString_e( "target_buff", "", szBuf, MAX_PATH );
	m_TargetBuff = szBuf;
}

ioSkill* ioSpaceMasterAttack::Clone()
{
	return new ioSpaceMasterAttack( *this );
}

bool ioSpaceMasterAttack::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	RemoveWeapon( pChar );

	Init();
	
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( GetSkillUseType() == SUT_NORMAL && 0 < m_dwSkillJumpPower )
	{
		pChar->SetJumpPower( m_dwSkillJumpPower );
	}

	//AutoTarget Direction
	ioBaseChar* pTarget = pChar->GetBaseChar( pChar->GetAutoTargetName() );
	if( pTarget )
	{
		m_vAutoTargetDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		D3DXVec3Normalize( &m_vAutoTargetDir, &m_vAutoTargetDir );
	}
	else
	{
		m_vAutoTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &m_vAutoTargetDir, &m_vAutoTargetDir );
	}

	return true;
}

void ioSpaceMasterAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_PRE:
		{
			CheckPreState( pChar, pStage );
		}
		break;
	case SS_ATTACK:
		{
			CheckActionState( pChar, pStage );
		}
		break;
	case SS_LOOP:
		{
			ProcessCharRotate( pChar, pStage );
			CheckLoopState( pChar, pStage );
		}
		break;
	case SS_TARGET_THROW:
		{
			CheckTargetThrowState( pChar, pStage );
		}
		break;
	case SS_END:
		{
		}
		break;
	}
}

void ioSpaceMasterAttack::CheckPreState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{	
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );

		if( pChar->IsNeedProcess() )
		{
			SetSkillActionState( pChar, pStage );
		}
	}
}

void ioSpaceMasterAttack::SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_ATTACK;
	ioAttackSkill::AttackSkillFire( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << (byte)SSS_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSpaceMasterAttack::CheckActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		DWORD dwAttackCount = 0;
		for( DWORDVec::iterator iter = m_MissileWeaponList.begin(); iter != m_MissileWeaponList.end(); ++iter )
		{
			ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, *iter );
			if( pWeapon )
			{
				dwAttackCount++;
			}
		}

		if( dwAttackCount == 0 )
		{
			if( m_TargetCharNameList.empty() )
			{
				SetEndState( pChar, pStage, false, true );
			}
			else
			{
				if( pChar->IsNeedProcess() )
				{
					SetSkillLoopState( pChar, pStage, true );
				}
			}
		}
	}
}

void ioSpaceMasterAttack::SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock )
{
	if( !pOwner || !pTarget || pOwner->GetTeam() == pTarget->GetTeam() || !pTarget->HasBuff( m_TargetBuff ) )
		return;

	AddTarget( pTarget );
}

void ioSpaceMasterAttack::SetSkillLoopState( ioBaseChar* pChar, ioPlayStage* pStage, bool bCallSpaceWeapon )
{
	m_SkillState		= SS_LOOP;
	m_dwLoopStartTime	= FRAMEGETTIME();

	if( m_LoopAnimation.IsEmpty() || !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/m_fLoopAnimationRate );

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	if( bCallSpaceWeapon )
	{
		for( WeaponInfoList::iterator iter = m_SpaceAttack.m_vWeaponInfoList.begin(); iter != m_SpaceAttack.m_vWeaponInfoList.end(); ++iter )
		{
			const WeaponInfo& Info = *iter;
			ioSelectPositionMoveWeapon* pWeapon = ToSelectPositionMoveWeapon( pChar->SkillFireExplicit( Info, GetName(), pChar->IncreaseWeaponIndexBase()+1, vPos, vDir ) );
			if( pWeapon )
			{
				m_dwSelectWeaponIdx = pWeapon->GetWeaponIndex();
				pWeapon->SetTargetList( m_TargetCharNameList );
			}
		}
	}

	ioSelectPositionMoveWeapon* pWeapon = ToSelectPositionMoveWeapon( g_WeaponMgr.FindWeapon( pChar, m_dwSelectWeaponIdx ) );
	if( pWeapon )
	{
		pWeapon->SetDisableMove( true );
		pWeapon->SetControlSkillName( GetName() );
		pWeapon->SetCurrMoveRange( 0.0f );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << (byte)SSS_LOOP;
		kPacket << bCallSpaceWeapon;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSpaceMasterAttack::CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwLoopStartTime + m_dwLoopDuration < FRAMEGETTIME() )
	{				
		if( CheckAliveWeapon( pChar ) && m_nTargetThrowCount < m_nTargetThrowMaxCount )
		{
			if( pChar->IsNeedProcess() )
			{
				SetTargetThrowState( pChar, pStage );
			}
		}
		else
		{
			if( pChar->IsNeedProcess() )
			{
				SetEndState( pChar, pStage, true, true );
			}
		}
	}
	else
	{
		if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
		{
			if( m_nTargetThrowCount < m_nTargetThrowMaxCount )
			{
				SetTargetThrowState( pChar, pStage );
			}
			else
			{
				SetEndState( pChar, pStage, true, true );
			}
		}

		ProcessWeaponPos( pChar );

		if( !CheckAliveWeapon( pChar ) && pChar-> IsNeedProcess() )
		{
			SetEndState( pChar, pStage, false, true );
		}
	}
}

void ioSpaceMasterAttack::SetEndState( ioBaseChar* pChar, ioPlayStage* pStage, bool bThrow, bool bSendNetWork )
{
	SetWeaponDir( pChar, pStage, m_fEndTargetThrowMoveRange );

	m_SkillState = SS_END;

	pChar->SetCurMoveSpeed( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetExtraMoveSpeed( 0.0f );

	if( GetSkillUseType()  == SUT_JUMP || pChar->IsCharDropZonePos() )
	{
		m_SkillEndJumpType = SEJT_JUMP;
	}
	else
	{
		m_SkillEndJumpType = SEJT_NOT_JUMP;
	}

	if( bSendNetWork )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << (byte)SSS_END;
		kPacket << bThrow;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSpaceMasterAttack::SetTargetThrowState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_nTargetThrowCount++;
	m_SkillState					= SS_TARGET_THROW;
	m_dwTargetCatchDelayStartTime	= FRAMEGETTIME();

	if( m_LoopAnimation.IsEmpty() || !pChar )
	{
		return;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_TargetCatchDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/m_fTargetCatchDelayAnimationRate );
	
	SetWeaponDir( pChar, pStage );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << (byte)SSS_TARGET_THROW;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSpaceMasterAttack::SetWeaponDir( ioBaseChar* pChar, ioPlayStage* pStage, float fRange /*= 0.0f */ )
{
	ioSelectPositionMoveWeapon* pWeapon = ToSelectPositionMoveWeapon( g_WeaponMgr.FindWeapon( pChar, m_dwSelectWeaponIdx ) );
	if( pWeapon )
	{	
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );			

		pWeapon->SetMoveDir( vDir );
		pWeapon->SetDisableMove( false );
		pWeapon->SetCurrMoveRange( 0.0f );

		if( 0.0f < fRange )
		{
			pWeapon->SetMoveRangeBySkill( fRange );
		}
	}
}

void ioSpaceMasterAttack::CheckTargetThrowState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwTargetCatchDelayStartTime + m_dwTargetCatchDelayStartDuration < FRAMEGETTIME() )
	{
		if( pChar->IsNeedProcess() )
		{
			ioSelectPositionMoveWeapon* pWeapon = ToSelectPositionMoveWeapon( g_WeaponMgr.FindWeapon( pChar, m_dwSelectWeaponIdx ) );
			if( pWeapon )
			{
				D3DXVECTOR3 vPos = pWeapon->GetPosition();
				vPos.y -= m_fCharAttachWeaponOffsetY;

				pChar->SetWorldPosition( vPos );			
				SetSkillLoopState( pChar, pStage, false );
			}
			else
			{
				SetEndState( pChar, pStage, true, true );
			}
		}
	}
	else
	{
		if( !CheckAliveWeapon( pChar ) && pChar->IsNeedProcess() )
		{
			SetEndState( pChar, pStage, false, true );
		}
	}
}

void ioSpaceMasterAttack::ProcessCharRotate( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwCharRotateTime == 0 )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec * 1000) / m_dwCharRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION qtNewRot;
	D3DXVECTOR3 vAxis = ioMath::UNIT_Y;

	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD( fRotateAngle ) );

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, true );
}

void ioSpaceMasterAttack::ProcessWeaponPos( ioBaseChar* pChar )
{
	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwSelectWeaponIdx );
	ioSelectPositionMoveWeapon* pSelectWepaon = ToSelectPositionMoveWeapon( pWeapon );
	if( pSelectWepaon )
	{
		D3DXVECTOR3 vPos	= pChar->GetWorldPosition();
		D3DXVECTOR3 vDir	= pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos				+= vDir * m_fCharAttachWeaponOffsetZ;
		vPos.y				+= m_fCharAttachWeaponOffsetY;

		pSelectWepaon->SetPosition( vPos );
	}
}

void ioSpaceMasterAttack::RemoveWeapon( ioBaseChar* pChar )
{
	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwSelectWeaponIdx );
	ioSelectPositionMoveWeapon* pSelectWepaon = ToSelectPositionMoveWeapon( pWeapon );
	if( pSelectWepaon )
	{
		pSelectWepaon->SetWeaponDead();
	}	
}

bool ioSpaceMasterAttack::CheckAliveWeapon( ioBaseChar* pChar )
{
	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwSelectWeaponIdx );
	ioSelectPositionMoveWeapon* pSelectWepaon = ToSelectPositionMoveWeapon( pWeapon );
	if( pSelectWepaon )
	{
		return true;
	}

	return false;
}

void ioSpaceMasterAttack::AddTarget( ioBaseChar* pTarget )
{
	if( !pTarget )
		return;

	for( ioHashStringVec::iterator iter = m_TargetCharNameList.begin(); iter != m_TargetCharNameList.end(); ++iter )
	{
		const ioHashString&	TargetName = *iter;
		if( TargetName == pTarget->GetCharName() )
			return;
	}
	m_TargetCharNameList.push_back( pTarget->GetCharName() );
}

void ioSpaceMasterAttack::OnSkillEndByWounded( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEndByWounded( pChar );

	if( pChar->IsNeedProcess() )
	{
		RemoveWeapon( pChar );

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << (byte)SSS_REMOVE_WEAPON;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSpaceMasterAttack::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( pChar )
		SetWeaponDir( pChar, pChar->GetCreator() );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	Init();
}

void ioSpaceMasterAttack::GetAutoTargetValue( float &fRange, float &fMinAngle, float &fMaxAngle )
{
	if( GetSkillUseType() == SUT_JUMP )
	{
		fRange		= m_fJumpAutoTargetRange;
		fMinAngle	= m_fJumpAutoTargetMinAngle;
		fMaxAngle	= m_fJumpAutoTargetMaxAngle;
	}
	else
	{
		ioSkill::GetAutoTargetValue( fRange, fMinAngle, fMaxAngle );
	}
}

bool ioSpaceMasterAttack::IsEndState() const
{
	if( 0.0f < m_fSkillEndNextJumpAmt )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioSpaceMasterAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_ATTACK:
	case SS_LOOP:
	case SS_TARGET_THROW:
		return false;
	}

	if( m_fSkillEndNextJumpAmt == 0.0f )
		return true;

	return false;
}

bool ioSpaceMasterAttack::IsAttackEndState() const
{
	if( m_fSkillEndNextJumpAmt == 0.0f )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:	
	case SS_ATTACK:
	case SS_LOOP:
	case SS_TARGET_THROW:
		return false;
	}

	return true;
}

bool ioSpaceMasterAttack::IsNoDropState() const
{
	switch( m_SkillState )
	{	
	case SS_PRE:
		return false;
	case SS_ATTACK:
	case SS_LOOP:
	case SS_TARGET_THROW:
		return true;
	}

	if( 0 < m_fSkillEndNextJumpAmt )
		return true;
	else
		return false;
}

SkillEndJumpType ioSpaceMasterAttack::GetSkillEndNextJumpMustEnable()
{
	return m_SkillEndJumpType;
}

bool ioSpaceMasterAttack::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioSpaceMasterAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon )
	{
		ioSelectPositionMoveWeapon* pSelect = ToSelectPositionMoveWeapon( pWeapon );
		if( pSelect && pSelect->GetWeaponStyle() == ioSelectPositionMoveWeapon::WS_SPACE_MASTER && m_dwSelectWeaponIdx == 0 )
		{
			pSelect->SetDisableMove( true );
		}
		else
		{
			m_MissileWeaponList.push_back( pWeapon->GetWeaponIndex() );
			pWeapon->SetMoveDir( m_vAutoTargetDir );
		}
	}
}

void ioSpaceMasterAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	byte eState;
	rkPacket >> eState;

	pChar->SetTargetRotToRotate( qtRot, true, true );
	pChar->SetWorldPosition( vPos );

	switch( eState )
	{
	case SSS_ATTACK:
		{
			SetSkillActionState( pChar, pStage );
		}
		break;
	case SSS_LOOP:
		{
			bool bCallSpaceWeapon;
			rkPacket >> bCallSpaceWeapon;

			SetSkillLoopState( pChar, pStage, bCallSpaceWeapon );
		}
		break;
	case SSS_TARGET_THROW:
		{
			SetTargetThrowState( pChar, pStage );
		}
		break;
	case SSS_REMOVE_WEAPON:
		{
			RemoveWeapon( pChar );
		}
		break;
	case SSS_END:
		{
			bool bThrow;
			rkPacket >> bThrow;
			SetEndState( pChar, pStage, bThrow, false );
		}
		break;
	}
}

bool ioSpaceMasterAttack::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	if( 0 < m_dwSkillProtectTime && FRAMEGETTIME() < m_dwSkillStartTime + m_dwSkillProtectTime )
		return true;

	return false;
}