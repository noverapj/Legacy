

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioCreateDummyCharBuff.h"

#include "DummyCharDefine.h"

ioCreateDummyCharBuff::ioCreateDummyCharBuff()
{
	m_bCreatedDummy = false;
}

ioCreateDummyCharBuff::ioCreateDummyCharBuff( const ioCreateDummyCharBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_fAlphaRate( rhs.m_fAlphaRate ),
m_bNoWoundState( rhs.m_bNoWoundState ),
m_bHideMiniMap( rhs.m_bHideMiniMap ),
m_bHideFootDustEffect( rhs.m_bHideFootDustEffect ),
m_bCheckCamera( rhs.m_bCheckCamera ),
m_DummyCharList( rhs.m_DummyCharList ),
m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList )
{
}

ioCreateDummyCharBuff::~ioCreateDummyCharBuff()
{
}

void ioCreateDummyCharBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fAlphaRate = rkLoader.LoadFloat_e( "alpha_rate", FLOAT1 );
	m_bNoWoundState = rkLoader.LoadBool_e( "no_wound_state", false );
	m_bHideMiniMap = rkLoader.LoadBool_e( "hide_minimap", false );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bHideFootDustEffect = rkLoader.LoadBool_e( "hide_foot_dust_effect", false );
	m_bCheckCamera = rkLoader.LoadBool_e( "check_camera", false );

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );
}

ioBuff* ioCreateDummyCharBuff::Clone()
{
	return new ioCreateDummyCharBuff( *this );
}

void ioCreateDummyCharBuff::StartBuff( ioBaseChar *pOwner )
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

	if( m_bNoWoundState )
		pOwner->SetNoWoundState( true );

	m_bCreatedDummy = false;

	if( m_bHideFootDustEffect )
	{
		pOwner->SetHideFootDustEffect( true );
	}

	m_iCurActiveCount = m_iMaxActiveCount;
}

bool ioCreateDummyCharBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

	if( m_bNoWoundState )
		pOwner->SetNoWoundState( true );

	if( m_bHideFootDustEffect )
	{
		pOwner->SetHideFootDustEffect( true );
	}

	return true;
}

void ioCreateDummyCharBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	if( m_bNoWoundState && !m_pOwner->IsCanNoWound() )
	{
		SetReserveEndBuff();
		return;
	}

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

	if( GetOwner() && GetOwner()->IsNeedProcess() && !m_bCreatedDummy )
	{
		CreateDummyChar( GetOwner() );
		m_bCreatedDummy = true;
	}

	if( m_bCreatedDummy )
		CheckDummyChar();
}

bool ioCreateDummyCharBuff::CheckOwnerStateCheck()
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

bool ioCreateDummyCharBuff::CheckRestoreStateCheck()
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

void ioCreateDummyCharBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		int iSize = m_CreateDummyList.size();
		for( int i=0; i<iSize; ++i )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_CreateDummyList[i].m_iDummyCharIdx );
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

		if( m_bNoWoundState )
			pOwner->SetNoWoundState( false );

		if( m_bHideFootDustEffect )
		{
			pOwner->SetHideFootDustEffect( false );
		}
	}

	ioBuff::EndBuff();
}

void ioCreateDummyCharBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo(rkPacket);
	rkPacket << m_iCurActiveCount;

	int iSize = m_CreateDummyList.size();
	rkPacket << iSize;
	for( int i=0; i<iSize; ++i )
	{
		rkPacket << m_CreateDummyList[i].m_DummyCharName;
		rkPacket << m_CreateDummyList[i].m_iDummyCharIdx;
		rkPacket << m_CreateDummyList[i].m_fDummyCharStartAngle;
		rkPacket << m_CreateDummyList[i].m_vTargetPos;
	}
}

void ioCreateDummyCharBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurActiveCount;

	m_CreateDummyList.clear();
	int iSize;
	rkPacket >> iSize;
	for( int i=0; i<iSize; ++i )
	{
		DummyCharCreateInfo kInfo;
		rkPacket >> kInfo.m_DummyCharName;
		rkPacket >> kInfo.m_iDummyCharIdx;
		rkPacket >> kInfo.m_fDummyCharStartAngle;
		rkPacket >> kInfo.m_vTargetPos;
		m_CreateDummyList.push_back(kInfo);
	}
}

void ioCreateDummyCharBuff::CreateDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;
	if( !pOwner->IsNeedProcess() )
		return;
	if( m_DummyCharList.empty() )
		return;

	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );

	m_CreateDummyList.clear();

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	int iSize = m_DummyCharList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
		float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

		D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition() + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
														pOwner->GetCharName(),
														vTargetPos,
														fStartAngle,
														0, true );

		DummyCharCreateInfo kInfo;
		kInfo.m_DummyCharName = szDummyCharName;
		kInfo.m_iDummyCharIdx = iCurIndex;
		kInfo.m_fDummyCharStartAngle = fStartAngle;
		kInfo.m_vTargetPos = vTargetPos;

		m_CreateDummyList.push_back( kInfo );
	}

	int iCreateCnt = m_CreateDummyList.size();
	SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
	kPacket << pOwner->GetCharName();
	kPacket << GetName();
	kPacket << iCreateCnt;
	if( pOwner->IsNeedSendNetwork() && iCreateCnt > 0 )
	{
		for( int i=0; i<iCreateCnt; ++i )
		{
			kPacket << m_CreateDummyList[i].m_DummyCharName;
			kPacket << m_CreateDummyList[i].m_iDummyCharIdx;
			kPacket << m_CreateDummyList[i].m_vTargetPos;
			kPacket << m_CreateDummyList[i].m_fDummyCharStartAngle;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateDummyCharBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	m_CreateDummyList.clear();

	int iSize;
	rkPacket >> iSize;
	for( int i=0; i<iSize; ++i )
	{
		int iCurIndex;
		ioHashString szDummyCharName;
		D3DXVECTOR3 vPos;

		float fAngle;
		rkPacket >> szDummyCharName;
		rkPacket >> iCurIndex;
		rkPacket >> vPos;
		rkPacket >> fAngle;
		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
														pOwner->GetCharName(),
														vPos, fAngle,
														0, false );

		DummyCharCreateInfo kInfo;
		kInfo.m_DummyCharName = szDummyCharName;
		kInfo.m_iDummyCharIdx = iCurIndex;
		kInfo.m_fDummyCharStartAngle = fAngle;
		kInfo.m_vTargetPos = vPos;

		m_CreateDummyList.push_back( kInfo );
	}

	m_bCreatedDummy = true;
}

void ioCreateDummyCharBuff::CheckDummyChar()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	int iSize = m_CreateDummyList.size();
	for( int i=0; i<iSize; ++i )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_CreateDummyList[i].m_iDummyCharIdx );
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
	}
}

D3DXVECTOR3 ioCreateDummyCharBuff::GetDummyCharPos()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return ioMath::VEC3_ZERO;

	int iSize = m_CreateDummyList.size();
	for( int i=0; i<iSize; ++i )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_CreateDummyList[i].m_iDummyCharIdx );
		if( pDummy  )
		{
			return pDummy->GetWorldPosition();
		}
	}

	return pOwner->GetWorldPosition();
}

void ioCreateDummyCharBuff::CheckActiveCount()
{
	m_iCurActiveCount--;
	m_iCurActiveCount = max( m_iCurActiveCount, 0 );
}