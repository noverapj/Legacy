

#ifndef _ioRc2DImage_h_
#define _ioRc2DImage_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioResource;

class __EX ioRc2DImage : public ioResource
{
protected:
	int		m_iWidth;
	int		m_iHeight;
	WORD	*m_pImage;

public:
	void SetWidth( int iWidth ) { m_iWidth = iWidth; }
	void SetHeight( int iHeight ) { m_iHeight = iHeight; }
	void SetImage( WORD *pImage );

	int GetWidth() const { return m_iWidth; }
	int GetHeight() const { return m_iHeight; }
	const WORD* GetImage() const { return m_pImage; }

public:
	ioRc2DImage( const ioHashString &name );
	ioRc2DImage( const ioHashString &name, int iWidth, int iHeight, WORD *pImage );
	virtual ~ioRc2DImage();
};

#endif