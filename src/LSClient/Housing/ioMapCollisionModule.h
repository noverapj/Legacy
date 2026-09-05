#pragma once
#include "ioBlock.h"

class ioPlayStage;
class ioBlockWorld;
class ioBlock;
class ioBlockMapBase;
struct BlockMapBuffer;

class ioMapCollisionModule
{
protected:
	ioBlockMapBase* m_pMapBase;

public:
	void SetMapBase( ioBlockMapBase* pBase ){ m_pMapBase = pBase; }

public:
	float GetHeight( float fXPos, float fZPos );
	float GetHeight( float fXPos, float fYPos, float fZPos );
	float GetHeight( D3DXVECTOR3 vPos );

	float GetHeightByOBB( float fXPos, float fZPos );
	float GetHeightByOBB( float fXPos, float fYPos, float fZPos );
	float GetHeightByOBB( D3DXVECTOR3 vPos );

public:
	bool ReCalculateMoveVectorByBlock( IN ioPlayEntity* pEntity,
									   IN const ioAxisAlignBox& kAxisMoveBox,
									   IN const D3DXVECTOR3& vMoveDir,
									   IN bool bFloating,
									   OUT D3DXVECTOR3 *pMove,
									   OUT bool &bCol );

	void CheckPlayEntityBlockCollision( ioGameEntity* pEntity, ioPlayStage* pStage );
	
	bool CheckCollLine( IN const D3DXVECTOR3 &vStart, IN const D3DXVECTOR3 &vEnd, OUT D3DXVECTOR3& vColPos );

	bool TestBlockMeshCollision( const ioRay &rkRay, D3DXVECTOR3 *pColPoint );
	bool TestBlockMeshCollision( const ioOrientBox &rkBox );

	bool CheckCollRayVsBlockByRange( IN const ioRay& rkRay, IN float fCheckRange );	

	bool CheckWeaponVSBlockCollision( IN ioPlayStage* pStage, IN ioWeapon* pWeapon, bool bPiercingCheck = false );
	bool CheckBlockVSBlockCollision( ioBlock* pBlock, const TilePos& NextPos, float fNextRotate );

	void ResetColl( BlockMapBuffer* List );
public:
	ioMapCollisionModule();
	virtual ~ioMapCollisionModule();

};