

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioHealItemMarker.h"

ioHealItemMarker::ioHealItemMarker( ioBaseChar *pOwner )
: ioTargetMarker( pOwner )
{
}

ioHealItemMarker::~ioHealItemMarker()
{
}

ioTargetMarker::MarkerType ioHealItemMarker::GetMarkerType() const
{
	return MT_HEAL_ITEM;
}

void ioHealItemMarker::SetMarkerState( ioPlayStage *pStage, MarkerState eState )
{
	ioTargetMarker::SetMarkerState( pStage, eState );
}

void ioHealItemMarker::Update( ioPlayStage *pStage )
{
	switch( m_MarkerState )
	{
	case MS_READY:
		UpdateReadyState( pStage );
		break;
	case MS_AIMED:
		UpdateAimedState( pStage );
		break;
	}
}

D3DXVECTOR3 ioHealItemMarker::GetHealItemPoint() const
{
	if( m_pOwner )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 22.0f + m_fMarkerYOffset, 0.0f );
		D3DXQUATERNION qtRotate = m_pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vTargetPos = vTargetPos + vDir * m_fMarkerRange;
		vTargetPos += m_pOwner->GetWorldPosition();
		return vTargetPos;
	}

	return D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}


UniqueObjID ioHealItemMarker::CreateEffect( ioPlayStage *pStage, const ioHashString &szEffect )
{
	if( !pStage ) return -1;
	if( szEffect.IsEmpty() ) return -1;

	D3DXVECTOR3 vPos = GetHealItemPoint();
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

	ioMapEffect *pEff = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if( pEff )
	{
		return pEff->GetUniqueID();
	}

	return -1;
}

void ioHealItemMarker::UpdateReadyState( ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( m_ReadyID == -1 ) return;

	bool bVisible = false;
	bool bDisable = false;

	if( m_pOwner->GetState() != CS_ATTACK && m_pOwner->GetState() != CS_JUMP )
		bDisable = true;

	if( m_pOwner->IsOwnerChar() )
		bVisible = true;

	D3DXVECTOR3 vPos = GetHealItemPoint();

	ioMapEffect *pEff = pStage->FindMapEffect( m_ReadyID );
	if( pEff )
	{
		pEff->SetWorldPosition( vPos );

		if( bDisable || !bVisible )
			pEff->SetVisible( false );
		else
			pEff->SetVisible( true );
	}
}

void ioHealItemMarker::UpdateAimedState( ioPlayStage *pStage )
{
	if( !pStage ) return;

	bool bVisible = false;
	bool bDisable = false;

	if( m_pOwner->GetState() != CS_ATTACK && m_pOwner->GetState() != CS_JUMP && m_pOwner->GetState() != CS_VEHICLE_DRIVER )
		bDisable = true;

	ioBaseChar *pTarget = GetTargetChar();
	if( m_pOwner->IsOwnerChar() || (pTarget && pTarget->IsOwnerChar()) )
		bVisible = true;

	if( !m_bPreUseEnable && m_bUseEnable )
	{
		DestroyMarker( pStage );
		m_AimedID = CreateEffect( pStage, m_szAimedEffect );
	}

	if( m_AimedID == -1 ) return;

	D3DXVECTOR3 vPos = GetHealItemPoint();
	if( pTarget )
		vPos = pTarget->GetMidPositionByRate();

	ioMapEffect *pEff = pStage->FindMapEffect( m_AimedID );
	if( pEff )
	{
		pEff->SetWorldPosition( vPos );

		if( bDisable || !bVisible )
			pEff->SetVisible( false );
		else
			pEff->SetVisible( true );
	}
}