

#include "stdafx.h"

#include "ioNode.h"
#include "ioSceneNode.h"
#include "ioEntityGroup.h"

#include "ioBiped.h"
#include "ioEntity.h"
#include "HelpFunc.h"

#include "ioMovableObject.h"
#include "ioRenderQueue.h"

UniqueObjID ioMovableObject::m_sNextUniqueID = 0;

ioMovableObject::ioMovableObject()
{
	m_UniqueID = GetNextUniqueID();
	m_pParentNode = NULL;

	m_RenderQueueID = RENDER_MAIN;

	m_bVisible = true;
	m_bShadowCastEnable = false;
	m_bShadowRecvEnable = false;
}

ioMovableObject::~ioMovableObject()
{
	ioSceneNode *pSNode = GetParentSceneNode();
	if( pSNode )
	{
		pSNode->DetachObject( this );
	}
}

void ioMovableObject::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;
}

void ioMovableObject::SetShadowCastEnable( bool bEnable )
{
	m_bShadowCastEnable = bEnable;
}

void ioMovableObject::SetShadowRecvEnable( bool bEnable )
{
	m_bShadowRecvEnable = bEnable;
}

void ioMovableObject::SetAlphaRate( int iAlphaRate )
{
}

void ioMovableObject::SetLightReceiveType( LightReceiveType eType )
{
}

void ioMovableObject::UpdateApplyLightList()
{
}

void ioMovableObject::SetRenderQueueGroup( RenderGroupID eGroupID )
{
	m_RenderQueueID = eGroupID;
}

RenderGroupID ioMovableObject::GetRenderQueueGroup() const
{
	return m_RenderQueueID;
}

ioNode* ioMovableObject::GetParentNode() const
{
	return m_pParentNode;
}

ioSceneNode* ioMovableObject::GetParentSceneNode() const
{
	return dynamic_cast< ioSceneNode* >( m_pParentNode );
}

void ioMovableObject::NotifyAttached( ioNode *pParent )
{
	m_pParentNode = pParent;
}

bool ioMovableObject::IsAttached() const
{
	if( m_pParentNode )
		return true;

	return false;
}

void ioMovableObject::Update( float fTime )
{
}

void ioMovableObject::UpdateBounds( bool bForceUpdate )
{
}

void ioMovableObject::UpdateGenShadowMapToQueue( ioRenderQueue &queue )
{
}

D3DXMATRIX* ioMovableObject::GetTransform() const
{
	if( m_pParentNode )
	{
		return m_pParentNode->GetFullTransform();
	}

	static D3DXMATRIX matIdentity( 1.0f, 0.0f, 0.0f, 0.0f,
								   0.0f, 1.0f, 0.0f, 0.0f,
								   0.0f, 0.0f, 1.0f, 0.0f,
								   0.0f, 0.0f, 0.0f, 1.0f );

	return &matIdentity;
}

const ioAxisAlignBox& ioMovableObject::GetWorldBoundBox( bool bDerive )
{
	if( bDerive )
	{
		m_WorldAABB = this->GetBoundBox();
		m_WorldAABB.Transform( GetParentNode()->GetFullTransform() );
	}

	return m_WorldAABB;
}

const ioSphere& ioMovableObject::GetWorldBoundSphere( bool bDerive )
{
	if( bDerive )
	{
		m_WorldSphere.SetCenter( this->GetBoundBox().GetCenter() );
		m_WorldSphere.SetRadius( this->GetBoundRadius() );

		m_WorldSphere.Transform( GetParentNode()->GetDerivedPosition(),
								 GetParentNode()->GetDerivedScale(),
								 GetParentNode()->GetDerivedOrientation() );
	}

	return m_WorldSphere;
}

UniqueObjID ioMovableObject::GetNextUniqueID()
{
	m_sNextUniqueID++;
	return m_sNextUniqueID;
}