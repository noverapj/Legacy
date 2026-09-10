

#include "stdafx.h"

#include "ioTargetFlowAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "FindPredicateImpl.h"

#include "WeaponDefine.h"

ioTargetFlowAttackSkill::ioTargetFlowAttackSkill()
{
	Init();
}

ioTargetFlowAttackSkill::ioTargetFlowAttackSkill( const ioTargetFlowAttackSkill &rhs )
	: ioAttackSkill( rhs ),
	m_szTargetBuff( rhs.m_szTargetBuff ),
	m_fTargetMinRange(rhs.m_fTargetMinRange ),
	m_fTargetMaxRange(rhs.m_fTargetMaxRange ),
	m_fTeleportOffsetZRange( rhs.m_fTeleportOffsetZRange ),
	m_fTeleportOffsetYRange( rhs.m_fTeleportOffsetYRange ),
	m_AttackAttributeList( rhs.m_AttackAttributeList ),
	m_FinishAttribute( rhs.m_FinishAttribute )
{
	Init();
}

ioTargetFlowAttackSkill::~ioTargetFlowAttackSkill()
{
}

void ioTargetFlowAttackSkill::Init()
{
	m_bNextLinerCombo	= false;
	m_bFinish			= false;
	m_SkillState		= SS_PRE;
	m_iLinerComoboIndex = 0;
	m_vFirePos			= ioMath::VEC3_ZERO;
	m_pCurAttribute		= NULL;
	m_eCurrDirKey		= ioUserKeyInput::DKI_NONE;
}

void ioTargetFlowAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "target_buff", "", szBuf, MAX_PATH );
	m_szTargetBuff = szBuf;
		
	m_fTargetMaxRange			= rkLoader.LoadFloat_e( "target_max_range", FLOAT1000 );
	m_fTargetMinRange			= rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTeleportOffsetYRange		= rkLoader.LoadFloat_e( "teleport_offset_y_range", 300.0f );
	m_fTeleportOffsetZRange		= rkLoader.LoadFloat_e( "teleport_offset_z_range", 300.0f );

	m_dwSkillProtectTime		= rkLoader.LoadInt_e( "skill_protect_time", 0 );

	int iMax = rkLoader.LoadInt_e( "air_liner_combo_max", 0 );
	m_AttackAttributeList.clear();
	for( int i = 1; i <= iMax; ++i )
	{
		AttackAttribute Attr;
		wsprintf_e( szBuf, "air_liner_combo%d", i );
		LoadAttackAttribute( szBuf, Attr, rkLoader );

		m_AttackAttributeList.push_back( Attr );
	}
		
	LoadAttackAttribute( "air_liner_finish", m_FinishAttribute, rkLoader );
}

ioSkill* ioTargetFlowAttackSkill::Clone()
{
	return new ioTargetFlowAttackSkill( *this );
}

bool ioTargetFlowAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	Init();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwSkillStartTime = FRAMEGETTIME();

	return true;
}

void ioTargetFlowAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pChar, pStage );
		}
		break;
	case SS_TARGET_FLOW:
		{
			CheckTargetComoboState( pChar, pStage );
		}
		break;
	case SS_TELEPORT:
		{
			SetLinerComoboState( pChar, pStage );
		}
		break;
	case SS_ATTACK:
		{
			CheckLinerComoboState( pChar, pStage );
		}
		break;
	case SS_END:
		{
		}
		break;
	}
}

void ioTargetFlowAttackSkill::CheckPreState( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioTargetFlowAttackSkill::CheckTargetComoboState( ioBaseChar *pChar, ioPlayStage *pStage )
{	
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( pChar->IsNeedProcess() )
		{
			SetTeleportState( pChar, pStage );
		}
	}
}

void ioTargetFlowAttackSkill::CheckLinerComoboState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( m_bNextLinerCombo )
		{
			SetLinerComoboState( pChar, pStage );
		}
		else
		{
			m_SkillState = SS_END;
		}
	}
	else
	{
		bool bReserveSkill = false;
		int iSkillNum = pChar->GetSkillKeyInput();
		if( iSkillNum >= 0 )
		{
			bReserveSkill = true;
		}

		if( m_iLinerComoboIndex < (int)m_AttackAttributeList.size() && !bReserveSkill && pChar->IsAttackKey() )
		{
			m_bNextLinerCombo = true;
		}

		ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			m_eCurrDirKey = eDir;
		}
	}
}

void ioTargetFlowAttackSkill::SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );
	m_vFirePos = pChar->GetWorldPosition() + 50.0f * vDir;

	ioAttackSkill::AttackSkillFire( pChar );
	m_SkillState = SS_TARGET_FLOW;
}

void ioTargetFlowAttackSkill::SetTeleportState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	FD_BuffHasTarget kFindPred( pChar, m_szTargetBuff, m_fTargetMaxRange, m_fTargetMinRange, false );
	ioBaseChar *pTarget = pChar->GetCreator()->FindUser( kFindPred );
	if( pTarget )
	{
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		vPos.y	+= m_fTeleportOffsetYRange;		
		vPos	+= vDir * m_fTeleportOffsetZRange;
		pChar->SetWorldPosition( vPos );
		m_SkillState = SS_TELEPORT;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << (int)SSS_TELEPORT;
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		m_SkillState = SS_END;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << (int)SSS_END;
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTargetFlowAttackSkill::SetLinerComoboState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_SkillState		= SS_ATTACK;
	m_bNextLinerCombo	= false;

	if( COMPARE( m_iLinerComoboIndex, 0, (int)m_AttackAttributeList.size() ) )
	{
		if( m_eCurrDirKey != ioUserKeyInput::DKI_NONE && m_iLinerComoboIndex + 1 == (int)m_AttackAttributeList.size() )
		{
			m_pCurAttribute = &m_FinishAttribute;
			pChar->SetTargetRotToDirKey( m_eCurrDirKey );
		}
		else
		{
			m_pCurAttribute = &m_AttackAttributeList[m_iLinerComoboIndex];
		}
	}
	else
	{
		m_pCurAttribute = NULL;
	}

	ioAttackSkill::AttackSkillFire( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)SSS_ATTACK;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		kPacket << m_iLinerComoboIndex;
		kPacket << (int)m_eCurrDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );		
	}
	m_iLinerComoboIndex++;
}

AttackAttribute& ioTargetFlowAttackSkill::GetAttribute()
{	
	if( m_SkillState != SS_PRE && m_pCurAttribute )
	{
		return *m_pCurAttribute;
	}

	return ioAttackSkill::GetAttribute();
}

const AttackAttribute& ioTargetFlowAttackSkill::GetAttributeConst() const
{
	if( m_SkillState != SS_PRE && m_pCurAttribute )
	{
		return *m_pCurAttribute;
	}

	return ioAttackSkill::GetAttributeConst();
}

void ioTargetFlowAttackSkill::OnSkillEnd( ioBaseChar *pChar )
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

bool ioTargetFlowAttackSkill::IsEndState() const
{
	if( 0.0f < m_fSkillEndNextJumpAmt )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioTargetFlowAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_TARGET_FLOW:
	case SS_TELEPORT:
	case SS_ATTACK:
		return false;
	}

	if( m_fSkillEndNextJumpAmt == 0.0f )
		return true;
	
	return false;
}

bool ioTargetFlowAttackSkill::IsAttackEndState() const
{
	if( m_fSkillEndNextJumpAmt == 0.0f )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_TARGET_FLOW:
	case SS_TELEPORT:
	case SS_ATTACK:
		return false;
	}

	return true;
}


bool ioTargetFlowAttackSkill::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_TELEPORT:
	case SS_ATTACK:
	case SS_END:
		return true;
	}

	if( 0 < m_fSkillEndNextJumpAmt )
		return true;
	else
		return false;
}

SkillEndJumpType ioTargetFlowAttackSkill::GetSkillEndNextJumpMustEnable()
{
	switch( m_SkillState )
	{
	case SS_TELEPORT:
	case SS_ATTACK:
	case SS_END:		
		return SEJT_DEFAULT;
	}

	return SEJT_NOT_JUMP;
}

bool ioTargetFlowAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTargetFlowAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioTargetFlowAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int eState;
	rkPacket >> eState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pChar->SetTargetRotToRotate( qtRot, true, true );
	pChar->SetWorldPosition( vPos );

	if( eState == SSS_TELEPORT )
	{
		m_SkillState = SS_TELEPORT;
	}
	else if( eState == SSS_ATTACK )
	{
		int eKey = ioUserKeyInput::AKI_NONE;
		rkPacket >> m_iLinerComoboIndex;
		rkPacket >> eKey;
		m_eCurrDirKey = (ioUserKeyInput::DirKeyInput)eKey;

		SetLinerComoboState( pChar, pStage );
	}
	else if( eState == SSS_END )
	{
		m_SkillState = SS_END;
	}
}

bool ioTargetFlowAttackSkill::IsProtected( int iDefenseBreakType ) const
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