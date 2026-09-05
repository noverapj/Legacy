

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioWorldDecoration.h"

ioWorldDecoration::ioWorldDecoration( ioEntityGroup *pGrp ) : ioGameEntity(pGrp)
{
	m_pCreator = NULL;
	m_bUseChangeAlpha = false;
	m_iChangeAlphaRate = MAX_ALPHA_RATE;
}

ioWorldDecoration::~ioWorldDecoration()
{
	m_pCreator = NULL;
}

void ioWorldDecoration::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	if( m_pCreator && m_bUseChangeAlpha )
	{
		ioBaseChar *pMyChar = m_pCreator->GetOwnerChar();
		if( !pMyChar )	return;

		ioCamera *pCamera = g_App.GetCamera();
		if( !pCamera )	return;

		D3DXVECTOR3 vMyPos = pMyChar->GetMidPositionByRate();
		D3DXVECTOR3 vDiff = pCamera->GetPosition() - vMyPos;
		ioRay kCamRay( vMyPos, vDiff );

		if( !IsNowAlphaChanging() )
		{
			if( ioMath::TestIntersection( kCamRay, GetWorldCollisionBox() ) )
			{
				SetTargetAlphaRate( m_iChangeAlphaRate );
			}
			else
			{
				SetTargetAlphaRate( MAX_ALPHA_RATE );
			}
		}
	}
}

void ioWorldDecoration::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioGameEntity::UpdateAfter( fFrameGap );
}

int ioWorldDecoration::GetTypeID() const
{
	return GET_WORLD_DECO;
}

bool ioWorldDecoration::IsMountingAvailable() const
{
	return false;
}

void ioWorldDecoration::UpdateBottomHeight() const
{
	m_fCurBottomHeight = GetWorldPosition().y;
}

void ioWorldDecoration::SetLoopAni( int iAniID )
{
	if( !m_pGroup )	return;
	if( !m_pGroup->HasSkeleton() )	return;

	if( COMPARE( iAniID, 0, m_pGroup->GetAnimationCnt() ) )
	{
		m_pGroup->SetLoopAni( iAniID, FLOAT100 );
	}
}

void ioWorldDecoration::SetRenderGroup( RenderGroupID eGroupID )
{
	if( m_pGroup )
	{
		m_pGroup->SetRenderQueueGroup( eGroupID );
	}
}

bool ioWorldDecoration::HasOpcodeShape() const
{
	if( m_pGroup )
	{
		if( m_pGroup->GetOpcodeShape() )
			return true;
	}

	return false;
}

ioOpcodeShape* ioWorldDecoration::GetOpcodeShape() const
{
	if( m_pGroup )
		return m_pGroup->GetOpcodeShape();

	return NULL;
}

UniqueObjID ioWorldDecoration::GetUniqueID() const
{
	if( m_pGroup )
		return m_pGroup->GetUniqueID();

	return -1;
}

void ioWorldDecoration::SetCreator( ioPlayStage *pStage )
{
	m_pCreator = pStage;
}

void ioWorldDecoration::SetChangeAlphaRate( bool bUse, int iRate )
{
	m_bUseChangeAlpha = bUse;
	m_iChangeAlphaRate = iRate;
}
