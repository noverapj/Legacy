

#ifndef _ioRenderStateDesc_h_
#define _ioRenderStateDesc_h_

#include "io3DCommon.h"

class ioVertexBufferInstance;
class ioVertexDeclaration;

#define MAX_LIGHTS			8
#define MAX_STREAM_LAYERS	8
#define MAX_TEXTURE_LAYERS	16

class ioRenderStateDesc
{
public:
	struct VertexStreamDesc
	{
		int iOffsetBytes;
		IDirect3DVertexBuffer9 *pVtxBuf;
	};
	
	struct SceneBlend
	{
		D3DBLENDOP eBlendOp;
		D3DBLEND   eSrc;
		D3DBLEND   eDest;
	};

	struct TextureStageDesc
	{
		IDirect3DTexture9 *pTex;
		int iTexCoordIndex;
		int iMaxAnisotropy;
		
		StageBlendOperation ColorBlend;
		StageBlendOperation AlphaBlend;
		TextureAddressMode  TextureAddress;
		
		TextureFilterOption TextureFilter;
		DWORD dwMagFilter;
		DWORD dwMinFilter;
		DWORD dwMipFilter;
		
		float fMipMapBias;
		DWORD m_dwTextureTransformFlags;
	};

public:
	DWORD m_dwFVF;
	VertexStreamDesc m_VertexStream[MAX_STREAM_LAYERS];
	int m_iLastVertexStreamCount;

	IDirect3DVertexDeclaration9 *m_pD3DDecl;
	IDirect3DIndexBuffer9 *m_pIndexBuf;

	TextureStageDesc m_TexStageDesc[MAX_TEXTURE_LAYERS];
	ioLight *m_pLights[MAX_LIGHTS];
	D3DMATERIAL9	m_Material;

	SceneBlend m_SceneBlend;
	DWORD m_dwAlphaBlendEnable;

	DWORD m_dwTextureStageColorWrite;

	D3DCMPFUNC m_DepthFunc;
	int m_iDepthCheckEnable;
	int m_iDepthWriteEnable;

	float m_fDepthBias;
	float m_fDepthSlope;

	CullingMode m_CullMode;
	ShadeType	m_ShadeType;
	FillMode	m_FillMode;

	D3DCMPFUNC m_AlphaTextFunc;
	int m_iAlphaTestRef;

	int m_iLightEnable;

public:
	void InitStateDesc();
	bool IsSameStream( UINT uStream, ioVertexBufferInstance *pInstance );
	void SetStreamDesc( UINT uStream, ioVertexBufferInstance *pInstance );
	void ClearStreamDesc();

public:
	ioRenderStateDesc();
	~ioRenderStateDesc();
};

#endif

