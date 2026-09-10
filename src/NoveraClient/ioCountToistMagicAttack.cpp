

#include "stdafx.h"

#include "ioCountToistMagicAttack.h"
#include "ioBombMissileWeapon3.h"
#include "FindPredicateImpl.h"

ioCountToistMagicAttack::ioCountToistMagicAttack()
{
	Init();
}

ioCountToistMagicAttack::ioCountToistMagicAttack( const ioCountToistMagicAttack &rhs )
	: ioCountSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_CountAttackList( rhs.m_CountAttackList ),
	m_DefaultAngleList( rhs.m_DefaultAngleList )
{
	Init();
}

ioCountToistMagicAttack::~ioCountToistMagicAttack()
{
}

void ioCountToistMagicAttack::Init()
{
	m_SkillState		= SS_PRE;

	m_AttackFireTime	= 0;
	m_dwMotionEndTime	= 0;
	m_iCountIndex		= 0;
	m_SkillEndJumpType	= SEJT_NOT_JUMP;
	m_bReduceGauge = false;
}

void ioCountToistMagicAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwSkillProtectTime	= rkLoader.LoadInt_e( "skill_protect_time", 1000 );

	int iMax = rkLoader.LoadInt_e( "max_angle", 8 );
	for( int i = 0; i < iMax; ++i )
	{
		sprintf_s_e( szBuf, "angle%d", i+1 );
		float fAngle = rkLoader.LoadFloat( szBuf, 0.0f );
		m_DefaultAngleList.push_back( fAngle );
	}

	iMax = rkLoader.LoadInt_e( "count_attack_info_count", 0 );
	for( int i = 0; i < iMax; ++i )
	{
		 CountAttackInfo cInfo;

		 sprintf_s_e( szBuf, "count_attack_info%d_check", i+1 );
		 cInfo.m_iCheckCount = rkLoader.LoadInt( szBuf, 0 );
		 sprintf_s_e( szBuf, "count_attack_info%d_max_target_range", i+1 );
		 cInfo.m_fTargetMaxRange = rkLoader.LoadFloat( szBuf, FLOAT500 );

		 sprintf_s_e( szBuf, "count_attack_info%d_attack", i+1 );
		 LoadWeaponInfo( szBuf, cInfo.m_NomralAttack, rkLoader );
		 sprintf_s_e( szBuf, "count_attack_info%d_attack_air", i+1 );
		 LoadWeaponInfo( szBuf, cInfo.m_NomralAttackAir, rkLoader );
		 sprintf_s_e( szBuf, "count_attack_info%d_jump_attack", i+1 );
		 LoadWeaponInfo( szBuf, cInfo.m_JumpAttack, rkLoader );
		 sprintf_s_e( szBuf, "count_attack_info%d_jump_attack_air", i+1 );
		 LoadWeaponInfo( szBuf, cInfo.m_JumpAttackAir, rkLoader );

		 m_CountAttackList.push_back( cInfo );
	}
}

ioSkill* ioCountToistMagicAttack::Clone()
{
	return new ioCountToistMagicAttack( *this );
}

bool ioCountToistMagicAttack::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioCountSkill::OnSkillStart( pChar ) )
		return false;

	Init();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( GetSkillUseType() == SUT_JUMP || pChar->IsCharDropZonePos() )
		m_SkillEndJumpType	= SEJT_JUMP;

	int iCount = m_CountAttackList.size();
	for( int iIndex = 0; iIndex < iCount; iIndex++ )
	{
		if( m_CountAttackList[iIndex].m_iCheckCount <= m_iCurUseCount )
		{
			m_iCountIndex = iIndex;
			break;
		}
	}

	FD_RangeTarget kFindPred( pChar, m_CountAttackList[m_iCountIndex].m_fTargetMaxRange, 0, true );
	ioBaseChar *pTarget = pChar->GetCreator()->FindUser( kFindPred );
	if( pTarget )
	{
		D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		D3DXVec3Normalize( &vDir, &vDir );
		pChar->SetTargetRotToDir( vDir, true, true );
	}

	return true;
}

void ioCountToistMagicAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_PRE:
		CheckPreState( pChar, pStage );
		break;
	case SS_ATTACK:
		CheckActionState( pChar, pStage );
		break;
	case SS_END:
		break;
	}
}

void ioCountToistMagicAttack::CheckPreState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{	
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetSkillActionState( pChar, pStage );
	}
}

void ioCountToistMagicAttack::FindTarget( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_TargetDirectionList.clear();

	//타겟 리스트 가져오기
	FD_RangeMultipleTarget kFindPred( pChar, m_CountAttackList[m_iCountIndex].m_fTargetMaxRange, 0, 360, true );
	BaseCharList TargetList, NotTargetList;
	kFindPred.GetFindCharList( pStage->GetBaseCharList(), TargetList, NotTargetList );

	//발사방향 계산
	for( BaseCharList::iterator iter = TargetList.begin(); iter != TargetList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;
		if( pTarget )
		{
			TargetInfo Info;			
			Info.vDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
			D3DXVec3Normalize( &Info.vDir, &Info.vDir );
			
			//타겟 상태 체크
			float fBottomHeight = pTarget->GetBottomHeight();
			if( 5.0f < pTarget->GetWorldPosition().y - fBottomHeight )
				Info.eState	= TS_AIR;
			else
				Info.eState	= TS_NORMAL;

			m_TargetDirectionList.push_back( Info );
		}
	}

	//타겟이 없으면 지정한 각도 세팅
	if( m_TargetDirectionList.empty() )
	{
		for( int i = 0; i < (int)m_DefaultAngleList.size(); ++i )
		{
			float fCurAngle = ioMath::ArrangeHead( m_DefaultAngleList[i] );

			D3DXVECTOR3	vDir;
			D3DXQUATERNION qtAngle;
			D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD( fCurAngle ), 0.0f, 0.0f );

			vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vDir = qtAngle * vDir;

			TargetInfo Info;
			Info.vDir = vDir;
			m_TargetDirectionList.push_back( Info );
		}
	}
}

void ioCountToistMagicAttack::SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
	int iCount = m_CountAttackList.size();
	for( int iIndex = 0; iIndex < iCount; iIndex++ )
	{
		if( m_CountAttackList[iIndex].m_iCheckCount <= m_iCurUseCount )
		{
			m_iCountIndex = iIndex;
			break;
		}
	}
	if( !m_bReduceGauge )
	{
		ReduceNeedCount( m_iMaxUseCount );
		m_bReduceGauge = true;
	}

	m_SkillState	= SS_ATTACK;
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID		= 0;
		float fTimeRate	= 0;
		if( GetSkillUseType() == SUT_JUMP )
		{
			iAniID = pGrp->GetAnimationIdx( m_SkillMotionName );
			fTimeRate = m_fAnimationTimeRate;
		}
		else
		{
			iAniID = pGrp->GetAnimationIdx( m_JumpSkillMotionName );
			fTimeRate = m_fJumpAnimationTimeRate;
		}

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fTimeRate, 0.0f, 0, true );
		m_AttackFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
		m_dwMotionEndTime += dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	}
}

void ioCountToistMagicAttack::AttackFire( ioBaseChar* pChar, ioPlayStage* pStage, const D3DXVECTOR3& vDir, TargetState eState )
{
	WeaponInfoList List;
	if( GetSkillUseType() != SUT_JUMP )
	{		
		switch( eState )
		{
		case TS_NONE:
		case TS_NORMAL:
			List = m_CountAttackList[m_iCountIndex].m_NomralAttack.m_vWeaponInfoList;
			break;
		case TS_AIR:
			List = m_CountAttackList[m_iCountIndex].m_NomralAttackAir.m_vWeaponInfoList;
			break;
		}
	}
	else
	{
		switch( eState )
		{
		case TS_NONE:
		case TS_NORMAL:
			List = m_CountAttackList[m_iCountIndex].m_JumpAttack.m_vWeaponInfoList;
			break;
		case TS_AIR:
			List = m_CountAttackList[m_iCountIndex].m_JumpAttackAir.m_vWeaponInfoList;
			break;
		}
	}

	for( WeaponInfoList::const_iterator iter = List.begin(); iter != List.end(); ++iter )
	{
		const WeaponInfo& rkInfo = *iter;
		pChar->SkillFireExplicit( rkInfo, GetName(), pChar->IncreaseWeaponIndexBase(), pChar->GetMidPositionByRate(), vDir );
	}
}

void ioCountToistMagicAttack::CheckActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		SetEndState( pChar, pStage );
	}
	else
	{
		if( 0 < m_AttackFireTime && m_AttackFireTime < dwCurTime )
		{
			m_AttackFireTime = 0;
			FindTarget( pChar, pStage );

			for( TargetDirectionList::iterator iter = m_TargetDirectionList.begin(); iter != m_TargetDirectionList.end(); ++iter )
			{
				const TargetInfo& Info = *iter;
				AttackFire( pChar, pStage, Info.vDir, Info.eState );
			}

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << pChar->GetWorldOrientation();
				kPacket << pChar->GetWorldPosition();
				kPacket << (byte)SSS_ATTACK_FIRE;
				kPacket << m_iCountIndex;
				int iSize = (int)m_TargetDirectionList.size();
				kPacket << iSize;
				for( int i = 0; i < iSize; ++i )
				{
					kPacket << (byte)m_TargetDirectionList[i].eState;
					kPacket << m_TargetDirectionList[i].vDir;
				}
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioCountToistMagicAttack::SetEndState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_SkillState = SS_END;

	pChar->SetCurMoveSpeed( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetExtraMoveSpeed( 0.0f );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << (byte)SSS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountToistMagicAttack::OnSkillEnd( ioBaseChar *pChar )
{
	ioCountSkill::OnSkillEnd( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	Init();
}

bool ioCountToistMagicAttack::IsEndState() const
{
	if( 0.0f < m_fSkillEndNextJumpAmt )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioCountToistMagicAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_ATTACK:
		return false;
	}

	if( m_fSkillEndNextJumpAmt == 0.0f )
		return true;

	return false;
}

bool ioCountToistMagicAttack::IsAttackEndState() const
{
	if( m_fSkillEndNextJumpAmt == 0.0f )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:	
	case SS_ATTACK:
		return false;
	}

	return true;
}

bool ioCountToistMagicAttack::IsNoDropState() const
{
	switch( m_SkillState )
	{	
	case SS_PRE:
	case SS_ATTACK:
	case SS_END:
		if( m_SkillEndJumpType == SEJT_JUMP )
			return true;
	}

	return false;
}

SkillEndJumpType ioCountToistMagicAttack::GetSkillEndNextJumpMustEnable()
{
	return m_SkillEndJumpType;
}

bool ioCountToistMagicAttack::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCountToistMagicAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioCountSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioCountToistMagicAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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
	case SSS_ATTACK_FIRE:
		{
			int iSize = 0;
			rkPacket >> m_iCountIndex;
			rkPacket >> iSize;

			for( int i = 0; i < iSize; ++i )
			{
				byte eState			= TS_NONE;
				D3DXVECTOR3 vDir	= ioMath::VEC3_ZERO;
				rkPacket >> eState;
				rkPacket >> vDir;

				AttackFire( pChar, pStage, vDir, (TargetState)eState );
			}
		}
		break;
	case SSS_END:
		{
			SetEndState( pChar, pStage );
		}
		break;
	}
}

bool ioCountToistMagicAttack::IsProtected( int iDefenseBreakType ) const
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