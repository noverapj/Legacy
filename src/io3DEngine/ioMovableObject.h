

#ifndef _ioMovableObject_h_
#define _ioMovableObject_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioSphere.h"
#include "ioAxisAlignBox.h"

class ioNode;
class ioSceneNode;
class ioCamera;
class ioRenderQueue;

class __EX ioMovableObject
{
protected:
	UniqueObjID	m_UniqueID;
	ioNode	*m_pParentNode;

	ioAxisAlignBox m_WorldAABB;
	ioSphere m_WorldSphere;

	RenderGroupID m_RenderQueueID;
	bool	m_bVisible;
	bool	m_bShadowCastEnable;
	bool	m_bShadowRecvEnable;

private:
	static UniqueObjID m_sNextUniqueID;

public:
	virtual void SetVisible( bool bVisible );
	virtual void SetShadowCastEnable( bool bEnable );
	virtual void SetShadowRecvEnable( bool bEnable );

	virtual bool IsVisible() const { return m_bVisible; }
	virtual bool IsShadowCastEnable() const { return m_bShadowCastEnable; }
	virtual bool IsShadowRecvEnable() const { return m_bShadowRecvEnable; }

	virtual void SetAlphaRate( int iAlphaRate );
	virtual void SetLightReceiveType( LightReceiveType eType );
	virtual void UpdateApplyLightList();

	virtual void SetRenderQueueGroup( RenderGroupID eGroupID );
	virtual RenderGroupID GetRenderQueueGroup() const;

public:
	virtual D3DXMATRIX* GetTransform() const;

	virtual ioNode* GetParentNode() const;
	virtual ioSceneNode* GetParentSceneNode() const;
	virtual void NotifyAttached( ioNode *pParent );
	virtual bool IsAttached() const;

public:
	virtual void Update( float fTime );
	virtual void UpdateBounds( bool bForceUpdate = false );

	virtual void NotifyCurrentCamera( ioCamera *pCamera ) = 0;
	virtual void UpdateRenderQueue( ioRenderQueue &queue ) = 0;
	virtual void UpdateGenShadowMapToQueue( ioRenderQueue &queue );

	virtual const ioAxisAlignBox& GetBoundBox() const = 0;
	virtual float GetBoundRadius() const = 0;

public:
	const ioAxisAlignBox& GetWorldBoundBox( bool bDerive = false );
	const ioSphere& GetWorldBoundSphere( bool bDerive = false );

	inline UniqueObjID GetUniqueID() const { return m_UniqueID; }

protected:
	UniqueObjID GetNextUniqueID();

public:
	ioMovableObject();
	virtual ~ioMovableObject();
};

#endif
