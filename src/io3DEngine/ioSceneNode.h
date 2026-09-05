

#ifndef _ioSceneNode_h_
#define _ioSceneNode_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioMapIterator.h"

#include "ioAxisAlignBox.h"

class ioNode;
class ioMovableObject;
class ioSceneManager;

class ioCamera;
class ioRenderQueue;
class ioSceneShadowBox;
class ioSoundBuffer;

class __EX ioSceneNode : public ioNode
{
public:
	typedef std::map< UniqueObjID, ioMovableObject* > AttachedObjList;
	typedef ioMapIterator< AttachedObjList > AttachedObjIter;

protected:
	AttachedObjList	m_AttachedObjList;
	mutable ioLightList m_LightList;

	ioSceneManager	*m_pSceneMgr;
	ioAxisAlignBox	m_WorldAABB;

	typedef std::list< ioSoundBuffer* > AttachedSoundList;
	AttachedSoundList m_NeedUpdateSoundList;

	DWORD	m_dwLightUpdateFrameCounter;
	bool	m_bDisableUpdateBound;

public:
	virtual void SetDisableUpdateBound( bool bDisable ){  m_bDisableUpdateBound = bDisable; }
	virtual void UpdateBound( bool bMust = false );

protected:
	void UpdateSound( bool bResetVolume );

public:
	virtual void Update( bool bUpdateChildren, bool bParentHasChanged, bool bResetVolume=false );
	virtual const ioAxisAlignBox& GetWorldAABB() const;

public:
	virtual void AttachObject( ioMovableObject *pObj );
	virtual ioMovableObject* GetAttachedObject( UniqueObjID eObjID );
	virtual int GetNumAttachedObject() const;

	virtual ioMovableObject* DetachObject( UniqueObjID eObjID );
	virtual void DetachObject( ioMovableObject *pObj );
    virtual void DetachAllObjects();

public:
	virtual ioSceneNode* CreateChildSceneNode();
	virtual ioSceneNode* GetParentSceneNode();

	virtual void RemoveAndDestroyChild( UniqueNodeID eNodeID );
	virtual void RemoveAndDestroyAllChildren();

protected:
	virtual ioNode* CreateChildImpl();

public:
	virtual void FindVisibleObjects( ioCamera *pCamera,
									 ioRenderQueue &queue,
									 bool bAllVisible = false );

	virtual void FindShadowCasterObjects( ioRenderQueue &queue,
										  ioSceneShadowBox &kSceneBox );

public:
	AttachedObjIter GetAttachedObjectIter();
	const ioLightList& GetLightList();

	void AttachSound( ioSoundBuffer *pBuf );

public:
	ioSceneManager* GetSceneMgr();

public:
	ioSceneNode( ioSceneManager *pSceneMgr );
	virtual ~ioSceneNode();
};

#endif
