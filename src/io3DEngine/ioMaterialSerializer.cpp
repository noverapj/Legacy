

#include "stdafx.h"

#include "ioMaterialSerializer.h"
#include "ioMaterialManager.h"
#include "ioMaterial.h"
#include "ioTechnique.h"
#include "ioPass.h"
#include "ioTextureUnitState.h"
#include "HelpFunc.h"

#include "ioStream.h"

using namespace std;

void LogParseError( const char *szMsg, const MaterialScriptContext &context )
{
	LOG.PrintLog( 0, "%s line : %d - %s", context.szScriptName, context.lineNo, szMsg );
}

D3DCOLORVALUE ParseColorValue( const StringVector &vecparams )
{
	D3DCOLORVALUE kValue;
	kValue.r = ioStringConverter::ParseFloat( vecparams[0].c_str() );
	kValue.g = ioStringConverter::ParseFloat( vecparams[1].c_str() );
	kValue.b = ioStringConverter::ParseFloat( vecparams[2].c_str() );
	if( vecparams.size() == 4 )
	{
		kValue.a = ioStringConverter::ParseFloat( vecparams[3].c_str() );
	}
	else
	{
		kValue.a = 1.0f;
	}

	return kValue;
}

bool ParseMaterial( string &params, MaterialScriptContext &context )
{
	char szMaterialName[MAX_PATH];
	wsprintf( szMaterialName, "%s/Sub%d", context.szScriptName, context.iMaterialCnt + 1 );

    context.material = g_MaterialMgr.CreateMaterial( ioHashString( szMaterialName ) );
	context.section = MSS_MATERIAL;
	context.iMaterialCnt++;

	// Return TRUE because this must be followed by a {
    return true;
}

bool ParseTechnique( string &params, MaterialScriptContext &context )
{
    context.technique = context.material->CreateTechnique();
    context.section = MSS_TECHNIQUE;

    // Return TRUE because this must be followed by a {
    return true;
}

bool ParsePass( string &params, MaterialScriptContext &context )
{
	context.pass = context.technique->CreatePass();
	context.section = MSS_PASS;

    // Return TRUE because this must be followed by a {
	return true;
}

bool ParseAmbient( string &params, MaterialScriptContext &context )
{
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );

	if( vecparams.size() == 3 || vecparams.size() == 4 )
	{
		D3DCOLORVALUE kValue = ParseColorValue( vecparams );
		context.material->SetAmbient( kValue.r, kValue.g, kValue.b, kValue.a );
	}
	else
	{
		LogParseError( "Bad ambient attribute, wrong number of parameters (expected 3 or 4)",
					   context );
	}

	return false;
}

bool ParseLocalGlow( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "true" )
		context.material->SetLocalGlow( true );
	else if( params == "false" )
		context.material->SetLocalGlow( false );
	else
	{
		LogParseError( "Bad LocalGlow attribute, valid parameters are \'true\' or \'false\'.",
			context );
	}

	return false;	
}

bool ParseDiffuse( string &params, MaterialScriptContext &context )
{
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );

	if( vecparams.size() == 3 || vecparams.size() == 4 )
	{
		D3DCOLORVALUE kValue = ParseColorValue( vecparams );
		context.material->SetDiffuse( kValue.r, kValue.g, kValue.b, kValue.a );
	}
	else
	{
		LogParseError( "Bad diffuse attribute, wrong number of parameters (expected 3 or 4)",
					   context );
	}

	return false;
}

bool ParseSpecular( string &params, MaterialScriptContext &context )
{
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );

	if( vecparams.size() == 3 || vecparams.size() == 4 )
	{
		D3DCOLORVALUE kValue = ParseColorValue( vecparams );
		context.material->SetSpecular( kValue.r, kValue.g, kValue.b, kValue.a );
	}
	else
	{
		LogParseError( "Bad specular attribute, wrong number of parameters (expected 3 or 4)",
					   context );
	}

	return false;
}

bool ParseSpecularLv( string &params, MaterialScriptContext &context )
{
	context.material->SetSpecularLv( ioStringConverter::ParseFloat( params.c_str() ) );
	return false;
}

bool ParseSpecularPower( string &params, MaterialScriptContext &context )
{
	context.material->SetSpecularPower( ioStringConverter::ParseFloat( params.c_str() ) );
	return false;
}

bool ParseEmissive( string &params, MaterialScriptContext &context )
{
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );

	if( vecparams.size() == 3 || vecparams.size() == 4 )
	{
		D3DCOLORVALUE kValue = ParseColorValue( vecparams );
		context.material->SetEmissive( kValue.r, kValue.g, kValue.b, kValue.a );
	}
	else
	{
		LogParseError( "Bad emissive attribute, wrong number of parameters (expected 3 or 4)",
					   context );
	}

	return false;
}

D3DBLEND ConvertBlendFactor( const string &token )
{
	if( token == "zero" )
		return D3DBLEND_ZERO;
	if( token == "one" )
		return D3DBLEND_ONE;
	if( token == "src_color" )
		return D3DBLEND_SRCCOLOR;
    if( token == "inv_src_color" )
		return D3DBLEND_INVSRCCOLOR;
	if( token == "src_alpha" )
		return D3DBLEND_SRCALPHA;
	if( token == "inv_src_alpha" )
		return D3DBLEND_INVSRCALPHA;
	if( token == "dest_alpha" )
		return D3DBLEND_DESTALPHA;
	if( token == "inv_dest_alpha" )
		return D3DBLEND_INVDESTALPHA;
	if( token == "dest_color" )
		return D3DBLEND_DESTCOLOR;
	if( token == "inv_dest_color" )
		return D3DBLEND_INVDESTCOLOR;

	LOG.PrintTimeAndLog( 0, "ConvertBlendFactor - Unknown Factor - %s", token.c_str() );

	return D3DBLEND_ONE;
}

const char* ConvertBlendFactorText( D3DBLEND eBlend )
{
	switch( eBlend )
	{
	case D3DBLEND_ZERO:
		return "zero";
	case D3DBLEND_ONE:
		return "one";
	case D3DBLEND_SRCCOLOR:
		return "src_color";
	case D3DBLEND_INVSRCCOLOR:
		return "inv_src_color";
	case D3DBLEND_SRCALPHA:
		return "src_alpha";
	case D3DBLEND_INVSRCALPHA:
		return "inv_src_alpha";
	case D3DBLEND_DESTALPHA:
		return "dest_alpha";
	case D3DBLEND_INVDESTALPHA:
		return "inv_dest_alpha";
	case D3DBLEND_DESTCOLOR:
		return "dest_color";
	case D3DBLEND_INVDESTCOLOR:
		return "inv_dest_color";
	}

	LOG.PrintTimeAndLog( 0, "ConvertBlendFactorText - Unknown Factor(%d)", eBlend );

	return "one";
}

D3DBLENDOP ConvertBlendOp( const string &token )
{
	if( token == "add" )
		return D3DBLENDOP_ADD;
	if( token == "subtract" )
		return D3DBLENDOP_SUBTRACT;
	if( token == "rev_subtract" )
		return D3DBLENDOP_REVSUBTRACT;
	if( token == "min" )
		return D3DBLENDOP_MIN;
	if( token == "max" )
		return D3DBLENDOP_MAX;

	LOG.PrintTimeAndLog( 0, "ConvertBlendOp - Unknown Op - %s", token.c_str() );

	return D3DBLENDOP_ADD;
}

const char* ConvertBlendOpText( D3DBLENDOP eBlend )
{
	switch( eBlend )
	{
	case D3DBLENDOP_ADD:
		return "add";
	case D3DBLENDOP_SUBTRACT:
		return "subtract";
	case D3DBLENDOP_REVSUBTRACT:
		return "rev_subtract";
	case D3DBLENDOP_MIN:
		return "min";
	case D3DBLENDOP_MAX:
		return "max";
	}

	LOG.PrintTimeAndLog( 0, "ConvertBlendOpText - Unknown Type(%d)", eBlend );

	return "add";
}

D3DCMPFUNC ConvertCmpFunc( const string &token )
{
	if( token == "never" )
		return D3DCMP_NEVER;
	if( token == "less" )
		return D3DCMP_LESS;
	if( token == "equal" )
		return D3DCMP_EQUAL;
	if( token == "less_equal" )
		return D3DCMP_LESSEQUAL;
	if( token == "greater" )
		return D3DCMP_GREATER;
	if( token == "not_equal" )
		return D3DCMP_NOTEQUAL;
	if( token == "greater_equal" )
		return D3DCMP_GREATEREQUAL;
	if( token == "always" )
		return D3DCMP_ALWAYS;

	LOG.PrintTimeAndLog( 0, "ConvertCmpFunc - Unknown Func - %s", token.c_str() );

	return D3DCMP_LESSEQUAL;
}

const char* ConvertCmpFuncText( D3DCMPFUNC eFunc )
{
	switch( eFunc )
	{
	case D3DCMP_NEVER:
		return "never";
	case D3DCMP_LESS:
		return "less";
	case D3DCMP_EQUAL:
		return "equal";
	case D3DCMP_LESSEQUAL:
		return "less_equal";
	case D3DCMP_GREATER:
		return "greater";
	case D3DCMP_NOTEQUAL:
		return "not_equal";
	case D3DCMP_GREATEREQUAL:
		return "greater_equal";
	case D3DCMP_ALWAYS:
		return "always";
	}

	LOG.PrintTimeAndLog( 0, "ConvertCmpFuncText - Unknown Func - %d", eFunc );

	return "less_equal";
}

bool ParseSceneBlend( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );

	if( vecparams.size() == 1 )
	{
		string token = vecparams[0];

		if( token == "no_alpha" )
			context.pass->SetSceneBlending( BLEND_NO_ALPHA );
		else if( token == "src_alpha" )
			context.pass->SetSceneBlending( BLEND_SRCALPHA );
		else if( token == "color_key" )
			context.pass->SetSceneBlending( BLEND_COLORKEY );
		else if( token == "add" )
			context.pass->SetSceneBlending( BLEND_ADD );
		else if( token == "screen" )
			context.pass->SetSceneBlending( BLEND_SCREEN );
		else if( token == "lighten" )
			context.pass->SetSceneBlending( BLEND_LIGHTEN );
		else if( token == "darken" )
			context.pass->SetSceneBlending( BLEND_DARKEN );
		else if( token == "rev_subtract" )
			context.pass->SetSceneBlending( BLEND_REV_SUBTRACT );
		else if( token == "subtract" )
			context.pass->SetSceneBlending( BLEND_SUBTRACT );
		else
		{
			string error( "Bad scene_blend attribute, unrecognised parameter" );
			error += token;
			LogParseError( error.c_str(), context );
		}
	}
	else if( vecparams.size() == 3 )
	{
		D3DBLENDOP eOp = ConvertBlendOp( vecparams[0] );
		D3DBLEND eSrc, eDest;
		eSrc  = ConvertBlendFactor( vecparams[1] );
		eDest = ConvertBlendFactor( vecparams[2] );

		context.pass->SetSceneBlending( eOp, eSrc, eDest );
	}
	else
	{
		LogParseError( "Bad scene_blend attribute, wrong number of parameters (expected 1 or 3)", 
					   context);
	}

	return false;
}

bool ParseDepthCheck( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "true" )
		context.pass->SetDepthCheckEnable( true );
	else if( params == "false" )
		context.pass->SetDepthCheckEnable( false );
	else
	{
		LogParseError( "Bad depth_check attribute, valid parameters are \'true\' or \'false\'.",
					   context );
	}

	return false;
}

bool ParseDepthWrite( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "true" )
		context.pass->SetDepthWriteEnable( true );
	else if( params == "false" )
		context.pass->SetDepthWriteEnable( false );
	else
	{
		LogParseError( "Bad depth_write attribute, valid parameters are \'true\' or \'false\'.",
					   context );
	}

	return false;
}

bool ParseDepthFunc( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );

	context.pass->SetDepthFunction( ConvertCmpFunc( params ) );

	return false;
}

bool ParseAlphaTest( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );
	if( vecparams.size() == 2 )
	{
		D3DCMPFUNC eFunc = ConvertCmpFunc( vecparams[0] );
		int iAlphaRef = ioStringConverter::ParseInt( vecparams[1].c_str() );
		context.pass->SetAlphaTestSetting( eFunc, iAlphaRef );
	}
	else
	{
		LogParseError( "Bad alpha_rejection attribute, wrong number of parameters (expected 2)",
					   context );
	}

	return false;
}

bool ParseColorWrite( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "true" )
		context.pass->SetColorWriteEnable( true );
	else if( params == "false" )
		context.pass->SetColorWriteEnable( false );
	else
	{
		LogParseError( "Bad color_write attribute, valid parameters are \'true\' or \'false\'.",
					   context );
	}

	return false;
}

bool ParseFillMode( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "solid" )
		context.pass->SetFillMode( FM_SOLID );
	else if( params == "wireframe" )
		context.pass->SetFillMode( FM_WIREFRAME );
	else if( params == "point" )
		context.pass->SetFillMode( FM_POINT );
	else
	{
		LogParseError( "Bad fill_mode attribute, valid parameters are "
					   "\'solid\', \'wireframe\' or \'point\'.", context );
	}

	return false;
}

bool ParseCull( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "none" )
		context.pass->SetCullingMode( CM_NO );
	else if( params == "clockwise" )
		context.pass->SetCullingMode( CM_CW );
	else if( params == "counter_clockwise" )
		context.pass->SetCullingMode( CM_CCW );
	else
	{
		LogParseError( "Bad cull attribute, valid parameters are "
					   "\'none\', \'clockwise\' or \'counter_clockwise\'.", context );
	}

	return false;
}

bool ParseLighting( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "true" )
		context.pass->SetLightingEnable( true );
	else if( params == "false" )
		context.pass->SetLightingEnable( false );
	else
	{
		LogParseError( "Bad lighting attribute, valid parameters are \'true\' or \'false\'.",
					   context );
	}

	return false;
}

bool ParseShading( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "flat" )
		context.pass->SetShadeType( ST_FLAT );
	else if( params == "gouraud" )
		context.pass->SetShadeType( ST_GOURAUD );
	else if( params == "phong" )
		context.pass->SetShadeType( ST_PHONG );
	else
	{
		LogParseError( "Bad shading attribute, valid parameters are \'flat\', "
					   "\'gouraud\' or \'phong\'.", context);
	}

	return false;
}

bool ParseDepthBias( string &params, MaterialScriptContext &context )
{
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );

	if( vecparams.size() == 2 )
	{
		float fDepthBias = ioStringConverter::ParseFloat( vecparams[0].c_str() );
		float fBiasSlope = ioStringConverter::ParseFloat( vecparams[1].c_str() );

		context.pass->SetDepthBias( fDepthBias, fBiasSlope );
	}
	else
	{
		LogParseError( "Bad depth_bias attribute, wrong number of parameters (expected 2)",
					   context );
	}

	return false;
}

bool ParseShaderGroup( string &params, MaterialScriptContext &context )
{
	context.pass->SetShaderGroup( params.c_str() );

	return false;
}

bool ParseShadowCast( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "true" )
		context.pass->AddPassProperty( ioPass::PP_SHADOW_CAST_PASS );

	return false;
}

bool ParseShadowRecv( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "true" )
		context.pass->AddPassProperty( ioPass::PP_SHADOW_RECV_PASS );

	return false;
}

bool ParseLightIterate( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "true" )
		context.pass->AddPassProperty( ioPass::PP_LIGHT_ITERATE_PASS );

	return false;
}

bool ParseRemoveLightType( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "directional" )
	{
		context.pass->RemovePassLightType( ioPass::PLT_DIRECTIONAL );
	}
	else if( params == "point" )
	{
		context.pass->RemovePassLightType( ioPass::PLT_POINT );
	}
	else if( params == "spot" )
	{
		context.pass->RemovePassLightType( ioPass::PLT_SPOT );
	}
	else
	{
		LogParseError( "Bad remove_light_type attribute, unknown type parameter",
					   context );
	}

	return false;
}

bool ParseCustomParam( string &params, MaterialScriptContext &context )
{
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );

	if( vecparams.size() < 3 )
	{
		LogParseError( "Bad custom_param attribute, wrong number of parameters ( at least 3 )",
					   context );
		return false;
	}

	ioShaderCustomArg kArg( vecparams[0].c_str() );
	ShaderCustomParamType eParamType = ConvertShaderParamType( vecparams[1] );

	switch( eParamType )
	{
	case CPT_FLOAT:
		if( vecparams.size() == 3 )
		{
			kArg.SetValue( atof( vecparams[2].c_str() ) );
			context.pass->AddCustomParameter( kArg );
		}
		return false;
	case CPT_VECTOR3:
		if( vecparams.size() == 5 )
		{
			D3DXVECTOR3 vVector3;
			vVector3.x = atof( vecparams[2].c_str() );
			vVector3.y = atof( vecparams[3].c_str() );
			vVector3.z = atof( vecparams[4].c_str() );
			kArg.SetValue( vVector3 );
			context.pass->AddCustomParameter( kArg );
		}
		return false;
	case CPT_VECTOR4:
		if( vecparams.size() == 6 )
		{
			D3DXVECTOR4 vVector4;
			vVector4.x = atof( vecparams[2].c_str() );
			vVector4.y = atof( vecparams[3].c_str() );
			vVector4.z = atof( vecparams[4].c_str() );
			vVector4.w = atof( vecparams[5].c_str() );
			kArg.SetValue( vVector4 );
			context.pass->AddCustomParameter( kArg );
		}
		return false;
	case CPT_COLORVALUE:
		if( vecparams.size() == 6 )
		{
			D3DCOLORVALUE kColor;
			kColor.r = atof( vecparams[2].c_str() );
			kColor.g = atof( vecparams[3].c_str() );
			kColor.b = atof( vecparams[4].c_str() );
			kColor.a = atof( vecparams[5].c_str() );
			kArg.SetValue( kColor );
			context.pass->AddCustomParameter( kArg );
		}
		return false;
	}

	LogParseError( "Bad custom_param attribute, unknown type or parameter count",
				   context );

	return false;
}

bool ParseTextureUnit( string &params, MaterialScriptContext &context )
{
	context.textureUnit = context.pass->CreateTextureUnitState();
	context.section = MSS_TEXTUREUNIT;

	// Return TRUE because this must be followed by a {
	return true;
}

bool ParseTexture( string &params, MaterialScriptContext &context )
{
	context.textureUnit->SetTextureName( params.c_str() );

	return false;
}

bool ParseAnimTexture( string &params, MaterialScriptContext &context )
{
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );
	
	StringVector::size_type tSize = vecparams.size();	
	if( tSize != 2 && tSize != 3 )
	{
		LogParseError( "Bad anim_texture attribute - wrong number of parameters (expected at 2 or 3)",
					   context );
		return false;
	}

	if( ioStringConverter::ParseInt( vecparams[0].c_str() ) < 1 )
	{
		LogParseError( "Bad anim_texture attribute - parameter 1 is at least 1", context );
		return false;
	}

	bool bOnce = false;
	if( tSize == 3 )
	{
		bOnce = ioStringConverter::ParseBool( vecparams[2].c_str() );
	}

	context.textureUnit->SetTextureAnimation( ioStringConverter::ParseInt( vecparams[0].c_str() ),
											  ioStringConverter::ParseFloat( vecparams[1].c_str() ),
											  bOnce );

	return false;
}

bool ParseTexCoord( string &params, MaterialScriptContext &context )
{
	context.textureUnit->SetTextureCoordSet( ioStringConverter::ParseInt( params.c_str() ) );

	return false;
}

bool ParseTexCustom( string &params, MaterialScriptContext &context )
{
	context.textureUnit->SetTextureCustomValue( ioStringConverter::ParseInt( params.c_str() ) );

	return false;
}

bool ParseTexAddressMode( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "wrap" )
		context.textureUnit->SetTextureAddressMode( TAM_WRAP );
	else if( params == "mirror" )
		context.textureUnit->SetTextureAddressMode( TAM_MIRROR );
	else if( params == "clamp" )
		context.textureUnit->SetTextureAddressMode( TAM_CLAMP );
	else
	{
		LogParseError( "Bad tex_address_mode attribute, valid parameters are "
					   "\'wrap\', \'clamp\' or \'mirror\'.", context);
	}

	return false;
}

bool ParseScrollAnim( string &params, MaterialScriptContext &context )
{
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );
	
	StringVector::size_type tSize = vecparams.size();
	if( tSize != 2 && tSize != 3 )
	{
		LogParseError( "Bad scroll_anim attribute, wrong number of "
				       "parameters (expected 2,3)", context );
		return false;
	}

	float fUSpeed, fVSpeed;
	fUSpeed = ioStringConverter::ParseFloat( vecparams[0].c_str() );
	fVSpeed = ioStringConverter::ParseFloat( vecparams[1].c_str() );

	bool bOnce = false;
	if( tSize == 3 )
	{
		bOnce = ioStringConverter::ParseBool( vecparams[2].c_str() );
	}

	context.textureUnit->SetScrollAnimation( fUSpeed, fVSpeed, bOnce );

	return false;
}

bool ParseRotateAnim( string &params, MaterialScriptContext &context )
{
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );

	StringVector::size_type tSize = vecparams.size();
	if( tSize != 1 && tSize != 2 )
	{
		LogParseError( "Bad rotate_anim attribute, wrong number of "
				       "parameters (expected 1 or 2 )", context );
		return false;
	}

	float fRotateSpeed = ioStringConverter::ParseFloat( vecparams[0].c_str() );

	bool bOnce = false;
	if( tSize == 2 )
	{
		bOnce = ioStringConverter::ParseBool( vecparams[1].c_str() );
	}

	context.textureUnit->SetRotateAnimation( fRotateSpeed, bOnce );

	return false;
}

bool ParseWaveXform( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );
	
	StringVector::size_type tSize = vecparams.size();
	if( tSize != 6 && tSize != 7 )
	{
		LogParseError( "Bad wave_xform attribute, wrong number of parameters "
					   "(expected 6 or 7)", context );
		return false;
	}

	TextureTransformType tType;
	if( vecparams[0] == "translate_u" )
		tType = TTT_TRANSLATE_U;
	else if( vecparams[0] == "translate_v" )
		tType = TTT_TRANSLATE_V;
	else if( vecparams[0] == "rotate" )
		tType = TTT_ROTATE;
	else if( vecparams[0] == "scale_u" )
		tType = TTT_SCALE_U;
	else if( vecparams[0] == "scale_v" )
		tType = TTT_SCALE_V;
	else
	{
		LogParseError( "Bad wave_xform attribute, parameter 1 must be \'translate_u\', "
					   "\'translate_v\', \'rotate\', \'scale_x\' or \'scale_y\'",
					   context );
		return false;
	}

	WaveformType waveType;
	if( vecparams[1] == "sine" )
		waveType = WFT_SINE;
	else if( vecparams[1] == "triangle" )
		waveType = WFT_TRIANGLE;
	else if( vecparams[1] == "square" )
		waveType = WFT_SQUARE;
	else if( vecparams[1] == "sawtooth" )
		waveType = WFT_SAWTOOTH;
	else if( vecparams[1] == "inverse_sawtooth" )
		waveType = WFT_INVERSE_SAWTOOTH;
	else
	{
		LogParseError( "Bad wave_xform attribute, parameter 2 must be \'sine\', "
					   "\'triangle\', \'square\', \'sawtooth\' or \'inverse_sawtooth\'"
					   , context );
		return false;
	}

	bool bOnce = false;
	if( tSize == 7 )
	{
		bOnce = ioStringConverter::ParseBool( vecparams[6].c_str() );
	}

	context.textureUnit->SetTransformAnimation( tType,
												waveType,
												ioStringConverter::ParseFloat( vecparams[2].c_str() ),
												ioStringConverter::ParseFloat( vecparams[3].c_str() ),
												ioStringConverter::ParseFloat( vecparams[4].c_str() ),
												ioStringConverter::ParseFloat( vecparams[5].c_str() ),
												bOnce );

	return false;
}

bool ParseColorXform( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	const StringVector &vecparams = ioStringConverter::Split( params, " \t" );
	StringVector::size_type tSize = vecparams.size();

	if( tSize != 6 && tSize != 7 )
	{
		LogParseError( "Bad color_xform attribute, wrong number of parameters "
					   "(expected 6 or 7)", context );
		return false;
	}

	ColorTransformType ctType;
	if( vecparams[0] == "ambient" )
		ctType = CTT_AMBIENT;
	else if( vecparams[0] == "diffuse" )
		ctType = CTT_DIFFUSE;
	else if( vecparams[0] == "specular" )
		ctType = CTT_SPECULAR;
	else if( vecparams[0] == "emissive" )
		ctType = CTT_EMISSIVE;
	else
	{
		LogParseError( "Bad color_xform attribute, parameter 1 must be \'ambient\', "
					   "\'diffuse\', \'specular\' or \'emissive\'",
					   context );
		return false;
	}
	
	WaveformType waveType;
	if( vecparams[1] == "sine" )
		waveType = WFT_SINE;
	else if( vecparams[1] == "triangle" )
		waveType = WFT_TRIANGLE;
	else if( vecparams[1] == "square" )
		waveType = WFT_SQUARE;
	else if( vecparams[1] == "sawtooth" )
		waveType = WFT_SAWTOOTH;
	else if( vecparams[1] == "inverse_sawtooth" )
		waveType = WFT_INVERSE_SAWTOOTH;
	else
	{
		LogParseError( "Bad color_xform attribute, parameter 2 must be \'sine\', "
					   "\'triangle\', \'square\', \'sawtooth\' or \'inverse_sawtooth\'"
					   , context );
		return false;
	}

	bool bOnce = false;
	if( tSize == 7 )
	{
		bOnce = ioStringConverter::ParseBool( vecparams[6].c_str() );
	}

	context.textureUnit->SetColorTransform( ctType,
											waveType,
											ioStringConverter::ParseFloat( vecparams[2].c_str() ),
											ioStringConverter::ParseFloat( vecparams[3].c_str() ),
											ioStringConverter::ParseFloat( vecparams[4].c_str() ),
											ioStringConverter::ParseFloat( vecparams[5].c_str() ),
											bOnce );

	return false;
}

bool ParseFiltering( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "none" )
	{
		if ( g_MaterialMgr.GetNearest() )
			context.textureUnit->SetTextureFiltering( TFO_NEAREST );
		else
			context.textureUnit->SetTextureFiltering( TFO_BILINEAR_NOMIPMAP );
		//context.textureUnit->SetTextureFiltering( TFO_BILINEAR_NOMIPMAP );
		//context.textureUnit->SetTextureFiltering( TFO_NEAREST );
	}
	else if( params == "bilinear" )
		context.textureUnit->SetTextureFiltering( TFO_BILINEAR );
	else if( params == "trilinear" )
		context.textureUnit->SetTextureFiltering( TFO_TRILINEAR );
	else if( params == "anisotropic" )
		context.textureUnit->SetTextureFiltering( TFO_ANISOTROPIC );
	else
	{
		LogParseError( "Bad filtering attribute, valid parameters for simple format are "
					   "\'none\', \'bilinear\', \'trilinear\' or \'anisotropic\'",
					   context );
	}

	return false;
}

const char* ConvertTextureBlendOpText( D3DTEXTUREOP eOp )
{
	switch( eOp )
	{
	case D3DTOP_DISABLE:
		return "disable";
	case D3DTOP_SELECTARG1:
		return "selectarg1";
	case D3DTOP_SELECTARG2:
		return "selectarg2";
	case D3DTOP_MODULATE:
		return "modulate";
	case D3DTOP_MODULATE2X:
		return "modulate2x";
	case D3DTOP_MODULATE4X:
		return "modulate4x";		
	case D3DTOP_ADD:
		return "add";
	case D3DTOP_ADDSIGNED:
		return "addsinged";
	case D3DTOP_ADDSIGNED2X:
		return "addsinged2x";
	case D3DTOP_SUBTRACT:
		return "subtract";
	case D3DTOP_BLENDDIFFUSEALPHA:
		return "blend_diffuse_alpha";
	case D3DTOP_BLENDTEXTUREALPHA:
		return "blend_texture_alpha";
	case D3DTOP_BLENDCURRENTALPHA:
		return "blend_current_alpha";
	case D3DTOP_MODULATEALPHA_ADDCOLOR:
		return "modulate_alpha_addcolor";
	case D3DTOP_MODULATECOLOR_ADDALPHA:
		return "modulate_color_addalpha";
	case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
		return "modulate_invalpha_addcolor";
	case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
		return "modulate_invcolor_addalpha";
	case D3DTOP_DOTPRODUCT3:
		return "dotproduct3";
	}

	return "modulate";
}

D3DTEXTUREOP ConvertTextureBlendOp( const string &token )
{
	if( token == "disable" )
		return D3DTOP_DISABLE;
	else if( token == "selectarg1" )
		return D3DTOP_SELECTARG1;
	else if( token == "selectarg2" )
		return D3DTOP_SELECTARG2;
	else if( token == "modulate" )
		return D3DTOP_MODULATE;
	else if( token == "modulate2x" )
		return D3DTOP_MODULATE2X;
	else if( token == "modulate4x" )
		return D3DTOP_MODULATE4X;
	else if( token == "add" )
		return D3DTOP_ADD;
	else if( token == "addsinged" )
		return D3DTOP_ADDSIGNED;
	else if( token == "addsinged2x" )
		return D3DTOP_ADDSIGNED2X;
	else if( token == "subtract" )
		return D3DTOP_SUBTRACT;
	else if( token == "blend_diffuse_alpha" )
		return D3DTOP_BLENDDIFFUSEALPHA;
	else if( token == "blend_texture_alpha" )
		return D3DTOP_BLENDTEXTUREALPHA;
	else if( token == "blend_current_alpha" )
		return D3DTOP_BLENDCURRENTALPHA;
	else if( token == "modulate_alpha_addcolor" )
		return D3DTOP_MODULATEALPHA_ADDCOLOR;
	else if( token == "modulate_color_addalpha" )
		return D3DTOP_MODULATECOLOR_ADDALPHA;
	else if( token == "modulate_invalpha_addcolor" )
		return D3DTOP_MODULATEINVALPHA_ADDCOLOR;
	else if( token == "modulate_invcolor_addalpha" )
		return D3DTOP_MODULATEINVCOLOR_ADDALPHA;
	else if( token == "dotproduct3" )
		return D3DTOP_DOTPRODUCT3;

	LOG.PrintTimeAndLog( 0, "ConvertTextureBlendOp - Unknown factor - %s", token.c_str() );

	return D3DTOP_DISABLE;
}

DWORD ConvertTextureBlendArg( const string &token )
{
	if( token == "current" )
		return D3DTA_CURRENT;
	else if( token == "diffuse" )
		return D3DTA_DIFFUSE;
	else if( token == "specular" )
		return D3DTA_SPECULAR;
	else if( token == "texture" )
		return D3DTA_TEXTURE;
	else if( token == "tfactor" )
		return D3DTA_TFACTOR;

	LOG.PrintTimeAndLog( 0, "ConvertTextureBlendArg - Unknown factor - %s", token.c_str() );

	return D3DTA_CURRENT;
}

const char* ConvertTextureBlendArgText( DWORD dwArg )
{
	switch( dwArg )
	{
	case D3DTA_CURRENT:
		return "current";
	case D3DTA_DIFFUSE:
		return "diffuse";
	case D3DTA_SPECULAR:
		return "specular";
	case D3DTA_TEXTURE:
		return "texture";
	case D3DTA_TFACTOR:
		return "tfactor";
	}

	return "current";
}

bool ParseTextureStageColorBlend( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	const StringVector &vecparams= ioStringConverter::Split( params, " \t" );
	if( vecparams.size() == 3 )
	{
		context.textureUnit->SetColorStageBlendOp( ConvertTextureBlendOp( vecparams[0] ),
												   ConvertTextureBlendArg( vecparams[1] ),
												   ConvertTextureBlendArg( vecparams[2] ) );
	}
	else
	{
		LogParseError( "Bad color_blend attribute, wrong number of parameters (expected 3)",
					   context );
	}

	return false;
}

bool ParseTextureStageAlphaBlend( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	const StringVector &vecparams= ioStringConverter::Split( params, " \t" );
	if( vecparams.size() == 3 )
	{
		context.textureUnit->SetAlphaStageBlendOp( ConvertTextureBlendOp( vecparams[0] ),
												   ConvertTextureBlendArg( vecparams[1] ),
												   ConvertTextureBlendArg( vecparams[2] ) );
	}
	else
	{
		LogParseError( "Bad alpha_blend attribute, wrong number of parameters (expected 3)",
					   context );
	}

	return false;
}

bool ParseUseLightTexture( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	if( params == "true" )
		context.textureUnit->SetUseLightTexture();

	return false;
}

bool ParseAnisotropy( string &params, MaterialScriptContext &context )
{
	context.textureUnit->SetTextureAnisotropy( ioStringConverter::ParseInt( params.c_str() ) );
	return false;
}

bool ParseScaleUVOnCenter( string &params, MaterialScriptContext &context )
{
	ioStringConverter::toLowerCase( params );
	context.textureUnit->SetTextureScaleOnCenter( ioStringConverter::ParseBool( params.c_str() ) );
	return false;
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

ioMaterialSerializer::ioMaterialSerializer()
{
	m_RootAttribParsers.insert(AttribParserList::value_type("material", (ATTRIBUTE_PARSER)ParseMaterial));

	m_MaterialAttribParsers.insert(AttribParserList::value_type("technique", (ATTRIBUTE_PARSER)ParseTechnique));
	m_MaterialAttribParsers.insert(AttribParserList::value_type("ambient", (ATTRIBUTE_PARSER)ParseAmbient));
	m_MaterialAttribParsers.insert(AttribParserList::value_type("local_glow", (ATTRIBUTE_PARSER)ParseLocalGlow));
    m_MaterialAttribParsers.insert(AttribParserList::value_type("diffuse", (ATTRIBUTE_PARSER)ParseDiffuse));
    m_MaterialAttribParsers.insert(AttribParserList::value_type("specular", (ATTRIBUTE_PARSER)ParseSpecular));
	m_MaterialAttribParsers.insert(AttribParserList::value_type("specularlv", (ATTRIBUTE_PARSER)ParseSpecularLv));
	m_MaterialAttribParsers.insert(AttribParserList::value_type("specular_power", (ATTRIBUTE_PARSER)ParseSpecularPower));
    m_MaterialAttribParsers.insert(AttribParserList::value_type("emissive", (ATTRIBUTE_PARSER)ParseEmissive));

	m_TechniqueAttribParsers.insert(AttribParserList::value_type("pass", (ATTRIBUTE_PARSER)ParsePass));

    m_PassAttribParsers.insert(AttribParserList::value_type("scene_blend", (ATTRIBUTE_PARSER)ParseSceneBlend));
    m_PassAttribParsers.insert(AttribParserList::value_type("depth_check", (ATTRIBUTE_PARSER)ParseDepthCheck));
    m_PassAttribParsers.insert(AttribParserList::value_type("depth_write", (ATTRIBUTE_PARSER)ParseDepthWrite));
    m_PassAttribParsers.insert(AttribParserList::value_type("depth_func", (ATTRIBUTE_PARSER)ParseDepthFunc));
	m_PassAttribParsers.insert(AttribParserList::value_type("alpha_test", (ATTRIBUTE_PARSER)ParseAlphaTest));
    m_PassAttribParsers.insert(AttribParserList::value_type("color_write", (ATTRIBUTE_PARSER)ParseColorWrite));
	m_PassAttribParsers.insert(AttribParserList::value_type("fill_mode", (ATTRIBUTE_PARSER)ParseFillMode));
    m_PassAttribParsers.insert(AttribParserList::value_type("cull", (ATTRIBUTE_PARSER)ParseCull));
    m_PassAttribParsers.insert(AttribParserList::value_type("lighting", (ATTRIBUTE_PARSER)ParseLighting));
    m_PassAttribParsers.insert(AttribParserList::value_type("shading", (ATTRIBUTE_PARSER)ParseShading));
    m_PassAttribParsers.insert(AttribParserList::value_type("depth_bias", (ATTRIBUTE_PARSER)ParseDepthBias));
	m_PassAttribParsers.insert(AttribParserList::value_type("shader_group", (ATTRIBUTE_PARSER)ParseShaderGroup));
	m_PassAttribParsers.insert(AttribParserList::value_type("shadow_cast", (ATTRIBUTE_PARSER)ParseShadowCast));
	m_PassAttribParsers.insert(AttribParserList::value_type("shadow_recv", (ATTRIBUTE_PARSER)ParseShadowRecv));
	m_PassAttribParsers.insert(AttribParserList::value_type("light_iterate", (ATTRIBUTE_PARSER)ParseLightIterate));
	m_PassAttribParsers.insert(AttribParserList::value_type("remove_light_type", (ATTRIBUTE_PARSER)ParseRemoveLightType));
	m_PassAttribParsers.insert(AttribParserList::value_type("custom_param", (ATTRIBUTE_PARSER)ParseCustomParam));
    m_PassAttribParsers.insert(AttribParserList::value_type("texture_unit", (ATTRIBUTE_PARSER)ParseTextureUnit));

	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("texture", (ATTRIBUTE_PARSER)ParseTexture));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("anim_texture", (ATTRIBUTE_PARSER)ParseAnimTexture));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("tex_coord_set", (ATTRIBUTE_PARSER)ParseTexCoord));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("tex_address_mode", (ATTRIBUTE_PARSER)ParseTexAddressMode));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("scroll_anim", (ATTRIBUTE_PARSER)ParseScrollAnim));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("rotate_anim", (ATTRIBUTE_PARSER)ParseRotateAnim));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("wave_xform", (ATTRIBUTE_PARSER)ParseWaveXform));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("color_xform", (ATTRIBUTE_PARSER)ParseColorXform));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("filtering", (ATTRIBUTE_PARSER)ParseFiltering));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("max_anisotropy", (ATTRIBUTE_PARSER)ParseAnisotropy));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("color_blend", (ATTRIBUTE_PARSER)ParseTextureStageColorBlend));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("alpha_blend", (ATTRIBUTE_PARSER)ParseTextureStageAlphaBlend));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("use_light_texture", (ATTRIBUTE_PARSER)ParseUseLightTexture));
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("scale_uv_on_center", (ATTRIBUTE_PARSER)ParseScaleUVOnCenter));		
	m_TextureUnitAttribParsers.insert(AttribParserList::value_type("custom_value", (ATTRIBUTE_PARSER)ParseTexCustom));		

	m_pExportfp = NULL;
	memset( m_szTab, 0, MAX_PATH );
}

ioMaterialSerializer::~ioMaterialSerializer()
{
}

void ioMaterialSerializer::InitMaterialScriptContext( const char *szScriptName )
{
	m_ScriptContext.section = MSS_NONE;
	m_ScriptContext.material = NULL;
	m_ScriptContext.technique = NULL;
	m_ScriptContext.pass = NULL;
	m_ScriptContext.textureUnit = NULL;

	strcpy( m_ScriptContext.szScriptName, szScriptName );
	m_ScriptContext.iMaterialCnt = 0;

	m_ScriptContext.lineNo = 0;
}

bool ioMaterialSerializer::ParseScriptLine( string &rkLine )
{
	switch( m_ScriptContext.section )
	{
	case MSS_NONE:
		if( rkLine == "}" )
		{
			LOG.PrintTimeAndLog( 0, "UnExpected terminating brace %s(%d)",
								    rkLine.c_str(), m_ScriptContext.lineNo );
		}
		else
		{
			return InvokeParser( rkLine, m_RootAttribParsers );
		}
		break;
	case MSS_MATERIAL:
		if( rkLine == "}" )	// Mateiral Load Ended...
		{
			m_ScriptContext.section = MSS_NONE;
			if( m_ScriptContext.material )
			{
				g_MaterialMgr.ApplyLoadScriptModifier( m_ScriptContext.material );
				m_ScriptContext.material = NULL;
			}
		}
		else
		{
			return InvokeParser( rkLine, m_MaterialAttribParsers );
		}
		break;
	case MSS_TECHNIQUE:
		if( rkLine == "}" )
		{
			m_ScriptContext.section = MSS_MATERIAL;
			m_ScriptContext.technique = NULL;
		}
		else
		{
			return InvokeParser( rkLine, m_TechniqueAttribParsers );
		}
		break;
	case MSS_PASS:
		if( rkLine == "}" )
		{
			m_ScriptContext.section = MSS_TECHNIQUE;
			m_ScriptContext.pass = NULL;
		}
		else
		{
			return InvokeParser( rkLine, m_PassAttribParsers );
		}
		break;
	case MSS_TEXTUREUNIT:
		if( rkLine == "}" )
		{
			m_ScriptContext.section = MSS_PASS;
			m_ScriptContext.textureUnit = NULL;
		}
		else
		{
			return InvokeParser( rkLine, m_TextureUnitAttribParsers );
		}
		break;
	}

	return false;
}

bool ioMaterialSerializer::InvokeParser( string &rkLine, AttribParserList &rkParsers )
{
	const StringVector &splitCmd = ioStringConverter::Split( rkLine, " \t", 1 );

	AttribParserList::iterator iparser = rkParsers.find( splitCmd[0] );
	if( iparser != rkParsers.end() )
	{
		string cmd;
		if( splitCmd.size() >= 2 )
			cmd = splitCmd[1];

		return iparser->second( cmd, m_ScriptContext );
	}
	
	LogParseError( "Bad Command ", m_ScriptContext );

	return false;
}

int ioMaterialSerializer::ParseScript( ioTextStream &rkStream, const char *szScriptName )
{
	InitMaterialScriptContext( szScriptName );

	string line;
	bool nextIsOpenBrace = false;

	while( !rkStream.IsEOF() )
	{
		rkStream.GetLine( line );
		m_ScriptContext.lineNo++;

		if( line.length() == 0 || line.substr( 0, 2 ) == "//" )
			continue;

		if( nextIsOpenBrace )
		{
			if( line != "{" )
			{
				LOG.PrintTimeAndLog( 0, " { not Exist. %s(%d)", line.c_str(), m_ScriptContext.lineNo );
			}
			nextIsOpenBrace = false;
		}
		else
		{
			nextIsOpenBrace = ParseScriptLine( line );
		}
	}

	if( m_ScriptContext.section != MSS_NONE )
	{
		LogParseError( "UnExpected end of file", m_ScriptContext );
		return -1;
	}

	return m_ScriptContext.iMaterialCnt;
}

void ioMaterialSerializer::BeginExport( FILE *fp )
{
	m_pExportfp = fp;
}

void ioMaterialSerializer::SetExportTabSize( int iSize )
{
	memset( m_szTab, 0, MAX_PATH );

	for( int i=0 ; i<iSize ; i++ )
	{
		strcat( m_szTab, "\t" );
	}
}

void ioMaterialSerializer::ExportScript( LPSTR fmt,... )
{
	if( !m_pExportfp )	return;

	char szBuf[1024];

	va_list args;
	va_start(args, fmt);
	vsprintf(szBuf, fmt, args);
	va_end(args);

	fprintf( m_pExportfp, "%s%s\n", m_szTab, szBuf );
}

void ioMaterialSerializer::EndExport()
{
	m_pExportfp = NULL;
	memset( m_szTab, 0, MAX_PATH );
}

void ioMaterialSerializer::SaveScript( FILE *fp, ioMaterial *pMtl )
{
	BeginExport( fp );
	SetExportTabSize( 0 );

	ExportScript( "material" );
	ExportScript( "{" );
	
	SetExportTabSize( 1 );

	D3DCOLORVALUE kValue;
	kValue = pMtl->GetAmbient();
	if( kValue.r != 1.0f || kValue.g != 1.0f || kValue.b != 1.0f )
	{
		ExportScript( "ambient %.1f %.1f %.1f", kValue.r, kValue.g, kValue.b );
	}

	kValue = pMtl->GetDiffuse();
	if( kValue.r != 1.0f || kValue.g != 1.0f || kValue.b != 1.0f )
	{
		ExportScript( "diffuse %.1f %.1f %.1f", kValue.r, kValue.g, kValue.b );
	}

	kValue = pMtl->GetEmissive();
	if( kValue.r != 0.0f || kValue.g != 0.0f || kValue.b != 0.0f )
	{
		ExportScript( "emissive %.1f %.1f %.1f", kValue.r, kValue.g, kValue.b );
	}

	kValue = pMtl->GetSpecular();
	if( kValue.r != 1.0f || kValue.g != 1.0f || kValue.b != 1.0f )
	{
		ExportScript( "specular %.1f %.1f %.1f", kValue.r, kValue.g, kValue.b );
	}

	if( pMtl->GetSpecularLv() != 0.0f )
	{
		ExportScript( "specularlv %.1f", pMtl->GetSpecularLv() );
	}

	if( pMtl->GetSpecularPower() != 0.0f )
	{
		ExportScript( "specular_power %.1f", pMtl->GetSpecularPower() );
	}

	ioMaterial::TechniqueIterator iter = pMtl->GetTechniqueIterator();
	while( iter.HasMoreElements() )
	{
		SaveTechnique( iter.Next() );
	}

	SetExportTabSize( 0 );
	ExportScript( "}" );
	EndExport();
}

void ioMaterialSerializer::SaveTechnique( ioTechnique *pTech )
{
	SetExportTabSize( 1 );

	ExportScript( "technique" );
	ExportScript( "{" );

	ioTechnique::PassIterator iter = pTech->GetPassIterator();
	while( iter.HasMoreElements() )
	{
		SavePass( iter.Next() );
	}

	SetExportTabSize( 1 );
	ExportScript( "}" );
}

const char* ConvertCullingModeText( CullingMode eCM )
{
	switch( eCM )
	{
	case CM_NO:
		return "none";
	case CM_CW:
		return "clockwise";
	case CM_CCW:
		return "counter_clockwise";
	}

	LOG.PrintTimeAndLog( 0, "ConvertCullingModeText - Unknown Type(%d)", eCM );

	return "counter_clockwise";
}

const char* ConvertShadeTypeText( ShadeType eType )
{
	switch( eType )
	{
	case ST_FLAT:
		return "flat";
	case ST_GOURAUD:
		return "gouraud";
	case ST_PHONG:
		return "phong";
	}

	return "gouraud";
}

void ioMaterialSerializer::SavePass( ioPass *pPass )
{
	SetExportTabSize( 2 );
	ExportScript( "pass" );
	ExportScript( "{" );
	
	SetExportTabSize( 3 );

	if( pPass->GetBlendOp() != D3DBLENDOP_ADD ||
		pPass->GetSourceBlendFactor() != D3DBLEND_ONE ||
		pPass->GetDestBlendFactor() != D3DBLEND_ZERO )
	{
		ExportScript( "scene_blend %s %s %s",
					  ConvertBlendOpText( pPass->GetBlendOp() ),
					  ConvertBlendFactorText( pPass->GetSourceBlendFactor() ),
					  ConvertBlendFactorText( pPass->GetDestBlendFactor() ) );
													
	}

	if( !pPass->GetDepthCheckEnable() )
	{
		ExportScript( "depth_check false" );
	}

	if( !pPass->GetDepthWriteEnable() )
	{
		ExportScript( "depth_write false" );
	}

	if( pPass->GetDepthFunction() != D3DCMP_LESSEQUAL )
	{
		ExportScript( "depth_func %s", ConvertCmpFuncText( pPass->GetDepthFunction() ) );
	}

	if( pPass->GetAlphaTestFunction() != D3DCMP_NEVER ||
		pPass->GetAlphaTestRef() != 0 )
	{
		ExportScript( "alpha_test %s %d", ConvertCmpFuncText( pPass->GetAlphaTestFunction() ),
										  pPass->GetAlphaTestRef() );
	}

	if( !pPass->GetColorWriteEnable() )
	{
		ExportScript( "color_write false" );
	}

	if( pPass->GetCullingMode() != CM_CCW )
	{
		ExportScript( "cull %s", ConvertCullingModeText( pPass->GetCullingMode() ) );
	}

	if( !pPass->IsLightingEnable() )
	{
		ExportScript( "lighting false" );
	}

	if( pPass->GetShadeType() != ST_GOURAUD )
	{
		ExportScript( "shading %s", ConvertShadeTypeText( pPass->GetShadeType() ) );
	}

	if( pPass->GetDepthBias() != 0.0f || pPass->GetBiasSlope() != 0.0f )
	{
		ExportScript( "depth_bias %.1f %.1f", pPass->GetDepthBias(), pPass->GetBiasSlope() );
	}

	if( pPass->GetShaderGroupName() )
	{
		ExportScript( "shader_group %s", pPass->GetShaderGroupName()->c_str() );
	}

	if( pPass->HasProperty( ioPass::PP_SHADOW_CAST_PASS ) )
	{
		ExportScript( "shadow_cast true" );
	}

	if( pPass->HasProperty( ioPass::PP_SHADOW_RECV_PASS ) )
	{
		ExportScript( "shadow_recv true" );
	}

	if( pPass->HasProperty( ioPass::PP_LIGHT_ITERATE_PASS ) )
	{
		ExportScript( "light_iterate true" );
	}

	if( !pPass->IsLightAvailableType( ioPass::PLT_DIRECTIONAL ) )
	{
		ExportScript( "remove_light_type directional" );
	}

	if( !pPass->IsLightAvailableType( ioPass::PLT_POINT ) )
	{
		ExportScript( "remove_light_type point" );
	}

	if( !pPass->IsLightAvailableType( ioPass::PLT_SPOT ) )
	{
		ExportScript( "remove_light_type spot" );
	}

	// custom_param
	const ioShaderCustomParameters &rkCustomParams = pPass->GetCustomParameters();
	if( rkCustomParams.GetCustomArgCount() > 0 )
	{
		int iParamCnt = rkCustomParams.GetCustomArgCount();
		for( int i=0 ; i<iParamCnt ; i++ )
		{
			const ioShaderCustomArg *pArg = rkCustomParams.GetValue( i );
			
			D3DXVECTOR4 kValue = pArg->GetVector4();
			switch( pArg->GetType() )
			{
			case CPT_FLOAT:
				ExportScript( "custom_param %s float %.3f", pArg->GetName().c_str(),
															kValue.x );
				break;
			case CPT_VECTOR3:
				ExportScript( "custom_param %s vector3 %.3f %.3f %.3f", pArg->GetName().c_str(),
																		kValue.x,
																		kValue.y,
																		kValue.z );
				break;
			case CPT_VECTOR4:
				ExportScript( "custom_param %s vector4 %.3f %.3f %.3f %.3f", pArg->GetName().c_str(),
																			 kValue.x,
																			 kValue.y,
																			 kValue.z,
																			 kValue.w );
				break;
			case CPT_COLORVALUE:
				ExportScript( "custom_param %s colorvalue %.3f %.3f %.3f %.3f", pArg->GetName().c_str(),
																				kValue.x,
																				kValue.y,
																				kValue.z,
																				kValue.w );
				break;
			}
		}
	}

	ioPass::TextureUnitStateIterator iter = pPass->GetTextureUnitStateIterator();
	while( iter.HasMoreElements() )
	{
		SaveTextureUnitState( iter.Next() );
	}

	SetExportTabSize( 2 );
	ExportScript( "}" );
}

const char* ConvertTexAddressModeText( TextureAddressMode eMode )
{
	switch( eMode )
	{
	case TAM_WRAP:
		return "wrap";
	case TAM_MIRROR:
		return "mirror";
	case TAM_CLAMP:
		return "clamp";
	}

	return "wrap";
}

const char* ConvertTextureFilteringText( TextureFilterOption eFilter )
{
	switch( eFilter )
	{
	case TFO_NEAREST:
		return "none";
	case TFO_BILINEAR:
		return "bilinear";
	case TFO_TRILINEAR:
		return "trilinear";
	case TFO_ANISOTROPIC:
		return "anisotropic";
	}

	return "bilinear";
}

const char* ConvertTransformType( TextureTransformType tType )
{
	switch( tType )
	{
	case TTT_TRANSLATE_U:
		return "translate_u";
	case TTT_TRANSLATE_V:
		return "translate_v";
	case TTT_ROTATE:
		return "rotate";
	case TTT_SCALE_U:
		return "scale_u";
	case TTT_SCALE_V:
		return "scale_v";
	}

	return "translate_u";
}

const char* ConvertColorTransformType( ColorTransformType ctType )
{
	switch( ctType )
	{
	case CTT_AMBIENT:
		return "ambient";
	case CTT_DIFFUSE:
		return "diffuse";
	case CTT_SPECULAR:
		return "specular";
	case CTT_EMISSIVE:
		return "emissive";
	}

	return "ambient";
}

const char* ConvertWaveTypeText( WaveformType waveType )
{
	switch( waveType )
	{
	case WFT_SINE:
		return "sine";
	case WFT_TRIANGLE:
		return "triangle";
	case WFT_SQUARE:
		return "square";
	case WFT_SAWTOOTH:
		return "sawtooth";
	case WFT_INVERSE_SAWTOOTH:
		return "inverse_sawtooth";
	}

	return "sine";
}

void ioMaterialSerializer::SaveTextureUnitState( ioTextureUnitState *pUnit )
{
	SetExportTabSize( 3 );

	ExportScript( "texture_unit" );
	ExportScript( "{" );

	SetExportTabSize( 4 );

	if( !pUnit->GetTextureName().IsEmpty() )
	{
		ExportScript( "texture %s", pUnit->GetTextureName().c_str() );
	}

	if( pUnit->GetTextureCoordSet() != 0 )
	{
		ExportScript( "tex_coord_set %d", pUnit->GetTextureCoordSet() );
	}

	if( pUnit->IsUseLightTexture() )
	{
		ExportScript( "use_light_texture true" );
	}

	if( pUnit->IsTextureScaleOnCenter() )
	{
		ExportScript( "scale_uv_on_center true" );
	}

	if( pUnit->GetTextureAddressMode() != TAM_WRAP )
	{
		ExportScript( "tex_address_mode %s",
					  ConvertTexAddressModeText( pUnit->GetTextureAddressMode() ) );
	}

	if( pUnit->GetTextureFiltering() != TFO_BILINEAR )
	{
		ExportScript( "filtering %s", 
					  ConvertTextureFilteringText( pUnit->GetTextureFiltering() ) );
	}

	if( pUnit->GetTextureAnisotropy() != 1 )
	{
		ExportScript( "max_anisotropy %d", pUnit->GetTextureAnisotropy() );
	}

	if( pUnit->GetTextureCustomValue() > 0 )
	{
		ExportScript( "custom_value %d", pUnit->GetTextureCustomValue() );
	}

	StageBlendOperation kOp = pUnit->GetColorStageBlendOp();
	if( kOp.m_StageOp != D3DTOP_MODULATE ||
		kOp.m_dwSrcFactor !=D3DTA_TEXTURE ||
		kOp.m_dwDestFactor != D3DTA_DIFFUSE )
	{
		ExportScript( "color_blend %s %s %s",
						ConvertTextureBlendOpText( kOp.m_StageOp ),
						ConvertTextureBlendArgText( kOp.m_dwSrcFactor ),
						ConvertTextureBlendArgText( kOp.m_dwDestFactor ) );
	}

	kOp = pUnit->GetAlphaStageBlendOp();
	if( kOp.m_StageOp != D3DTOP_MODULATE ||
		kOp.m_dwSrcFactor !=D3DTA_TEXTURE ||
		kOp.m_dwDestFactor != D3DTA_DIFFUSE )
	{
		ExportScript( "alpha_blend %s %s %s",
						ConvertTextureBlendOpText( kOp.m_StageOp ),
						ConvertTextureBlendArgText( kOp.m_dwSrcFactor ),
						ConvertTextureBlendArgText( kOp.m_dwDestFactor ) );
	}

	int iTexEffectCnt = pUnit->GetTextureEffectCnt();
	for( int i=0 ; i<iTexEffectCnt ; i++ )
	{
		TextureEffect *pEffect = pUnit->GetTextureEffect( i );

		switch( pEffect->m_Type )
		{
		case TET_SCROLL:
			if( pEffect->m_dwOnce )
			{
				ExportScript( "scroll_anim %.3f %.3f true", pEffect->m_fArg1, pEffect->m_fArg2 );
			}
			else
			{
				ExportScript( "scroll_anim %.3f %.3f", pEffect->m_fArg1, pEffect->m_fArg2 );
			}
			break;
		case TET_ROTATE:
			if( pEffect->m_dwOnce )
			{
				ExportScript( "rotate_anim %.3f true", pEffect->m_fArg1 );
			}
			else
			{
				ExportScript( "rotate_anim %.3f", pEffect->m_fArg1 );
			}
			break;
		case TET_ANIMATE:
			if( pEffect->m_dwOnce )
			{
				ExportScript( "anim_texture %d %.1f true", pUnit->GetNumFrames(),
														   pUnit->GetAnimationDuration() );
			}
			else
			{
				ExportScript( "anim_texture %d %.1f", pUnit->GetNumFrames(),
												 	  pUnit->GetAnimationDuration() );
			}
			break;
		case TET_TRANSFORM:
			if( pEffect->m_dwOnce )
			{
				ExportScript( "wave_xform %s %s %.3f %.3f %.3f %.3f true",
								ConvertTransformType( pEffect->m_XformType ),
								ConvertWaveTypeText( pEffect->m_WaveType ),
								pEffect->m_fBase,
								pEffect->m_fFrequency,
								pEffect->m_fPhase,
								pEffect->m_fAmplitude );
			}
			else
			{
				ExportScript( "wave_xform %s %s %.3f %.3f %.3f %.3f",
								ConvertTransformType( pEffect->m_XformType ),
								ConvertWaveTypeText( pEffect->m_WaveType ),
								pEffect->m_fBase,
								pEffect->m_fFrequency,
								pEffect->m_fPhase,
								pEffect->m_fAmplitude );
			}
			break;
		case TET_COLOR_TRANSFORM:
			if( pEffect->m_dwOnce )
			{
				ExportScript( "color_xform %s %s %.3f %.3f %.3f %.3f true",
								ConvertColorTransformType( pEffect->m_ColorXformType ),
								ConvertWaveTypeText( pEffect->m_WaveType ),
								pEffect->m_fBase,
								pEffect->m_fFrequency,
								pEffect->m_fPhase,
								pEffect->m_fAmplitude );
			}
			else
			{
				ExportScript( "color_xform %s %s %.3f %.3f %.3f %.3f",
								ConvertColorTransformType( pEffect->m_ColorXformType ),
								ConvertWaveTypeText( pEffect->m_WaveType ),
								pEffect->m_fBase,
								pEffect->m_fFrequency,
								pEffect->m_fPhase,
								pEffect->m_fAmplitude );
			}
			break;
		}
	}

	SetExportTabSize( 3 );
	ExportScript( "}" );
}
