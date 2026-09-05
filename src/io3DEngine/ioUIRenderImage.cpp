

#include "stdafx.h"

#include "ioUIImage.h"
#include "ioUIRenderImage.h"
#include "ioUIRenderer.h"
#include "HelpFunc.h"
#include "ioStringConverter.h"

#include "ioMemoryPool.h"

class ioImageQuad : public IUIQuad, public ioPoolObject< ioImageQuad, 512 >
{
public:
	D3DXVECTOR2 m_vDrawPos;
	D3DXVECTOR2 m_vDrawSize;
	D3DXVECTOR4 m_vTexUV;

	IDirect3DTexture9 *m_pTex;
	UIRenderType m_RenderType;
	TextureFilterOption m_Filter;
	DWORD m_dwColor;

	const ioUIRenderImage *m_pAlphaImage;

public:
	virtual void AddInfo( ioQuadRenderArray *pQuadArray );
	virtual int GetQuadCount() const { return 1; }

public:
	ioImageQuad(){}
	virtual ~ioImageQuad(){}
};

void ioImageQuad::AddInfo( ioQuadRenderArray *pQuadArray )
{
	pQuadArray->AddRenderQuadInfo( m_vDrawPos,
								   m_vDrawSize,
								   m_pTex,
								   m_vTexUV,
								   m_RenderType,
								   m_Filter,
								   m_dwColor,
								   m_pAlphaImage );
}

//---------------------------------------------------------------------

ioUIRenderImage::ioUIRenderImage()
{
	m_pImage = NULL;
	
	m_HorzFormat = HF_LEFT_ALIGN;
	m_VertFormat = VF_TOP_ALIGN;
	m_ReverseFormat = RF_NONE_REVERSE;
	m_FixedRenderType = UI_RENDER_NORMAL;
	
	m_pAlphaImage = NULL;

	m_isFromFile = true;
	m_isAutoSize = false;
	m_isUseOffset = true;

	m_exoffsetx = 0;
	m_exoffsety = 0;

	m_RenderedPos = D3DXVECTOR2( 0.0f, 0.0f );
}

ioUIRenderImage::ioUIRenderImage( const ioUIImage *pImg )
{
	m_RenderRect.left   = 0;
	m_RenderRect.top    = 0;
	m_RenderRect.right  = pImg->GetWidth();
	m_RenderRect.bottom = pImg->GetHeight();

	m_pImage = pImg;
	m_pImage->NotifyAttached();

	m_HorzFormat = HF_LEFT_ALIGN;
	m_VertFormat = VF_TOP_ALIGN;
	m_ReverseFormat = RF_NONE_REVERSE;
	m_FixedRenderType = UI_RENDER_NORMAL;

	m_pAlphaImage = NULL;

	m_isFromFile = true;
	m_isAutoSize = false;
	m_isUseOffset = true;

	m_exoffsetx = 0;
	m_exoffsety = 0;
}

ioUIRenderImage::ioUIRenderImage( const ioUIRenderImage &rhs )
: ioUIRenderElement( rhs )
{
	m_pImage = rhs.m_pImage;
	m_pImage->NotifyAttached();

	m_HorzFormat = rhs.m_HorzFormat;
	m_VertFormat = rhs.m_VertFormat;
	m_ReverseFormat = rhs.m_ReverseFormat;
	m_FixedRenderType = rhs.m_FixedRenderType;
	m_szFixedRenderType = rhs.m_szFixedRenderType;

	m_pAlphaImage = rhs.m_pAlphaImage;

	m_isFromFile = rhs.m_isFromFile;
	m_isAutoSize = rhs.m_isAutoSize;
	m_isUseOffset = rhs.m_isUseOffset;

	m_imagename = rhs.m_imagename;
	m_imagesetname = rhs.m_imagesetname;
	m_imagesfilename = rhs.m_imagesfilename;

	m_exoffsetx = rhs.m_exoffsetx;
	m_exoffsety = rhs.m_exoffsety;

	m_szFilterOpt = rhs.m_szFilterOpt;
}

ioUIRenderImage::~ioUIRenderImage()
{
	if( m_pImage )
	{
		m_pImage->NotifyDetached();
	}
}

void ioUIRenderImage::Render( float fOffX,
							  float fOffY,
							  UIRenderType eRenderType,
							  TextureFilterOption eOption,
							  bool bNoMagicAlign,
							  UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	IDirect3DTexture9 *pTex = GetTexture( eRenderType );
	if( !pTex )	return;

	if( eRenderType == UI_RENDER_GRAY || eRenderType == UI_RENDER_GRAY_LIGHT )
		eRenderType = eGrayRenderType;

	if ( eRenderType == UI_RENDER_NORMAL ) {
		eRenderType = m_eRenderType;
	}
	if ( eOption == TFO_NEAREST ) {
		eOption = m_eTextureFilterOption;
	}
	if ( eGrayRenderType == UI_RENDER_NORMAL ) {
		eGrayRenderType = m_eGrayRenderType;
	}

	D3DXVECTOR2 vSize = GetDestinationSize();

	float fBaseX = GetBaseXCoord( vSize ) + (float)GetPosX() + fOffX - GetOffsetX();
	if( GetOffsetX() == 0 )
	{
		fBaseX += ( vSize.x - vSize.x * m_fXScaleRate ) * FLOAT05;
	}
	else
	{
		fBaseX += GetOffsetX() * ( 1.0f - m_fXScaleRate );
	}

	float fBaseY = GetBaseYCoord( vSize ) + (float)GetPosY() + fOffY - GetOffsetY();
	if( GetOffsetY() == 0 )
	{
		fBaseY += ( vSize.y - vSize.y * m_fYScaleRate ) * FLOAT05;
	}
	else
	{
		fBaseY += GetOffsetY() * ( 1.0f - m_fYScaleRate );
	}
	
	vSize.x *= m_fXScaleRate;
	vSize.y *= m_fYScaleRate;

	if( !bNoMagicAlign )
	{
		fBaseX  = PixelAligned( fBaseX );
		fBaseY  = PixelAligned( fBaseY );
	}

	D3DXVECTOR2 vDrawPos;
	vDrawPos.y = fBaseY;
	m_RenderedPos.x = fBaseX;
	m_RenderedPos.y = fBaseY;

	D3DXVECTOR4 vTexUV = GetTexUV();
	int iHorzTiles = GetHorzTileCount();
	int iVertTiles = GetVertTileCount();

	for( int iRow=0 ; iRow<iVertTiles ; iRow++ )
	{
		vDrawPos.x = fBaseX;
		for( int iCol=0 ; iCol<iHorzTiles ; iCol++ )
		{
			AddToRenderer( vDrawPos, vSize, pTex, vTexUV, eRenderType, eOption, eGrayRenderType );

			vDrawPos.x += vSize.x;
		}

		vDrawPos.y += vSize.y;
	}
}

ioUIRenderElement* ioUIRenderImage::Clone() const
{
	return new ioUIRenderImage( *this );
}

ioUIRenderElement::UIElementType ioUIRenderImage::GetType() const
{
	return UIE_IMAGE;
}

void ioUIRenderImage::RenderWidthCut( float fOffX,
									  float fOffY,
									  int iWidthStart,
									  int iWidthEnd,
									  UIRenderType eRenderType,
									  TextureFilterOption eOption,
									  UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	RECT rcRect;
	SetRect( &rcRect, iWidthStart, 0, iWidthEnd, GetHeight() );

	RenderRect( fOffX, fOffY, eRenderType, rcRect, eOption, eGrayRenderType );
}

void ioUIRenderImage::RenderHeightCut( float fOffX,
									   float fOffY,
									   int iHeightStart,
									   int iHeightEnd,
									   UIRenderType eRenderType,
									   TextureFilterOption eOption,
									   UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	RECT rcRect;
	SetRect( &rcRect, 0, iHeightStart, GetWidth(), iHeightEnd );

	RenderRect( fOffX, fOffY, eRenderType, rcRect, eOption, eGrayRenderType );
}

void ioUIRenderImage::RenderRect( float fOffX,
								  float fOffY,
								  UIRenderType eRenderType,
								  const RECT &rcRect,
								  TextureFilterOption eOption,
								  UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	IDirect3DTexture9 *pTex = GetTexture( eRenderType );
	if( !pTex )
		return;

	if( eRenderType == UI_RENDER_GRAY || eRenderType == UI_RENDER_GRAY_LIGHT )
		eRenderType = eGrayRenderType;

	D3DXVECTOR2 vDrawPos;
	vDrawPos.x = fOffX + GetPosX() + rcRect.left - GetOffsetX();
	vDrawPos.y = fOffY + GetPosY() + rcRect.top  - GetOffsetY();

	D3DXVECTOR2 vSize;
	vSize.x = rcRect.right - rcRect.left;
	vSize.y = rcRect.bottom - rcRect.top;

	switch( m_HorzFormat )
	{
	case HF_CENTER:
		vDrawPos.x -= vSize.x * FLOAT05;
		break;
	case HF_RIGHT_ALIGN:
		vDrawPos.x -= vSize.x;
		break;
	}

	switch( m_VertFormat )
	{
	case VF_CENTER:
		vDrawPos.y -= vSize.y * FLOAT05;
		break;
	case VF_BOTTOM_ALIGN:
		vDrawPos.y -= vSize.y;
		break;
	}

	if( GetOffsetX() == 0 )
	{
		vDrawPos.x += ( vSize.x - vSize.x * m_fXScaleRate ) * FLOAT05;
	}
	else
	{
		float fScaleRate = ( 1.0f - m_fXScaleRate );
		vDrawPos.x += GetOffsetX() * fScaleRate;
		vDrawPos.x -= rcRect.left * fScaleRate;
	}

	if( GetOffsetY() == 0 )
	{
		vDrawPos.y += ( vSize.y - vSize.y * m_fYScaleRate ) * FLOAT05;
	}
	else
	{
		float fScaleRate = ( 1.0f - m_fYScaleRate );
		vDrawPos.y += GetOffsetY() * fScaleRate;
		vDrawPos.y -= rcRect.top * fScaleRate;
	}

	vSize.x *= m_fXScaleRate;
	vSize.y *= m_fYScaleRate;
	vDrawPos.x = PixelAligned( vDrawPos.x );
	vDrawPos.y = PixelAligned( vDrawPos.y );

	m_RenderedPos = vDrawPos;

	float fScaleRate = 0.0f;
	D3DXVECTOR4 vTexUV;
	fScaleRate = (float)rcRect.left/(float)m_pImage->GetWidth();
	vTexUV.x = max( 0.0f, min( 1.0f, fScaleRate ) );
	
	fScaleRate = (float)rcRect.top/(float)m_pImage->GetHeight();
	vTexUV.y = max( 0.0f, min( 1.0f, fScaleRate ) );

	fScaleRate = (float)rcRect.right/(float)m_pImage->GetWidth();
	vTexUV.z = max( 0.0f, min( 1.0f, fScaleRate ) );

	fScaleRate = (float)rcRect.bottom/(float)m_pImage->GetHeight();
	vTexUV.w = max( 0.0f, min( 1.0f, fScaleRate ) );

	D3DXVECTOR4 vSrcUV = GetTexUV();
	vTexUV.x = vSrcUV.x + ( vSrcUV.z - vSrcUV.x ) * vTexUV.x;
	vTexUV.y = vSrcUV.y + ( vSrcUV.w - vSrcUV.y ) * vTexUV.y;
	vTexUV.z = vSrcUV.x + ( vSrcUV.z - vSrcUV.x ) * vTexUV.z;
	vTexUV.w = vSrcUV.y + ( vSrcUV.w - vSrcUV.y ) * vTexUV.w;	

	AddToRenderer( vDrawPos, vSize, pTex, vTexUV, eRenderType, eOption, eGrayRenderType );
}

// Number
void ioUIRenderImage::RenderNum( float fXPos,
								 float fYPos,
								 int iNum,
								 float fGap,
								 float fScale,
								 UIRenderType eRenderType,
								 TextureFilterOption eOption,
								 UIRenderType eGrayRenderType,
								 RenerNumerFormatting eNumberPrintType )
{
	if(!IsShow()) return;

	D3DXVECTOR2 vSize;
	vSize.x  = (float)GetWidth()/10.0f * fScale;
	vSize.y = (float)GetHeight() * fScale;

	int iCount = GetNumberCount( iNum );

	D3DXVECTOR2 vDrawPos;
	vDrawPos.x = fXPos + GetPosX() - GetOffsetX();
	switch( m_HorzFormat )
	{
	case HF_CENTER:
		vDrawPos.x -= ( ( vSize.x * iCount ) * FLOAT05 );
		vDrawPos.x -= ( fGap * iCount ) * FLOAT05;
		break;
	case HF_RIGHT_ALIGN:
		vDrawPos.x -= vSize.x * iCount;
		break;
	}

	vDrawPos.y = fYPos + GetPosY() - GetOffsetY();
	switch( m_VertFormat )
	{
	case VF_CENTER:
		vDrawPos.y -= vSize.y * FLOAT05;
		break;
	case VF_BOTTOM_ALIGN:
		vDrawPos.y -= vSize.y;
		break;
	}

	m_RenderedPos = vDrawPos;

	switch( eNumberPrintType )
	{
	case RNF_FORWARD:
		{
			int iDigit = 0;
			for( int i=iCount-1 ; i>=0 ; i-- )
			{
				iDigit = GetNumberPos( iNum, i );
				RenderDigit( vDrawPos, vSize, iDigit, 10, eRenderType, eOption, eGrayRenderType );

				vDrawPos.x += vSize.x + fGap;
			}
		}
		break;
	case RNF_REVERSE:
		{			
			int iDigit = 0;
			vDrawPos.x += vSize.x * iCount + fGap * iCount;
			for( int i = 0; i < iCount ; i++ )
			{
				vDrawPos.x -= vSize.x + fGap;
				iDigit = GetNumberPos( iNum, i );
				RenderDigit( vDrawPos, vSize, iDigit, 10, eRenderType, eOption, eGrayRenderType );				
			}			
		}
		break;
	}
}

// Number+P ( ex : 10P, 10G )
void ioUIRenderImage::RenderNumPoint( float fXPos,
									  float fYPos,
									  int iNum,
									  float fGap,
									  float fScale,
									  UIRenderType eRenderType,
									  TextureFilterOption eOption,
									  UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	D3DXVECTOR2 vSize;
	vSize.x  = (float)GetWidth()/11.0f * fScale;
	vSize.y = (float)GetHeight() * fScale;

	int iCount = GetNumberCount( iNum ) + 1;	// + Point

	D3DXVECTOR2 vDrawPos;
	vDrawPos.x = fXPos + GetPosX() - GetOffsetX();
	switch( m_HorzFormat )
	{
	case HF_CENTER:
		vDrawPos.x -= ( vSize.x * iCount ) * FLOAT05;
		vDrawPos.x -= ( fGap * iCount ) * FLOAT05;
		break;
	case HF_RIGHT_ALIGN:
		vDrawPos.x -= vSize.x * iCount;
		break;
	}

	vDrawPos.y = fYPos + GetPosY() - GetOffsetY();
	switch( m_VertFormat )
	{
	case VF_CENTER:
		vDrawPos.y -= vSize.y * FLOAT05;
		break;
	case VF_BOTTOM_ALIGN:
		vDrawPos.y -= vSize.y;
		break;
	}

	m_RenderedPos = vDrawPos;

	int iDigit = 0;
	for( int i=iCount-2 ; i>=0 ; i-- )	// - Point
	{
		iDigit = GetNumberPos( iNum, i );
		RenderDigit( vDrawPos, vSize, iDigit, 11, eRenderType, eOption, eGrayRenderType );

		vDrawPos.x += vSize.x + fGap;
	}

	RenderDigit( vDrawPos, vSize, 10, 11, eRenderType, eOption, eGrayRenderType );
}

// Sign + Number ( ex : +10, -10 )
void ioUIRenderImage::RenderNumSign( float fXPos,
									 float fYPos,
									 int iNum,
									 float fGap,
									 float fScale,
									 UIRenderType eRenderType,
									 TextureFilterOption eOption,
									 UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	D3DXVECTOR2 vSize;
	vSize.x  = (float)GetWidth()/12.0f * fScale;
	vSize.y = (float)GetHeight() * fScale;

	int iCount = GetNumberCount( iNum ) + 1;	// + Sign

	D3DXVECTOR2 vDrawPos;
	vDrawPos.x = fXPos + GetPosX() - GetOffsetX();
	switch( m_HorzFormat )
	{
	case HF_CENTER:
		vDrawPos.x -= ( vSize.x * iCount ) * FLOAT05;
		vDrawPos.x -= ( fGap * iCount ) * FLOAT05;
		break;
	case HF_RIGHT_ALIGN:
		vDrawPos.x -= vSize.x * iCount;
		break;
	}

	vDrawPos.y = fYPos + GetPosY() - GetOffsetY();
	switch( m_VertFormat )
	{
	case VF_CENTER:
		vDrawPos.y -= vSize.y * FLOAT05;
		break;
	case VF_BOTTOM_ALIGN:
		vDrawPos.y -= vSize.y;
		break;
	}

	if( iNum >= 0 )
		RenderDigit( vDrawPos, vSize, 10, 12, eRenderType, eOption, eGrayRenderType );
	else
		RenderDigit( vDrawPos, vSize, 11, 12, eRenderType, eOption, eGrayRenderType );

	vDrawPos.x += vSize.x;

	int iDigit = 0;
	for( int i=iCount-2 ; i>=0 ; i-- )	// - Sign
	{
		iDigit = GetNumberPos( iNum, i );
		RenderDigit( vDrawPos, vSize, iDigit, 12, eRenderType, eOption, eGrayRenderType );

		vDrawPos.x += vSize.x + fGap;
	}	
}

void ioUIRenderImage::AddToRenderer( const D3DXVECTOR2 &vPos,
									 const D3DXVECTOR2 &vSize,
									 IDirect3DTexture9 *pTex,
									 const D3DXVECTOR4 &vTexUV,
									 UIRenderType eRenderType,
									 TextureFilterOption eOption,
									 UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	ioImageQuad *pQuad = new ioImageQuad;
	if( pQuad )
	{
		pQuad->m_vDrawPos    = vPos + D3DXVECTOR2( m_exoffsetx, m_exoffsety );
		pQuad->m_vDrawSize   = vSize;
		pQuad->m_vTexUV      = vTexUV;

		pQuad->m_pTex        = pTex;
		pQuad->m_Filter      = eOption;
		pQuad->m_dwColor     = m_dwColor;
		pQuad->m_pAlphaImage = m_pAlphaImage;

		if( m_szFixedRenderType.IsEmpty() )
			pQuad->m_RenderType  = eRenderType;
		else
			pQuad->m_RenderType  = m_FixedRenderType;

		if( m_szFilterOpt.IsEmpty() )
			pQuad->m_Filter      = eOption;
		else
			pQuad->m_Filter      = m_FilterOpt;

		g_UIRenderer.AddUIQuad( pQuad );
	}
}

void ioUIRenderImage::RenderDigit( D3DXVECTOR2 vDrawPos,
								   D3DXVECTOR2 vSize,
								   int iDigit,
								   int iMaxDigit,
								   UIRenderType eRenderType,
								   TextureFilterOption eOption,
								   UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	IDirect3DTexture9 *pTex = GetTexture( eRenderType );
	if( !pTex )	return;

	if( eRenderType == UI_RENDER_GRAY || eRenderType == UI_RENDER_GRAY_LIGHT )
		eRenderType = eGrayRenderType;

	vDrawPos.x = PixelAligned( vDrawPos.x );
	vDrawPos.y = PixelAligned( vDrawPos.y );

	D3DXVECTOR4 vSrcUV = GetTexUV();

	D3DXVECTOR4 vTexUV;
	vTexUV.x = vSrcUV.x + ( vSrcUV.z - vSrcUV.x ) * (float)iDigit/(float)iMaxDigit;
	vTexUV.y = vSrcUV.y;
	vTexUV.z = vSrcUV.x + ( vSrcUV.z - vSrcUV.x ) * (float)(iDigit+1)/(float)iMaxDigit;
	vTexUV.w = vSrcUV.w;

	AddToRenderer( vDrawPos, vSize, pTex, vTexUV, eRenderType, eOption, eGrayRenderType );
}

void ioUIRenderImage::RenderIndex( float fXPos,
								   float fYPos,
								   int iCurIdx,
								   int iMaxIdx,
								   UIRenderType eRenderType,
								   TextureFilterOption eOption,
								   UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	D3DXVECTOR2 vSize;
	vSize.x = GetWidth() / (float)iMaxIdx;
	vSize.y = GetHeight();

	D3DXVECTOR2 vDrawPos;
	vDrawPos.x = fXPos + GetPosX() - GetOffsetX();
	switch( m_HorzFormat )
	{
	case HF_CENTER:
		vDrawPos.x -= vSize.x * FLOAT05;
		break;
	case HF_RIGHT_ALIGN:
		vDrawPos.x -= vSize.x;
		break;
	}

	vDrawPos.y = fYPos + GetPosY() - GetOffsetX();
	switch( m_VertFormat )
	{
	case VF_CENTER:
		vDrawPos.y -= vSize.y * FLOAT05;
		break;
	case VF_BOTTOM_ALIGN:
		vDrawPos.y -= vSize.y;
		break;
	}

	RenderDigit( vDrawPos, vSize, iCurIdx, iMaxIdx, eRenderType, eOption, eGrayRenderType );
}

void ioUIRenderImage::SetImage( const ioUIImage *pImage )
{
	if( m_pImage )
	{
		m_pImage->NotifyDetached();
		m_pImage = NULL;
	}

	if( pImage )
	{
		m_pImage = pImage;
		m_pImage->NotifyAttached();
	}
}

void ioUIRenderImage::SetAlphaImage( const ioUIRenderImage *pImage )
{
	m_pAlphaImage = pImage;
}

int ioUIRenderImage::GetImageWidth() const
{
	if ( m_pImage ) {
		return m_pImage->GetWidth();
	}
	return 0;
}

int ioUIRenderImage::GetImageHeight() const
{
	if ( m_pImage ) {
		return m_pImage->GetHeight();
	}
	return 0;
}

std::string ioUIRenderImage::GetHorzFormatting()
{
	switch ( m_HorzFormat )
	{
	case HF_LEFT_ALIGN:
		return "Left";
	case HF_RIGHT_ALIGN:
		return "Right";
	case HF_CENTER:
		return "Center";
	case HF_STRETCH:
		return "Stretch";
	case HF_TILE:
		return "Tile";
	}
	return "Left";
}

void ioUIRenderImage::SetHorzFormatting( const ioHashString &szAlign )
{
	if( szAlign.IsEmpty() )
		return;

	HorzFormatting eFormat = HF_LEFT_ALIGN;
	if( szAlign == "Left" )
		eFormat = HF_LEFT_ALIGN;
	else if( szAlign == "Right" )
		eFormat = HF_RIGHT_ALIGN;
	else if( szAlign == "Center" )
		eFormat = HF_CENTER;
	else if( szAlign == "Stretch" )
		eFormat = HF_STRETCH;
	else if( szAlign == "Tile" )
		eFormat = HF_TILE;

	SetHorzFormatting( eFormat );
}

std::string ioUIRenderImage::GetVertFormatting()
{
	switch ( m_VertFormat )
	{
	case VF_TOP_ALIGN:
		return "Top";
	case VF_BOTTOM_ALIGN:
		return "Bottom";
	case VF_CENTER:
		return "Center";
	case VF_STRETCH:
		return "Stretch";
	case VF_TILE:
		return "Tile";
	}
	return "Top";
}

void ioUIRenderImage::SetVertFormatting( const ioHashString &szAlign )
{
	if( szAlign.IsEmpty() )
		return;

	VertFormatting eFormat = VF_TOP_ALIGN;
	if( szAlign == "Top" )
		eFormat = VF_TOP_ALIGN;
	else if( szAlign == "Bottom" )
		eFormat = VF_BOTTOM_ALIGN;
	else if( szAlign == "Center" )
		eFormat = VF_CENTER;
	else if( szAlign == "Stretch" )
		eFormat = VF_STRETCH;
	else if( szAlign == "Tile" )
		eFormat = VF_TILE;

	SetVertFormatting( eFormat );
}

std::string ioUIRenderImage::GetReverseFormattingStr()
{
	switch ( m_ReverseFormat )
	{
	case RF_HORZ_REVERSE:
		return "Horz";
	case RF_VERT_REVERSE:
		return "Vert";
	case RF_HORZ_VERT_REVERSE:
		return "HorzVert";
	}
	return "";
}

void ioUIRenderImage::SetReverseFormatting( const ioHashString &szReverse )
{
	if( szReverse.IsEmpty() )
		return;

	ReverseFormatting eFormat = RF_NONE_REVERSE;
	if( szReverse == "Horz" )
		eFormat = RF_HORZ_REVERSE;
	else if( szReverse == "Vert" )
		eFormat = RF_VERT_REVERSE;
	else if( szReverse == "HorzVert" )
		eFormat = RF_HORZ_VERT_REVERSE;

	SetReverseFormatting( eFormat );
}

std::string ioUIRenderImage::GetFixedRenderType()
{
	return m_szFixedRenderType.c_str();
}

void ioUIRenderImage::SetFixedRenderType( const ioHashString &szRenderType )
{
	if( szRenderType.IsEmpty() )
		return;

	m_szFixedRenderType = szRenderType;
	m_FixedRenderType   = ioStringConverter::ParseRenderType( m_szFixedRenderType );
}

int ioUIRenderImage::GetHorzTileCount() const
{
	if( m_HorzFormat != HF_TILE )
		return 1;

	return ( GetWidth() + ( m_pImage->GetWidth() - 1 ) ) / m_pImage->GetWidth();
}

int ioUIRenderImage::GetVertTileCount() const
{
	if( m_VertFormat != VF_TILE )
		return 1;

	return ( GetHeight() + ( m_pImage->GetHeight() - 1 ) ) / m_pImage->GetHeight();
}

float ioUIRenderImage::GetBaseXCoord( const D3DXVECTOR2 &vSize ) const
{
	switch( m_HorzFormat )
	{
	case HF_LEFT_ALIGN:
	case HF_STRETCH:
	case HF_TILE:
		return 0.0f;
	case HF_CENTER:
		return PixelAligned( -vSize.x * FLOAT05 );
	case HF_RIGHT_ALIGN:
		return -vSize.x;
	}

	return 0.0f;
}

float ioUIRenderImage::GetBaseYCoord( const D3DXVECTOR2 &vSize ) const
{
	switch( m_VertFormat )
	{
	case VF_TOP_ALIGN:
	case VF_STRETCH:
	case VF_TILE:
		return 0.0f;
	case VF_CENTER:
		return PixelAligned( -vSize.y * FLOAT05 );
	case VF_BOTTOM_ALIGN:
		return -vSize.y;
	}

	return 0.0f;
}

D3DXVECTOR4 ioUIRenderImage::GetTexUV() const
{
	D3DXVECTOR4 vSrcUV  = m_pImage->GetTexUV();
	D3DXVECTOR4 vDestUV = vSrcUV;
	switch( m_ReverseFormat )
	{
	case RF_HORZ_REVERSE:
		{			
			vDestUV.x = vSrcUV.z,vDestUV.y = vSrcUV.y;
			vDestUV.z = vSrcUV.x,vDestUV.w = vSrcUV.w;			
		}
		break;
	case RF_VERT_REVERSE:
		{
			vDestUV.x = vSrcUV.x,vDestUV.y = vSrcUV.w;
			vDestUV.z = vSrcUV.z,vDestUV.w = vSrcUV.y;				
		}
		break;
	case RF_HORZ_VERT_REVERSE:
		{
			vDestUV.x = vSrcUV.z,vDestUV.y = vSrcUV.w;
			vDestUV.z = vSrcUV.x,vDestUV.w = vSrcUV.y;			
		}
		break;
	}

	return vDestUV;
}

RECT ioUIRenderImage::GetImageRect() const
{
	return m_pImage->GetAreaRect();
}

D3DXVECTOR2 ioUIRenderImage::GetDestinationSize() const
{
	D3DXVECTOR2 vSize;

	if( m_HorzFormat == HF_STRETCH )
		vSize.x = GetWidth();
	else
		vSize.x = m_pImage->GetWidth();

	if( m_VertFormat == VF_STRETCH )
		vSize.y = GetHeight();
	else
		vSize.y = m_pImage->GetHeight();

	if ( m_isAutoSize )
	{
		vSize.x = GetWidth();
		vSize.y = GetHeight();
	}

	return vSize;
}

IDirect3DTexture9* ioUIRenderImage::GetTexture( UIRenderType eType ) const
{
	if ( !m_pImage ) 
		return 0;

	if( eType == UI_RENDER_GRAY )
		return m_pImage->GetGrayTexture();
	else if( eType == UI_RENDER_GRAY_LIGHT )
		return m_pImage->GetGrayLightTexture();

	return m_pImage->GetD3DTexture();;
}


int ioUIRenderImage::GetOffsetX() const
{
	if ( m_isUseOffset ) {
		return m_pImage->GetOffsetX();
	}
	return 0;
}

int ioUIRenderImage::GetOffsetY() const
{
	if ( m_isUseOffset ) {
		return m_pImage->GetOffsetY();
	}
	return 0;
}

bool ioUIRenderImage::FillFileImage( const char *szFileName,
									 const int iFileWidth,
									 const int iFileHeight,
									 OUT D3DXIMAGE_INFO *pSrcInfo /*= NULL*/ )
{
	if( !m_pImage )
		return false;

	return m_pImage->FillFileImage( szFileName, iFileWidth, iFileHeight, pSrcInfo );
}

void ioUIRenderImage::ForceTouch() const
{
	m_pImage->ForceTouch();
}

void ioUIRenderImage::SetTexFilterOpt( const ioHashString &szFilterOpt )
{
	if( szFilterOpt.IsEmpty() )
		return;

	m_szFilterOpt = szFilterOpt;
	m_FilterOpt   = ioStringConverter::ParseTextureFilterOption( m_szFilterOpt );
}