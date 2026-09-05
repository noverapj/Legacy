

#include "stdafx.h"

#include "ioStringConverter.h"

#include "ioUIRenderImage.h"
#include "ioUIRenderFrame.h"

ioUIRenderFrame::ioUIRenderFrame()
{
	for( int i=0 ; i<MAX_FRAME_LOCATION ; i++ )
	{
		m_FrameData[i].Init();
	}

	m_bAutoReSize = true;
	m_RenderedPos = D3DXVECTOR2( 0.0f, 0.0f );
}

ioUIRenderFrame::ioUIRenderFrame( const ioUIRenderFrame &rhs )
: ioUIRenderElement( rhs )
{
	for( int i=0 ; i<MAX_FRAME_LOCATION ; i++ )
	{
		if( rhs.m_FrameData[i].m_Frame )
		{
			m_FrameData[i] = dynamic_cast<ioUIRenderImage*>( rhs.m_FrameData[i].m_Frame->Clone() );
			for(int j = 0;j < MAX_FRAME_SIZE;j++)
			{
				m_FrameData[i].m_FrameSize[j] = rhs.m_FrameData[i].m_FrameSize[j];
			}
		}
		else
		{
			m_FrameData[i].Init();
		}
	}
	m_bAutoReSize = true;
	m_name = rhs.m_name;
}

ioUIRenderFrame::~ioUIRenderFrame()
{
	ClearFrameImages();
}

ioUIRenderElement* ioUIRenderFrame::Clone() const
{
	return new ioUIRenderFrame( *this );
}

ioUIRenderElement::UIElementType ioUIRenderFrame::GetType() const
{
	return UIE_FRAME;
}

void ioUIRenderFrame::Render( float fOffX,
							  float fOffY,
							  UIRenderType eRenderType,
							  TextureFilterOption eOption,
							  bool bNoMagicAlign,
							  UIRenderType eGrayRenderType )
{
	if(!IsShow()) return;

	if ( eRenderType == UI_RENDER_NORMAL ) {
		eRenderType = m_eRenderType;
	}
	if ( eOption == TFO_NEAREST ) {
		eOption = m_eTextureFilterOption;
	}
	if ( eGrayRenderType == UI_RENDER_NORMAL ) {
		eGrayRenderType = m_eGrayRenderType;
	}

	m_RenderedPos = D3DXVECTOR2( fOffX, fOffY );

	for( int i=0 ; i<MAX_FRAME_LOCATION ; i++ )
	{
		if( m_FrameData[i].m_Frame )
		{
			m_FrameData[i].m_Frame->Render( fOffX, fOffY, eRenderType, eOption, bNoMagicAlign, eGrayRenderType );
		}
	}
}

void ioUIRenderFrame::SetRenderRect( const RECT &rcRect )
{
	ioUIRenderElement::SetRenderRect( rcRect );

	AdjustFrameImages();
}

void ioUIRenderFrame::SetScale( float fRate )
{
	ioUIRenderElement::SetScale( fRate );

	AdjustFrameImages();
}

void ioUIRenderFrame::SetColor( DWORD dwColor )
{
	ioUIRenderElement::SetColor( dwColor );
	UpdateFrameImagesColor();
}

void ioUIRenderFrame::SetColor( int iRed, int iGreen, int iBlue )
{
	ioUIRenderElement::SetColor( iRed, iGreen, iBlue );
	UpdateFrameImagesColor();
}

void ioUIRenderFrame::SetAlpha( BYTE iAlpha )
{
	ioUIRenderElement::SetAlpha( iAlpha );
	UpdateFrameImagesAlpha( iAlpha );
}

void ioUIRenderFrame::SetAlphaWithColor( BYTE iAlpha )
{
	ioUIRenderElement::SetAlphaWithColor( iAlpha );
	UpdateFrameImagesColor();
}

void ioUIRenderFrame::UpdateFrameImagesColor()
{
	for( int i=0 ; i<MAX_FRAME_LOCATION ; i++ )
	{
		if( m_FrameData[i].m_Frame )
		{
			m_FrameData[i].m_Frame->SetColor( m_dwColor );
		}
	}
}

void ioUIRenderFrame::UpdateFrameImagesAlpha( int iAlpha )
{
	for( int i=0 ; i<MAX_FRAME_LOCATION ; i++ )
	{
		if( m_FrameData[i].m_Frame )
		{
			m_FrameData[i].m_Frame->SetAlpha( iAlpha );
		}
	}
}

void ioUIRenderFrame::AdjustFrameImages()
{
	float fFrameWidth, fFrameHeight;
	fFrameWidth  = GetWidth()  * m_fXScaleRate;
	fFrameHeight = GetHeight() * m_fYScaleRate;

	float fXPos, fYPos;
	fXPos = GetPosX() + ( GetWidth() - fFrameWidth ) * FLOAT05;
	fYPos = GetPosY() + ( GetHeight() - fFrameHeight ) * FLOAT05;

	float fSizeAdj, fCoordAdj;
	if( m_FrameData[FL_TOP_EDGE].m_Frame )
	{
		fSizeAdj  = EdgeWidth( FL_TOP_LEFT ) + EdgeWidth( FL_TOP_RIGHT );
		fCoordAdj = EdgeWidth( FL_TOP_LEFT );

		ioUIRenderImage *pTopEdge = m_FrameData[FL_TOP_EDGE].m_Frame;
		pTopEdge->SetPosition( fXPos + fCoordAdj, fYPos );
		pTopEdge->SetSize( fFrameWidth - fSizeAdj, pTopEdge->GetHeight() );
		pTopEdge->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	}

	if( m_FrameData[FL_BOTTOM_EDGE].m_Frame )
	{
		fSizeAdj  = EdgeWidth( FL_BOTTOM_LEFT ) + EdgeWidth( FL_BOTTOM_RIGHT );
		fCoordAdj = EdgeWidth( FL_BOTTOM_LEFT );

		ioUIRenderImage *pBottomEdge = m_FrameData[FL_BOTTOM_EDGE].m_Frame;
		pBottomEdge->SetPosition( fXPos + fCoordAdj,
								  fYPos + fFrameHeight - EdgeHeight( FL_BOTTOM_EDGE ) );
		pBottomEdge->SetSize( fFrameWidth - fSizeAdj, pBottomEdge->GetHeight() );
		pBottomEdge->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	}

	if( m_FrameData[FL_LEFT_EDGE].m_Frame )
	{
		fSizeAdj  = EdgeHeight( FL_TOP_LEFT ) + EdgeHeight( FL_BOTTOM_LEFT );
		fCoordAdj = EdgeHeight( FL_TOP_LEFT );

		ioUIRenderImage *pLeftEdge = m_FrameData[FL_LEFT_EDGE].m_Frame;
		pLeftEdge->SetPosition( fXPos, fYPos + fCoordAdj );
		pLeftEdge->SetSize( pLeftEdge->GetWidth(), fFrameHeight - fSizeAdj );
		pLeftEdge->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	}

	if( m_FrameData[FL_RIGHT_EDGE].m_Frame )
	{
		fSizeAdj  = EdgeHeight( FL_TOP_RIGHT ) + EdgeHeight( FL_BOTTOM_RIGHT );
		fCoordAdj = EdgeHeight( FL_TOP_RIGHT );

		ioUIRenderImage *pRightEdge = m_FrameData[FL_RIGHT_EDGE].m_Frame;
		pRightEdge->SetPosition( fXPos + fFrameWidth - EdgeWidth( FL_RIGHT_EDGE ),
								 fYPos + fCoordAdj );
		pRightEdge->SetSize( pRightEdge->GetWidth(), fFrameHeight - fSizeAdj );
		pRightEdge->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	}

	if( m_FrameData[FL_CENTER].m_Frame )
	{
		ioUIRenderImage *pCenter = m_FrameData[FL_CENTER].m_Frame;
		pCenter->SetPosition( fXPos + EdgeWidth(FL_LEFT_EDGE),
							  fYPos + EdgeHeight(FL_TOP_EDGE) );

		pCenter->SetSize( fFrameWidth - EdgeWidth(FL_LEFT_EDGE) - EdgeWidth(FL_RIGHT_EDGE),
						  fFrameHeight - EdgeHeight(FL_TOP_EDGE) - EdgeHeight(FL_BOTTOM_EDGE) );

		pCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		pCenter->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	}

	if( m_FrameData[FL_TOP_LEFT].m_Frame )
	{
		m_FrameData[FL_TOP_LEFT].m_Frame->SetPosition( fXPos, fYPos );
	}

	if( m_FrameData[FL_TOP_RIGHT].m_Frame )
	{
		ioUIRenderImage *pTopRight = m_FrameData[FL_TOP_RIGHT].m_Frame;
		pTopRight->SetPosition( fXPos + fFrameWidth - EdgeWidth( FL_TOP_RIGHT ), fYPos );
	}

	if( m_FrameData[FL_BOTTOM_LEFT].m_Frame )
	{
		ioUIRenderImage *pBottomLeft = m_FrameData[FL_BOTTOM_LEFT].m_Frame;
		pBottomLeft->SetPosition( fXPos, fYPos + fFrameHeight - EdgeHeight( FL_BOTTOM_LEFT ) );
	}

	if( m_FrameData[FL_BOTTOM_RIGHT].m_Frame )
	{
		ioUIRenderImage *pBottomRight = m_FrameData[FL_BOTTOM_RIGHT].m_Frame;
		pBottomRight->SetPosition( fXPos + fFrameWidth - EdgeWidth( FL_BOTTOM_RIGHT ),
								   fYPos + fFrameHeight - EdgeHeight( FL_BOTTOM_RIGHT ) );
	}
}

void ioUIRenderFrame::SetImages( ioUIRenderImage *pTopLeft,
								 ioUIRenderImage *pTopEdge,
								 ioUIRenderImage *pTopRight,
								 ioUIRenderImage *pLeftEdge,
								 ioUIRenderImage *pCenter,
								 ioUIRenderImage *pRightEdge,
								 ioUIRenderImage *pBottomLeft,
								 ioUIRenderImage *pBottomEdge,
								 ioUIRenderImage *pBottomRight )
{
	ClearFrameImages();

	m_FrameData[ FL_TOP_LEFT ]     = pTopLeft;
	m_FrameData[ FL_TOP_EDGE ]     = pTopEdge;
	m_FrameData[ FL_TOP_EDGE ]     = pTopEdge;
	m_FrameData[ FL_TOP_RIGHT ]    = pTopRight;
	m_FrameData[ FL_LEFT_EDGE ]    = pLeftEdge;
	m_FrameData[ FL_CENTER ]       = pCenter;
	m_FrameData[ FL_RIGHT_EDGE ]   = pRightEdge;
	m_FrameData[ FL_BOTTOM_LEFT ]  = pBottomLeft;
	m_FrameData[ FL_BOTTOM_EDGE ]  = pBottomEdge;
	m_FrameData[ FL_BOTTOM_RIGHT ] = pBottomRight;
}

bool ioUIRenderFrame::SetImageForLocation( FrameLocation eLocation, ioUIRenderImage *pImage )
{
	if( COMPARE( eLocation, FL_TOP_LEFT, MAX_FRAME_LOCATION ) )
	{
		m_FrameData[ eLocation ].Clear();
		m_FrameData[ eLocation ] = pImage;
		return true;
	}

	LOG.PrintTimeAndLog( 0, "ioUIRenderFrame::SetImageForLocation - %d Location Overflow", eLocation );
	return false;
}

bool ioUIRenderFrame::SetImageForLocation( const ioHashString &szLocation, ioUIRenderImage *pImage )
{
	FrameLocation eLocation = ConvertFrame( szLocation );

	return SetImageForLocation( eLocation, pImage );
}

bool ioUIRenderFrame::SetImageForSize( const ioHashString &szLocation, const ioHashString &szSize )
{
	if( szSize.IsEmpty() )
		return true;

	FrameData *pFrameData = GetDataForLocation( ConvertFrame( szLocation ) );
	if( !pFrameData )
	{
		LOG.PrintTimeAndLog( 0, "ioUIRenderFrame::SetImageForSize - %s Location Overflow", szLocation.c_str() );
		return false;
	}

	std::string szStringSize( szSize.c_str() );
	const StringVector &vSizeList = ioStringConverter::Split( szStringSize, "." );

	int iSizeCount = 0;
	StringVector::const_iterator iter;
	for( iter=vSizeList.begin() ; iter!=vSizeList.end() ; ++iter, ++iSizeCount )
	{
		if( iSizeCount >= MAX_FRAME_SIZE )
		{
			LOG.PrintTimeAndLog( 0, "ioUIRenderFrame::SetImageForSize - %s Size Overflow", szSize.c_str() );
			break;
		}

		const std::string &szSplitSize = *iter;
		pFrameData->m_FrameSize[iSizeCount] = atoi( szSplitSize.c_str() );
	}

	return true;
}

ioUIRenderFrame::FrameData* ioUIRenderFrame::GetDataForLocation( FrameLocation eLocation )
{
	if( COMPARE( eLocation, FL_TOP_LEFT, MAX_FRAME_LOCATION ) )
		return &m_FrameData[eLocation];

	return NULL;
}

void ioUIRenderFrame::ClearFrameImages()
{
	for( int i=0 ; i<MAX_FRAME_LOCATION ; i++ )
	{
		m_FrameData[i].Clear();
	}
}

float ioUIRenderFrame::EdgeWidth( FrameLocation eLocation )
{
	FrameData *pFrameData = GetDataForLocation( eLocation );
	if( pFrameData )
	{
		if( !pFrameData->m_Frame )
		{
			return 0.0f;
		}
		else if( pFrameData->m_FrameSize[FS_WIDTH] > 0 )
		{
			return pFrameData->m_FrameSize[FS_WIDTH];
		}
		else if( pFrameData->m_Frame )
		{
			return pFrameData->m_Frame->GetWidth();
		}
	}

	return 0.0f;
}

float ioUIRenderFrame::EdgeHeight( FrameLocation eLocation )
{
	FrameData *pFrameData = GetDataForLocation( eLocation );
	if( pFrameData )
	{
		if( !pFrameData->m_Frame )
		{
			return 0.0f;
		}
		else if( pFrameData->m_FrameSize[FS_HEIGHT] > 0 )
		{
			return pFrameData->m_FrameSize[FS_HEIGHT];
		}
		else if( pFrameData->m_Frame )
		{
			return pFrameData->m_Frame->GetHeight();
		}
	}

	return 0.0f;
}

ioUIRenderFrame::FrameLocation ioUIRenderFrame::ConvertFrame( const ioHashString &szType )
{
	if( szType == "TopLeft" )
		return FL_TOP_LEFT;
	else if( szType == "TopEdge" )
		return FL_TOP_EDGE;
	else if( szType == "TopRight" )
		return FL_TOP_RIGHT;
	else if( szType == "LeftEdge" )
		return FL_LEFT_EDGE;
	else if( szType == "Center" )
		return FL_CENTER;
	else if( szType == "RightEdge" )
		return FL_RIGHT_EDGE;
	else if( szType == "BottomLeft" )
		return FL_BOTTOM_LEFT;
	else if( szType == "BottomEdge" )
		return FL_BOTTOM_EDGE;
	else if( szType == "BottomRight" )
		return FL_BOTTOM_RIGHT;

	LOG.PrintTimeAndLog( 0, "ioUIRenderFrame::ConvertFrame - %s Unknown Location",
							szType.c_str() );

	return FL_UNKNOWN_FRAME;
}