

#include "stdafx.h"

#include "ioNode.h"
#include "ioCamera.h"
#include "HelpFunc.h"
#include "ioFrameTimer.h"

UniqueNodeID ioNode::m_sNextNodeID = 0;

ioNode::ioNode()
{
	m_pParent = NULL;
	m_UniqueID = GetNextUniqueID();

	D3DXQuaternionIdentity( &m_qtOrientation );
	m_vPosition.x = m_vPosition.y = m_vPosition.z = 0.0f;
	m_vScale.x = m_vScale.y = m_vScale.z = 1.0f;

	D3DXQuaternionIdentity( &m_qtDerivedOrientation );
	m_vDerivedPosition.x = m_vDerivedPosition.y = m_vDerivedPosition.z = 0.0f;
	m_vDerivedScale.x = m_vDerivedScale.y = m_vDerivedScale.z = 1.0f;

	m_fSquaredViewDepth = 0.0f;
	m_dwLastViewDepthUpdateFrame = 1;
	m_bParentNotified = false;

	NeedUpdate();
}

ioNode::~ioNode()
{
	RemoveAllChildren();

	if( m_pParent )
	{
		m_pParent->RemoveChild( this );
	}
}

UniqueObjID ioNode::GetNextUniqueID()
{
	m_sNextNodeID++;
	return m_sNextNodeID;
}

void ioNode::SetParent( ioNode *pParent )
{
	m_pParent = pParent;

	m_bParentNotified = false;
	NeedUpdate();
}

ioNode* ioNode::GetParent() const
{
	return m_pParent;
}

D3DXMATRIX* ioNode::GetFullTransform() const
{
	if( m_bCachedTransformOutOfDate )
	{
		ioMath::MakeTransform( GetDerivedOrientation(),
							   GetDerivedPosition(),
							   GetDerivedScale(),
							   m_CachedTransform );

		m_bCachedTransformOutOfDate = false;
	}

	return &m_CachedTransform;
}

void ioNode::Update( bool bUpdateChildren, bool bParentHasChanged, bool bResetVolume )
{
	m_bParentNotified = false;

	if( !bUpdateChildren && !m_bNeedParentUpdate && !m_bNeedChildUpdate && !bParentHasChanged )
		return;

	if( m_bNeedParentUpdate || bParentHasChanged )
	{
		UpdateFromParent();
	}

	if( m_bNeedChildUpdate || bParentHasChanged )
	{
		ChildNodeMap::iterator iter=m_ChildMap.begin();
		for( ; iter!=m_ChildMap.end() ; ++iter )
		{
			ioNode *pChild = iter->second;
			if( pChild )
				pChild->Update( true, true, bResetVolume );
		}
	}
	else
	{
		ChildUpdateSet::iterator iter=m_ChildrenToUpdate.begin();
		for( ; iter!=m_ChildrenToUpdate.end() ; ++iter )
		{
			ioNode *pChild = *iter;
			if( pChild )
				pChild->Update( true, false, bResetVolume );
		}
	}

	m_ChildrenToUpdate.clear();
	m_bNeedChildUpdate = false;
}

void ioNode::UpdateFromParent() const
{
	if( m_pParent )
	{
		D3DXQUATERNION qtParent  = m_pParent->GetDerivedOrientation();
		D3DXVECTOR3 vParentPos   = m_pParent->GetDerivedPosition();
		D3DXVECTOR3 vParentScale = m_pParent->GetDerivedScale();

		D3DXQuaternionMultiply( &m_qtDerivedOrientation, &m_qtOrientation, &qtParent );
		m_vDerivedPosition = qtParent * ( m_vPosition * vParentScale ) + vParentPos;
		m_vDerivedScale = m_vScale * vParentScale;
	}
	else
	{
		m_qtDerivedOrientation = m_qtOrientation;
		m_vDerivedPosition = m_vPosition;
		m_vDerivedScale = m_vScale;
	}

	m_bCachedTransformOutOfDate = true;
	m_bNeedParentUpdate = false;
}

ioNode* ioNode::CreateChild()
{
	ioNode *pNewNode = CreateChildImpl();
	if( pNewNode )
	{
		this->AddChild( pNewNode );
	}

	return pNewNode;
}

void ioNode::AddChild( ioNode *pChild )
{
	if( pChild )
	{
		m_ChildMap.insert( ChildNodeMap::value_type( pChild->GetUniqueID(), pChild ) );
		pChild->SetParent( this );
	}
}

int ioNode::GetNumChildren() const
{
	return static_cast< int >( m_ChildMap.size() );
}

ioNode* ioNode::GetChildByIndex( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetNumChildren() ) )
	{
		ChildNodeMap::const_iterator iter = m_ChildMap.begin();
		std::advance( iter, iIndex );
		return iter->second;
	}

	return NULL;
}

ioNode* ioNode::GetChildByUniqueID( UniqueNodeID eNodeID ) const
{
	ChildNodeMap::const_iterator iter = m_ChildMap.find( eNodeID );
	if( iter != m_ChildMap.end() )
		return iter->second;

	return NULL;
}

ioNode* ioNode::RemoveChild( UniqueNodeID eNodeID ) 
{
	ioNode *pChild = NULL;

	ChildNodeMap::iterator iter = m_ChildMap.find( eNodeID );
	if( iter != m_ChildMap.end() )
	{
		pChild = iter->second;
		CancelUpdate( pChild );

		m_ChildMap.erase( iter );
		pChild->SetParent( NULL );
	}

	return pChild;
}

ioNode* ioNode::RemoveChild( ioNode *pChild )
{
	if( pChild )
	{
		ChildNodeMap::iterator iter = m_ChildMap.find( pChild->GetUniqueID() );
		if( iter != m_ChildMap.end() && iter->second == pChild )
		{
			CancelUpdate( pChild );

			m_ChildMap.erase( iter );
			pChild->SetParent( NULL );
		}
	}

	return pChild;
}

void ioNode::SetOrientation( const D3DXQUATERNION &qt )
{
	m_qtOrientation = qt;
	NeedUpdate();
}

const D3DXQUATERNION& ioNode::GetOrientation() const
{
	return m_qtOrientation;
}

void ioNode::SetPosition( float fX, float fY, float fZ )
{
	SetPosition( D3DXVECTOR3( fX, fY, fZ ) );
}

void ioNode::SetPosition( const D3DXVECTOR3 &vPos )
{
	m_vPosition = vPos;
	NeedUpdate();
}

const D3DXVECTOR3& ioNode::GetPosition() const
{
	return m_vPosition;
}

D3DXMATRIX ioNode::GetLocalAxes() const
{
	D3DXVECTOR3 vAxisX, vAxisY, vAxisZ;
	vAxisX = m_qtOrientation * ioMath::UNIT_X;
	vAxisY = m_qtOrientation * ioMath::UNIT_Y;
	vAxisZ = m_qtOrientation * ioMath::UNIT_Z;

	D3DXMATRIX matAxis;
	D3DXMatrixIdentity( &matAxis );
	matAxis._11 = vAxisX.x;
	matAxis._21 = vAxisX.y;
	matAxis._31 = vAxisX.z;

	matAxis._12 = vAxisY.x;
	matAxis._22 = vAxisY.y;
	matAxis._32 = vAxisY.z;

	matAxis._13 = vAxisZ.x;
	matAxis._23 = vAxisZ.y;
	matAxis._33 = vAxisZ.z;

	return matAxis;
}

void ioNode::Translate( const D3DXVECTOR3 &vMove, TransformSpace eRelativeTo )
{
	switch( eRelativeTo )
	{
	case TS_LOCAL:
		m_vPosition += m_qtOrientation * vMove;
		break;
	case TS_WORLD:
		if( m_pParent )
		{
			D3DXQUATERNION qtParent, qtParentInverse;
			qtParent = m_pParent->GetDerivedOrientation();
			D3DXQuaternionInverse( &qtParentInverse, &qtParent );

			m_vPosition += qtParentInverse * vMove;
		}
		else
		{
			m_vPosition += vMove;
		}
		break;
	case TS_PARENT:
		m_vPosition += vMove;
		break;
	}

	NeedUpdate();
}

void ioNode::Translate( float fX, float fY, float fZ, TransformSpace eRelativeTo )
{
	Translate( D3DXVECTOR3( fX, fY, fZ ), eRelativeTo );
}

void ioNode::Translate( const D3DXMATRIX &matAxis,
					    const D3DXVECTOR3 &vMove,
						TransformSpace eRelativeTo )
{
	D3DXVECTOR3 vDerivedMove;
	D3DXVec3TransformNormal( &vDerivedMove, &vMove, &matAxis );
	Translate( vDerivedMove, eRelativeTo );
}

void ioNode::Translate( const D3DXMATRIX &matAxis,
						float fX, float fY, float fZ,
						TransformSpace eRelativeTo )
{
	Translate( matAxis, D3DXVECTOR3( fX, fY, fZ ), eRelativeTo );
}

void ioNode::RollZ( float fDegrees, TransformSpace eRelativeTo )
{
	Rotate( ioMath::UNIT_Z, fDegrees, eRelativeTo );
}

void ioNode::PitchX( float fDegrees, TransformSpace eRelativeTo )
{
	Rotate( ioMath::UNIT_X, fDegrees, eRelativeTo );
}

void ioNode::YawY( float fDegrees, TransformSpace eRelativeTo )
{
	Rotate( ioMath::UNIT_Y, fDegrees, eRelativeTo );
}

void ioNode::Rotate( const D3DXVECTOR3 &vAxis, float fDegrees, TransformSpace eRelativeTo )
{
	D3DXQUATERNION qt;
	D3DXQuaternionRotationAxis( &qt, &vAxis, DEGtoRAD(fDegrees) );

	Rotate( qt, eRelativeTo );
}

void ioNode::Rotate( const D3DXQUATERNION &qt, TransformSpace eRelativeTo )
{
	D3DXQUATERNION qtDerived, qtDerivedInverse;

	switch( eRelativeTo )
	{
	case TS_PARENT:
		D3DXQuaternionMultiply( &m_qtOrientation, &m_qtOrientation, &qt );
		break;
	case TS_WORLD:
		qtDerived = GetDerivedOrientation();
		D3DXQuaternionInverse( &qtDerivedInverse, &qtDerived );

		D3DXQuaternionMultiply( &m_qtOrientation, &qtDerivedInverse, &m_qtOrientation );
		D3DXQuaternionMultiply( &m_qtOrientation, &m_qtOrientation, &qt );
		D3DXQuaternionMultiply( &m_qtOrientation, &m_qtOrientation, &qtDerived );
		break;
	case TS_LOCAL:
		D3DXQuaternionMultiply( &m_qtOrientation, &qt, &m_qtOrientation );
		break;
	}

	NeedUpdate();
}

const D3DXQUATERNION& ioNode::GetDerivedOrientation() const
{
	if( m_bNeedParentUpdate )
	{
		UpdateFromParent();
	}

	return m_qtDerivedOrientation;
}

const D3DXVECTOR3& ioNode::GetDerivedPosition() const
{
	if( m_bNeedParentUpdate )
	{
		UpdateFromParent();
	}

	return m_vDerivedPosition;
}

const D3DXVECTOR3& ioNode::GetDerivedScale() const
{
	if( m_bNeedParentUpdate )
	{
		UpdateFromParent();
	}

	return m_vDerivedScale;
}

void ioNode::RemoveAllChildren()
{
	ChildNodeMap::iterator iter;
	for( iter=m_ChildMap.begin() ; iter!=m_ChildMap.end() ; ++iter )
	{
		iter->second->SetParent( NULL );
	}

	m_ChildMap.clear();
	m_ChildrenToUpdate.clear();
}

void ioNode::SetScale( float fX, float fY, float fZ )
{
	m_vScale.x = fX;
	m_vScale.y = fY;
	m_vScale.z = fZ;
	NeedUpdate();
}

void ioNode::SetScale( const D3DXVECTOR3 &vScale )
{
	m_vScale = vScale;
	NeedUpdate();
}

const D3DXVECTOR3& ioNode::GetScale() const
{
	return m_vScale;
}

void ioNode::Scale( float fX, float fY, float fZ )
{
	m_vScale.x *= fX;
	m_vScale.y *= fY;
	m_vScale.z *= fZ;
	NeedUpdate();
}

void ioNode::Scale( const D3DXVECTOR3 &vScale )
{
	m_vScale = m_vScale * vScale;
	NeedUpdate();
}

void ioNode::NeedUpdate()
{
	m_bNeedParentUpdate = true;
	m_bNeedChildUpdate  = true;
	m_bCachedTransformOutOfDate = true;

	if( m_pParent && !m_bParentNotified )
	{
		m_pParent->RequestUpdate( this );
		m_bParentNotified = true;
	}

	m_ChildrenToUpdate.clear();
}

void ioNode::RequestUpdate( ioNode *pChild )
{
	if( !pChild || m_bNeedChildUpdate )
		return;

	m_ChildrenToUpdate.insert( pChild );
	if( m_pParent && !m_bParentNotified )
	{
		m_pParent->RequestUpdate( this );
		m_bParentNotified = true;
	}
}

void ioNode::CancelUpdate( ioNode *pChild )
{
	m_ChildrenToUpdate.erase( pChild );

	if( m_ChildrenToUpdate.empty() && m_pParent && !m_bNeedChildUpdate )
	{
		m_pParent->CancelUpdate( this );
		m_bParentNotified = false;
	}
}

float ioNode::GetSquaredViewDepth( const ioCamera *pCam ) const
{
	if( m_dwLastViewDepthUpdateFrame < g_FrameTimer.GetFrameCounter() )
	{
		D3DXVECTOR3 vDiff = GetDerivedPosition() - pCam->GetPosition();
		m_fSquaredViewDepth = D3DXVec3LengthSq( &vDiff );
		m_dwLastViewDepthUpdateFrame = g_FrameTimer.GetFrameCounter();
	}

	return m_fSquaredViewDepth;
}

