

#include "stdafx.h"

#include "ioSubEntity.h"
#include "ioEntity.h"
#include "ioNode.h"
#include "ioSceneNode.h"
#include "ioMaterial.h"
#include "ioMaterialManager.h"
#include "ioRenderOperation.h"
#include "ioSubMesh.h"

ioSubEntity::ioSubEntity( ioEntity *pParent, ioSubMesh *pSubMesh )
{
	m_pParent   = pParent;
	m_pSubMesh  = pSubMesh;
	m_pMaterial = NULL;
}

ioSubEntity::~ioSubEntity()
{
	SAFEDELETE( m_pMaterial );
}

void ioSubEntity::SetMaterialName( const ioHashString &name )
{
	SAFEDELETE( m_pMaterial );

	m_DefaultMaterialName = name;
	m_MaterialName = name;

	m_pMaterial = g_MaterialMgr.CloneMaterial( name );
	if( m_pMaterial )
	{
		m_pMaterial->Load( EP_MANUAL );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioSubEntity::SetMaterialName - %s Not Exit", name.c_str() );
	}
}

void ioSubEntity::ChangeMaterial( const ioHashString &name, bool bSoft )
{
	if( name.IsEmpty() )
		return;

	SAFEDELETE( m_pMaterial );

	int iMaterialCnt = g_MaterialMgr.ParseScript( name.c_str() );
	if( iMaterialCnt > 0 )
	{
		char szMaterial[MAX_PATH];
		wsprintf( szMaterial, "%s/Sub%d", name.c_str(), 1 );

		m_MaterialName = szMaterial;

		m_pMaterial = g_MaterialMgr.CloneMaterial( m_MaterialName );
		if( m_pMaterial )
		{
			m_pMaterial->Load( EP_MANUAL );

			if( bSoft )
				m_pMaterial->ConvertShaderAnimateToStatic();
			else
				m_pMaterial->ConvertShaderStaticToAnimate();
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioSubEntity::ChangeMaterial - %s Not Exit", name.c_str() );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioSubEntity::ChangeMaterial - %s Material Load Fail", name.c_str() );
	}
}

void ioSubEntity::RestoreMaterial( bool bSoft )
{
	if( m_DefaultMaterialName.IsEmpty() )
		return;

	SAFEDELETE( m_pMaterial );

	m_MaterialName = m_DefaultMaterialName;

	m_pMaterial = g_MaterialMgr.CloneMaterial( m_DefaultMaterialName );
	if( m_pMaterial )
	{
		m_pMaterial->Load( EP_MANUAL );
		
		if( bSoft )
			m_pMaterial->ConvertShaderAnimateToStatic();
		else
			m_pMaterial->ConvertShaderStaticToAnimate();
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioSubEntity::RestoreMaterial - %s Not Exit", m_DefaultMaterialName.c_str() );
	}
}

const ioHashString& ioSubEntity::GetMaterialName() const
{
	return m_MaterialName;
}

void ioSubEntity::ConvertShaderStaticToVertexColor()
{
	if( m_pMaterial )
	{
		m_pMaterial->ConvertShaderStaticToVertexColor();
	}
}

void ioSubEntity::ConvertShaderAnimateToStatic()
{
	if( m_pMaterial )
	{
		m_pMaterial->ConvertShaderAnimateToStatic();
	}
}

void ioSubEntity::ConvertShaderStaticToAnimate()
{
	if( m_pMaterial )
	{
		m_pMaterial->ConvertShaderStaticToAnimate();
	}
}

int ioSubEntity::GetMinIndex() const
{
	return m_pSubMesh->m_iMinIndex;
}

int ioSubEntity::GetVertexCount() const
{
	return m_pSubMesh->m_iVertexCount;
}

int ioSubEntity::GetFaceCount() const
{
	return m_pSubMesh->m_iFaceCount;
}

int ioSubEntity::GetIndexStart() const
{
	return m_pSubMesh->m_iIndexStart;
}

ioMaterial* ioSubEntity::GetMaterial() const
{
	return m_pMaterial;
}

float ioSubEntity::GetSquaredViewDepth( const ioCamera *pCamera )
{
	return m_pParent->GetSquaredViewDepth( pCamera );
}

void ioSubEntity::GetRenderOperation( ioRenderOperation &rkRendOp )
{
	rkRendOp.m_pMeshData = m_pParent->GetMeshData();
	rkRendOp.m_PrimitiveType = D3DPT_TRIANGLELIST;

	rkRendOp.m_iMinIndex    = m_pSubMesh->m_iMinIndex;
	rkRendOp.m_iVertexCount = m_pSubMesh->m_iVertexCount;
	rkRendOp.m_iIndexStart  = m_pSubMesh->m_iIndexStart;
	rkRendOp.m_iFaceCount   = m_pSubMesh->m_iFaceCount;

	rkRendOp.m_bUseIndex = true;
}

DWORD ioSubEntity::GetMeshHash() const
{
	return m_pSubMesh->GetMeshHash();
}

const D3DCOLORVALUE& ioSubEntity::GetDiffuseRate() const
{
	return m_pParent->GetDiffuseRate();
}

void ioSubEntity::GetWorldTransform( D3DXMATRIX *pMatWorld ) const
{
	m_pParent->GetWorldTransform( pMatWorld );
}

const D3DXVECTOR4* ioSubEntity::GetBipedList( int &iBipedListCnt ) const
{
	return m_pParent->GetBipedList( iBipedListCnt );
}

bool ioSubEntity::IsTransparent() const
{
	if( m_pMaterial )
		return m_pMaterial->IsTransparent();

	return false;
}

bool ioSubEntity::IsMaterialLoadDone()
{
	if( m_pMaterial )
		return m_pMaterial->IsBestTechLoadDone();

	return false;
}

int ioSubEntity::GetWeightedBipedCnt() const
{
	return m_pSubMesh->GetWeightedBipedCnt();
}

const ioLightList& ioSubEntity::GetLights() const
{
	return m_pParent->GetLights();
}

int ioSubEntity::GetAlphaRate() const
{
	return m_pParent->GetAlphaRate();
}

bool ioSubEntity::IsShadowRecvRender() const
{
	return m_pParent->IsShadowRecvRender();
}

