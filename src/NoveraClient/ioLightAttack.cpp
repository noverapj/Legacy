#include "StdAfx.h"

#include "ioLightAttack.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioLightAttack::ioLightAttack(void)
{
}

ioLightAttack::ioLightAttack( const ioLightAttack &rhs )
	: ioAttackSkill( rhs ),
m_OwnerBuffList( rhs.m_OwnerBuffList ),
m_ActionOwnerBuffList( rhs.m_ActionOwnerBuffList ),
m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_fCircleRange( rhs.m_fCircleRange ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
m_fCircleOffSet( rhs.m_fCircleOffSet ),
m_Circle( rhs.m_Circle ),
m_fCharJumpPower( rhs.m_fCharJumpPower ),
m_fJumpPowerInJump( rhs.m_fJumpPowerInJump ),
m_strPreActionAni_Normal( rhs.m_strPreActionAni_Normal ),
m_fPreActionAniRate_Normal( rhs.m_fPreActionAniRate_Normal ),
m_fJumpAngle( rhs.m_fJumpAngle ),
m_fJumpAngleInJump( rhs.m_fJumpAngleInJump ),
m_strPreActionAni_Jump( rhs.m_strPreActionAni_Jump ),
m_fPreActionAniRate_Jump( rhs.m_fPreActionAniRate_Jump ),
m_dwTeleportEndTime( rhs.m_dwTeleportEndTime ),
m_BlueMapEffect( rhs.m_BlueMapEffect ),
m_RedMapEffect( rhs.m_RedMapEffect )
{
	ClearData();
}

ioLightAttack::~ioLightAttack(void)
{
	ClearData();
}

void ioLightAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	LoadActionOwnerBuffList( rkLoader );
	LoadOwnerBuffList( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;	
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );	
	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "magic_circle", "", szBuf, MAX_PATH );
	m_Circle = szBuf;

	rkLoader.LoadString_e( "blue_map_effect", "", szBuf, MAX_PATH );
	m_BlueMapEffect = szBuf;

	rkLoader.LoadString_e( "red_map_effect", "", szBuf, MAX_PATH );
	m_RedMapEffect = szBuf;

	// Jump
	m_fCharJumpPower = rkLoader.LoadFloat_e( "jump_power", 0 );
	m_fJumpPowerInJump = rkLoader.LoadFloat_e( "jump_power_InJump", 0);

	rkLoader.LoadString_e( "pre_skill_motion", "", szBuf, MAX_PATH );
	m_strPreActionAni_Normal = szBuf;
	m_fPreActionAniRate_Normal = rkLoader.LoadFloat_e( "pre_animation_time_rate", FLOAT1 );
	
	// JumpMotion
	rkLoader.LoadString_e( "jump_pre_skill_motion", "", szBuf, MAX_PATH );
	m_strPreActionAni_Jump = szBuf;
	m_fPreActionAniRate_Jump = rkLoader.LoadFloat_e( "jump_pre_animation_time_rate", FLOAT1 );

	m_fJumpAngle = rkLoader.LoadFloat_e( "jump_angle", FLOAT1 );
	m_fJumpAngleInJump = rkLoader.LoadFloat_e( "jump_angle_InJump", FLOAT1 );

	m_dwTeleportEndTime = rkLoader.LoadInt_e( "teleport_enable_time", 50 );

}

void ioLightAttack::LoadActionOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "action_owner_buff_cnt", 0 );
	m_ActionOwnerBuffList.clear();

	if( iCnt <= 0 ) return;
	m_ActionOwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "action_owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_ActionOwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

void ioLightAttack::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;
	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

void ioLightAttack::ClearData()
{
	m_bSetHandMesh = false;
	m_State = SS_PRE;

	m_fPreActionAniRate = FLOAT1;
	m_strPreActionAni.Clear();
	m_dwCurTeleportEndTime = 0;
	m_bEnd = false;
}

ioSkill* ioLightAttack::Clone()
{
	return new ioLightAttack( *this );
}

bool ioLightAttack::OnSkillStart( ioBaseChar *pChar )
{
	if ( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if ( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_bSkillInJump = false;
	if( m_SkillUseType == SUT_JUMP )
		m_bSkillInJump = true;

	SetCameraBuff( pChar );

	pChar->SetSkillProtection();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	if( !m_bSkillInJump )
	{
		m_strPreActionAni	= m_strPreActionAni_Normal;
		m_fPreActionAniRate = m_fPreActionAniRate_Normal;
	}
	else
	{
		m_strPreActionAni = m_strPreActionAni_Jump;
		m_fPreActionAniRate = m_fPreActionAniRate_Jump;
	}

	return true;
}

void ioLightAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;
				if( pChar->IsNeedProcess() )
				{
					SetPreAction( pChar );
				}
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_PREACTION:
		{
			if( m_dwPreActionEndTime < dwCurTime )
			{
				AttackSkillFire( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				SetLoopState( pChar, pStage );
				AddActionOwnerBuff( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			if( pChar->CheckMagicCircle() && pChar->IsNeedProcess() )
			{
				m_vCirclePos = pChar->GetMagicCirclePos();
				pChar->SetTargetRotToTargetPos( m_vCirclePos, true, false );
			}

			if( pChar->IsNeedProcess() )
			{
				m_vCirclePos = pChar->GetMagicCirclePos();
				//키입력 여부 검사
				bool bReserveSkill = false;
				int iSkillNum = pChar->GetSkillKeyInput();
				if( iSkillNum >= 0 )
				{
					bReserveSkill = true;
				}

				if( pChar->IsAttackKey() || pChar->IsDefenseKey() || pChar->IsJumpKey() && !bReserveSkill )
				{
					SetTeleport( pChar, true );
				}

				if( 0 < m_dwCurTeleportEndTime && m_dwCurTeleportEndTime < dwCurTime )
				{
					SetTeleport( pChar, true );
				}
			}
		}
		break;
	case SS_TELEPORT:
		{
			m_State = SS_END;
			RemoveActionOwnerBuff( pChar );			
		}
		break;
	case SS_END:
		{			
			AddOwnerBuff( pChar );
			m_bEnd = true;
		}
		break;
	}
}

void ioLightAttack::SetPreAction( ioBaseChar *pChar )
{
	if ( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_strPreActionAni );
	if ( iCurSkillAniID == -1 ) return;

	float fTimeRate = m_fPreActionAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	float fJumpAngle = m_bSkillInJump ?	m_fJumpAngleInJump : m_fJumpAngle;

	//////////////////////////////////////////////////////////////////////////
	float fAngleH = ioMath::ArrangeHead( 0 );
	float fAngleV = ioMath::ArrangeHead( fJumpAngle );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	vTargetDir = qtRot * vTargetDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	vTargetDir = qtRot * vTargetDir;
	//////////////////////////////////////////////////////////////////////////

	if( !m_bSkillInJump )
		pChar->SetForcePower( vTargetDir, fabs(m_fCharJumpPower), FLOAT1, true );
	else
		pChar->SetForcePower( vTargetDir, fabs(m_fJumpPowerInJump), FLOAT1, true );

	DWORD dwCurTime = FRAMEGETTIME();
	
	m_dwPreActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_State = SS_PREACTION;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_PREACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioLightAttack::AttackSkillFire( ioBaseChar *pChar )
{
	if ( !pChar )
		return;	
	m_State = SS_ACTION;

	ioAttackSkill::AttackSkillFire( pChar );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	
}

void ioLightAttack::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;
	
	pChar->SetInvisibleState( true, true );
	pChar->SetSpecialHide( true );

	m_dwCurTeleportEndTime = dwCurTime;
	m_dwCurTeleportEndTime += m_dwTeleportEndTime;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_Circle, m_Circle, szDummyCircle, m_fCircleOffSet );
	}
}

void ioLightAttack::SetTeleport( ioBaseChar *pChar, bool bSend )
{
	if( !pChar ) return;
	
	auto pCreator = pChar->GetCreator();
	if ( !pCreator ) return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect* pMapEffect = nullptr;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pCreator->CreateMapEffect( m_RedMapEffect, m_vCirclePos, vScale );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pCreator->CreateMapEffect( m_BlueMapEffect, m_vCirclePos, vScale );
	}
	
	if( pMapEffect )
	{
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();
	}

	pChar->SetWorldPosition( m_vCirclePos );
	m_State = SS_TELEPORT;
	
	ioEntityGroup *pGrp = pChar->GetGroup();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pChar->SetInvisibleState( false, true );
	pChar->SetSpecialHide( false );
	pChar->DestroyMagicCircle();

	if( pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_TELEPORT;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLightAttack::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if ( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	/*if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect, false );
		m_dwSkillMapEffect = -1;
	}*/

	EndCameraBuff( pChar );

	pChar->ClearSkillReserveKey();
	pChar->SetInvisibleState( false, true );
	pChar->SetSpecialHide( false );
	pChar->DestroyMagicCircle();
	
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	ClearData();
}

bool ioLightAttack::IsNoDropState() const
{
	if ( m_State == SS_END || m_State == SS_TELEPORT )
		return false;

	return true;	
}


bool ioLightAttack::IsAttackEndState() const
{
	return IsEndState();
}

bool ioLightAttack::IsEndState() const
{
	if( m_State == SS_END && m_bEnd )
		return true;

	return false;
}

bool ioLightAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	return IsEndState();
}

void ioLightAttack::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
	}
}

void ioLightAttack::AddActionOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_ActionOwnerBuffList.empty() ) return;

	int iCnt = m_ActionOwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		pChar->AddNewBuff( m_ActionOwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
	}
}

void ioLightAttack::RemoveActionOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_ActionOwnerBuffList.empty() ) return;
	int iCnt = m_ActionOwnerBuffList.size();

	for( int i=0; i<iCnt; ++i)
	{
		ioBuff* pBuff = pChar->GetBuff(m_ActionOwnerBuffList[i].m_BuffName );
		if( pBuff )
		{
			pBuff->SetReserveEndBuff();
		}
	}
}

void ioLightAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_PREACTION:
		SetPreAction( pChar );
		break;
	case SSC_TELEPORT:
		rkPacket >> m_vCirclePos;
		SetTeleport( pChar , false );
		break;
	}
}

bool ioLightAttack::HasBuff( int iBuffType ) const
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_OwnerBuffList[i].m_iBuffType == iBuffType )
			return true;
	}

	return false;
}


bool ioLightAttack::CheckUseBuff( ioBaseChar *pChar )
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( pChar->HasBuff( m_OwnerBuffList[i].m_BuffName ) )
			return true;
	}

	return false;
}