

#ifndef _ioFTFace_h_
#define _ioFTFace_h_

#include "ioFTDefine.h"

class FREETYPE_API ioFTFace
{
public:
	virtual void SetCharSize( int iSize ) = 0;
	virtual void SetDeviceResolution( int iHorz, int iVert ) = 0;	

	virtual void SetWhiteSpaceSize( int iSize ) = 0;

public:
	virtual const GlyphImg* GetGlyphImg( WORD wCode ) = 0;

	virtual int GetGlyphAdvance( WORD wCode ) = 0;
	virtual int GetGlyphImgWidth( WORD wCode ) = 0;
	virtual int GetGlyphImgHeight( WORD wCode ) = 0;

	virtual int GetGlyphBearingX( WORD wCode ) = 0;
	virtual int GetGlyphBearingY( WORD wCode ) = 0;

	virtual int GetMaxGlyphWidth() const = 0;
	virtual int GetMaxGlyphHeight() const = 0;
	virtual int GetMaxBearingY() const = 0;

public:
	virtual const char* GetName() const = 0;
	virtual int GetNumGlyphs() const = 0;
	virtual int GetNumFaces() const = 0;

	virtual ioFTEncode GetEncode() const = 0;
	virtual const char* GetEncodeText() const = 0;

public:
	ioFTFace(){}
	virtual ~ioFTFace(){}
};

#endif