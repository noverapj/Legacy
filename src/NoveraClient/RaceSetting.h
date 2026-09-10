#pragma once

#include "GameEnumType.h"

struct CHARACTER;
class ioINILoader;

namespace RaceSetting
{
	struct RaceInfo;

	void LoadSetting();
	void LoadAvata( RaceInfo &rkInfo, ioINILoader &rkLoader );
	void LoadSkinColor( RaceInfo &rkInfo, ioINILoader &rkLoader );
	void LoadHair( RaceInfo &rkInfo, ioINILoader &rkLoader );
	void LoadBeard( RaceInfo &rkInfo, ioINILoader &rkLoader );
	void LoadFace( RaceInfo &rkInfo, ioINILoader &rkLoader );
	void LoadAccesory( RaceInfo &rkInfo, ioINILoader &rkLoader );
	void LoadUnderwear( RaceInfo &rkInfo, ioINILoader &rkLoader );

	void SetCurRaceDetail( RaceType eRace, bool bMale );
	void SetCurRaceDetail( RaceDetailType eType );
	void SetCurRaceDetail( const CHARACTER &rkInfo );

	const D3DXVECTOR3& AvataPosition();
	const D3DXVECTOR3& AvataCamPosition();
	float AvataAngleX();
	float AvataAngleY();

	// 디비에서 가져온 데이터가 1부터시작하기 때문에
	// 실제 데이터의 index는 1부터 시작한다. 갯수는 앞에 0번에 더미가 하나들어있어서 실 개수보다 1개 많다.

	int GetNumSkinColor();
	D3DCOLORVALUE GetSkinColor( int iIndex );
	
	int GetNumSkinOutLineColor();
	D3DCOLORVALUE GetSkinOutLineColor( int iIndex );

	int GetNumHairColor( int iHairIndex );
	ioHashString GetHairColorTex( int iHairIndex, int iIndex );
	D3DCOLORVALUE GetHairOutLineColor( int iHairIndex, int iIndex );

	int GetNumBeardParts();
	ioHashString GetBeardParts( int iIndex );

	int GetNumBeardColor( int iBeardIndex );
	ioHashString GetBeardColorTex( int iBeardIndex, int iIndex );
	D3DCOLORVALUE GetBeardOutLineColor( int iBeardIndex, int iIndex );

	int GetNumFaceParts();
	ioHashString GetFaceParts( int iIndex );
	ioHashString GetFaceEffect( int iIndex );

	int GetNumHairParts();
	ioHashString GetHairParts( int iIndex );
	ioHashString GetHairEffect( int iIndex );

	int GetNumAccessoryParts();
	ioHashString GetAccessoryParts( int iIndex );

	int GetNumUnderwearParts();
	ioHashString GetUnderwearParts( int iIndex );

	int GetSingleModeFaceType();
	int GetSingleModeHairType();
	int GetSingleModeHairColorType();
	int GetSingleModeSkinType();
	int GetSingleModeUnderwearType();
}



