#include "stdafx.h"
#include "BlockDefine.h"
#include "ioBlockManager.h"

void TileCoordToWorldCoord( int nTileX, int nTileY, int nTileZ, float& fOutWorldX, float& fOutWorldY, float& fOutWorldZ )
{

	D3DXVECTOR3 vPos;
	vPos.x = nTileX * UD_SIZE;
	vPos.z = ((g_BlockManager.GetTileMax()-1) - nTileZ) * UD_SIZE - ((g_BlockManager.GetTileMax()-1) * UD_SIZE);
	vPos.y = nTileY * UD_SIZE + g_BlockManager.GetBlockStartHeight();
	
	vPos = g_BlockManager.GetBlockRotation() * vPos;
	fOutWorldX = vPos.x;
	fOutWorldZ = vPos.z;
	fOutWorldY = vPos.y;
}

float RoundOff(float f)
{
	return floorf(f + FLOAT05);
}

void BlockCoordRotate( int fSrcX, int fSrcZ, float fTheta, float& fOutX, float& fOutZ )
{
	float mMat[2][2];
	mMat[0][0] = cos( DEGtoRAD( fTheta ) );
	mMat[1][1] = cos( DEGtoRAD( fTheta ) );

	mMat[0][1] = -1 * sin( DEGtoRAD( fTheta ) );
	mMat[1][0] = sin( DEGtoRAD( fTheta ) );	

	fOutX = mMat[0][0] * fSrcX + mMat[0][1] * fSrcZ;
	fOutZ = mMat[1][0] * fSrcX + mMat[1][1] * fSrcZ;
	fOutX = RoundOff( fOutX );
	fOutZ = RoundOff( fOutZ );
}
// 150911 JSM 최적화로 인해 함수 제거
/*
void GetAdjacentIndexList( IN int xPos, IN int zPos, IN int Range, OUT IN int nBlockMax, IntVec& adjacentIndexList )
{	
	for( int nZ =  max( 0, zPos - Range ); nZ <= min( nBlockMax - 1, zPos + Range ); ++nZ )
	{
		for( int nX = max( 0, xPos - Range ); nX <= min( nBlockMax - 1, xPos + Range ); ++nX )
		{
			adjacentIndexList.push_back( nX + nZ * nBlockMax );
		}
	}
}
*/