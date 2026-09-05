

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioCountRollingSkill.h"

ioCountRollingSkill::ioCountRollingSkill()
{
	Init();
}

ioCountRollingSkill::ioCountRollingSkill( const ioCountRollingSkill &rhs )
	: ioCountSkill( rhs ),
	 m_OwnerBuffList( rhs.m_OwnerBuffList )
{
	Init();
}

ioCountRollingSkill::~ioCountRollingSkill()
{
}

void ioCountRollingSkill::Init()
{
	m_SkillState	= SS_PRE;
	m_iCurDirKey	= ioUserKeyInput::DKI_NONE;
}


void ioCountRollingSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );

	m_OwnerBuffList.clear();
		
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	
	int iCount = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i = 0; i < iCount; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );
		
		ioBuffInfo BuffInfo;
		BuffInfo.m_BuffName = szBuff;

		if( !BuffInfo.m_BuffName.IsEmpty() )
		{			
			BuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( BuffInfo.m_BuffName );			
			if( BuffInfo.m_iBuffType != BT_NONE )
			{	
				m_OwnerBuffList.push_back( BuffInfo );
			}
		}
	}
}

ioSkill* ioCountRollingSkill::Clone()
{
	return new ioCountRollingSkill( *this );		
}

bool ioCountRollingSkill::IsCanUseSkill( ioBaseChar *pOwner, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		ioHashString szSkillMotion = GetSkillMotionName();

		if( !pAttackable->IsCanFire( pOwner ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	return true;
}

bool ioCountRollingSkill::OnSkillStart( ioBaseChar *pOwner )
{
	if( !ioCountSkill::OnSkillStart(pOwner) )
		return false;

	Init();

	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	SetOwnerBuff( pOwner );

	return true;
}

void ioCountRollingSkill::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pOwner, pStage );
		}
		break;
	case SS_ROLLING:
		{
			CheckRollingState( pOwner, pStage );
		}
		break;
	case SS_JUMP_ROLLING:
		{
			CheckRollingState( pOwner, pStage );
		}
		break;
	}
}

void ioCountRollingSkill::SetOwnerBuff( ioBaseChar *pOwner )
{
	ioItem *pItem = GetOwnerItem();
	if( !pOwner || !pItem )
		return;

	int iCnt = m_OwnerBuffList.size();	
	for( int i=0; i < iCnt; ++i )
	{			
		ioBuffInfo pInfo = m_OwnerBuffList[i];
		pOwner->AddNewBuff( pInfo.m_BuffName,  pOwner->GetCharName(), pItem->GetName(), this );
	}
}

void ioCountRollingSkill::CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime  = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( SUT_JUMP )
			{
				SetJumpRollingState( pOwner, pStage );
			}
			else
			{
				SetRollingState( pOwner, pStage );
			}
		}
	}
	else
	{
		ioUserKeyInput::DirKeyInput iCurDirKey = pOwner->CheckCurDirKey();
		if( iCurDirKey != ioUserKeyInput::DKI_NONE && m_iCurDirKey != iCurDirKey )
		{
			m_iCurDirKey = iCurDirKey;
		}
	}
}

void ioCountRollingSkill::SetRollingState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_iCurDirKey != ioUserKeyInput::DKI_NONE )
	{
		pOwner->SetTargetRotToDirKey( m_iCurDirKey );
	}

	m_SkillState = SS_ROLLING;
	ioCountSkill::AttackSkillFire( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();		
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (byte)SS_ROLLING;
		kPacket << (int)m_iCurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountRollingSkill::CheckRollingState( ioBaseChar *pOwner, ioPlayStage *pStage )
{	
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_SkillState = SS_END;
	}
}

void ioCountRollingSkill::SetJumpRollingState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_iCurDirKey != ioUserKeyInput::DKI_NONE )
	{
		pOwner->SetTargetRotToDirKey( m_iCurDirKey );
	}

	m_SkillState = SS_JUMP_ROLLING;
	ioCountSkill::AttackSkillFire( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (byte)SS_JUMP_ROLLING;
		kPacket << (int)m_iCurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountRollingSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	ioCountSkill::OnSkillEnd( pOwner );

	pOwner->SetSkillMoveEnable( false, false );

	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	Init();
}

bool ioCountRollingSkill::IsEndState() const
{
	if( 0.0f < m_fSkillEndNextJumpAmt )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}


bool ioCountRollingSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_ROLLING:
	case SS_JUMP_ROLLING:
		return false;
	}

	if( m_fSkillEndNextJumpAmt == 0.0f )
		return true;

	return false;
}

bool ioCountRollingSkill::IsAttackEndState() const
{
	if( m_fSkillEndNextJumpAmt == 0.0f )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:		
	case SS_ROLLING:
	case SS_JUMP_ROLLING:
		return false;
	}

	return true;
}

bool ioCountRollingSkill::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_JUMP_ROLLING:	
		return true;
	}

	return false;
}


bool ioCountRollingSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	return false;
}

bool ioCountRollingSkill::IsCanSendControl() const
{
	return false;
}

void ioCountRollingSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;
	rkPacket >> qtRot;
	rkPacket >> vPos;

	pChar->SetTargetRot( qtRot );
	pChar->SetWorldPosition( vPos );

	byte eState;
	rkPacket >> eState;
	switch( eState )
	{
	case SS_ROLLING:
		{
			int iCurKey = ioUserKeyInput::DKI_NONE;
			rkPacket >> iCurKey;
			m_iCurDirKey = (DirKey)iCurKey;
			SetRollingState( pChar, pChar->GetCreator() );

		}
		break;
	case SS_JUMP_ROLLING:
		{
			int iCurKey = ioUserKeyInput::DKI_NONE;
			rkPacket >> iCurKey;
			m_iCurDirKey = (DirKey)iCurKey;
			SetJumpRollingState( pChar, pChar->GetCreator() );
		}
		break;
	}
}

