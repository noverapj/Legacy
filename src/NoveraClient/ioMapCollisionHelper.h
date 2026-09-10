#pragma once
#include "../io3DEngine/ioAxisAlignBox.h"

namespace ioMapCollisionHelper
{

	enum
	{
		CHECK_COLLISION_RANGE = 800,
	};

	bool IsCollisionCheckRange( const D3DXVECTOR3 &vPos1, const D3DXVECTOR3 &vPos2 );

	bool ModifyMoveDir( IN const ioAxisAlignBox &rkSrcBox,
						IN const ioAxisAlignBox &rkMoveBox,
						IN const ioAxisAlignBox &rkDestBox,
						IN bool bForceMove,
						OUT D3DXVECTOR3 *pMove );

	bool HasMountedEntity( IN ioGameEntity* pMount, IN const MountedEntityList& rkMount );

	bool ReCalculateMoveVectorByMountAvailableEntity( IN ioPlayEntity* pEntity, 
													  IN ioAxisAlignBox& kAxisMoveBox,
													  IN const ioAxisAlignBox& kAxisBox,
													  IN bool bMonsterVsMonsterCollision,
													  IN const MountAvailableList& AvailableList,
													  IN const MountedEntityList* pMountList,
													  IN const D3DXVECTOR3& vMoveDir,
													  OUT D3DXVECTOR3 *pMove,
													  OUT bool &bCol );

	bool ReCalculateMoveVectorByEntity( IN ioPlayEntity* pEntity, 
										IN const GameEntityList& List,
										IN ioAxisAlignBox& kAxisMoveBox,
										IN const ioAxisAlignBox& kAxisBox,
										IN const D3DXVECTOR3& vMoveDir,
										OUT D3DXVECTOR3 *pMove,
										OUT bool &bCol );


	bool ReCalculateMoveVectorByWorldMesh( IN ioPlayEntity* pEntity,
										   IN const ioOrientBox& kMoveBox, 
										   IN ioOpcodeShape* pShape,
										   IN const D3DXMATRIX* pShapeWorldMat,
										   IN float fInverseMapHeight,
										   IN const D3DXVECTOR3& vMoveDir,
										   IN bool bFloating,
										   OUT D3DXVECTOR3 *pMove,
										   OUT bool &bCol );

	bool CheckPlayEntityMapCollision( IN ioPlayStage* pStage, IN ioGameEntity* pEntity, IN ioOpcodeShape *pShape, IN int nMoveCount, IN const D3DXMATRIX* pShapeWorldMat = NULL );


	bool CheckCollisionLine( OUT D3DXVECTOR3& vColPos, IN ioOpcodeShape *pShape, IN const D3DXVECTOR3 &vStart, IN const D3DXVECTOR3 &vEnd, IN const D3DXMATRIX* pShapeWorldMat = NULL );

	bool CheckMountAvailableEntityVsSegment( const MountAvailableList& MountList, const ioSegment& kSegment );
	bool CheckRayVsMapCollisionByRange( IN const ioRay& kRay, IN ioOpcodeShape *pShape, IN const D3DXMATRIX* pShapeWorldMat, IN float fCheckRange );
	bool CheckWeaponPiercingCollision( IN ioWeapon* pWeapon, IN ioOpcodeShape *pShape, IN const D3DXMATRIX* pShapeWorldMat = NULL );
}