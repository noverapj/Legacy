#include "StdAfx.h"
#include "ioCreateDummyByAttackedBuff.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioCreateDummyByAttackedBuff::ioCreateDummyByAttackedBuff(void)
{
}

ioCreateDummyByAttackedBuff::ioCreateDummyByAttackedBuff( const ioCreateDummyByAttackedBuff &rhs ) :
ioBuff( rhs ),
m_DummyList( rhs.m_DummyList ),
m_vDestroyDummyList( rhs.m_vDestroyDummyList ),
m_iMaxAttackedCount( rhs.m_iMaxAttackedCount ),
m_iMaxCreateDummyCount( rhs.m_iMaxCreateDummyCount )
{

}

ioCreateDummyByAttackedBuff::~ioCreateDummyByAttackedBuff(void)
{
}

ioBuff* ioCreateDummyByAttackedBuff::Clone()
{
	return new ioCreateDummyByAttackedBuff(*this);
}

void ioCreateDummyByAttackedBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyList, m_vDestroyDummyList );

	m_iMaxAttackedCount = rkLoader.LoadInt_e( "max_attaced_count", 0 );
	m_iMaxCreateDummyCount = rkLoader.LoadInt_e( "max_create_dummy_count", 0 );
}

void ioCreateDummyByAttackedBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	m_iCurAttackedCount = 0;
	m_iCurCreatedDummyCount = 0;
}

void ioCreateDummyByAttackedBuff::OwnerCharApplyWeapon()
{
	if ( !m_pOwner && !m_pOwner->IsNeedProcess() )
		return;

	m_iCurAttackedCount++;
	if ( m_iCurAttackedCount >= m_iMaxAttackedCount )
	{
		m_iCurAttackedCount = 0;
		if ( m_iCurCreatedDummyCount < m_iMaxCreateDummyCount )
		{
			m_iCurCreatedDummyCount++;
			CreateDummyChar( m_pOwner );
		}
		else
			SetReserveEndBuff();
	}
}

void ioCreateDummyByAttackedBuff::CreateDummyChar( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;
	if( m_DummyList.empty() )
		return;

	//g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyList );

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	int iSize = m_DummyList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szDummyCharName = m_DummyList[i].m_DummyCharName;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		float fOffset = m_DummyList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyList[i].m_fDummyCharSideOffset;
		float fStartAngle = m_DummyList[i].m_fDummyCharStartAngle;

		D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition() + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
			pOwner->GetCharName(),
			vTargetPos,
			fStartAngle,
			0, true );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << BS_CREATE_DUMMY;
			kPacket << i;
			kPacket << iCurIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioCreateDummyByAttackedBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	int iType;
	rkPacket >> iType;
	if ( iType == BS_CREATE_DUMMY )
	{
		int iDummyNum,iDummyIndex;
		rkPacket >> iDummyNum >> iDummyIndex;

		ioPlayStage *pStage = pOwner->GetCreator();
		if( !pStage ) return;
		if( m_DummyList.empty() )
			return;

		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSideDir;
		D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSideDir, &vSideDir );

		if ( COMPARE( iDummyNum, 0, (int)m_DummyList.size() ) )
		{
			ioHashString szDummyCharName = m_DummyList[iDummyNum].m_DummyCharName;

			float fOffset = m_DummyList[iDummyNum].m_fDummyCharOffset;
			float fSideOffset = m_DummyList[iDummyNum].m_fDummyCharSideOffset;
			float fStartAngle = m_DummyList[iDummyNum].m_fDummyCharStartAngle;

			D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition() + (fOffset * vDir);
			vTargetPos += fSideOffset * vSideDir;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iDummyIndex,
				pOwner->GetCharName(),
				vTargetPos,
				fStartAngle,
				0, true );
		}
	}
	else if ( iType == BS_END )
		SetReserveEndBuff();
}

void ioCreateDummyByAttackedBuff::ProcessBuff( float fTimePerSec )
{
	ioBuff::ProcessBuff( fTimePerSec );
	if ( m_pOwner && m_pOwner->IsNeedProcess() )
	{
		if ( m_iCurCreatedDummyCount >= m_iMaxCreateDummyCount )
		{
			SetReserveEndBuff();
			if ( m_pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
				kPacket << m_pOwner->GetCharName();
				kPacket << GetName();
				kPacket << BS_END;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	
}