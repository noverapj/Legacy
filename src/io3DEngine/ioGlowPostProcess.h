

#ifndef _ioGlowPostProcess_h_
#define _ioGlowPostProcess_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioShader;
class ioRenderSystem;
class ioRenderTexture;
class ioMeshData;
class ioSceneManager;
class ioTexture;

class ioGlowPostProcess
{
protected:
	ioRenderSystem *m_pRenderSystem;

	ioMeshData *m_pRHWMeshData;
	ioMeshData *m_pGaussianMeshData;
	ioShader   *m_pGaussian;

	ioShader   *m_pVBlur;
	ioShader   *m_pHBlur;

	ioTexture  *m_pBlurFilter;
	ioTexture  *m_pFilter;

	int   m_iCurWindowWidth;
	int   m_iCurWindowHeight;

	DWORD m_dwAlphaValue;
	DWORD m_dwSecondAlphaValue;
	DWORD m_dwLocalGlowValue;

	float m_fGlowness;
	float m_fTexcel;

protected:
	D3DXVECTOR4	m_vTexOffsetX[6];
	D3DXVECTOR4	m_vTexOffsetY[6];

public:
	bool InitProcess( const ioHashString &szBlurFilter, const ioHashString &szFilter );

	void RenderLocalGlow( ioRenderTexture *pFirst, ioRenderTexture *pSecond );
	void RenderProcess( ioRenderTexture *pFirst, ioRenderTexture *pSecond );
	void CalcLocalGlow( ioRenderTexture *pFirst, ioRenderTexture *pSecond );
	void ClearResource();

	void SetAlphaValue( int iAlpha );
	void SetSecondAlphaValue( int iAlpha );

	void SetGlownessAndTexcelInc( float fGlowness, float fTexcel);

protected:
	bool CheckChangedResource( int iTexWidth, int iTexHeight );
	bool CreateVertexData( int iTexWidth, int iTexHeight );

	void UpdateTexOffsetArray( int iTexWidth, int iTexHeight );
	void ApplyGaussianBlur( ioRenderTexture *pFirst, ioRenderTexture *pSecond, int iTech );

	void ApplyHBlur( ioRenderTexture *pFirst, ioRenderTexture *pSecond );
	void ApplyVBlur( ioRenderTexture *pFirst, ioRenderTexture *pSecond );
public:
	ioGlowPostProcess( ioRenderSystem *pRenderSystem );
	virtual ~ioGlowPostProcess();
};

#endif
