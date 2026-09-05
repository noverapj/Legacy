#pragma once
#include "../../io3DEngine/ioStringConverter.h"

#include "BlockDefine.h"
#include "ioBlockProperty.h"

class ioBlock;
class ioPlayStage;

typedef std::map<int, ioBlockProperty*> BlockTempleteMap;
typedef std::vector<ioBlock*> BlockList;

class ioBlockManager : public Singleton< ioBlockManager >
{
protected:
	enum
	{
		UPDATE_BOUND_TIME = 3000,
	};

protected:
	ioPlayStage*		m_pStage;
	BlockModeType		m_eType;

	BlockTempleteMap	m_BlockTempleteMap;
	BlockList			m_BlockList;

	DWORD				m_dwPreUpdatedTime;
	DWORD				m_dwPlayStartTime;

protected:
	float				m_fBlockStartHeight;
	D3DXQUATERNION		m_BlockRotation;
	int					m_nTileMax;
	int					m_nHeightMax;

public:
	static ioBlockManager& GetSingleton();

public:
	void SetBlockModeType( BlockModeType eType );
	void Initialize( ioPlayStage* pStage );

	void Destroy();	

protected:
	ioBlockProperty* CreateBlockTemplete( ioINILoader& rkLoader, int nCode );

public:
	const ioBlockProperty* GetBlockTemplete( int nCode );

public:
	ioBlock* CreateBlock( int nCode, __int64 nUniqueIndex );
	void DestroyBlock( __int64 nUniqueIndex );
	ioBlock* GetBlock( __int64 nUniqueIndex );
	int GetBlockCount( int nCode );

	const BlockList& GetAllBlock();
	int GetBlockTotalCount();

public:
	void Update();
	void UpdateAfter();
	void UpdateBound();

public:
	void StartUpdateBound();

public:
	void SetTileMax( int nMax, int nHeight );
	int GetTileMax(){ return m_nTileMax; }
	int GetHeightMax(){ return m_nHeightMax; }
	float GetBlockStartHeight(){ return m_fBlockStartHeight; }
	const D3DXQUATERNION& GetBlockRotation(){ return m_BlockRotation; }

public:
	void SetShadowMapEnable( bool bEnable );
	void EnableMapEdge( bool bEnable );
	void StopAllTextureEffect( bool bStop );

public:
	void ExportBlockWorld();

public:
	ioBlockManager();
	virtual ~ioBlockManager();

};

#define g_BlockManager ioBlockManager::GetSingleton()

