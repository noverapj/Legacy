

#include "StdAfx.h"

#include "ioRcBiped.h"
#include "ioBiped.h"
#include "ioSkeleton.h"
#include "ioMath.h"
#include "HelpFunc.h"

ioBiped::ioBiped( ioSkeleton *pCreator )
{
	m_pCreator = pCreator;

	m_bCachedBipedXformOutOfDate = true;
	m_bCachedTransposeXformOutOfDate = true;

	D3DXMatrixIdentity( &m_ObjectOffsetTM );
	m_fLinkOffsetRate = 1.0f;

	m_accumulatedWeightAbsolute = 0.0f;
	m_accumulatedWeight = 0.0f;

	m_bUseExtraRotation = false;
	m_bLocalExtraRotation = true;

	m_bManualBlending = false;
}

ioBiped::~ioBiped()
{
}

ioNode* ioBiped::CreateChildImpl()
{
	return m_pCreator->CreateBiped();
}

void ioBiped::InitializeByRcBiped( ioRcBiped *pRcBiped )
{
	m_qtInitialOrientation = pRcBiped->GetRotation();
	m_vInitialPosition	= pRcBiped->GetTranslation();

	m_qtOrientation = m_qtInitialOrientation;
	m_vPosition = m_vInitialPosition;

	if( m_pCreator->IsLinkedSkeleton() )
	{
		m_ObjectOffsetTM = pRcBiped->GetObjectOffsetTM();
		m_fLinkOffsetRate = pRcBiped->GetLinkOffsetRate();

		D3DXMatrixMultiply( &m_BipedSpaceTM,
							&m_ObjectOffsetTM,
							&pRcBiped->GetBipedSpaceTM() );
	}
	else
	{
		m_BipedSpaceTM = pRcBiped->GetBipedSpaceTM();
	}
}

void ioBiped::SaveDefDerivedXform()
{
	m_vInitDerivedPosition = GetDerivedPosition();
	m_qtInitDerivedOrientation = GetDerivedOrientation();
}

void ioBiped::ClearBlend()
{
	if( !m_bManualBlending )
	{
		m_qtOrientation = m_qtInitialOrientation;
		m_vPosition = m_vInitialPosition;
	}

	m_accumulatedWeightAbsolute = 0.0f;
	m_accumulatedWeight = 0.0f;
}

void ioBiped::Blend( float fWeight, const D3DXVECTOR3 &vTrans, const D3DXQUATERNION &qtRot )
{
	if( !m_bManualBlending )
	{
		if( m_accumulatedWeightAbsolute == 0.0f )	// First Time
		{
			m_accumulatedWeightAbsolute = fWeight;
			
			m_qtRotationAbsolute   = qtRot;
			m_vTranslationAbsolute = vTrans;
		}
		else // Not First Time So Blend..
		{
			m_accumulatedWeightAbsolute += fWeight;

			float fFactor = fWeight / m_accumulatedWeightAbsolute;	// 앞쪽에 있던 애니가 가중치가 더 높다.
			m_qtRotationAbsolute   = ioMath::Slerp( fFactor, m_qtRotationAbsolute, qtRot );
			m_vTranslationAbsolute = ioMath::Lerp( fFactor, m_vTranslationAbsolute, vTrans );
		}
	}
}

void ioBiped::LockBlend()
{
	if( m_accumulatedWeightAbsolute > 0.0f )
	{
		if( m_accumulatedWeight == 0.0f )
		{
			m_accumulatedWeight = m_accumulatedWeightAbsolute;

			SetOrientation( m_qtRotationAbsolute );
			SetPosition( m_vTranslationAbsolute );
		}
		else
		{
			float fBlend = 1.0f;
			if( m_accumulatedWeight > 0.0f )
			{
				m_accumulatedWeight += m_accumulatedWeightAbsolute;
				fBlend = m_accumulatedWeightAbsolute / m_accumulatedWeight;
			}

			D3DXQUATERNION qtOrientation = ioMath::Slerp( fBlend,
														  m_qtOrientation,
														  m_qtRotationAbsolute );
			SetOrientation( qtOrientation );
			
			D3DXVECTOR3 vPosition = ioMath::Lerp( fBlend,
												  m_vPosition,
												  m_vTranslationAbsolute );
			SetPosition( vPosition );
		}

		m_accumulatedWeightAbsolute = 0.0f;
	}
}

void ioBiped::UpdateFromParent() const
{
	ioNode::UpdateFromParent();

	if( m_bUseExtraRotation )
	{
		if( m_bLocalExtraRotation )
		{
			D3DXQuaternionMultiply( &m_qtDerivedOrientation,
									&m_qtExtraRotation,
									&m_qtDerivedOrientation );
		}
		else
		{
			D3DXQuaternionMultiply( &m_qtDerivedOrientation,
									&m_qtDerivedOrientation,
									&m_qtExtraRotation );
		}
	}
}

void ioBiped::SetExtraRotation( const D3DXQUATERNION &qtExtra, bool bLocal )
{
	m_qtExtraRotation = qtExtra;
	m_bUseExtraRotation = true;
	m_bLocalExtraRotation = bLocal;
}

void ioBiped::ReleaseExtraRotation()
{
	m_bUseExtraRotation = false;
}

void ioBiped::SetManualBlending( bool bManual )
{
	m_bManualBlending = bManual;
}

D3DXMATRIX* ioBiped::GetFullTransform() const
{
	if( m_bCachedTransformOutOfDate )
	{
		ioMath::MakeTransform( GetDerivedOrientation(),
							   GetDerivedPosition(),
							   GetDerivedScale(),
							   m_CachedTransform );

		m_bCachedTransformOutOfDate = false;

		m_bCachedBipedXformOutOfDate = true;
		m_bCachedTransposeXformOutOfDate = true;
	}

	return &m_CachedTransform;
}

D3DXMATRIX* ioBiped::GetBipedSpaceFullXform() const
{
	GetFullTransform();

	if( m_bCachedBipedXformOutOfDate )
	{
		D3DXMatrixMultiply( &m_CachedBipedTransform, &m_BipedSpaceTM, &m_CachedTransform );
		m_bCachedBipedXformOutOfDate = false;
	}

	return &m_CachedBipedTransform;
}

D3DXMATRIX* ioBiped::GetTransposeBipedSpaceFullXform() const
{
	GetBipedSpaceFullXform();

	if( m_bCachedTransposeXformOutOfDate )
	{
		D3DXMatrixTranspose( &m_CachedTransposeTransform, &m_CachedBipedTransform );
		m_bCachedTransposeXformOutOfDate = true;
	}

	return &m_CachedTransposeTransform;
}

ioBiped* ioBiped::GetParentBiped() const
{
	return dynamic_cast< ioBiped* >( this->GetParent() );
}

