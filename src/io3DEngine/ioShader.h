

#ifndef _ioShader_h_
#define _ioShader_h_

#include "io3DCommon.h"
#include "ioResource.h"

#include "ioShaderDefine.h"
#include "ioShaderParameter.h"

class ioLight;
class ioAutoShaderParamSource;
class ioShaderCustomParameters;

class __EX ioShader : public ioResource
{
protected:
	struct ShaderConstant
	{
		D3DXHANDLE m_Handle;

		int m_iValue;
		D3DXVECTOR4 m_vValue;
		D3DXMATRIX m_matValue;

		ShaderConstant();
	};

	struct AutoParameter
	{
		AutoConstantEntry m_Entry;
		ShaderConstant *m_pConstant;
	};

	typedef std::vector< AutoParameter > AutoParameterList;
	AutoParameterList m_AutoParameters;

	struct CustomParameter
	{
		ioHashString m_ParamName;
		AutoConstantEntry m_Entry;
		ShaderConstant *m_pConstant;
	};

	typedef std::vector< CustomParameter > CustomParameterList;
	CustomParameterList m_CustomParameters;

	typedef std::map< ioHashString, ShaderConstant* > NameParameterList;
	NameParameterList m_NameParameters;

	ID3DXEffect *m_pEffect;
	int		m_iCurTechIdx;

	LightCombination m_LightCombination;
	VertexShaderProfile m_VertexProfile;
	PixelShaderProfile  m_PixelProfile;
	ShaderType m_ShaderType;

	bool	m_bValidate;

public:
	void InitShader( ID3DXEffect *pEffect );
	void ValidateTechnique( int iIndex );
	
	inline int GetCurValidTech() const { return m_iCurTechIdx; }

public:
	void SetLightCombination( LightCombination eCombo );
	void SetVertexShaderProfile( VertexShaderProfile eProfile );
	void SetPixelShaderProfile( PixelShaderProfile eProfile );
	void SetShaderType( ShaderType eType );

	inline LightCombination GetLightCombination() const { return m_LightCombination; }
	inline VertexShaderProfile GetVertexShaderProfile() const { return m_VertexProfile; }
	inline PixelShaderProfile GetPixelShaderProfile() const { return m_PixelProfile; }
	inline ShaderType GetShaderType() const { return m_ShaderType; }
	
public:
	void OnLostDevice();
	void OnResetDevice();

public:
	int  Begin();
	void End();

	void BeginPass( int iPass );
	void CommitChanges();
	void EndPass();

	void BindAutoShaderParamNoLights( const ioAutoShaderParamSource &rkSource );
	void BindAutoShaderParamOnlyLights( const ioAutoShaderParamSource &rkSource );
	void BindCustomShaderParam( const ioShaderCustomParameters &rkParamList );

protected:
	void BindDirectionalLight( const ioLight *pLight, const ioAutoShaderParamSource &rkSource );
	void BindPointLight( const ioLight *pLight, const ioAutoShaderParamSource &rkSource );
	void BindSpotLight( const ioLight *pLight, const ioAutoShaderParamSource &rkSource );

protected:
	void ClearParameterHandle();
	void ParseParameterHandle();

	ShaderConstant* GetNameConstant( const ioHashString &szName );
	ShaderConstant* GetEntryConstant( const AutoConstantEntry &rkEntry );
	ShaderConstant* GetCustomConstant( const ioHashString &szName );

public:
	void SetInt( const ioHashString &szName, int iVal );
	void SetFloat( const ioHashString &szName, float fVal );
	void SetFloatArray( const ioHashString &szName, const float *pArray, int iCount );
	void SetVector( const ioHashString &szName, const D3DXVECTOR3 &vVec3 );
	void SetVector( const ioHashString &szName, const D3DXVECTOR4 &vVec4 );
	void SetVectorArray( const ioHashString &szName, const D3DXVECTOR4 *pVec4, int iCount );
	void SetColorValue( const ioHashString &szName, const D3DCOLORVALUE &rColor );
	void SetMatrix( const ioHashString &szName, const D3DXMATRIX &mat );
	void SetTexture( const ioHashString &szName, IDirect3DTexture9 *pTex );

protected:
	void SetInt( ShaderConstant *pConstant, int iVal );
	void SetFloat( ShaderConstant *pConstant, float fVal );
	void SetFloatArray( ShaderConstant *pConstant, const float *pArray, int iCount );
	void SetVector( ShaderConstant *pConstant, const D3DXVECTOR3 &vVec3 );
	void SetVector( ShaderConstant *pConstant, const D3DXVECTOR4 &vVec4 );
	void SetColorValue( ShaderConstant *pConstant, const D3DCOLORVALUE &rColor );
	void SetVectorArray( ShaderConstant *pConstant, const D3DXVECTOR4 *pVec4, int iCount );
	void SetMatrix( ShaderConstant *pConstant, const D3DXMATRIX *mat );

	void SetBipedList( ShaderConstant *pConstant, const D3DXVECTOR4 *pVec4, int iCount );

public:
	ioShader( const ioHashString &name );
	virtual ~ioShader();
};

#endif
