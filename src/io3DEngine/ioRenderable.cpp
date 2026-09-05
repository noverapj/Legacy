

#include "stdafx.h"

#include "ioRenderable.h"
#include "ioMaterial.h"

ioRenderable::ioRenderable()
{
}

ioRenderable::~ioRenderable()
{
}

void ioRenderable::SetCustomParameter( const ioShaderCustomArg &rkArg )
{
	m_CustomParameters.SetCustomArg( rkArg );
}

const ioShaderCustomArg* ioRenderable::GetCustomParameter( const ioHashString &kParamName )
{
	return m_CustomParameters.GetValue( kParamName );
}

DWORD ioRenderable::GetMeshHash() const
{
	return INT_MAX;
}

ioTechnique* ioRenderable::GetTechnique() const
{
	ioMaterial *pMtl = GetMaterial();
	if( pMtl )
		return pMtl->GetBestTechnique();

	return NULL;
}

const D3DCOLORVALUE& ioRenderable::GetDiffuseRate() const
{
	static D3DXCOLOR sDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
	return sDiffuse;
}

const ioLightList& ioRenderable::GetLights() const
{
	static ioLightList s_List;
	return s_List;
}

const D3DXVECTOR4* ioRenderable::GetBipedList( int &iBipedListCnt ) const
{
	iBipedListCnt = 0;
	return NULL;
}

int ioRenderable::GetWeightedBipedCnt() const
{
	return 0;
}

int ioRenderable::GetAlphaRate() const
{
	return MAX_ALPHA_RATE;
}

bool ioRenderable::IsShadowRecvRender() const
{
	return false;
}

