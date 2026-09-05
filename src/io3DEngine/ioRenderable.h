

#ifndef _ioRenderable_h_
#define _ioRenderable_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioShaderParameter.h"

class ioShader;
class ioMaterial;
class ioTechnique;
class ioRenderOperation;
class ioCamera;

class __EX ioRenderable
{
protected:
	ioShaderCustomParameters m_CustomParameters;

public:
	void SetCustomParameter( const ioShaderCustomArg &rkArg );
	const ioShaderCustomArg* GetCustomParameter( const ioHashString &kParamName );
	
	inline const ioShaderCustomParameters& GetCustomParameters() const { return m_CustomParameters; }

public:
	virtual ioMaterial* GetMaterial() const = 0;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera ) = 0;
	virtual void GetWorldTransform( D3DXMATRIX *pMatWorld ) const = 0;

	virtual void GetRenderOperation( ioRenderOperation &rkRenderOp ) = 0;
	virtual DWORD GetMeshHash() const;

public:
	virtual ioTechnique* GetTechnique() const;
	virtual const D3DCOLORVALUE& GetDiffuseRate() const;
	virtual const ioLightList& GetLights() const;

	virtual const D3DXVECTOR4* GetBipedList( int &iBipedListCnt ) const;
	virtual int GetWeightedBipedCnt() const;
	virtual int GetAlphaRate() const;

	virtual bool IsShadowRecvRender() const;

public:
	ioRenderable();
	virtual ~ioRenderable();
};

#endif
