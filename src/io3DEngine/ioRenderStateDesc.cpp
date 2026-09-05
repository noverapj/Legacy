

#include "stdafx.h"

#include "ioRenderStateDesc.h"
#include "ioVertexBufferInstance.h"

ioRenderStateDesc::ioRenderStateDesc()
{
	InitStateDesc();
}

ioRenderStateDesc::~ioRenderStateDesc()
{
}

void ioRenderStateDesc::InitStateDesc()
{
	m_dwFVF = 0;

	memset( m_VertexStream, 0xff, sizeof(VertexStreamDesc) * MAX_STREAM_LAYERS );
	m_iLastVertexStreamCount = 0;

	m_pD3DDecl = NULL;
	m_pIndexBuf = NULL;

	memset( m_TexStageDesc, 0xff, sizeof(TextureStageDesc) * MAX_TEXTURE_LAYERS );
	for( int i=0 ; i<MAX_TEXTURE_LAYERS ; i++ )
	{
		m_TexStageDesc[i].fMipMapBias = 10000.0f;
	}

	for( i=0 ; i<MAX_LIGHTS ; i++ )
	{
		m_pLights[i] = NULL;
	}

	memset( &m_Material, 0xff, sizeof(m_Material) );

	memset( &m_SceneBlend, 0xff, sizeof(SceneBlend) );
	m_dwAlphaBlendEnable = -1;

	m_dwTextureStageColorWrite = 0xFFFFFFFF;

	m_DepthFunc = D3DCMP_FORCE_DWORD;
	m_iDepthCheckEnable = -1;
	m_iDepthWriteEnable = -1;

	m_fDepthBias = 100000.0f;
	m_fDepthSlope = 1000000.0f;

	m_CullMode  = CM_NO_INIT;
	m_ShadeType = ST_NONE_INIT;
	m_FillMode	= FM_POINT;

	m_AlphaTextFunc = D3DCMP_FORCE_DWORD;
	m_iAlphaTestRef = -1;

	m_iLightEnable = -1;
}

bool ioRenderStateDesc::IsSameStream( UINT uStream, ioVertexBufferInstance *pInstance )
{
	if( m_VertexStream[uStream].iOffsetBytes != pInstance->GetFullOffsetBytes() ||
		m_VertexStream[uStream].pVtxBuf != pInstance->GetD3DBuffer() )
		return false;

	return true;
}

void ioRenderStateDesc::SetStreamDesc( UINT uStream, ioVertexBufferInstance *pInstance )
{
	if( pInstance )
	{
		m_VertexStream[uStream].iOffsetBytes = pInstance->GetFullOffsetBytes();
		m_VertexStream[uStream].pVtxBuf = pInstance->GetD3DBuffer();
	}
	else
	{
		m_VertexStream[uStream].iOffsetBytes = 0;
		m_VertexStream[uStream].pVtxBuf = NULL;
	}
}

void ioRenderStateDesc::ClearStreamDesc()
{
	memset( m_VertexStream, -1, sizeof(VertexStreamDesc) * MAX_STREAM_LAYERS );
	m_iLastVertexStreamCount = 0;
	
	m_pIndexBuf = NULL;
}
