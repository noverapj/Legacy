

#include "stdafx.h"

#include "ioControlDummySkill2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioControlDummySkill2::ioControlDummySkill2()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_iDummyIndex = -1;
	m_dwCheckActiveTime = 0;
	m_bLoopInit = false;
}

ioControlDummySkill2::ioControlDummySkill2( const ioControlDummySkill2 &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_DummyCharName( rhs.m_DummyCharName ),
	  m_fDummyCharOffset( rhs.m_fDummyCharOffset ),
	  m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	  m_dwActiveTime( rhs.m_dwActiveTime ),
	  m_szLoopAnimation( rhs.m_szLoopAnimation )
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_iDummyIndex = -1;	
	m_dwCheckActiveTime = 0;
	m_bLoopInit = false;
}

ioControlDummySkill2::~ioControlDummySkill2()
{
	m_vDestroyDummyCharList.clear();
}

void ioControlDummySkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	
	rkLoader.LoadString_e( "loop_animaion", "", szBuf, MAX_PATH );
	m_szLoopAnimation = szBuf;
	
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_dwActiveTime = rkLoader.LoadInt_e( "active_time", 0);
	
	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;
	m_fDummyCharOffset	= rkLoader.LoadFloat_e( "dummy_char_offset", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "destroy_dummy_cnt", 0 );
	m_vDestroyDummyCharList.clear();
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "destroy_dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vDestroyDummyCharList.push_back( szBuf );
	}
}

ioSkill* ioControlDummySkill2::Clone()
{
	return new ioControlDummySkill2( *this );
}

bool ioControlDummySkill2::OnSkillStart( ioBaseChar *pChar )
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
	m_dwCheckActiveTime = FRAMEGETTIME();
	m_bLoopInit = false;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;
	
	return true;
}

void ioControlDummySkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
		}
		break;
	case SS_LOOP:
		{
			//루프 애니메이션 초기화
			if ( !m_bLoopInit )
			{
				SetLoopAnimation( pChar );
				m_bLoopInit = true;
			}

			//더미가 죽으면 스킬 종료
			if( !IsDummyCharEnable( pChar ) )
			{
				SetSkillEndState( pChar );
				return;
			}	

			//캐릭터가 더미를 바라본다
			LookAtDummy( pChar );
			CheckInputKey( pChar );
		}
		break;
	case SS_END:
		break;
	}

	if ( m_dwCheckActiveTime + m_dwActiveTime < dwCurTime )
	{
		SetSkillEndState( pChar );
	}
}

void ioControlDummySkill2::OnSkillEnd( ioBaseChar *pChar )
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

	RemoveDummy( pChar );
}

void ioControlDummySkill2::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	m_SkillState = SS_ACTION;
}

bool ioControlDummySkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioControlDummySkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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

			m_SkillState = SS_LOOP;

			g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );
						
			ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, m_iDummyIndex,
				pChar->GetCharName(),
				vPos,
				0.0f,
				0, true );

			if( pDummy )
				pDummy->SetControlSkillName( GetName() );
		}
		break;
	case SS_REMOVE_DUMMY:
		{			
			RemoveDummy( pChar );
		}
		break;
	}
}

void ioControlDummySkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

bool ioControlDummySkill2::IsProtected( int iDefenseBreakType ) const
{
	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return ioSkill::IsProtected( iDefenseBreakType );
}

bool ioControlDummySkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;
	
	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_ACTION:
	case SS_LOOP:
		return false;	
	case SS_END:
		return true;
	}

	return false;
}

bool ioControlDummySkill2::IsDummyCharControl( ioBaseChar *pChar )
{
	if( !pChar || m_iDummyIndex == -1 )
		return false;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( !pDummy )
		return false;

	return true;
}

bool ioControlDummySkill2::IsDummyCharEnable( ioBaseChar *pChar )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );

	if( !pDummy )	return false;

	if( pDummy->GetState() == DCS_DIE )
		return false;

	return true;
}

D3DXVECTOR3 ioControlDummySkill2::GetDummyCharControlPos( ioBaseChar *pChar )
{
	if( !pChar || m_iDummyIndex == -1 )
		return ioMath::VEC3_ZERO;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( pDummy )
	{
		return pDummy->GetCameraPosition();
	}

	return ioMath::VEC3_ZERO;
}

void ioControlDummySkill2::SetLoopAnimation( ioBaseChar *pChar )
{
	if ( !pChar || m_szLoopAnimation.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopAnimation );
	if( iAniID == -1 )	
		return;

	pGrp->SetLoopAni( iAniID, FLOAT100 );	
}

void ioControlDummySkill2::LookAtDummy( ioBaseChar *pChar )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( !pDummy )
		return;

	pChar->SetTargetRotToTargetPos( pDummy->GetWorldPosition(), true, false );
}

void ioControlDummySkill2::CheckInputKey( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;
	
	if ( pChar->IsAttackKey() )
	{
		RemoveDummy( pChar );
	}	
}

void ioControlDummySkill2::CreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage || !pChar->IsNeedProcess() ) 
		return;

	if( m_DummyCharName.IsEmpty() ) 
		return;

	m_SkillState = SS_LOOP;
	
	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

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

void ioControlDummySkill2::SetSkillEndState( ioBaseChar *pChar )
{
	m_SkillState = SS_END;	
}

void ioControlDummySkill2::RemoveDummy( ioBaseChar *pChar )
{
	if ( !pChar || m_iDummyIndex == -1)
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( pDummy )	
	{
		pDummy->SetDieState( true );
		m_iDummyIndex = -1;
	}

	//패킷 전송
	if ( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_REMOVE_DUMMY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}