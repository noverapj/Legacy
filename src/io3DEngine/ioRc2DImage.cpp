

#include "stdafx.h"

#include "ioResource.h"
#include "ioRc2DImage.h"

ioRc2DImage::ioRc2DImage( const ioHashString &name ) : ioResource( name )
{
	m_iWidth  = 0;
	m_iHeight = 0;

	m_pImage = NULL;
}

ioRc2DImage::ioRc2DImage( const ioHashString &name,
						  int iWidth,
						  int iHeight,
						  WORD *pImage ) : ioResource( name )
{
	m_iWidth  = iWidth;
	m_iHeight = iHeight;
	m_pImage  = pImage;
}

ioRc2DImage::~ioRc2DImage()
{
	SAFEDELETEARRAY( m_pImage );
}

void ioRc2DImage::SetImage( WORD *pImage )
{
	SAFEDELETEARRAY( m_pImage );
	m_pImage = pImage;
}