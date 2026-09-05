

#include "stdafx.h"
#include "ioShaderDefine.h"

ShaderCustomParamType ConvertShaderParamType( const std::string &param )
{
	if( param == "float" )
		return CPT_FLOAT;
	else if( param == "vector3" )
		return CPT_VECTOR3;
	else if( param == "vector4" )
		return CPT_VECTOR4;
	else if( param == "colorvalue" )
		return CPT_COLORVALUE;

	LOG.PrintTimeAndLog( 0, "ConvertShaderParamType - Unknown Type(%s)", param.c_str() );

	return CPT_VECTOR4;
}

LightCombination ConvertLightCombination( const std::string &param )
{
	if( param == "nolight" )
		return LC_NOLIGHT;
	else if( param == "dir" )
		return LC_DIR;
	else if( param == "point" )
		return LC_POINT;
	else if( param == "spot" )
		return LC_SPOT;
	else if( param == "dir_point" )
		return LC_DIR_POINT;
	else if( param == "dir_spot" )
		return LC_DIR_SPOT;

	LOG.PrintTimeAndLog( 0, "ConvertLightCombination - Unknown Type(%s)", param.c_str() );

	return LC_NOLIGHT;
}

VertexShaderProfile ConvertVertexShaderProfile( const std::string &param )
{
	if( param == "fix" )
		return VS_FIXED;
	else if( param == "vs_1_1" )
		return VS_1_1;
	else if( param == "vs_2_0" )
		return VS_2_0;

	LOG.PrintTimeAndLog( 0, "ConvertVertexShaderProfile - Unknown Type(%s)", param.c_str() );

	return VS_1_1;
}

PixelShaderProfile ConvertPixelShaderProfile( const std::string &param )
{
	if( param == "fix" )
		return PS_FIXED;
	else if( param == "ps_1_1" )
		return PS_1_1;
	else if( param == "ps_1_2" )
		return PS_1_2;
	else if( param == "ps_1_3" )
		return PS_1_3;
	else if( param == "ps_1_4" )
		return PS_1_4;
	else if( param == "ps_2_0" )
		return PS_2_0;

	LOG.PrintTimeAndLog( 0, "ConvertPixelShaderProfile - Unknown Type(%s)", param.c_str() );

	return PS_FIXED;
}

ShaderGroupType ConvertShaderGroupType( const std::string &param )
{
	if( param == "default" )
		return SGT_DEFAULT;
	else if( param == "specular" )
		return SGT_SPECULAR;
	else if( param == "gen_shadow" )
		return SGT_GEN_SHADOW;
	else if( param == "shadowmap" )
		return SGT_SHADOWMAP;
	else if( param == "outline" )
		return SGT_OUTLINE;
	else if( param == "glow" )
		return SGT_GLOW;
	else if( param == "toon_shade" )
		return SGT_TOON_SHADE;

	return SGT_NONE;
}

ShaderType ConvertShaderType( const std::string &param )
{
	ShaderType eType = ST_NORMAL_SHADER;
	if( param == "normal" )
		eType = ST_NORMAL_SHADER;
	else if( param == "generate_shadow" )
		eType = ST_GEN_SHADOW_SHADER;
	else if( param == "render_shadow" )
		eType = ST_RENDER_SHADOW_SHADER;

	return eType;
}