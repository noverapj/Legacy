

#ifndef _ioShaderDefine_h_
#define _ioShaderDefine_h_

enum LightCombination
{
	LC_NOLIGHT,
	LC_DIR,
	LC_POINT,
	LC_SPOT,
	LC_DIR_POINT,
	LC_DIR_SPOT,
	LC_MAX_COMBINATION
};

enum VertexShaderProfile
{
	VS_FIXED,
	VS_1_1,
	VS_2_0,
};

enum PixelShaderProfile
{
	PS_FIXED,
	PS_1_1,
	PS_1_2,
	PS_1_3,
	PS_1_4,
	PS_2_0
};

enum ShaderCustomParamType
{
	CPT_FLOAT,
	CPT_VECTOR3,
	CPT_VECTOR4,
	CPT_COLORVALUE
};

enum ShaderGroupType
{
	SGT_NONE,
	SGT_DEFAULT,
	SGT_SPECULAR,
	SGT_GEN_SHADOW,
	SGT_SHADOWMAP,
	SGT_OUTLINE,
	SGT_GLOW,
	SGT_TOON_SHADE,
};

enum ShaderType
{
	ST_NORMAL_SHADER,
	ST_GEN_SHADOW_SHADER,
	ST_RENDER_SHADOW_SHADER,
};

ShaderCustomParamType ConvertShaderParamType( const std::string &param );
LightCombination ConvertLightCombination( const std::string &param );
VertexShaderProfile ConvertVertexShaderProfile( const std::string &param );
PixelShaderProfile ConvertPixelShaderProfile( const std::string &param );
ShaderGroupType ConvertShaderGroupType( const std::string &param );
ShaderType ConvertShaderType( const std::string &param );

#endif