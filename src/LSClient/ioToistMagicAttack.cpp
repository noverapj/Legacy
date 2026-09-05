

#include "stdafx.h"

#include "ioToistMagicAttack.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioBombMissileWeapon3.h"
#include "FindPredicateImpl.h"

ioToistMagicAttack::ioToistMagicAttack()
{
	Init();
}

ioToistMagicAttack::ioToistMagicAttack( const ioToistMagicAttack &rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_nMaxTarget( rhs.m_nMaxTarget ),
	m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	m_DefaultAngleList( rhs.m_DefaultAngleList ),
	m_NomralAttackByAir( rhs.m_NomralAttackByAir ),
	m_JumpAttackByAir( rhs.m_JumpAttackByAir )
{
	Init();
}

ioToistMagicAttack::~ioToistMagicAttack()
{
}

void ioToistMagicAttack::Init()
{
	m_SkillState		= SS_PRE;

	m_nCurrFireIndex	= 0;
	m_AttackFireTime	= 0;
	m_dwMotionEndTime	= 0;

	m_SkillEndJumpType	= SEJT_NOT_JUMP;
}

void ioToistMagicAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwSkillProtectTime	= rkLoader.LoadInt_e( "skill_protect_time", 1000 );
	m_nMaxTarget			= rkLoader.LoadInt_e( "max_target", 8 );
	m_fTargetMaxRange		= rkLoader.LoadFloat_e( "max_target_range", FLOAT500 );

	int iMax = rkLoader.LoadInt_e( "max_angle", 8 );
	for( int i = 0; i < iMax; ++i )
	{
		sprintf_s_e( szBuf, "angle%d", i+1 );
		float fAngle = rkLoader.LoadFloat( szBuf, 0.0f );
		m_DefaultAngleList.push_back( fAngle );
	}

	LoadWeaponInfo( "attack_air", m_NomralAttackByAir, rkLoader );
	LoadWeaponInfo( "jump_attack_air", m_JumpAttackByAir, rkLoader );
}

ioSkill* ioToistMagicAttack::Clone()
{
	return new ioToistMagicAttack( *this );
}

bool ioToistMagicAttack::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

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

	if( GetSkillUseType() == SUT_JUMP || pChar->IsCharDropZonePos() )
		m_SkillEndJumpType	= SEJT_JUMP;

	FD_RangeTarget kFindPred( pChar, m_fTargetMaxRange, 0, false );
	ioBaseChar *pTarget = pChar->GetCreator()->FindUser( kFindPred );
	if( pTarget )
	{
		D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		D3DXVec3Normalize( &vDir, &vDir );
		pChar->SetTargetRotToDir( vDir, true, true );
	}

	return true;
}

void ioToistMagicAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
	case SS_END:
		{
		}
		break;
	}
}

void ioToistMagicAttack::CheckPreState( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioToistMagicAttack::FindTarget( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_TargetDirectionList.clear();

	//타겟 리스트 가져오기
	FD_RangeMultipleTarget kFindPred( pChar, m_fTargetMaxRange, 0, 360, true );
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
			{
				Info.eState	= TS_AIR;

			}
			else
			{
				Info.eState	= TS_NORMAL;
			}

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

void ioToistMagicAttack::SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
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

void ioToistMagicAttack::AttackFire( ioBaseChar* pChar, ioPlayStage* pStage, const D3DXVECTOR3& vDir, TargetState eState )
{
	WeaponInfoList List;
	if( GetSkillUseType() == SUT_JUMP )
	{		
		switch( eState )
		{
		case TS_NONE:
		case TS_NORMAL:
			List = GetAttribute().m_vWeaponInfoList;
			break;
		case TS_AIR:
			List = m_NomralAttackByAir.m_vWeaponInfoList;
			break;
		}
	}
	else
	{
		switch( eState )
		{
		case TS_NONE:
		case TS_NORMAL:
			List = GetAttribute().m_vWeaponInfoList;
			break;
		case TS_AIR:
			List = m_JumpAttackByAir.m_vWeaponInfoList;
			break;
		}
	}
	
	for( WeaponInfoList::const_iterator iter = List.begin(); iter != List.end(); ++iter )
	{
		const WeaponInfo& rkInfo = *iter;
		ioBombMissileWeapon3* pWeapon = ToBombMissileWeapon3( pChar->SkillFireExplicit( rkInfo, GetName(), pChar->IncreaseWeaponIndexBase(), pChar->GetMidPositionByRate(), vDir ) );
		if( pWeapon && eState != TS_NONE )
			pWeapon->SetDsiableChangeDir( true );
	}
}

void ioToistMagicAttack::CheckActionState( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioToistMagicAttack::SetEndState( ioBaseChar* pChar, ioPlayStage* pStage )
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

void ioToistMagicAttack::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

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

bool ioToistMagicAttack::IsEndState() const
{
	if( 0.0f < m_fSkillEndNextJumpAmt )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioToistMagicAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
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

bool ioToistMagicAttack::IsAttackEndState() const
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

bool ioToistMagicAttack::IsNoDropState() const
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

SkillEndJumpType ioToistMagicAttack::GetSkillEndNextJumpMustEnable()
{
	return m_SkillEndJumpType;
}

bool ioToistMagicAttack::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioToistMagicAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioToistMagicAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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

bool ioToistMagicAttack::IsProtected( int iDefenseBreakType ) const
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