
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioCreateDummyChar4Buff.h"

#include "DummyCharDefine.h"

ioCreateDummyChar4Buff::ioCreateDummyChar4Buff()
{
	m_dwMotionEndTime     = 0;
	m_dwFireMotionEndTime = 0;
	m_dwCreateTime        = 0;
	m_iCurActiveCount     = 0;
}

ioCreateDummyChar4Buff::ioCreateDummyChar4Buff( const ioCreateDummyChar4Buff &rhs )
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

	m_nDummyIndex = -1;

	m_bSendCreateDummy = false;
}

ioCreateDummyChar4Buff::~ioCreateDummyChar4Buff()
{
}

void ioCreateDummyChar4Buff::LoadProperty( ioINILoader &rkLoader )
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

ioBuff* ioCreateDummyChar4Buff::Clone()
{
	return new ioCreateDummyChar4Buff( *this );
}

void ioCreateDummyChar4Buff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	m_dwMotionEndTime     = 0;
	m_dwFireMotionEndTime = 0;
	m_dwCreateTime        = 0;
	m_iCurActiveCount     = m_iMaxActiveCount;;

	//CreateDummyChar( pOwner );

	m_dwBuffStartTime = dwCurTime;
	if( m_iOperationType == OT_DELAY )
		m_dwBuffStartTime += m_dwDelayDuration;

	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
	
	//SetReserveEndBuff();
}

bool ioCreateDummyChar4Buff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

void ioCreateDummyChar4Buff::ProcessBuff( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pOwner = GetOwner();
	
	if( !m_bSendCreateDummy )
	{
		CreateDummyChar(pOwner);
		m_bSendCreateDummy = !m_bSendCreateDummy;
	}


	if( !pOwner || (pOwner->IsNeedProcess() &&	m_nDummyIndex == -1) )
	{	
		SetReserveEndBuff();
		return;
	}

	if( m_dwBuffEndTime <= dwCurTime )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_nDummyIndex != -1 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_nDummyIndex );
		if( !pDummy )
			SetReserveEndBuff();
	}
}

void ioCreateDummyChar4Buff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	if( m_nDummyIndex != -1 && m_pOwner && !m_CreateChar.IsEmpty() )
	{
		ioBaseChar* pCreater = m_pOwner->GetBaseChar( m_CreateChar );

		if( pCreater )
		{
			ioDummyChar* pDummyChar = g_DummyCharMgr.FindDummyChar( pCreater, m_nDummyIndex );
			if( pDummyChar )
				pDummyChar->SetDieState( true );
		}
	}
	ioBuff::EndBuff();
}

void ioCreateDummyChar4Buff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_iCurActiveCount;
}

void ioCreateDummyChar4Buff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurActiveCount;
}

void ioCreateDummyChar4Buff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
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

		ioCurseDummyChar *pCurseDummy = ToCurseDummyChar( pDummy );
		if( pCurseDummy )
		{
			pCurseDummy->SetCharNameInfo( pOwner->GetCharName(), m_CreateChar );
			pCurseDummy->SetCopyOwnerInfo( pOwner, true );
			m_nDummyIndex = iCurIndex;
		}
	}
}

void ioCreateDummyChar4Buff::CreateDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	
	if( !pStage ) return;
	
	if( !pOwner->IsNeedProcess() )
		return;
	
	if( m_DummyCharList.empty() )
		return;
	
	ioBaseChar *pAttacker = pOwner->GetBaseChar( m_CreateChar );
	if( !pAttacker )
		return;
	
	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );

	vDummyCharCreateInfoList vCreateDummyList;
	vCreateDummyList.clear();

	D3DXVECTOR3 vDir = pAttacker->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
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

		//반드시 하나만 존재 해야 한다.( 룰 )
		ioDummyChar* pDummyCheck = g_DummyCharMgr.FindDummyCharToName( pOwner, szDummyCharName );
		if( pDummyCheck )
		{
			SetReserveEndBuff();
			return;
		}

		D3DXVECTOR3 vTargetPos = pAttacker->GetWorldPosition() + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
													   pOwner->GetCharName(),
													   vTargetPos,
													   fStartAngle,
													   0, true );
		
		ioCurseDummyChar *pCurseDummy = ToCurseDummyChar( pDummy );
		if( pCurseDummy )
		{
			pCurseDummy->SetCharNameInfo( pOwner->GetCharName(), m_CreateChar );
			pCurseDummy->SetCopyOwnerInfo( pOwner, true );
			m_nDummyIndex = iCurIndex;
		}

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

bool ioCreateDummyChar4Buff::IsProtected()
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