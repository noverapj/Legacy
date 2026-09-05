

#ifndef _ioShaderGroup_h_
#define _ioShaderGroup_h_

#include "io3DCommon.h"
#include "ioResource.h"

#include "ioShaderDefine.h"

class ioShader;

class __EX ioShaderGroup : public ioResource
{
protected:
	ShaderGroupType m_GroupType;
	ioShader* m_ShaderTable[ LC_MAX_COMBINATION ];
	ioShader* m_pGenShadow;
	ioShader* m_pRenderShadow;

public:
	void SetGroupType( ShaderGroupType eType );
	inline ShaderGroupType GetGroupType() const { return m_GroupType; }

public:
	void AddShader( ioShader *pShader );

	ioShader* GetShader( const ioLightList &rkLightList );
	ioShader* GetShader( const ioLight *pLight );
	ioShader* GetShader( LightCombination eLight );

	inline ioShader* GetGenShadowShader() const { return m_pGenShadow; }
	inline ioShader* GetRenderShadowShader() const { return m_pRenderShadow;}

protected:
	void UnLoadShader( ioShader* &pShader );

public:
	ioShaderGroup( const ioHashString &name );
	virtual ~ioShaderGroup();
};

#endif