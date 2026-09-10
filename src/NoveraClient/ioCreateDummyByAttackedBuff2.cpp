#include "StdAfx.h"
#include "ioCreateDummyByAttackedBuff2.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioCreateDummyByAttackedBuff2::ioCreateDummyByAttackedBuff2(void)
{
}

ioCreateDummyByAttackedBuff2::ioCreateDummyByAttackedBuff2( const ioCreateDummyByAttackedBuff2 &rhs ):
ioBuff( rhs ),
m_DummyList( rhs.m_DummyList ),
m_vDestroyDummyList( rhs.m_vDestroyDummyList ),
m_iCheckPercent( rhs.m_iCheckPercent )
{

}

ioCreateDummyByAttackedBuff2::~ioCreateDummyByAttackedBuff2(void)
{
}

ioBuff* ioCreateDummyByAttackedBuff2::Clone()
{
	return new ioCreateDummyByAttackedBuff2( *this );
}

void ioCreateDummyByAttackedBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyList, m_vDestroyDummyList );

	m_iCheckPercent = rkLoader.LoadInt_e( "check_percent", 0 );
}

void ioCreateDummyByAttackedBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

void ioCreateDummyByAttackedBuff2::ProcessBuff( float fTimePerSec )
{
	ioBuff::ProcessBuff( fTimePerSec );
}

void ioCreateDummyByAttackedBuff2::OwnerCharApplyWeapon()
{
	if ( !m_pOwner || !m_pOwner->IsNeedProcess() )
		return;

	IORandom random;
	random.SetRandomSeed( timeGetTime() );
	int iRand = random.Random( 1000 );

	if ( iRand > m_iCheckPercent )
		return;

	CreateDummyChar( m_pOwner );
}

void ioCreateDummyByAttackedBuff2::CreateDummyChar( ioBaseChar *pOwner )
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
			kPacket << i;
			kPacket << iCurIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioCreateDummyByAttackedBuff2::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

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
