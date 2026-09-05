

#include "stdafx.h"

#include "RaceSetting.h"

namespace RaceSetting
{
	typedef std::vector<D3DCOLORVALUE> ColorList;

	struct HairInfo
	{
		ioHashString m_HairMesh;
		ioHashStringVec m_HairColorTexList;
		ColorList		m_HairOutLineColorList;
	};
	typedef std::vector< HairInfo > HairList;

	struct BeardInfo
	{
		ioHashString m_BeardMesh;
		ioHashStringVec m_BeardColorTexList;
		ColorList m_BeardOutLineColorList;
	};
	typedef std::vector< BeardInfo > BeardList;
	
	struct RaceInfo
	{
		D3DXVECTOR3	m_AvataPos;
		D3DXVECTOR3 m_AvataCamPos;
		float m_fAvataAngleX;
		float m_fAvataAngleY;

		ColorList m_SkinColorList;
		ColorList m_SkinOutLineColorList;
		DWORDVec  m_SkinColorPeso;

		HairList m_HairList;
		ioHashStringVec m_HairEffectList;
		DWORDVec m_HairPeso;
		DWORDVec m_HairColorPeso;

		BeardList m_BeardList;
		DWORDVec  m_BeardPeos;

		ioHashStringVec m_FaceList;
		ioHashStringVec m_FaceEffectList;
		DWORDVec        m_FacePeso;

		ioHashStringVec m_AccessoryList;
		DWORDVec        m_AccessoryPeso;

		ioHashStringVec m_UnderwearList;

		int m_iSingleModeFaceType;
		int m_iSingleModeHairType;
		int m_iSingleModeHairColorType;
		int m_iSingleModeSkinType;
		int m_iSingleModeUnderwearType;
	};

	RaceInfo m_RaceTable[MAX_RACE_DETAIL];
	RaceDetailType m_RaceType = RDT_HUMAN_MAN;

	void LoadSetting()
	{
		ioINILoader kLoader;
		for( int i=0 ; i<MAX_RACE_DETAIL ; i++ )
		{
			SetCharLoader( kLoader, (RaceDetailType)i );

			LoadAvata( m_RaceTable[i], kLoader );
			LoadSkinColor( m_RaceTable[i], kLoader );
			LoadBeard( m_RaceTable[i], kLoader );
			LoadHair( m_RaceTable[i], kLoader );
			LoadFace( m_RaceTable[i], kLoader );
			LoadAccesory( m_RaceTable[i], kLoader );
			LoadUnderwear( m_RaceTable[i], kLoader );
		}
	}

	void LoadAvata( RaceInfo &rkInfo, ioINILoader &rkLoader )
	{
		rkLoader.SetTitle_e( "avata_pos" );
		rkInfo.m_AvataPos.x = rkLoader.LoadFloat_e( "char_pos_x", -175.0f );
		rkInfo.m_AvataPos.y = rkLoader.LoadFloat_e( "char_pos_y", -200.0f );
		rkInfo.m_AvataPos.z = rkLoader.LoadFloat_e( "char_pos_z", 530.0f );

		rkInfo.m_fAvataAngleX = rkLoader.LoadFloat_e( "char_angle_x", 0.0f );
		rkInfo.m_fAvataAngleY = rkLoader.LoadFloat_e( "char_angle_y", 0.0f );

		rkInfo.m_AvataCamPos.x = rkLoader.LoadFloat_e( "cam_pos_x", -38.0f );
		rkInfo.m_AvataCamPos.y = rkLoader.LoadFloat_e( "cam_pos_y", -46.0f );
		rkInfo.m_AvataCamPos.z = rkLoader.LoadFloat_e( "cam_pos_z", 0.0f );
	}
	
	void LoadSkinColor( RaceInfo &rkInfo, ioINILoader &rkLoader )
	{
		rkLoader.SetTitle_e( "skin_color" );
		rkInfo.m_iSingleModeSkinType = rkLoader.LoadInt_e( "singlemode_skin_parts", 0 );

		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, FLOAT1, FLOAT1, FLOAT1, FLOAT1 );
		rkInfo.m_SkinColorList.push_back( kColor );

		Help::InitColorValue( kColor, 0.0f, 0.0f, 0.0f, FLOAT1 );
		rkInfo.m_SkinOutLineColorList.push_back( kColor );

		char szKey[MAX_PATH];
		int iMaxSkinColor = rkLoader.LoadInt_e( "max_color", 0 );
		for( int iSkin=0 ; iSkin<iMaxSkinColor ; iSkin++ )
		{
			wsprintf_e( szKey, "color%d_r", iSkin+1 );
			kColor.r = rkLoader.LoadFloat( szKey, 0.0f ) / 255.0f;

			wsprintf_e( szKey, "color%d_g", iSkin+1 );
			kColor.g = rkLoader.LoadFloat( szKey, 0.0f ) / 255.0f;

			wsprintf_e( szKey, "color%d_b", iSkin+1 );
			kColor.b = rkLoader.LoadFloat( szKey, 0.0f ) / 255.0f;

			rkInfo.m_SkinColorList.push_back( kColor );

			wsprintf_e( szKey, "color%d_outline_r", iSkin+1 );
			kColor.r = rkLoader.LoadFloat( szKey, 0.0f ) / 255.0f;

			wsprintf_e( szKey, "color%d_outline_g", iSkin+1 );
			kColor.g = rkLoader.LoadFloat( szKey, 0.0f ) / 255.0f;

			wsprintf_e( szKey, "color%d_outline_b", iSkin+1 );
			kColor.b = rkLoader.LoadFloat( szKey, 0.0f ) / 255.0f;
			rkInfo.m_SkinOutLineColorList.push_back( kColor );
		}
	}

	void LoadBeard( RaceInfo &rkInfo, ioINILoader &rkLoader )
	{
		rkLoader.SetTitle_e( "beard_parts" );
		
		BeardInfo kDummyBeard;
		kDummyBeard.m_BeardColorTexList.push_back( ioHashString() );
		kDummyBeard.m_BeardOutLineColorList.push_back( D3DCOLORVALUE() );
		rkInfo.m_BeardList.push_back( kDummyBeard );

		char szKey[MAX_PATH], szBuf[MAX_PATH];
		int iMaxBeard = rkLoader.LoadInt_e( "max_beard", 0 );
		for( int iBeard=0 ; iBeard<iMaxBeard ; iBeard++ )
		{
			wsprintf_e( szKey, "parts%d", iBeard+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			BeardInfo kBeard;
			kBeard.m_BeardMesh = szBuf;
			kBeard.m_BeardColorTexList.push_back( ioHashString() );

			D3DCOLORVALUE kOutLine;
			Help::InitColorValue( kOutLine, 0.0f, 0.0f, 0.0f, FLOAT1 );
			kBeard.m_BeardOutLineColorList.push_back( kOutLine );

			wsprintf_e( szKey, "parts%d_max_color", iBeard+1 );
			int iMaxBeardColor = rkLoader.LoadInt( szKey, 0 );
			for( int iBeardColor=0 ; iBeardColor<iMaxBeardColor ; iBeardColor++ )
			{
				wsprintf_e( szKey, "parts%d_color%d", iBeard+1, iBeardColor+1 );
				rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kBeard.m_BeardColorTexList.push_back( ioHashString(szBuf) );

				wsprintf_e( szKey, "parts%d_outline%d", iBeard+1, iBeardColor+1 );
				rkLoader.LoadString( szKey, "0 0 0", szBuf, MAX_PATH );

				StringVector vColorList = ioStringConverter::Split( std::string(szBuf), " " );
				while( vColorList.size() < 3 )
				{
					vColorList.push_back( std::string("0") );
				}

				kOutLine.r = atof( vColorList[0].c_str() ) / 255.0f;
				kOutLine.g = atof( vColorList[1].c_str() ) / 255.0f;
				kOutLine.b = atof( vColorList[2].c_str() ) / 255.0f;
				kBeard.m_BeardOutLineColorList.push_back( kOutLine );
			}

			rkInfo.m_BeardList.push_back( kBeard );
		}
	}

	void LoadHair( RaceInfo &rkInfo, ioINILoader &rkLoader )
	{
		rkLoader.SetTitle_e( "hair_parts" );
		rkInfo.m_iSingleModeHairType = rkLoader.LoadInt_e( "singlemode_hair_parts", 0 );
		rkInfo.m_iSingleModeHairColorType = rkLoader.LoadInt_e( "singlemode_haircolor_parts", 0 );

		HairInfo kDummyHair;
		kDummyHair.m_HairColorTexList.push_back( ioHashString() );
		kDummyHair.m_HairOutLineColorList.push_back( D3DCOLORVALUE() );
		rkInfo.m_HairList.push_back( kDummyHair );
		rkInfo.m_HairEffectList.push_back( ioHashString() );

		char szKey[MAX_PATH], szBuf[MAX_PATH];
		int iMaxHair = rkLoader.LoadInt_e( "max_hair", 0 );
		for( int iHair=0 ; iHair<iMaxHair ; iHair++ )
		{
			wsprintf_e( szKey, "parts%d", iHair+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			HairInfo kHair;
			kHair.m_HairMesh = szBuf;
			kHair.m_HairColorTexList.push_back( ioHashString() );

			D3DCOLORVALUE kOutLine;
			Help::InitColorValue( kOutLine, 0.0f, 0.0f, 0.0f, FLOAT1 );
			kHair.m_HairOutLineColorList.push_back( kOutLine );

			wsprintf_e( szKey, "parts%d_max_color", iHair+1 );
			int iMaxHairColor = rkLoader.LoadInt( szKey, 0 );
			for( int iHairColor=0 ; iHairColor<iMaxHairColor ; iHairColor++ )
			{
				wsprintf_e( szKey, "parts%d_color%d", iHair+1, iHairColor+1 );
				rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kHair.m_HairColorTexList.push_back( ioHashString(szBuf) );

				wsprintf_e( szKey, "parts%d_outline%d", iHair+1, iHairColor+1 );
				rkLoader.LoadString( szKey, "0 0 0", szBuf, MAX_PATH );

				StringVector vColorList = ioStringConverter::Split( std::string(szBuf), " " );
				while( vColorList.size() < 3 )
				{
					vColorList.push_back( std::string("0") );
				}

				kOutLine.r = atof( vColorList[0].c_str() ) / 255.0f;
				kOutLine.g = atof( vColorList[1].c_str() ) / 255.0f;
				kOutLine.b = atof( vColorList[2].c_str() ) / 255.0f;
				kHair.m_HairOutLineColorList.push_back( kOutLine );
			}

			rkInfo.m_HairList.push_back( kHair );

			wsprintf_e( szKey, "parts%d_effect", iHair+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			rkInfo.m_HairEffectList.push_back( ioHashString(szBuf) );
		}
	}

	void LoadFace( RaceInfo &rkInfo, ioINILoader &rkLoader )
	{
		rkLoader.SetTitle_e( "face_parts" );
		rkInfo.m_iSingleModeFaceType = rkLoader.LoadInt_e( "singlemode_face_parts", 0 );
		rkInfo.m_FaceList.push_back( ioHashString() );
		rkInfo.m_FaceEffectList.push_back( ioHashString() );

		char szKey[MAX_PATH], szBuf[MAX_PATH];
		int iMaxFace = rkLoader.LoadInt_e( "max_face", 0 );
		for( int iFace=0 ; iFace<iMaxFace ; iFace++ )
		{
			wsprintf_e( szKey, "parts%d", iFace+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			rkInfo.m_FaceList.push_back( ioHashString(szBuf) );

			wsprintf_e( szKey, "parts%d_effect", iFace+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			rkInfo.m_FaceEffectList.push_back( ioHashString(szBuf) );
		}
	}
	
	void LoadAccesory( RaceInfo &rkInfo, ioINILoader &rkLoader )
	{
		rkLoader.SetTitle_e( "accessory_parts" );
		rkInfo.m_AccessoryList.push_back( ioHashString() );
	
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		int iMaxAcc = rkLoader.LoadInt_e( "max_accessory", 0 );
		for( int iAcc=0 ; iAcc<iMaxAcc ; iAcc++ )
		{
			wsprintf_e( szKey, "parts%d", iAcc+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			rkInfo.m_AccessoryList.push_back( ioHashString(szBuf) );
		}
	}

	void LoadUnderwear( RaceInfo &rkInfo, ioINILoader &rkLoader )
	{
		rkLoader.SetTitle_e( "underwear_parts" );
		rkInfo.m_iSingleModeUnderwearType = rkLoader.LoadInt_e( "singlemode_underwear_parts", 0 );

		rkInfo.m_UnderwearList.push_back( ioHashString() );

		char szKey[MAX_PATH], szBuf[MAX_PATH];
		int iMax = rkLoader.LoadInt_e( "max_underwear", 0 );
		for( int i=0 ; i<iMax ; i++ )
		{
			wsprintf_e( szKey, "parts%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			rkInfo.m_UnderwearList.push_back( ioHashString(szBuf) );
		}
	}

	void SetCurRaceDetail( RaceType eRace, bool bMale )
	{
		CHARACTER kCharInfo;
		kCharInfo.m_kindred = (int)eRace + 1;

		if( bMale )
			kCharInfo.m_sex = 1;
		else
			kCharInfo.m_sex = 2;

		SetCurRaceDetail( kCharInfo );
	}

	void SetCurRaceDetail( const CHARACTER &rkInfo )
	{
		SetCurRaceDetail( GetRaceDetailType( rkInfo ) );
	}

	void SetCurRaceDetail( RaceDetailType eType )
	{
		m_RaceType = eType;
	}

	const D3DXVECTOR3& AvataPosition()
	{
		return m_RaceTable[m_RaceType].m_AvataPos;
	}

	const D3DXVECTOR3& AvataCamPosition()
	{
		return m_RaceTable[m_RaceType].m_AvataCamPos;
	}

	float AvataAngleX()
	{
		return m_RaceTable[m_RaceType].m_fAvataAngleX;
	}

	float AvataAngleY()
	{
		return m_RaceTable[m_RaceType].m_fAvataAngleY;
	}

	int GetNumSkinColor()
	{
		return m_RaceTable[m_RaceType].m_SkinColorList.size();
	}

	D3DCOLORVALUE GetSkinColor( int iIndex )
	{
		if( COMPARE( iIndex, 0, GetNumSkinColor() ) )
			return m_RaceTable[m_RaceType].m_SkinColorList[iIndex];

		LOG.PrintTimeAndLog( 0, "RaceSetting::GetSkinColor - %d Race Overflow(%d/%d)",
								m_RaceType, iIndex, GetNumSkinColor() );

		if( GetNumSkinColor() > 0 )
			return m_RaceTable[m_RaceType].m_SkinColorList[1];

		return m_RaceTable[m_RaceType].m_SkinColorList[0];
	}

	int GetNumSkinOutLineColor()
	{
		return m_RaceTable[m_RaceType].m_SkinOutLineColorList.size();
	}

	D3DCOLORVALUE GetSkinOutLineColor( int iIndex )
	{
		if( COMPARE( iIndex, 0, GetNumSkinOutLineColor() ) )
			return m_RaceTable[m_RaceType].m_SkinOutLineColorList[iIndex];

		LOG.PrintTimeAndLog( 0, "RaceSetting::GetSkinColor - %d Race Overflow(%d/%d)",
								m_RaceType, iIndex, GetNumSkinOutLineColor() );

		if( GetNumSkinOutLineColor() > 0 )
			return m_RaceTable[m_RaceType].m_SkinOutLineColorList[1];

		return m_RaceTable[m_RaceType].m_SkinOutLineColorList[0];
	}

	int GetNumHairColor( int iHairIndex )
	{
		if( COMPARE( iHairIndex, 0, GetNumHairParts() ) )
		{
			return m_RaceTable[m_RaceType].m_HairList[iHairIndex].m_HairColorTexList.size();
		}

		return 0;
	}

	ioHashString GetHairColorTex( int iHairIndex, int iIndex )
	{
		if( !COMPARE( iHairIndex, 0, GetNumHairParts() ) )
		{
			LOG.PrintTimeAndLog( 0, "RaceSetting::GetHairColorTex - %d Race %d Hair Not Exist",
									m_RaceType, iHairIndex );
			return ioHashString();
		}

		if( !COMPARE( iIndex, 0, GetNumHairColor( iHairIndex ) ) )
		{
			LOG.PrintTimeAndLog( 0, "RaceSetting::GetHairColorTex - %d Race %d-%d Hair Color Not Exist",
									m_RaceType, iHairIndex, iIndex );
			return ioHashString();
		}

		return m_RaceTable[m_RaceType].m_HairList[iHairIndex].m_HairColorTexList[iIndex];
	}

	D3DCOLORVALUE GetHairOutLineColor( int iHairIndex, int iIndex )
	{
		if( !COMPARE( iHairIndex, 0, GetNumHairParts() ) )
		{
			LOG.PrintTimeAndLog( 0, "RaceSetting::GetHairOutLineColor - %d Race %d Hair Not Exist",
									m_RaceType, iHairIndex );

			D3DCOLORVALUE kBlackColor;
			Help::InitColorValue( kBlackColor, 0.0f, 0.0f, 0.0f, FLOAT1 );
			return kBlackColor;
		}

		if( !COMPARE( iIndex, 0, GetNumHairColor( iHairIndex ) ) )
		{
			LOG.PrintTimeAndLog( 0, "RaceSetting::GetHairOutLineColor - %d Race %d-%d Hair Color Not Exist",
									m_RaceType, iHairIndex, iIndex );

			D3DCOLORVALUE kBlackColor;
			Help::InitColorValue( kBlackColor, 0.0f, 0.0f, 0.0f, FLOAT1 );
			return kBlackColor;
		}

		return m_RaceTable[m_RaceType].m_HairList[iHairIndex].m_HairOutLineColorList[iIndex];
	}

	int GetNumBeardParts()
	{
		return m_RaceTable[m_RaceType].m_BeardList.size();
	}

	ioHashString GetBeardParts( int iIndex )
	{
		if( COMPARE( iIndex, 0, GetNumBeardParts() ) )
			return m_RaceTable[m_RaceType].m_BeardList[iIndex].m_BeardMesh;

		LOG.PrintTimeAndLog( 0, "RaceSetting::GetBeardParts - %d Race Overflow(%d/%d)",
								m_RaceType, iIndex, GetNumBeardParts() );

		return ioHashString();
	}

	int GetNumBeardColor( int iBeardIndex )
	{
		if( COMPARE( iBeardIndex, 0, GetNumBeardParts() ) )
		{
			return m_RaceTable[m_RaceType].m_BeardList[iBeardIndex].m_BeardColorTexList.size();
		}

		return 0;
	}

	ioHashString GetBeardColorTex( int iBeardIndex, int iIndex )
	{
		if( !COMPARE( iBeardIndex, 0, GetNumBeardParts() ) )
		{
			LOG.PrintTimeAndLog( 0, "RaceSetting::GetBeardColorTex - %d Race overflow(%d/%d)",
									m_RaceType, iBeardIndex, GetNumBeardParts() );
			return ioHashString();
		}

		if( !COMPARE( iIndex, 0, GetNumBeardColor( iBeardIndex ) ) )
		{
			LOG.PrintTimeAndLog( 0, "RaceSetting::GetBeardColorTex - %d Race %d-%d Not Exist",
									m_RaceType, iBeardIndex, iIndex );
			return ioHashString();
		}

		return m_RaceTable[m_RaceType].m_BeardList[iBeardIndex].m_BeardColorTexList[iIndex];
	}

	D3DCOLORVALUE GetBeardOutLineColor( int iBeardIndex, int iIndex )
	{
		if( !COMPARE( iBeardIndex, 0, GetNumBeardParts() ) )
		{
			LOG.PrintTimeAndLog( 0, "RaceSetting::GetBeardOutLineColor - %d Race %d Beard Not Exist",
									m_RaceType, iBeardIndex );

			D3DCOLORVALUE kBlackColor;
			Help::InitColorValue( kBlackColor, 0.0f, 0.0f, 0.0f, FLOAT1 );
			return kBlackColor;
		}

		if( !COMPARE( iIndex, 0, GetNumBeardColor( iBeardIndex ) ) )
		{
			LOG.PrintTimeAndLog( 0, "RaceSetting::GetBeardOutLineColor - %d Race %d-%d Beard Color Not Exist",
									m_RaceType, iBeardIndex, iIndex );

			D3DCOLORVALUE kBlackColor;
			Help::InitColorValue( kBlackColor, 0.0f, 0.0f, 0.0f, FLOAT1 );
			return kBlackColor;
		}

		return m_RaceTable[m_RaceType].m_BeardList[iBeardIndex].m_BeardOutLineColorList[iIndex];
	}

	int GetNumFaceParts()
	{
		return m_RaceTable[m_RaceType].m_FaceList.size();
	}

	ioHashString GetFaceParts( int iIndex )
	{
		if( COMPARE( iIndex, 0, GetNumFaceParts() ) )
			return m_RaceTable[m_RaceType].m_FaceList[iIndex];

		LOG.PrintTimeAndLog( 0, "RaceSetting::GetFaceParts - %d Race Overflow(%d/%d)",
								m_RaceType, iIndex, GetNumFaceParts() );

		return ioHashString();
	}

	ioHashString GetFaceEffect( int iIndex )
	{
		if( COMPARE( iIndex, 0, GetNumFaceParts() ) )
			return m_RaceTable[m_RaceType].m_FaceEffectList[iIndex];

		LOG.PrintTimeAndLog( 0, "RaceSetting::GetFaceEffect - %d Race Overflow(%d/%d)",
			m_RaceType, iIndex, GetNumFaceParts() );

		return ioHashString();
	}

	int GetNumHairParts()
	{
		return m_RaceTable[m_RaceType].m_HairList.size();
	}

	ioHashString GetHairParts( int iIndex )
	{
		if( COMPARE( iIndex, 0, GetNumHairParts() ) )
			return m_RaceTable[m_RaceType].m_HairList[iIndex].m_HairMesh;

		LOG.PrintTimeAndLog( 0, "RaceSetting::GetHairParts - %d Race Overflow(%d/%d)",
								m_RaceType, iIndex, GetNumHairParts() );

		return ioHashString();
	}

	ioHashString GetHairEffect( int iIndex )
	{
		if( COMPARE( iIndex, 0, GetNumHairParts() ) )
			return m_RaceTable[m_RaceType].m_HairEffectList[iIndex];

		LOG.PrintTimeAndLog( 0, "RaceSetting::GetHairEffect - %d Race Overflow(%d/%d)",
			m_RaceType, iIndex, GetNumHairParts() );

		return ioHashString();
	}

	int GetNumAccessoryParts()
	{
		return m_RaceTable[m_RaceType].m_AccessoryList.size();
	}

	ioHashString GetAccessoryParts( int iIndex )
	{
		if( COMPARE( iIndex, 0, GetNumAccessoryParts() ) )
			return m_RaceTable[m_RaceType].m_AccessoryList[iIndex];

		LOG.PrintTimeAndLog( 0, "RaceSetting::GetAccessoryParts - %d Race Overflow(%d/%d)",
								m_RaceType, iIndex, GetNumAccessoryParts() );

		return ioHashString();
	}

	int GetNumUnderwearParts()
	{
		return m_RaceTable[m_RaceType].m_UnderwearList.size();
	}

	ioHashString GetUnderwearParts( int iIndex )
	{
		if( COMPARE( iIndex, 0, GetNumUnderwearParts() ) )
			return m_RaceTable[m_RaceType].m_UnderwearList[iIndex];

		LOG.PrintTimeAndLog( 0, "RaceSetting::GetUnderwearParts - %d Race Overflow(%d/%d)",
			m_RaceType, iIndex, GetNumUnderwearParts() );

		return ioHashString();
	}

	int GetSingleModeFaceType()
	{
		return m_RaceTable[m_RaceType].m_iSingleModeFaceType;
	}

	int GetSingleModeHairType()
	{
		return m_RaceTable[m_RaceType].m_iSingleModeHairType;
	}

	int GetSingleModeHairColorType()
	{
		return m_RaceTable[m_RaceType].m_iSingleModeHairColorType;
	}

	int GetSingleModeSkinType()
	{
		return m_RaceTable[m_RaceType].m_iSingleModeSkinType;
	}

	int GetSingleModeUnderwearType()
	{
		return m_RaceTable[m_RaceType].m_iSingleModeUnderwearType;
	}
}





