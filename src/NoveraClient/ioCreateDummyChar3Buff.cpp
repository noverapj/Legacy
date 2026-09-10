

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioCreateDummyChar3Buff.h"


ioCreateDummyChar3Buff::ioCreateDummyChar3Buff()
{
	m_dwMotionEndTime     = 0;
	m_dwFireMotionEndTime = 0;
	m_dwDummyCreteTime	  = 0;
	m_iCurActiveCount     = 0;
	m_dwCreateTime		  = 0;
}

ioCreateDummyChar3Buff::ioCreateDummyChar3Buff( const ioCreateDummyChar3Buff &rhs )
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
	m_dwDummyCreteTime    = 0;

	m_nDummyIndex = -1;
}

ioCreateDummyChar3Buff::~ioCreateDummyChar3Buff()
{
}

void ioCreateDummyChar3Buff::LoadProperty( ioINILoader &rkLoader )
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

ioBuff* ioCreateDummyChar3Buff::Clone()
{
	return new ioCreateDummyChar3Buff( *this );
}

void ioCreateDummyChar3Buff::StartBuff( ioBaseChar *pOwner )
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
	m_iCurActiveCount     = m_iMaxActiveCount;

	//동기화 문제로 생성 시점을 다음 프레임으로 변경, 이부분에서 
	//더미 생성 패킷을 실어서 보내면 받는 쪽은 버프가 생성이 안되있음으로 동기화 불가
	//CreateDummyChar( pOwner );

	m_dwDummyCreteTime = dwCurTime;

	m_dwBuffStartTime = dwCurTime;
	if( m_iOperationType == OT_DELAY )
		m_dwBuffStartTime += m_dwDelayDuration;

	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
}

bool ioCreateDummyChar3Buff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

	if( GetOwner() )
	{
		ioBaseChar* pCreater = GetOwner()->GetBaseChar( m_CreateChar );
		if( pCreater )
		{
			ioDummyChar* pDummyChar = g_DummyCharMgr.FindDummyChar( pCreater, m_nDummyIndex );
			if( pDummyChar )		
				pDummyChar->SetCurseDollTargetName( GetOwner()->GetCharName() );
		}
	}

	return true;
}

void ioCreateDummyChar3Buff::ProcessBuff( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( GetOwner() && GetOwner()->IsNeedProcess() && 0 < m_dwDummyCreteTime )
	{
		m_dwDummyCreteTime = 0;
		CreateDummyChar( GetOwner() );
	}

	if( m_dwBuffEndTime <= dwCurTime )
	{
		SetReserveEndBuff();
		return;
	}
}

void ioCreateDummyChar3Buff::EndBuff()
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

void ioCreateDummyChar3Buff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_iCurActiveCount;
	rkPacket << m_nDummyIndex;
}

void ioCreateDummyChar3Buff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurActiveCount;
	rkPacket >> m_nDummyIndex;
}

void ioCreateDummyChar3Buff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	int iSize;
	rkPacket >> iSize;
	for( int i = 0; i < iSize; ++i )
	{
		int iCurIndex;
		ioHashString szDummyCharName;
		D3DXVECTOR3 vPos;
		float fAngle;

		rkPacket >> szDummyCharName;
		rkPacket >> iCurIndex;
		rkPacket >> vPos;
		rkPacket >> fAngle;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex, m_CreateChar, vPos, fAngle, 0, false );

		if( pDummy )		
		{
			pDummy->SetCurseDollTargetName( pOwner->GetCharName() );
			m_nDummyIndex = iCurIndex;
		}
	}
}


void ioCreateDummyChar3Buff::CreateDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

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

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex, m_CreateChar, vTargetPos,fStartAngle, 0, true );

		if( pDummy )
		{
			pDummy->SetCurseDollTargetName( pOwner->GetCharName() );
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

bool ioCreateDummyChar3Buff::IsProtected()
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