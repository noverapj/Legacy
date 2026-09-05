

#ifndef _ioUIRenderImage_h_
#define _ioUIRenderImage_h_

#include "ioUIRenderElement.h"

class ioUIImage;

class __EX ioUIRenderImage : public ioUIRenderElement
{
public:
	enum HorzFormatting
	{
		HF_LEFT_ALIGN,
		HF_RIGHT_ALIGN,
		HF_CENTER,
		HF_STRETCH,
		HF_TILE,
	};
	enum VertFormatting
	{
		VF_TOP_ALIGN,
		VF_BOTTOM_ALIGN,
		VF_CENTER,
		VF_STRETCH,
		VF_TILE,
	};
	enum ReverseFormatting
	{
		RF_NONE_REVERSE,
		RF_HORZ_REVERSE,
		RF_VERT_REVERSE,		
		RF_HORZ_VERT_REVERSE,
	};

	enum RenerNumerFormatting
	{
		RNF_FORWARD,
		RNF_REVERSE,
	};

protected:
	const ioUIImage *m_pImage;

	HorzFormatting m_HorzFormat;
	VertFormatting m_VertFormat;
	ReverseFormatting m_ReverseFormat;
	UIRenderType   m_FixedRenderType;
	ioHashString   m_szFixedRenderType;

	const ioUIRenderImage *m_pAlphaImage;

	ioHashString   m_szFilterOpt;
	TextureFilterOption m_FilterOpt;
	D3DXVECTOR2 m_RenderedPos;

	//-------------------------------------------------
	// Tool을 위한 인터페이스들
protected:	
	bool m_isFromFile;
	std::string m_imagename;
	std::string m_imagesetname;
	std::string m_imagesfilename;
	bool m_isAutoSize;
	bool m_isUseOffset;

public:
	__forceinline void SetAutoSize( bool b ) { m_isAutoSize = b; }
	__forceinline bool IsAutoSize() const { return m_isAutoSize; }
	__forceinline void SetUseOffset( bool b ) { m_isUseOffset = b; }
	__forceinline bool GetUseOffset() const { return m_isUseOffset; }
	__forceinline void SetImageName( const std::string& name ) { m_imagename = name; }
	__forceinline const std::string& GetImageName() const { return m_imagename; }
	__forceinline void SetImageSetName( const std::string& name ) { m_imagesetname = name; }
	__forceinline const std::string& GetImageSetName() const { return m_imagesetname; }
	__forceinline void SetImageFileName( const std::string& filename ) { m_imagesfilename = filename; }
	__forceinline void SetImageFileName( const std::string& setname, const std::string& imgname ) {	m_imagesfilename = setname + "#" + imgname; }
	__forceinline const std::string& GetImageFileName() const { return m_imagesfilename; }

	__forceinline void SetFromFile( bool b ) { m_isFromFile = b; }
	__forceinline bool IsFromFile() const { return m_isFromFile; }
	int GetImageWidth() const;
	int GetImageHeight() const;
	std::string GetHorzFormatting();
	std::string GetVertFormatting();
	std::string GetReverseFormattingStr();
	std::string GetFixedRenderType();	
	//-------------------------------------------------

public:
	virtual void Render( float fOffX,
						 float fOffY,
						 UIRenderType eRenderType = UI_RENDER_NORMAL,
						 TextureFilterOption eOption = TFO_NEAREST,
						 bool bNoMagicAlign = false,
						 UIRenderType eGrayRenderType = UI_RENDER_NORMAL );

	virtual ioUIRenderElement* Clone() const;
	virtual UIElementType GetType() const;

public:
	void RenderWidthCut( float fOffX,
						 float fOffY,
						 int iWidthStart,
						 int iWidthEnd,
						 UIRenderType eRenderType = UI_RENDER_NORMAL,
						 TextureFilterOption eOption = TFO_NEAREST,
						 UIRenderType eGrayRenderType = UI_RENDER_NORMAL );

	void RenderHeightCut( float fOffX,
					      float fOffY,
						  int iHeightStart,
						  int iHeightEnd,
						  UIRenderType eRenderType = UI_RENDER_NORMAL,
						  TextureFilterOption eOption = TFO_NEAREST,
						  UIRenderType eGrayRenderType = UI_RENDER_NORMAL );

	void RenderRect( float fOffX,
					 float fOffY,
					 UIRenderType eRenderType,
					 const RECT &rcRect,
					 TextureFilterOption eOption = TFO_NEAREST,
					 UIRenderType eGrayRenderType = UI_RENDER_NORMAL );

public:
	// Number
	void RenderNum( float fXPos,
					float fYPos,
					int iNum,
					float fGap = 0.0f,
					float fScale = 1.0f,
					UIRenderType eRenderType = UI_RENDER_NORMAL,
					TextureFilterOption eOption = TFO_NEAREST,
					UIRenderType eGrayRenderType = UI_RENDER_NORMAL,
					RenerNumerFormatting eNumberPrintType = RNF_FORWARD );

	// Number+P ( ex : 10P, 10G )
	void RenderNumPoint( float fXPos,
						 float fYPos,
						 int iNum,
						 float fGap = 0.0f,
						 float fScale = 1.0f,
						 UIRenderType eRenderType = UI_RENDER_NORMAL,
						 TextureFilterOption eOption = TFO_NEAREST,
						 UIRenderType eGrayRenderType = UI_RENDER_NORMAL );

	// Sign + Number ( ex : +10, -10 )
	void RenderNumSign( float fXPos,
						float fYPos,
						int iNum,
						float fGap = 0.0f,
						float fScale = 1.0f,
						UIRenderType eRenderType = UI_RENDER_NORMAL,
						TextureFilterOption eOption = TFO_NEAREST,
						UIRenderType eGrayRenderType = UI_RENDER_NORMAL );
	
	void RenderIndex( float fXPos,
					  float fYPos,
					  int iCurIdx,
					  int iMaxIdx,
					  UIRenderType eRenderType = UI_RENDER_NORMAL,
					  TextureFilterOption eOption = TFO_NEAREST,
					  UIRenderType eGrayRenderType = UI_RENDER_NORMAL );

protected:
	void RenderDigit( D3DXVECTOR2 vDrawPos,
					  D3DXVECTOR2 vSize,
					  int iDigit,
					  int iMaxDigit,
					  UIRenderType eRenderType,
					  TextureFilterOption eOption,
					  UIRenderType eGrayRenderType );

	void AddToRenderer( const D3DXVECTOR2 &vPos,
					    const D3DXVECTOR2 &vSize,
					    IDirect3DTexture9 *pTex,
					    const D3DXVECTOR4 &vTexUV,
					    UIRenderType eRenderType,
						TextureFilterOption eOption,
						UIRenderType eGrayRenderType );

public:
	void SetImage( const ioUIImage *pImage );
	void SetAlphaImage( const ioUIRenderImage *pImage );

	void SetHorzFormatting( HorzFormatting eFormat ) { m_HorzFormat = eFormat; }
	void SetHorzFormatting( const ioHashString &szAlign );

	void SetVertFormatting( VertFormatting eFormat ) { m_VertFormat = eFormat; }
	void SetVertFormatting( const ioHashString &szAlign );

	void SetReverseFormatting( ReverseFormatting eFormat ){ m_ReverseFormat = eFormat; }
	void SetReverseFormatting( const ioHashString &szReverse );

	void SetFixedRenderType( UIRenderType eRenderType ){ m_FixedRenderType = eRenderType; }
	void SetFixedRenderType( const ioHashString &szRenderType );
	
	inline const ioUIImage* GetImage() const { return m_pImage; }
	inline HorzFormatting GetHorzFormatting() const { return m_HorzFormat; }
	inline VertFormatting GetVertFormatting() const { return m_VertFormat; }
	inline ReverseFormatting GetReverseFormatting() const { return m_ReverseFormat; } 

	int GetOffsetX() const;
	int GetOffsetY() const;

public:
	int m_exoffsetx;
	int m_exoffsety;
	void SetEXOffsetX( int x ) { m_exoffsetx = x; }
	void SetEXOffsetY( int y ) { m_exoffsety = y; }
	int GetEXOffsetX() const { return m_exoffsetx; }
	int GetEXOffsetY() const { return m_exoffsety; }

protected:
	int GetHorzTileCount() const;
	int GetVertTileCount() const;

	float GetBaseXCoord( const D3DXVECTOR2 &vSize ) const;
	float GetBaseYCoord( const D3DXVECTOR2 &vSize ) const;

	D3DXVECTOR2 GetDestinationSize() const;

public:
	inline const D3DXVECTOR2& GetRenderedPos(){ return m_RenderedPos; }

public:
	IDirect3DTexture9* GetTexture( UIRenderType eType ) const;

	D3DXVECTOR4 GetTexUV() const;
	RECT GetImageRect() const;

public:
	bool FillFileImage( const char *szFileName, const int iFileWidth, const int iFileHeight, OUT D3DXIMAGE_INFO *pSrcInfo = NULL );
	void ForceTouch() const;

	void SetTexFilterOpt( const ioHashString &szFilterOpt );

public:
	ioUIRenderImage();
	ioUIRenderImage( const ioUIImage *pImg );
	ioUIRenderImage( const ioUIRenderImage &rhs );
	virtual ~ioUIRenderImage();
};

inline ioUIRenderImage* ToUIImage( ioUIRenderElement *pElement )
{
	if( !pElement || pElement->GetType() != ioUIRenderElement::UIE_IMAGE )
		return NULL;

	return dynamic_cast< ioUIRenderImage* >( pElement );
}

#endif
