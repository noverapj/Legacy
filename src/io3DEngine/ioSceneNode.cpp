

#include "stdafx.h"

#include "ioFrameTimer.h"
#include "ioMath.h"
#include "ioCamera.h"

#include "ioNode.h"
#include "ioMovableObject.h"
#include "ioEntity.h"

#include "ioSceneNode.h"
#include "ioRenderQueue.h"
#include "ioSceneShadowBox.h"
#include "ioSceneManager.h"

#include "ioSound.h"
#include "ioSoundManager.h"

ioSceneNode::ioSceneNode( ioSceneManager *pSceneMgr )
: ioNode(), m_pSceneMgr(pSceneMgr)
{
	m_dwLightUpdateFrameCounter = 0;
	m_bDisableUpdateBound = false;
	NeedUpdate();
}

ioSceneNode::~ioSceneNode()
{
	// HARDCODE: NeedUpdate() 함수 호출을 피하기 위해 DetachAllObjects를 쓰지않고 직접한다.

	AttachedObjList::iterator iter=m_AttachedObjList.begin();
	for( ; iter!=m_AttachedObjList.end() ; ++iter )
	{
		iter->second->NotifyAttached( NULL );
	}

	m_AttachedObjList.clear();
}

void ioSceneNode::UpdateBound( bool bMust /* = false */ )
{
 	m_WorldAABB.SetNull();

	AttachedObjList::iterator iter=m_AttachedObjList.begin();
	for( ; iter!=m_AttachedObjList.end() ; ++iter )
	{
		ioMovableObject *pObj = iter->second;
		if( pObj )
		{
			pObj->UpdateBounds( bMust );
			m_WorldAABB.Merge( pObj->GetWorldBoundBox(true) );
		}
	}

	ChildNodeMap::iterator iChild=m_ChildMap.begin();
	for( ; iChild!=m_ChildMap.end() ; iChild++ )
	{
		ioSceneNode *pSChild = dynamic_cast<ioSceneNode*>( iChild->second );
		if( pSChild )
		{
			m_WorldAABB.Merge( pSChild->m_WorldAABB );
		}
	}
}

void ioSceneNode::UpdateSound( bool bResetVolume )
{
	if( m_NeedUpdateSoundList.empty() )
		return;

	int iVolume = g_SoundMgr.GetVolumeByDistance( GetDerivedPosition() );

	AttachedSoundList::iterator iter = m_NeedUpdateSoundList.begin();
	while( iter != m_NeedUpdateSoundList.end() )
	{
		ioSoundBuffer *pBuf = *iter;

		if( pBuf->IsPlaying() )
		{
			if( bResetVolume )
				pBuf->SetVolume( DSBVOLUME_MIN );

			pBuf->SetVolume( iVolume );
			++iter;
		}
		else
		{
			iter = m_NeedUpdateSoundList.erase( iter );
			pBuf->ReleaseAttach();
		}
	}
}

void ioSceneNode::Update( bool bUpdateChildren, bool bParentHasChanged, bool bResetVolume )
{
	ioNode::Update( bUpdateChildren, bParentHasChanged, bResetVolume );

	if( !m_bDisableUpdateBound )
		UpdateBound();

	UpdateSound( bResetVolume );
}

void ioSceneNode::AttachObject( ioMovableObject *pObj )
{
	if( pObj )
	{
		pObj->NotifyAttached( this );
		m_AttachedObjList.insert( AttachedObjList::value_type( pObj->GetUniqueID(), pObj ) );

		NeedUpdate();
	}
}

ioMovableObject* ioSceneNode::GetAttachedObject( UniqueObjID eObjID )
{
	AttachedObjList::iterator iter = m_AttachedObjList.find( eObjID );
	if( iter != m_AttachedObjList.end() )
		return iter->second;

	return NULL;
}

ioMovableObject* ioSceneNode::DetachObject( UniqueObjID eObjID )
{
	AttachedObjList::iterator iter = m_AttachedObjList.find( eObjID );
	if( iter == m_AttachedObjList.end() )
	{
		LOG.PrintTimeAndLog( 0, "ioSceneNode::DetachObject - Not Exist Object : %d", eObjID );
		return NULL;
	}

	ioMovableObject *pRet = iter->second;
	m_AttachedObjList.erase( iter );
	pRet->NotifyAttached( NULL );

	NeedUpdate();

	return pRet;
}

void ioSceneNode::DetachObject( ioMovableObject *pObj )
{
	if( !pObj )	return;

	AttachedObjList::iterator iter=m_AttachedObjList.begin();
	for( ; iter!=m_AttachedObjList.end() ; ++iter )
	{
		if( iter->second == pObj )
		{
			m_AttachedObjList.erase( iter );
			break;
		}
	}

	pObj->NotifyAttached( NULL );

	NeedUpdate();
}

void ioSceneNode::DetachAllObjects()
{
	ioMovableObject *pObj;

	AttachedObjList::iterator iter=m_AttachedObjList.begin();
	for( ; iter!=m_AttachedObjList.end() ; ++iter )
	{
		pObj = iter->second;
		if( pObj )
		{
			pObj->NotifyAttached( NULL );
		}
	}

	m_AttachedObjList.clear();

	NeedUpdate();
}

int ioSceneNode::GetNumAttachedObject() const
{
	return m_AttachedObjList.size();
}

void ioSceneNode::FindVisibleObjects( ioCamera *pCamera,
									  ioRenderQueue &queue,
									  bool bAllVisible )
{
	if( !bAllVisible )
	{
		if( m_pParent && !pCamera->IsVisible( m_WorldAABB ) )
			return;
	}

	AttachedObjList::iterator iObj = m_AttachedObjList.begin();
	for( ; iObj!=m_AttachedObjList.end() ; ++iObj )
	{
		ioMovableObject *pObj = iObj->second;
		if( pObj )
		{
			pObj->NotifyCurrentCamera( pCamera );

			if( pObj->IsVisible() )
			{
				pObj->UpdateRenderQueue( queue );
			}
		}
	}

	ChildNodeMap::iterator iNode = m_ChildMap.begin();
	for( ; iNode != m_ChildMap.end() ; ++iNode )
	{
		ioSceneNode *pChildNode = dynamic_cast< ioSceneNode* >(iNode->second);
		if( pChildNode )
		{
			pChildNode->FindVisibleObjects( pCamera, queue, bAllVisible );
		}
	}
}

void ioSceneNode::FindShadowCasterObjects( ioRenderQueue &queue, ioSceneShadowBox &kSceneBox )
{
	AttachedObjList::iterator iterObj=m_AttachedObjList.begin();
	for( ; iterObj != m_AttachedObjList.end() ; iterObj++ )
	{
		ioMovableObject *pObj = iterObj->second;
		if( pObj )
		{
			if( pObj->IsVisible() )
			{
				if( pObj->IsShadowCastEnable() && kSceneBox.AddShadowCaster( pObj ) )
				{
					pObj->UpdateGenShadowMapToQueue( queue );
				}
			}
		}
	}

	ChildNodeMap::iterator iChild = m_ChildMap.begin();
	for( ; iChild != m_ChildMap.end() ; iChild++ )
	{
		ioSceneNode *pChildNode = dynamic_cast< ioSceneNode* >(iChild->second);
		if( pChildNode )
		{
			pChildNode->FindShadowCasterObjects( queue, kSceneBox );
		}
	}
}

const ioAxisAlignBox& ioSceneNode::GetWorldAABB() const
{
	return m_WorldAABB;
}

ioNode* ioSceneNode::CreateChildImpl()
{
	return m_pSceneMgr->CreateSceneNode();
}

ioSceneNode* ioSceneNode::CreateChildSceneNode()
{
	return dynamic_cast< ioSceneNode* >( this->CreateChild() );
}

ioSceneNode* ioSceneNode::GetParentSceneNode()
{
	return dynamic_cast< ioSceneNode* >( this->GetParent() );
}

void ioSceneNode::RemoveAndDestroyChild( UniqueNodeID eNodeID )
{
	ioSceneNode *pChild = dynamic_cast<ioSceneNode*>( GetChildByUniqueID( eNodeID ) );
	if( pChild )
	{
		pChild->RemoveAndDestroyAllChildren();
	
		RemoveChild( eNodeID );
		pChild->GetSceneMgr()->DestroySceneNode( eNodeID );
	}
}

void ioSceneNode::RemoveAndDestroyAllChildren()
{
	ChildNodeMap::iterator iter = m_ChildMap.begin();
	for( ; iter != m_ChildMap.end() ; )
	{
		ioSceneNode *pNode = dynamic_cast< ioSceneNode* >( iter->second );

		// DestroySceneNode에서 pNode가 현재 부모인 this에 대해 RemoveChild를 호출하기때문에
		// 반복자가 망가진다. 때문에 여기서 먼저 ++ 한다.
		++iter;

		pNode->RemoveAndDestroyAllChildren();
		pNode->GetSceneMgr()->DestroySceneNode( pNode->GetUniqueID() );
	}

	m_ChildMap.clear();
	NeedUpdate();
}

const ioLightList& ioSceneNode::GetLightList()
{
	if( m_dwLightUpdateFrameCounter < g_FrameTimer.GetFrameCounter() )
	{
		if( m_pSceneMgr )
		{
			m_pSceneMgr->PopulateLightList( m_WorldAABB.GetCenter(),
											m_WorldAABB.GetHalfDiagonal(),
											m_LightList );
		}

		m_dwLightUpdateFrameCounter = g_FrameTimer.GetFrameCounter();
	}

	return m_LightList;
}

ioSceneNode::AttachedObjIter ioSceneNode::GetAttachedObjectIter()
{
	return AttachedObjIter( m_AttachedObjList.begin(), m_AttachedObjList.end() );
}

void ioSceneNode::AttachSound( ioSoundBuffer *pBuf )
{
	AttachedSoundList::iterator iter = std::find( m_NeedUpdateSoundList.begin(),
												  m_NeedUpdateSoundList.end(),
												  pBuf );

	if( iter == m_NeedUpdateSoundList.end() )
	{
		pBuf->AddAttachRef();
		m_NeedUpdateSoundList.push_back( pBuf );
	}
}

ioSceneManager* ioSceneNode::GetSceneMgr()
{
	return m_pSceneMgr;
}