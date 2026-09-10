

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioCrossBowMarker.h"

ioCrossBowMarker::ioCrossBowMarker( ioBaseChar *pOwner )
: ioTargetMarker( pOwner )
{
}

ioCrossBowMarker::~ioCrossBowMarker()
{
}

ioTargetMarker::MarkerType ioCrossBowMarker::GetMarkerType() const
{
	return MT_CROSS_BOW;
}

void ioCrossBowMarker::SetMarkerState( ioPlayStage *pStage, MarkerState eState )
{
	ioTargetMarker::SetMarkerState( pStage, eState );
}

void ioCrossBowMarker::Update( ioPlayStage *pStage )
{
	switch( m_MarkerState )
	{
	case MS_NONE:
		break;
	case MS_READY:
		UpdateReadyState( pStage );
		break;
	case MS_AIMING:
		UpdateAimingState( pStage );
		break;
	case MS_AIMED:
		UpdateAimedState( pStage );
		break;
	case MS_ZERO_AIMED:
		UpdateZeroAimedState( pStage );
		break;
	case MS_JUMP_AIM:
		UpdateJumpAimState( pStage );
		break;
	}
}

D3DXVECTOR3 ioCrossBowMarker::GetFireItemPoint() const
{
	ioBiped *pBiped = m_pOwner->GetGroup()->GetBiped( "Bip01 R Hand" );
	if( pBiped )
	{
		D3DXQUATERNION qtRotate = m_pOwner->GetTargetRot();
		D3DXVECTOR3 vTargetPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vTargetPos = vTargetPos * m_fMarkerRange;
		vTargetPos = vTargetPos + pBiped->GetDerivedPosition();
		vTargetPos = vTargetPos + m_pOwner->GetWorldPosition();

		return vTargetPos;
	}

	return D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

D3DXVECTOR3 ioCrossBowMarker::GetFireItemJumpPoint() const
{
	ioBiped *pBiped = m_pOwner->GetGroup()->GetBiped( "Bip01 R Hand" );
	if( pBiped )
	{
		D3DXQUATERNION qtRotate = m_pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_fJumpMarkerAngle) );
		vDir = qtRot * vDir;

		D3DXVECTOR3 vTargetPos = m_fMarkerRange * vDir;
		vTargetPos += m_pOwner->GetWorldPosition() + pBiped->GetDerivedPosition();

		return vTargetPos;
	}

	return D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

UniqueObjID ioCrossBowMarker::CreateEffect( ioPlayStage *pStage, const ioHashString &szEffect )
{
	if( !pStage ) return -1;
	if( szEffect.IsEmpty() ) return -1;

	D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

	if( m_MarkerState == MS_JUMP_AIM )
		vPos = GetFireItemJumpPoint();
	else
		vPos = GetFireItemPoint();

	ioMapEffect *pEff = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if( pEff )
	{
		return pEff->GetUniqueID();
	}

	return -1;
}

void ioCrossBowMarker::UpdateReadyState( ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( m_ReadyID == -1 ) return;

	bool bVisible = false;
	bool bDisable = false;

	if( m_pOwner->GetState() != CS_ATTACK && m_pOwner->GetState() != CS_JUMP )
		bDisable = true;

	if( m_pOwner->IsOwnerChar() )
		bVisible = true;

	D3DXVECTOR3 vPos = GetFireItemPoint();

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

void ioCrossBowMarker::UpdateAimingState( ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( m_AimingID == -1 ) return;

	bool bVisible = false;
	bool bDisable = false;

	if( m_pOwner->GetState() != CS_ATTACK && m_pOwner->GetState() != CS_JUMP )
		bDisable = true;

	if( m_pOwner->IsOwnerChar() )
		bVisible = true;

	D3DXVECTOR3 vPos = GetFireItemPoint();

	ioMapEffect *pEff = pStage->FindMapEffect( m_AimingID );
	if( pEff )
	{
		pEff->SetWorldPosition( vPos );

		if( bDisable || !bVisible )
			pEff->SetVisible( false );
		else
			pEff->SetVisible( true );
	}
}

void ioCrossBowMarker::UpdateAimedState( ioPlayStage *pStage )
{
	if( !pStage ) return;

	bool bVisible = false;
	bool bDisable = false;

	if( m_pOwner->GetState() != CS_ATTACK && m_pOwner->GetState() != CS_JUMP )
		bDisable = true;

	D3DXVECTOR3 vPos = GetFireItemPoint();
	if( IsBallTarget() )
	{
		ioBall *pBall = m_pOwner->FindBall();
		if( pBall )
		{
			bVisible = true;
			vPos = pBall->GetMidPositionByRate();
		}
	}
	else
	{
		ioBaseChar *pTarget = GetTargetChar();
		if( pTarget )
		{
			if( pTarget->IsOwnerChar() )
				bVisible = true;

			vPos = pTarget->GetMidPositionByRate();
		}
	}

	if( m_pOwner->IsOwnerChar() )
		bVisible = true;

	if( !m_bPreUseEnable && m_bUseEnable )
	{
		DestroyMarker( pStage );
		m_AimedID = CreateEffect( pStage, m_szAimedEffect );
	}

	if( m_AimedID == -1 ) return;

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

void ioCrossBowMarker::UpdateZeroAimedState( ioPlayStage *pStage )
{
	if( !pStage ) return;

	bool bVisible = false;
	bool bDisable = false;

	if( m_pOwner->GetState() != CS_ATTACK && m_pOwner->GetState() != CS_JUMP )
		bDisable = true;

	D3DXVECTOR3 vPos = GetFireItemPoint();
	if( IsBallTarget() )
	{
		ioBall *pBall = m_pOwner->FindBall();
		if( pBall )
		{
			bVisible = true;
			vPos = pBall->GetMidPositionByRate();
		}
	}
	else
	{
		ioBaseChar *pTarget = GetTargetChar();
		if( pTarget )
		{
			if( pTarget->IsOwnerChar() )
				bVisible = true;

			vPos = pTarget->GetMidPositionByRate();
		}
	}

	if( m_pOwner->IsOwnerChar() )
		bVisible = true;

	if( !m_bPreUseEnable && m_bUseEnable )
	{
		DestroyMarker( pStage );
		m_ZeroAimedID = CreateEffect( pStage, m_szZeroAimedEffect );
	}

	if( m_ZeroAimedID == -1 ) return;

	ioMapEffect *pEff = pStage->FindMapEffect( m_ZeroAimedID );
	if( pEff )
	{
		pEff->SetWorldPosition( vPos );

		if( bDisable || !bVisible )
			pEff->SetVisible( false );
		else
			pEff->SetVisible( true );
	}
}

void ioCrossBowMarker::UpdateJumpAimState( ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( m_JumpAimID == -1 ) return;

	bool bVisible = false;
	bool bDisable = false;

	if( m_pOwner->GetState() != CS_ATTACK && m_pOwner->GetState() != CS_JUMP )
		bDisable = true;

	if( m_pOwner->IsOwnerChar() )
		bVisible = true;

	D3DXVECTOR3 vPos = GetFireItemJumpPoint();

	ioMapEffect *pEff = pStage->FindMapEffect( m_JumpAimID );
	if( pEff )
	{
		pEff->SetWorldPosition( vPos );

		if( bDisable || !bVisible )
			pEff->SetVisible( false );
		else
			pEff->SetVisible( true );
	}
}
