

#ifndef _ioShaderParameter_h_
#define _ioShaderParameter_h_

#include "io3DCommon.h"
#include "ioShaderDefine.h"

enum AutoConstantType
{
	/// The current world matrix
	ACT_WORLD_MATRIX,
    /// The current view matrix
    ACT_VIEW_MATRIX,
    /// The current projection matrix
    ACT_PROJECTION_MATRIX,
	/// The current Biped List
	ACT_BIPED_LIST,
	/// The current Mesh weighted biped count
	ACT_WEIGHTED_BIPED_COUNT,
    /// The current view & projection matrices concatenated
    ACT_VIEWPROJ_MATRIX,
    /// The current world & view matrices concatenated
    ACT_WORLDVIEW_MATRIX,
    /// The current world, view & projection matrices concatenated
    ACT_WORLDVIEWPROJ_MATRIX,
    /// The current world matrix, inverted
    ACT_INVERSE_WORLD_MATRIX,
	/// The current view matrix, inverted
	ACT_INVERSE_VIEW_MATRIX,
    /// The current world & view matrices concatenated, then inverted
	ACT_INVERSE_WORLDVIEW_MATRIX,
    /// The current world matrix, inverted & transposed
    ACT_INVERSE_TRANSPOSE_WORLD_MATRIX,
    /// The current world & view matrices concatenated, then inverted & tranposed
    ACT_INVERSE_TRANSPOSE_WORLDVIEW_MATRIX,

	/// The current stage Texture matrix
	ACT_TEXTURE_MATRIX,

	/// The Main Light's Light World * View * Proj matrix ( for shadowmap )
	ACT_LIGHT_WORLDVIEWPROJ_MATRIX,
	/// The Shadowmap matrix
	ACT_SHADOWMAP_MATRIX,
	/// Shadow Color
	ACT_SHADOW_COLOR,

	/// No Light
	ACT_MATERIAL_DIFFUSE_COLOR,

	/// Directional Light
	ACT_DIRECTIONAL_LIGHT_DIRECTION_OBJECT_SPACE,
    ACT_DIRECTIONAL_LIGHT_DIFFUSE_COLOR,
    ACT_DIRECTIONAL_LIGHT_SPECULAR_COLOR,
	ACT_SPECULAR_PARAM,	//( specular power, specular lv, 0.0f, 0.0f )

	/// Point Light
	ACT_POINT_LIGHT_POSITION_OBJECT_SPACE,
	ACT_POINT_LIGHT_DIFFUSE_COLOR,
	ACT_POINT_LIGHT_ATTENUATION,

	// Spot Light
	ACT_SPOT_LIGHT_DIFFUSE_COLOR,
	ACT_SPOT_LIGHT_SPECULAR_COLOR,
	ACT_SPOT_LIGHT_ATTENUATION,
	ACT_SPOT_LIGHT_DIRECTION_OBJECT_SPACE,
	ACT_SPOT_LIGHT_POSITION_OBJECT_SPACE,
	ACT_SPOT_LIGHT_FACTOR,
	/// vSpotFactor.x = cosf( DEGtoRAD( m_fSpotOuter * 0.5f ) );
	/// vSpotFactor.y = cosf( DEGtoRAD( m_fSpotInner * 0.5f ) ) - vSpotFactor.x;
	/// vSpotFactor.y = 1.0f / vSpotFactor.y;
	/// vSpotFactor.z = m_fSpotFallOff;

	/// The ambient + material's emissive
	ACT_AMBIENT_EMISSIVE_COLOR,

	/// The current camera's position in object space 
    ACT_CAMERA_POSITION_OBJECT_SPACE,
	/// The current camera's direction in object space
	ACT_CAMERA_DIRECTION_OBJECT_SPACE,

	/// The current viewport ( width, height, 1.0f/width, 1.0f/height )
	ACT_VIEWPORT,
	/// The current camera's ( FovX, FovY, Near, Far )
	ACT_CAMERA_FOV_NEAR_FAR,
	/// The current Fog Factor ( End, 1.0f/( End - Start ), 0.0f, 0.0f )
	ACT_FOG_FACTOR,
	/// The curent Alpha Factor ( End, 1.0f/( End - Start ), AlphaRate, 0.0f )
	ACT_ALPHA_FACTOR,
	/// OutLine Constant ( distance obj<->camera, FovX, 1.0f/width, distant * FovX / width )
	ACT_OUTLINE_CONSTANT,
	/// Max Constant Type Count, Always last...	
	ACT_MAX_CONSTANT_TYPE,
	/// Custom
	ACT_CUSTOM,
};

struct AutoConstantEntry
{
	AutoConstantType m_Type;
	int m_iUnitIndex;

	AutoConstantEntry() : m_Type(ACT_CUSTOM), m_iUnitIndex(0){}
	AutoConstantEntry( AutoConstantType eType, int iIndex )
		: m_Type(eType), m_iUnitIndex(iIndex){}

	bool operator<( const AutoConstantEntry &rhs ) const
	{
		if( m_Type < rhs.m_Type )
			return true;
		else if( m_Type == rhs.m_Type )
		{
			if( m_iUnitIndex < rhs.m_iUnitIndex )
				return true;
		}
		
		return false;
	}
};

AutoConstantEntry ParsingConstantEntry( const D3DXPARAMETER_DESC &rkDesc );

__EX bool IsAutoConstant( const ioHashString &szName );

class __EX ioShaderCustomArg
{
private:
	ioHashString m_Name;
	ShaderCustomParamType m_ValueType;
	D3DXVECTOR4 m_vData;

public:
	void SetName( const char *szName );
	void SetValue( float fVal );
	void SetValue( const D3DXVECTOR3 &kVal );
	void SetValue( const D3DXVECTOR4 &kVal );
	void SetValue( const D3DCOLORVALUE &kVal );
	void SetValue( const ioShaderCustomArg &kVal );

	inline float GetFloat() const { return m_vData.x; }
	inline const D3DXVECTOR3& GetVector3() const { return ( const D3DXVECTOR3& )m_vData; }
	inline const D3DXVECTOR4& GetVector4() const { return m_vData; }
	inline const D3DCOLORVALUE& GetColor() const { return ( const D3DCOLORVALUE& )m_vData; }

public:
	inline ShaderCustomParamType GetType() const { return m_ValueType; }
	inline const ioHashString& GetName() const { return m_Name; }

public:
	bool operator==( const ioShaderCustomArg &rhs ) const;
	bool operator!=( const ioShaderCustomArg &rhs ) const;

public:
	ioShaderCustomArg( const char *szName );
	ioShaderCustomArg( const ioShaderCustomArg &rhs );
	~ioShaderCustomArg();
};

class __EX ioShaderCustomParameters
{
private:
	typedef std::vector< ioShaderCustomArg > CustomArgList;
	CustomArgList m_CustomArgList;

public:
	void SetCustomArg( const ioShaderCustomArg &kVal );
	void SetCustomArg( const char *szName, float fVal );
	void SetCustomArg( const char *szName, const D3DXVECTOR3 &kVal );
	void SetCustomArg( const char *szName, const D3DXVECTOR4 &kVal );
	void SetCustomArg( const char *szName, const D3DCOLORVALUE &kVal );

public:
	const ioShaderCustomArg* GetValue( const ioHashString &name ) const;
	const ioShaderCustomArg* GetValue( int iIndex ) const;

	ioShaderCustomArg* GetArg( const ioHashString &name );
	ioShaderCustomArg* GetArg( int iIndex );

	const ioShaderCustomArg* GetArg( const ioHashString &name ) const;
	const ioShaderCustomArg* GetArg( int iIndex ) const;
	int GetCustomArgCount() const { return m_CustomArgList.size(); }

public:
	ioShaderCustomParameters& operator=( const ioShaderCustomParameters &rhs );
	bool operator==( const ioShaderCustomParameters &rhs ) const;
	bool operator!=( const ioShaderCustomParameters &rhs ) const;

public:
	ioShaderCustomParameters();
	~ioShaderCustomParameters();
};

#endif
