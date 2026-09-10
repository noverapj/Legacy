#pragma once
#include "io3DCommon.h"
#define _USE_MATH_DEFINES
#include <math.h>

enum BlockRotate
{
	BR_UP,
	BR_DOWN,
	BR_LEFT,
	BR_RIGHT,
	BR_MAX,
};

enum UintDefine
{
	UD_SIZE			= 100,
	UD_SIZE_HARF	= UD_SIZE / 2,
};

enum FieldDefine
{
	TILE_MAX_GUILD	= 64,
	TILE_MAX_HOME	= 32,
	HEIGHT_MAX		= 20,	
	HEIGHT_MAX_HOME	= 8,
	BUFFER_MAX		= 64 * 64 * HEIGHT_MAX,
};

enum HousingBlockType
{
	HBT_CUT_TYPE	= 1000000,
	HBT_NONE		= 0,
};

enum CollRenderType
{
	RENDER_NONE,
	RENDER_BOX,
	RENDER_SPHERE,
	RENDER_INTERPLAY_BOX,
};

enum RotateSet
{
	ROTATE_0		= 0,
	ROTATE_90		= 1,
	ROTATE_180		= 2,
	ROTATE_270		= 3,
	ROTATE_360		= 4,
	RTATE_MAX		= 5,
	ROTATE_VALUE	= 90,
};

enum BlockModeType
{
	BMT_GUILD		= 0,
	BMT_PERSONAL	= 1,
};

#define COS_45 0.707107f
#define SIN_45 0.707107f

float RoundOff(float f);

//타일 좌표계 -> 월드 좌표계
void TileCoordToWorldCoord( int nTileX, int nTileY, int nTileZ, float& fOutWorldX, float& fOutWorldY, float& fOutWorldZ );

//타일 로컬 좌표계 회전
void BlockCoordRotate( int fSrcX, int fSrcZ, float fTheta, float& fOutX, float& fOutZ );

//현재 타일의 인접한 타일의 위치를 구한다
void GetAdjacentIndexList( IN int xPos, IN int zPos, IN int Range, IN int nBlockMax, OUT IntVec& adjacentIndexList );

class TilePos
{
public:
	enum TilePosEvent
	{
		TPE_NONE	= 0,
		TPE_FISH	= 1,
		TPE_MAX,
	};

public:
	int x;
	int z;
	int y;
	int event;

	TilePos()
	{
		Init();
	}

	TilePos( int nX, int nY, int nZ )
	{
		x		= nX;
		y		= nY;
		z		= nZ;
		event	= 0;
	}

	void Init()
	{
		x = z = y = -1;
		event = 0;
	}

	TilePos operator+( const TilePos& rhs ) const
	{
		TilePos Pos;
		Pos.x = x + rhs.x;
		Pos.y = y + rhs.y;
		Pos.z = z + rhs.z;
		return Pos;
	}

	D3DXVECTOR3 operator +( const D3DXVECTOR3& rhs )const
	{
		D3DXVECTOR3 vPos( x, y, z );
		vPos += rhs;
		return vPos;
	}

	bool operator == ( const TilePos& rhs ) const
	{
		if( rhs.x == x && rhs.y == y && rhs.z == z )
			return true;

		return false;
	}

	bool operator != ( const TilePos& rhs ) const
	{
		if( rhs.x != x || rhs.y != y || rhs.z != z )
			return true;

		return false;
	}

	bool IsNotTilePos()
	{
		if( x == -1 || y == - 1 || z == -1 )
			return true;

		return false;
	}

	void ToWorldPos( const TilePos& Pos )
	{
		x += Pos.x;
		y += Pos.y;
		z += Pos.z;
	}

	void ToRotatePos( float fRotate )
	{
		float fX, fZ;
		BlockCoordRotate( x, z, fRotate, fX, fZ );
		x = fX;
		z = fZ;
	}
};

static TilePos ZERO_TILE_POS( 0, 0, 0 );

typedef std::vector<TilePos> TilePosList;
