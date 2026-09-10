#include "StdAfx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "DummyCharDefine.h"

#include "ioTargetDummyBuff.h"

ioTargetDummyBuff::ioTargetDummyBuff()
{

}

ioTargetDummyBuff::ioTargetDummyBuff( const ioTargetDummyBuff &rhs )
	:ioBuff( rhs ),
	m_bDisableDownState( rhs.m_bDisableDownState ),
	m_fAlphaRate( rhs.m_fAlphaRate ),
	m_bHideMiniMap( rhs.m_bHideMiniMap ),
	m_bHideFootDustEffect( rhs.m_bHideFootDustEffect ),
	m_DummyChar( rhs.m_DummyChar ),
	m_DestroyDummyChar( rhs.m_DestroyDummyChar )
{
	m_CreateDummy.Init();
}

ioTargetDummyBuff::~ioTargetDummyBuff()
{
	m_CreateDummy.Init();
}

void ioTargetDummyBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_fAlphaRate = rkLoader.LoadFloat_e( "alpha_rate", FLOAT1 );
	m_bHideMiniMap = rkLoader.LoadBool_e( "hide_minimap", false );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bHideFootDustEffect = rkLoader.LoadBool_e( "hide_foot_dust_effect", false );
	
	wsprintf_e( szKey, "dummy_char_name" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_DummyChar.m_DummyCharName = szBuf;

	wsprintf_e( szKey, "dummy_char_offset" );
	m_DummyChar.m_fDummyCharOffset = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "dummy_char_side_offset" );
	m_DummyChar.m_fDummyCharSideOffset = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "dummy_char_start_angle" );
	m_DummyChar.m_fDummyCharStartAngle = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "destroy_dummy_name" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_DestroyDummyChar = szBuf;
}

ioBuff* ioTargetDummyBuff::Clone()
{
	return new ioTargetDummyBuff( *this );
}

void ioTargetDummyBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	int iAlphaRate = m_fAlphaRate * MAX_ALPHA_RATE;
	iAlphaRate = min( iAlphaRate, MAX_ALPHA_RATE );
	iAlphaRate = max( iAlphaRate, 0 );
	if( iAlphaRate < MAX_ALPHA_RATE )
	{
		m_pOwner->SetHideChar( true );
		m_pOwner->SetAlphaRateDirect( iAlphaRate );
	}
	m_bSetChar = false;
	m_bCreatedDummy = false;

	if( m_bHideFootDustEffect )
	{
		pOwner->SetHideFootDustEffect( true );
	}
}

bool ioTargetDummyBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	int iAlphaRate = m_fAlphaRate * MAX_ALPHA_RATE;
	iAlphaRate = min( iAlphaRate, MAX_ALPHA_RATE );
	iAlphaRate = max( iAlphaRate, 0 );
	if( iAlphaRate < MAX_ALPHA_RATE )
	{
		m_pOwner->SetHideChar( true );
		m_pOwner->SetAlphaRateDirect( iAlphaRate );
	}
	m_bSetChar = false;

	if( m_bHideFootDustEffect )
	{
		pOwner->SetHideFootDustEffect( true );
	}

	return true;
}

void ioTargetDummyBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			if( GetOwner() && CheckRestoreStateCheck() )
				GetOwner()->SetState( CS_DELAY, m_bSendDelayNetWork );

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				if( CheckRestoreStateCheck() )
				{
					m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				}

				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
	
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if ( pCreator &&  pCreator->IsOwnerChar() )
	{
		if( !m_bCreatedDummy )
		{
			CreateDummyChar( GetOwner() );
		}
	}
	if( m_bCreatedDummy && GetOwner()->IsNeedProcess() )
		SetCharState( GetOwner(), true );

	if( m_bSetChar )
	{
		CheckDummyChar();
	}
}

void ioTargetDummyBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
		if ( pCreator )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pCreator, m_CreateDummy.m_iDummyCharIdx );
			ioCopyOwnerDummyChar *pCopyOwnerDummy = ToCopyOwnerDummyChar( pDummy );
			if( pCopyOwnerDummy )
			{
				ioItem *pItem = GetOwnerItem();
				if( ( GetBuffDuration() > 0 && m_dwBuffEndTime <= FRAMEGETTIME() ) || ( pItem && pItem->GetCurSkillGuage() <= 0.0f ) )
					pCopyOwnerDummy->SetExplosion();
				else
					pCopyOwnerDummy->SetDieState( true );
			}
		}

		pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
		pOwner->SetHideChar( false );

		if( m_bHideFootDustEffect )
		{
			pOwner->SetHideFootDustEffect( false );
		}
	}

	ioBuff::EndBuff();
}

bool ioTargetDummyBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioTargetDummyBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

void ioTargetDummyBuff::SetCharState( ioBaseChar *pOwner, bool bSend )
{
	if( !pOwner ) return;

	if( pOwner->GetState() != CS_DIE &&
		pOwner->GetState() != CS_LOADING &&
		pOwner->GetState() != CS_OBSERVER &&
		pOwner->GetState() != CS_VIEW )
	{
		pOwner->SetState( CS_LOCKUP_BUFF );
		m_bSetChar = true;
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( bSend && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SS_SET_CHAR;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTargetDummyBuff::CreateDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if ( !pCreator ) return;

	m_CreateDummy.Init();

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );
		
	ioHashString szDummyCharName = m_DummyChar.m_DummyCharName;
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	float fOffset = m_DummyChar.m_fDummyCharOffset;
	float fSideOffset = m_DummyChar.m_fDummyCharSideOffset;
	float fStartAngle = m_DummyChar.m_fDummyCharStartAngle;
	D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition() + (fOffset * vDir);
	vTargetPos += fSideOffset * vSideDir;

	ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
										m_CreateChar,
										vTargetPos,
										fStartAngle,
										0, true );
	if( pDummy )
		pDummy->AddWoundedTargetName( pOwner->GetCharName() );

	DummyCharCreateInfo kInfo;
	kInfo.m_DummyCharName = szDummyCharName;
	kInfo.m_iDummyCharIdx = iCurIndex;
	kInfo.m_fDummyCharStartAngle = fStartAngle;
	kInfo.m_vTargetPos = vTargetPos;

	m_CreateDummy = kInfo;

	m_bCreatedDummy = true;

	if( pCreator->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SS_CREATE;
		kPacket << m_CreateDummy.m_DummyCharName;
		kPacket << m_CreateDummy.m_iDummyCharIdx;
		kPacket << m_CreateDummy.m_vTargetPos;
		kPacket << m_CreateDummy.m_fDummyCharStartAngle;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetDummyBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;
	int iType;
	rkPacket >> iType;

	switch ( iType )
	{
		case SS_CREATE:
			{
				m_CreateDummy.Init();

				int iCurIndex;
				ioHashString szDummyCharName;
				D3DXVECTOR3 vPos;

				float fAngle;
				rkPacket >> szDummyCharName;
				rkPacket >> iCurIndex;
				rkPacket >> vPos;
				rkPacket >> fAngle;
				ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
																m_CreateChar,
																vPos, fAngle,
																0, false );
				if( pDummy )
					pDummy->AddWoundedTargetName( pOwner->GetCharName() );

				DummyCharCreateInfo kInfo;
				kInfo.m_DummyCharName = szDummyCharName;
				kInfo.m_iDummyCharIdx = iCurIndex;
				kInfo.m_fDummyCharStartAngle = fAngle;
				kInfo.m_vTargetPos = vPos;

				m_CreateDummy = kInfo;

				m_bCreatedDummy = true;
			}
			break;
		case SS_SET_CHAR:
			{
				SetCharState( pOwner, false );
				m_bSetChar = true;
			}
			break;
	}
	
}

void ioTargetDummyBuff::CheckDummyChar()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if ( !pCreator ) return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pCreator, m_CreateDummy.m_iDummyCharIdx );
	if( pDummy && pDummy->GetState() == DCS_DIE )
	{
		pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}
	
	if( !pDummy )
	{
		pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}
	else if( pDummy )
	{
		const D3DXVECTOR3& vPos = pDummy->GetWorldPosition();
		m_pOwner->SetWorldPosition( vPos );
	}
}
