

#ifndef _ioRcFont_h_
#define _ioRcFont_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

// www.gamecode.org by noerror

class ioResource;
class ioFont;

typedef struct tagChInfo
{
	int offset;
	BYTE w, h;
}ChInfo;

class __EX ioRcFont : public ioResource
{
private:

friend class ioFont;

private:
	int m_iMaxHeight;
	int m_iCodeCnt;

	ChInfo *m_pChInfo;
	WORD *m_pCode;
	BYTE *m_pImage;
	WORD *m_pPalette;

public:
	void SetCode( int iCnt, WORD *pCode );
	void SetImage( BYTE *pImage );
	void SetChInfo( ChInfo *pInfo );
	void SetPalette( WORD *pPalette );

	void SetHeight( int iMaxHeight ){ m_iMaxHeight = iMaxHeight; }
	int GetHeight() const { return m_iMaxHeight; }

public:
	ioRcFont( const ioHashString &name );
	virtual ~ioRcFont();
};

#endif
