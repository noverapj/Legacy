#pragma once
#include "BlockDefine.h"
#include "ioBlockManager.h"
#include "ioBlock.h"
#include "ioBlockEventAgency.h"
#include "ioMapCollisionModule.h"
#include "ioBlockMapBase.h"

class ioPlayStage;
class ioBlockManager;
class ioBlockSetupManager;

class ioBlockWorld : public ioBlockEventListener, public ioMapCollisionModule
{
public:
	struct BaseCharEvent
	{
		TilePos			m_SetUpPos;
		TilePos			m_CurrPos;

		BaseCharEvent(){};
		~BaseCharEvent(){};
	};
	typedef std::map<ioHashString, BaseCharEvent*> BaseCharEventMap;

protected:
	ioPlayStage*			m_pGameStage;
	BlockModeType			m_BlockModeType;

	ioBlockSetupManager*	m_pSetupMgr;
	ioBlockMapBase*			m_pMapBase;

	ioBlock*				m_CurrSetupBlock;
	ioCamera*				m_pCullingSetUpCamera;
	ioCamera*				m_pCullingCharCamera;

protected:
	CollRenderType			m_BlockCollRender;

protected:
	BaseCharEventMap		m_BaseCharEventMap;

public:
	inline ioPlayStage* GetCrator(){ return m_pGameStage; }
	inline ioBlockMapBase* GetMapBase(){ return m_pMapBase; }

public:
	void Initialize( BlockModeType eType );

public:
	inline BlockModeType GetBlockModeType(){ return m_BlockModeType; }

public:
	void LoadPlayModeBlock();

public:
	void DestroySetupBlock();
	void CreateSetupBlock( DWORD dwCode, const TilePos& Pos );
	ioBlock* GetSetUpBlock();

public:
	void CreateBlock( int nCode, int nX, int nY, int nZ, float fRotate, __int64 nUniqueIndex );
	void CreateBlock( int nCode, const TilePos& Pos, float fRotate, __int64 nUniqueIndex );
	void DestroyBlock( const TilePos& Pos );

protected:
	void DestroyBaseCharEvent();
	ioBlockWorld::BaseCharEvent* GetBaseCharEvent( const ioHashString& szName );

public:	
	void CheckAfterLoop( ioCamera *pCamera );

	void ProcessBaseCharAlphaEvent( ioBaseChar* pOwner, ioCamera *pCamera );
	void ProcessBaseCharAlphaEvent( const ioPlayEntity* pEntity, const ioCamera* pCamera, const TilePos& Pos, ioCamera* pCullingCam );
	void GetCameraVisibleList( IN const ioCamera* pDisplayCamera,
							   IN const ioCamera* pCullingCamera,
							   IN const ioPlayEntity* pEntity,
							   IN const TilePos& Pos,
							   OUT ioBlockList& InList,
							   OUT ioBlockList& OutList );

	bool CheckInterPlay( ioBaseChar* pOwner );

public:
	void SetBlockCollRender( ioBlockWorld::CollRenderType eRender ){ m_BlockCollRender = eRender; }

public:
	bool CheckFishEnable( const D3DXVECTOR3& vTargetPos );

protected:
	virtual void ReciveEvent( ioBlockEventListener::EventType eType, ioBlockEventParameter* param );

public:
	void ProcessUpdate();

public:
	virtual void RenderObject( ioCamera* pCamera );
	virtual void RenderUI();

public:	
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );
	
public:
	void TestMapBlock( SP2Packet &rkPacket );

public:
	ioBlockWorld( ioPlayStage* pStage );
	virtual ~ioBlockWorld();
};