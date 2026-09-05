

#ifndef _ioFTEnum_h_
#define _ioFTEnum_h_

#define ENC_TAG( value, a, b, c, d )         \
          value = ( ( (DWORD)(a) << 24 ) |  \
                    ( (DWORD)(b) << 16 ) |  \
                    ( (DWORD)(c) <<  8 ) |  \
                      (DWORD)(d)         )

enum ioFTEncode
{
    ENC_TAG( ENCODING_NONE, 0, 0, 0, 0 ),

    ENC_TAG( ENCODING_MS_SYMBOL,  's', 'y', 'm', 'b' ),
    ENC_TAG( ENCODING_UNICODE,    'u', 'n', 'i', 'c' ),

    ENC_TAG( ENCODING_SJIS,    's', 'j', 'i', 's' ),
    ENC_TAG( ENCODING_GB2312,  'g', 'b', ' ', ' ' ),
    ENC_TAG( ENCODING_BIG5,    'b', 'i', 'g', '5' ),
    ENC_TAG( ENCODING_WANSUNG, 'w', 'a', 'n', 's' ),
    ENC_TAG( ENCODING_JOHAB,   'j', 'o', 'h', 'a' ),

    ENC_TAG( ENCODING_ADOBE_STANDARD, 'A', 'D', 'O', 'B' ),
    ENC_TAG( ENCODING_ADOBE_EXPERT,   'A', 'D', 'B', 'E' ),
    ENC_TAG( ENCODING_ADOBE_CUSTOM,   'A', 'D', 'B', 'C' ),
    ENC_TAG( ENCODING_ADOBE_LATIN_1,  'l', 'a', 't', '1' ),

    ENC_TAG( ENCODING_OLD_LATIN_2, 'l', 'a', 't', '2' ),

    ENC_TAG( ENCODING_APPLE_ROMAN, 'a', 'r', 'm', 'n' )
};

struct GlyphImg
{
	short int iBearingX;	// offsetX
	short int iBearingY;	// offsetY
	int iAdvance;			// char width
	short int iWidth;		// image width
	short int iHeight;		// image height
	void *pBuf;				// image ptr

	GlyphImg()
	{
		pBuf = NULL;
	}
	~GlyphImg()
	{
		if( pBuf )
		{
			delete []pBuf;
			pBuf = NULL;
		}
	}

	// Origin축을 고려하지 않은 순수한 가로
	inline int NeedPureDrawWidth() const
	{
		if( iBearingX >= 0 )
			return max( iAdvance, iWidth + iBearingX );

		// iBearingX < 0
		return max( iAdvance - iBearingX, iWidth );
	}

	// Origin축 이후부터의 가로
	inline int NeedOriginDrawWidth() const
	{
		return max( iAdvance, iWidth + iBearingX );
	}
};

#endif