

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioCreateDummyChar2Buff.h"


ioCreateDummyChar2Buff::ioCreateDummyChar2Buff()
{
	m_dwMotionEndTime     = 0;
	m_dwFireMotionEndTime = 0;
	m_dwCreateTime        = 0;
	m_iCurActiveCount     = 0;
}

ioCreateDummyChar2Buff::ioCreateDummyChar2Buff( const ioCreateDummyChar2Buff &rhs )
	: ioBuff( rhs ),
	m_bDisableDownState( rhs.m_bDisableDownState ),
	m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
	m_szActionAnimation( rhs.m_szActionAnimation ),
	m_fActionAniRate( rhs.m_fActionAniRate ),
	m_DummyCharList( rhs.m_DummyCharList ),
	m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList )
{
	m_dwMotionEndTime     = 0;
	m_dwFireMotionEndTime = 0;
	m_dwCreateTime        = 0;
	m_iCurActiveCount     = 0;
}

ioCreateDummyChar2Buff::~ioCreateDummyChar2Buff()
{
}

void ioCreateDummyChar2Buff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "action_animation", "", szBuf, MAX_PATH );
	m_szActionAnimation = szBuf;
	m_fActionAniRate = rkLoader.LoadFloat_e( "action_ani_rate", FLOAT1 );

	//
	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );
}

ioBuff* ioCreateDummyChar2Buff::Clone()
{
	return new ioCreateDummyChar2Buff( *this );
}

void ioCreateDummyChar2Buff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	m_BuffState = BS_READY;
	m_dwMotionEndTime     = 0;
	m_dwFireMotionEndTime = 0;
	m_dwCreateTime        = 0;
	m_iCurActiveCount     = m_iMaxActiveCount;;
}

bool ioCreateDummyChar2Buff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	m_BuffState = BS_READY;
	m_dwMotionEndTime     = 0;
	m_dwFireMotionEndTime = 0;
	m_dwCreateTime        = 0;

	return true;
}

void ioCreateDummyChar2Buff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	ProcessOwnerState();

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

			if( m_pOwner->GetState() != CS_JUMP || m_pOwner->GetJumpState() != JS_JUMPPING || m_BuffState != BS_CREATE_DUMMY )
			{
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
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

bool ioCreateDummyChar2Buff::CheckOwnerStateCheck()
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

bool ioCreateDummyChar2Buff::CheckRestoreStateCheck()
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

void ioCreateDummyChar2Buff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	ioBuff::EndBuff();
}

void ioCreateDummyChar2Buff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_iCurActiveCount;
}

void ioCreateDummyChar2Buff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurActiveCount;
}

void ioCreateDummyChar2Buff::CheckActiveCount()
{
	m_iCurActiveCount--;
	m_iCurActiveCount = max( m_iCurActiveCount, 0 );
}

void ioCreateDummyChar2Buff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

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
	}
}

void ioCreateDummyChar2Buff::ProcessOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	switch( m_BuffState )
	{
	case BS_READY:
		if( m_iCurActiveCount <= 0 )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}
		ProcessReadyState( pOwner );
		break;
	case BS_CREATE_DUMMY:
		ProcessCreateDummyState( pOwner );
		break;
	}
}

void ioCreateDummyChar2Buff::ProcessReadyState( ioBaseChar *pOwner )
{
	if( pOwner->IsAttackKey() )
	{
		SetCreateDummyState( pOwner );
		CheckActiveCount();
	}
}

void ioCreateDummyChar2Buff::ProcessCreateDummyState( ioBaseChar *pOwner )
{
	if( m_dwCreateTime > 0 && m_dwCreateTime < FRAMEGETTIME() )
	{
		m_dwCreateTime = 0;
		CreateDummyChar( pOwner );
	}

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		SetReadyState( pOwner );
		if( CheckRestoreStateCheck() )
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
}

void ioCreateDummyChar2Buff::SetReadyState( ioBaseChar *pOwner )
{
	m_BuffState = BS_READY;
	m_dwFireMotionEndTime = 0;
	m_dwMotionEndTime     = 0;
	m_dwCreateTime        = 0;
}

void ioCreateDummyChar2Buff::SetCreateDummyState( ioBaseChar *pOwner )
{
	m_BuffState = BS_CREATE_DUMMY;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_szActionAnimation );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fActionAniRate );

		m_dwCreateTime = m_dwMotionEndTime = m_dwFireMotionEndTime = FRAMEGETTIME();
		m_dwCreateTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_fActionAniRate;
		m_dwMotionEndTime  += pGrp->GetAnimationFullTime( iAniID ) * m_fActionAniRate;
		m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * m_fActionAniRate;
	}
}

void ioCreateDummyChar2Buff::CreateDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;
	if( !pOwner->IsNeedProcess() )
		return;
	if( m_DummyCharList.empty() )
		return;

	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );

	vDummyCharCreateInfoList vCreateDummyList;
	vCreateDummyList.clear();

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

		vCreateDummyList.push_back( kInfo );
	}

	int iCreateCnt = vCreateDummyList.size();
	SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
	kPacket << pOwner->GetCharName();
	kPacket << GetName();
	kPacket << iCreateCnt;
	if( pOwner->IsNeedSendNetwork() && iCreateCnt > 0 )
	{
		for( int i=0; i<iCreateCnt; ++i )
		{
			kPacket << vCreateDummyList[i].m_DummyCharName;
			kPacket << vCreateDummyList[i].m_iDummyCharIdx;
			kPacket << vCreateDummyList[i].m_vTargetPos;
			kPacket << vCreateDummyList[i].m_fDummyCharStartAngle;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioCreateDummyChar2Buff::IsProtected()
{
	if( m_BuffState == BS_CREATE_DUMMY )
	{
		if( m_dwFireMotionEndTime > 0 && m_dwFireMotionEndTime > FRAMEGETTIME() )
		{
			return true;
		}
	}
	return false;
}

bool ioCreateDummyChar2Buff::CheckDontMoveState()
{
	if( m_BuffState == BS_CREATE_DUMMY )
		return true;

	return false;
}