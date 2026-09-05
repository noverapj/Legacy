

#ifndef _ioVertexFormat_h_
#define _ioVertexFormat_h_

#define MAX_BLEND_BIPED	4

struct BlendWeight
{
	float	fWeight[MAX_BLEND_BIPED];
	float	fBipedID[MAX_BLEND_BIPED];
};

struct JointWeight 
{
	float fWeight;
	int iJointMatOffset;
	int iNextVertexOffset;
};

struct ioVtxNrm4
{
	D3DXVECTOR4 vPos;
	D3DXVECTOR4 vNrm;
};

struct ioVtxMesh
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR3 vNormal;
	float		u, v;
};

struct ioVtxLightMapMesh
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR3 vNormal;
	float		u0, v0;
	float		u1, v1;
};

struct ioVtxBill
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vCenter;
	D3DXVECTOR3 vNormal;
	float		u, v;	
};

struct ioVtxSkin
{
	D3DXVECTOR3	vPos;
	BlendWeight	Weight;
	D3DXVECTOR3	vNormal;
	float		u, v;
};

struct ioVtxColorSkin
{
	D3DXVECTOR3	vPos;
	BlendWeight	Weight;
	D3DXVECTOR3	vNormal;
	DWORD		dwColor;
	float		u, v;
};

struct ioVtxSkinEff
{
	D3DXVECTOR3 vPos;
	BlendWeight Weight;
	D3DXVECTOR3	vNormal;
	float	u0, v0;
	float	u1, v1;
};

struct ioVtxBillSkin
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR3 vCenter;
	BlendWeight	Weight;
	D3DXVECTOR3	vNormal;
	float		u, v;
};

struct ioVtxMeshNrm
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR3 vNormal;
	float		u, v;
	D3DXVECTOR3 vTangent;
	D3DXVECTOR3 vBiNormal;
};

struct ioVtxLightMapMeshNrm
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR3 vNormal;
	float		u0, v0;
	float		u1, v1;
	D3DXVECTOR3 vTangent;
	D3DXVECTOR3 vBiNormal;
};

struct ioVtxBillNrm
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR3 vCenter;
	D3DXVECTOR3 vNormal;
	float		u, v;
	D3DXVECTOR3 vTangent;
	D3DXVECTOR3 vBiNormal;
};

struct ioVtxSkinNrm
{
	D3DXVECTOR3 vPos;
	BlendWeight	Weight;
	D3DXVECTOR3 vNormal;
	float		u, v;
	D3DXVECTOR3 vTangent;
	D3DXVECTOR3 vBiNormal;
};

struct ioVtxColorSkinNrm
{
	D3DXVECTOR3 vPos;
	BlendWeight	Weight;
	D3DXVECTOR3 vNormal;	
	DWORD		dwColor;
	float		u, v;
	D3DXVECTOR3 vTangent;
	D3DXVECTOR3 vBiNormal;
};

struct ioVtxBillSkinNrm
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vCenter;
	BlendWeight	Weight;
	D3DXVECTOR3 vNormal;
	float		u, v;
	D3DXVECTOR3 vTangent;
	D3DXVECTOR3 vBiNormal;
};

struct ioVtxColor
{
	D3DXVECTOR3 vPos;
	DWORD       dwColor;
};

struct ioVtxColorTex
{
	D3DXVECTOR3 vPos;
	DWORD		dwColor;
	float		u, v;
};

struct ioVtxNrm
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNrm;
};

struct ioVtxTex
{
	D3DXVECTOR3 vPos;
	float		u, v;
};

struct ioVtxRHWColorTex
{
	D3DXVECTOR3 vPos;
	float		fRHW;
	DWORD		dwColor;
	float		u, v;
};

struct ioVtxRHWColorTex2
{
	D3DXVECTOR3 vPos;
	float		fRHW;
	DWORD		dwColor;
	float		u, v;
	float		u2, v2;
};

struct ioVtxRHWColor
{
	D3DXVECTOR3 vPos;
	float		fRHW;
	DWORD		dwColor;
};

struct ioVtxRHWTex
{
	D3DXVECTOR3 vPos;
	float		fRHW;
	float		u, v;
};

struct ioVtxRHW
{
	D3DXVECTOR3 vPos;
	float		fRHW;
};

#endif