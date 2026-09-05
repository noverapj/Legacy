

#include "Stdafx.h"

#include "AxisXYZ.h"
#include "ioRenderSystem.h"

AxisXYZ::AxisXYZ()
{
	memset( m_Axis, 0, sizeof(ioVtxColor)*6 );
}

AxisXYZ::~AxisXYZ()
{
}

void AxisXYZ::Create(float fLineLen)
{
	m_Axis[0].dwColor = 0xffff0000;
	m_Axis[1].vPos = D3DXVECTOR3(fLineLen, 0.0f, 0.0f);
	m_Axis[1].dwColor = 0xffff0000;
	
	m_Axis[2].dwColor = 0xff00ff00;
	m_Axis[3].vPos = D3DXVECTOR3(0.0f, fLineLen, 0.0f);
	m_Axis[3].dwColor = 0xff00ff00;

	m_Axis[4].dwColor = 0xff0000ff;
	m_Axis[5].vPos = D3DXVECTOR3(0.0f, 0.0f, fLineLen);
	m_Axis[5].dwColor = 0xff0000ff;
}

void AxisXYZ::DrawAxis()
{
	ioRenderSystem &rkSystem = RenderSystem();
	
	rkSystem.SetModelMatrixIdentity();
	rkSystem.SetFixedFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	rkSystem.DisableShader();
	rkSystem.SetLightEnable( false );
	rkSystem.SetDepthFunction( D3DCMP_LESSEQUAL, true, true );
	rkSystem.DisableAlphaBlend();
	rkSystem.DisableAlphaTest();

	rkSystem.SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_DIFFUSE, D3DTA_DIFFUSE );
	rkSystem.SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_DIFFUSE, D3DTA_DIFFUSE );
	rkSystem.DisableTextureUnitsFrom( 1 );
	rkSystem.ClearStreamDesc();

	rkSystem.GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, 3, m_Axis, sizeof(ioVtxColor) );
}
