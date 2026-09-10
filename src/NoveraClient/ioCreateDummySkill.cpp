

#include "stdafx.h"

#include "ioCreateDummySkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioCreateDummySkill::ioCreateDummySkill()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_iDummyIndex = -1;
}

ioCreateDummySkill::ioCreateDummySkill( const ioCreateDummySkill &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_DummyCharName( rhs.m_DummyCharName ),
	  m_fDummyCharOffset( rhs.m_fDummyCharOffset ),
	  m_nMaxCreateCnt( rhs.m_nMaxCreateCnt ),
	  m_bSetUseActiveCnt( rhs.m_bSetUseActiveCnt )
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_iDummyIndex = -1;	
}

ioCreateDummySkill::~ioCreateDummySkill()
{
}

void ioCreateDummySkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
		
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	
	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;
	m_fDummyCharOffset	= rkLoader.LoadFloat_e( "dummy_char_offset", 0.0f );
	m_nMaxCreateCnt = rkLoader.LoadInt_e( "max_create_cnt", 0 );
	m_bSetUseActiveCnt = rkLoader.LoadBool_e( "use_active_cnt", true );
}

ioSkill* ioCreateDummySkill::Clone()
{
	return new ioCreateDummySkill( *this );
}

bool ioCreateDummySkill::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_NONE;
	m_iDummyIndex = -1;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;
	
	return true;
}

void ioCreateDummySkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			CreateDummyChar( pChar, pStage );
			SetSkillEndState( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioCreateDummySkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}
	
	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	
	m_bSetHandMesh = false;
}

void ioCreateDummySkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	m_SkillState = SS_ACTION;
}

bool ioCreateDummySkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCreateDummySkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{		
	case SS_CREATE_DUMMY:
		{
			if( m_DummyCharName.IsEmpty() ) 
				return;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			rkPacket >> m_iDummyIndex;
									
			ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, m_iDummyIndex,
				pChar->GetCharName(),
				vPos,
				0.0f,
				0, true );

			if( pDummy )
				pDummy->SetControlSkillName( GetName() );

			if ( m_bSetUseActiveCnt )
				g_DummyCharMgr.CheckMaxCreateDummyCnt( pChar, pDummy, m_nMaxCreateCnt);
		}
		break;
	}
}

void ioCreateDummySkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

bool ioCreateDummySkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return ioSkill::IsProtected( iDefenseBreakType );
}

bool ioCreateDummySkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;
	
	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_ACTION:
		return false;	
	case SS_END:
		return true;
	}

	return false;
}

void ioCreateDummySkill::CreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage || !pChar->IsNeedProcess() ) 
		return;

	if( m_DummyCharName.IsEmpty() ) 
		return;
		
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( m_fDummyCharOffset * vDir );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	m_iDummyIndex = iCurIndex;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, m_iDummyIndex,
												   pChar->GetCharName(),
												   vPos,
												   0.0f,
												   0, true );

	if( pDummy )
		pDummy->SetControlSkillName( GetName() );

	if ( m_bSetUseActiveCnt )
		g_DummyCharMgr.CheckMaxCreateDummyCnt( pChar, pDummy, m_nMaxCreateCnt);

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_CREATE_DUMMY;
		kPacket << pChar->GetTargetRot();
		kPacket << vPos;
		kPacket << m_iDummyIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateDummySkill::SetSkillEndState( ioBaseChar *pChar )
{
	m_SkillState = SS_END;	
}

bool ioCreateDummySkill::IsUseActiveCnt() const
{
	return m_bSetUseActiveCnt;
}

int ioCreateDummySkill::GetMaxActiveCnt( ioBaseChar *pChar ) const
{ 
	return m_nMaxCreateCnt; 
}

int ioCreateDummySkill::GetCurActiveCnt( ioBaseChar *pOwner ) const
{
	return g_DummyCharMgr.GetCurDummyCharCnt( pOwner, m_DummyCharName );
}