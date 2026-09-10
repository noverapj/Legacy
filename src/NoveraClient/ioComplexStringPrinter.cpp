

#include "stdafx.h"

#include "ChatMessageFunc.h"
#include "ioComplexStringPrinter.h"

char ioComplexStringPrinter::m_szBuffer[ioComplexStringPrinter::BUFFER_SIZE];

ioComplexStringPrinter::ioComplexStringPrinter()
{
	m_dwTextColor = 0xffffffff;
	m_dwBkColor = 0xff000000;
	m_fFullWidth = 0.0f;
}

ioComplexStringPrinter::~ioComplexStringPrinter()
{
	ClearList();
}

void ioComplexStringPrinter::SetTextColor( int iRed, int iGreen, int iBlue )
{
	m_dwTextColor = D3DCOLOR_XRGB( iRed, iGreen, iBlue );
}

void ioComplexStringPrinter::SetTextColor( DWORD dwTextColor )
{
	m_dwTextColor = dwTextColor;
}

void ioComplexStringPrinter::SetFullTextColor( DWORD dwTextColor )
{
	int iPieceCnt = m_vPieceList.size();
	for( int i=0 ; i<iPieceCnt ; i++ )
	{
		TextPiece &rkPiece = m_vPieceList[i];
		rkPiece.dwTextColor = dwTextColor;
	}
}

void ioComplexStringPrinter::SetFullTextTableColor()
{
	int iPieceCnt = m_vPieceList.size();
	for( int i=0 ; i<iPieceCnt ; i++ )
	{
		TextPiece &rkPiece = m_vPieceList[i];
		rkPiece.dwTextColor = g_ChatMgr.ChangeColorTableChat( rkPiece.dwTextColor );
	}
}

void ioComplexStringPrinter::SetFullTextStyle( TextStyle eStyle )
{
	int iPieceCnt = m_vPieceList.size();
	for( int i=0 ; i<iPieceCnt ; i++ )
	{
		TextPiece &rkPiece = m_vPieceList[i];
		rkPiece.eStyle = eStyle;
	}
}

void ioComplexStringPrinter::SetBkColor( int iRed, int iGreen, int iBlue )
{
	m_dwBkColor = D3DCOLOR_XRGB( iRed, iGreen, iBlue );
}

void ioComplexStringPrinter::SetBkColor( DWORD dwBkColor )
{
	m_dwBkColor = dwBkColor;
}

void ioComplexStringPrinter::SetFullBkColor( DWORD dwBkColor )
{
	int iPieceCnt = m_vPieceList.size();
	for( int i=0 ; i<iPieceCnt ; i++ )
	{
		TextPiece &rkPiece = m_vPieceList[i];
		rkPiece.dwBkColor = dwBkColor;
	}
}

void ioComplexStringPrinter::SetTextStyle( TextStyle eStyle )
{
	m_TextStyle = eStyle;
}

void ioComplexStringPrinter::UpdateFullWidth()
{
	m_fFullWidth = 0.0f;

	if( !m_vPieceList.empty() )
	{
		int iSize = m_vPieceList.size();

		for( int i=0; i<iSize; i++ )
		{
			m_fFullWidth += m_vPieceList[i].fWidth;
		}
	}
}

void ioComplexStringPrinter::SetTextPiece( float fScale, float fYOffset )
{
	if( strcmp( m_szBuffer, "" ) == 0 )
		return;

	TextPiece kPiece;
	kPiece.dwTextColor = m_dwTextColor;
	kPiece.dwBkColor   = m_dwBkColor;
	kPiece.eStyle      = m_TextStyle;
	kPiece.fScale      = fScale;
	kPiece.fYOffset    = fYOffset;
	kPiece.fWidth      = g_FontMgr.GetTextWidth( m_szBuffer, m_TextStyle, fScale );
	kPiece.szText      = m_szBuffer;
	m_vPieceList.push_back( kPiece );

	m_fFullWidth += kPiece.fWidth;
}

void ioComplexStringPrinter::SetTextPieceWithoutXOffset( float fScale, float fYOffset )
{
	if( strcmp( m_szBuffer, "" ) == 0 )
		return;

	TextPiece kPiece;
	kPiece.dwTextColor = m_dwTextColor;
	kPiece.dwBkColor   = m_dwBkColor;
	kPiece.eStyle      = m_TextStyle;
	kPiece.fScale      = fScale;
	kPiece.fYOffset    = fYOffset;
	kPiece.fWidth      = g_FontMgr.GetTextWidth( m_szBuffer, TS_NORMAL, fScale );
	kPiece.szText      = m_szBuffer;
	m_vPieceList.push_back( kPiece );

	m_fFullWidth += kPiece.fWidth;
}

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szBuffer, BUFFER_SIZE, szSource, args );
		va_end( args );

		SetTextPiece( fScale, 0.0f );
	}
	else
	{
		SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource );
		SetTextPiece( fScale, 0.0f );
	}
}
#else
void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource );
	SetTextPiece( fScale, 0.0f );
}
#endif

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, int iArg1 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, iArg1 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, DWORD dwArg1 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, dwArg1 );
	SetTextPiece( fScale, 0.0f );
}


void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, float fArg1 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, fArg1 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, __int64 i64Arg1 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, i64Arg1 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, const char *szArg1 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, szArg1 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, int iArg1, int iArg2 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, iArg1, iArg2 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, int iArg1, const char *szArg2 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, iArg1, szArg2 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, const char *szArg1, int iArg2 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, szArg1, iArg2 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, const char *szArg1, const char *szArg2 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, szArg1, szArg2 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, int iArg1, int iArg2, int iArg3 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, iArg1, iArg2, iArg3 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, int iArg1, const char *szArg2, const char *szArg3 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, iArg1, szArg2, szArg3 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, const char *szArg1, int iArg2, int iArg3 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, szArg1, iArg2, iArg3 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, const char *szArg1, int iArg2, const char *szArg3 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, szArg1, iArg2, szArg3 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, const char *szArg1, const char *szArg2, const char *szArg3 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, szArg1, szArg2, szArg3 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, iArg1, iArg2, iArg3, iArg4 );
	SetTextPiece( fScale, 0.0f );
}

void ioComplexStringPrinter::AddTextPiece( float fScale, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4, int iArg5 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, iArg1, iArg2, iArg3, iArg4, iArg5 );
	SetTextPiece( fScale, 0.0f );
}

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioComplexStringPrinter::AddTextPieceWithYOffset( float fScale, float fYOffset, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szBuffer, BUFFER_SIZE, szSource, args );
		va_end( args );
		SetTextPiece( fScale, fYOffset );
	}
	else
	{
		SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource );
		SetTextPiece( fScale, fYOffset );
	}
}
#else
void ioComplexStringPrinter::AddTextPieceWithYOffset( float fScale, float fYOffset, const char *szSource )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource );
	SetTextPiece( fScale, fYOffset );
}
#endif

void ioComplexStringPrinter::AddTextPieceWithYOffset( float fScale, float fYOffset, const char *szSource, int iArg1 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, iArg1 );
	SetTextPiece( fScale, fYOffset );
}

void ioComplexStringPrinter::AddTextPieceWithYOffset( float fScale, float fYOffset, const char *szSource, const char *szArg1 )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource, szArg1 );
	SetTextPiece( fScale, fYOffset );
}

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioComplexStringPrinter::AddTextPieceWithoutXOffset( float fScale, const char *szSource, ... )
{
	if ( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szBuffer, BUFFER_SIZE, szSource, args );
		va_end( args );

		SetTextPieceWithoutXOffset( fScale, 0.0f  );
	}
	else
	{
		SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource );
		SetTextPieceWithoutXOffset( fScale, 0.0f );
	}
}
#else
void ioComplexStringPrinter::AddTextPieceWithoutXOffset( float fScale, const char *szSource )
{
	SafeSprintf( m_szBuffer, BUFFER_SIZE, szSource );
	SetTextPieceWithoutXOffset( fScale, 0.0f );
}
#endif

void ioComplexStringPrinter::CopyFullText( ioComplexStringPrinter &rkPrinter ) const
{
	rkPrinter.ClearList();
	rkPrinter.m_dwTextColor = m_dwTextColor;
	rkPrinter.m_dwBkColor   = m_dwBkColor;
	rkPrinter.m_TextStyle   = m_TextStyle;
	rkPrinter.m_fFullWidth  = m_fFullWidth;

	if( !m_vPieceList.empty() )
	{
		int iSize = m_vPieceList.size();
		rkPrinter.m_vPieceList.reserve( iSize );

		for( int i=0; i<iSize; i++ )
		{
			rkPrinter.m_vPieceList.push_back( m_vPieceList[i] );
		}
	}
}

void ioComplexStringPrinter::CopyOnlyText( OUT std::string &rkText ) const
{
	rkText.clear();

	if( !m_vPieceList.empty() )
	{
		int iSize = m_vPieceList.size();
		for( int i=0; i<iSize; i++ )
		{
			rkText += m_vPieceList[i].szText;
		}
	}
}

void ioComplexStringPrinter::CopyModifyScaleText( ioComplexStringPrinter &rkPrinter, float fScale )
{
	rkPrinter.ClearList();
	rkPrinter.m_dwTextColor = m_dwTextColor;
	rkPrinter.m_dwBkColor   = m_dwBkColor;
	rkPrinter.m_TextStyle   = m_TextStyle;
	rkPrinter.m_fFullWidth  = m_fFullWidth;

	if( !m_vPieceList.empty() )
	{
		int iSize = m_vPieceList.size();
		rkPrinter.m_vPieceList.reserve( iSize );

		for( int i=0; i<iSize; i++ )
		{
			TextPiece kPiece = m_vPieceList[i];
			kPiece.fScale = fScale;
			kPiece.fWidth = g_FontMgr.GetTextWidth( kPiece.szText.c_str(), m_TextStyle, fScale );

			rkPrinter.m_vPieceList.push_back( kPiece );
		}
	}
}

void ioComplexStringPrinter::SplitFullText( ioComplexStringPrinter &rkPrinterA, ioComplexStringPrinter &rkPrinterB, float fSplitWidth )
{
	rkPrinterA.ClearList();
	rkPrinterB.ClearList();

	if( GetFullWidth() <= fSplitWidth )
	{
		CopyFullText( rkPrinterA );
		return;
	}
    
	if( !m_vPieceList.empty() )
	{
		int i = 0;
		float fCurrentWidth = 0.0f;
		int iSize = m_vPieceList.size();

		// Printer A
		for(;i < iSize;i++)
		{
			TextPiece kPiece = m_vPieceList[i];
			float fWidth = g_FontMgr.GetTextWidth( kPiece.szText.c_str(), kPiece.eStyle, kPiece.fScale );
			if( fCurrentWidth +	fWidth <= fSplitWidth )
				rkPrinterA.m_vPieceList.push_back( kPiece );
			else
			{
				// 범위를 넘어갔으면 A와 B에 나눠서 넣어준다.
				float fRemainWidth = fSplitWidth - fCurrentWidth;
				char szDst[MAX_PATH], szSrc[MAX_PATH];
				memset( szDst, 0, sizeof( szDst ) );
				sprintf( szSrc, "%s", kPiece.szText.c_str() );
				int iLen = Help::StringCutFun( kPiece.fScale, fRemainWidth, kPiece.eStyle, szDst, sizeof( szDst ), szSrc );
				kPiece.szText = szDst;
				kPiece.fWidth = g_FontMgr.GetTextWidth( kPiece.szText.c_str(), kPiece.eStyle, kPiece.fScale );
				rkPrinterA.m_vPieceList.push_back( kPiece );
				
				// 나머지는 몽땅 두번째 줄에 추가
				if( iLen < (int)strlen( szSrc ) )
				{
					kPiece.szText = &szSrc[iLen];
					kPiece.fWidth = g_FontMgr.GetTextWidth( kPiece.szText.c_str(), kPiece.eStyle, kPiece.fScale );
					rkPrinterB.m_vPieceList.push_back( kPiece );
				}
				i++;
				break;
			}
			fCurrentWidth += fWidth;
		}

		// Printer B
		for(;i < iSize;i++)
		{
			rkPrinterB.m_vPieceList.push_back( m_vPieceList[i] );
		}

		if( !rkPrinterA.IsEmpty() )
		{
			rkPrinterA.m_dwTextColor = m_dwTextColor;
			rkPrinterA.m_dwBkColor   = m_dwBkColor;
			rkPrinterA.m_TextStyle   = m_TextStyle;
			rkPrinterA.UpdateFullWidth();
		}

		if( !rkPrinterB.IsEmpty() )
		{
			rkPrinterB.m_dwTextColor = m_dwTextColor;
			rkPrinterB.m_dwBkColor   = m_dwBkColor;
			rkPrinterB.m_TextStyle   = m_TextStyle;
			rkPrinterB.UpdateFullWidth();
		}
	}
}

bool ioComplexStringPrinter::IsCompare( const ioComplexStringPrinter &rkPrinter ) const
{
	if( m_vPieceList.size() != rkPrinter.m_vPieceList.size() )
		return false;

	int iDstSize = m_vPieceList.size();
	for(int i = 0;i < iDstSize;i++)
	{
		if( m_vPieceList[i].szText != rkPrinter.m_vPieceList[i].szText )
			return false;
	}
	
	return true;
}

void ioComplexStringPrinter::PrintFullText( float fXPos,
										    float fYPos,
											TextAlignType eAlign,
											BYTE bAlpha )
{
	float fStartX = fXPos;
	switch( eAlign )
	{
	case TAT_CENTER:
		fStartX -= m_fFullWidth * FLOAT05;
		break;
	case TAT_RIGHT:
		fStartX -= m_fFullWidth;
		break;
	}

	g_FontMgr.SetAlignType( TAT_LEFT );

	int iPieceCnt = m_vPieceList.size();
	for( int i=0 ; i<iPieceCnt ; i++ )
	{
		const TextPiece &rkPiece = m_vPieceList[i];

		g_FontMgr.SetTextColor( rkPiece.dwTextColor );
		g_FontMgr.SetBkColor( rkPiece.dwBkColor );
		g_FontMgr.SetTextStyle( rkPiece.eStyle );

		if( bAlpha == 255 )
		{
			g_FontMgr.PrintText( fStartX, fYPos + rkPiece.fYOffset, rkPiece.fScale, rkPiece.szText.c_str() );
		}
		else
		{
			g_FontMgr.PrintTextAlpha( fStartX, fYPos + rkPiece.fYOffset, rkPiece.fScale, bAlpha, rkPiece.szText.c_str() );
		}

		fStartX += rkPiece.fWidth;
	}
}

void ioComplexStringPrinter::PrintFullModifyText( float fXPos,
												  float fYPos,
												  DWORD dwTextColor,
												  TextStyle eStyle,
												  TextAlignType eAlign,
												  BYTE bAlpha )
{
	float fStartX = fXPos;
	switch( eAlign )
	{
	case TAT_CENTER:
		fStartX -= m_fFullWidth * FLOAT05;
		break;
	case TAT_RIGHT:
		fStartX -= m_fFullWidth;
		break;
	}

	g_FontMgr.SetAlignType( TAT_LEFT );

	int iPieceCnt = m_vPieceList.size();
	for( int i=0 ; i<iPieceCnt ; i++ )
	{
		const TextPiece &rkPiece = m_vPieceList[i];

		g_FontMgr.SetTextColor( dwTextColor );
		g_FontMgr.SetBkColor( rkPiece.dwBkColor );
		g_FontMgr.SetTextStyle( eStyle );

		if( bAlpha == 255 )
		{
			g_FontMgr.PrintText( fStartX, fYPos + rkPiece.fYOffset, rkPiece.fScale, rkPiece.szText.c_str() );
		}
		else
		{
			g_FontMgr.PrintTextAlpha( fStartX, fYPos + rkPiece.fYOffset, rkPiece.fScale, bAlpha, rkPiece.szText.c_str() );
		}

		fStartX += rkPiece.fWidth;
	}
}

void ioComplexStringPrinter::PrintFullTextWidthCut( float fXPos,
										            float fYPos,
										            TextAlignType eAlign,
													float fWidth,
										            BYTE bAlpha /*=255*/)
{
	// fullwidth 재계산
	if( m_fFullWidth > fWidth )
	{
		m_fFullWidth = 0;
		std::string szFullString;
		float fScale = 0.0f;
		int iPieceCnt = m_vPieceList.size();
		for( int i=0 ; i<iPieceCnt ; i++ )
		{
			const TextPiece &rkPiece = m_vPieceList[i];
			szFullString += rkPiece.szText;
			fScale        = rkPiece.fScale;
		}
		m_fFullWidth = g_FontMgr.GetTextWidthCutSize( szFullString.c_str(), m_TextStyle, fScale, fWidth );
	}

	float fStartX = fXPos;
	switch( eAlign )
	{
	case TAT_CENTER:
		fStartX -= m_fFullWidth * FLOAT05;
		break;
	case TAT_RIGHT:
		fStartX -= m_fFullWidth;
		break;
	}

	g_FontMgr.SetAlignType( TAT_LEFT );

	int iPieceCnt = m_vPieceList.size();
	for( int i=0 ; i<iPieceCnt ; i++ )
	{
		const TextPiece &rkPiece = m_vPieceList[i];

		g_FontMgr.SetTextColor( rkPiece.dwTextColor );
		g_FontMgr.SetBkColor( rkPiece.dwBkColor );
		g_FontMgr.SetTextStyle( rkPiece.eStyle );

		if( bAlpha == 255 )
		{
			g_FontMgr.PrintTextWidthCut( fStartX, fYPos + rkPiece.fYOffset, rkPiece.fScale, fWidth, rkPiece.szText.c_str() );
		}
		else
		{
			g_FontMgr.PrintTextWidthCutAlpha( fStartX, fYPos + rkPiece.fYOffset, rkPiece.fScale, fWidth, bAlpha, rkPiece.szText.c_str() );
		}

		fStartX += rkPiece.fWidth;
		fWidth  -= rkPiece.fWidth;
	}
}

float ioComplexStringPrinter::PrintFullTextWidthDirectCut( float fXPos,
														  float fYPos,
														  TextAlignType eAlign,
														  float fWidth,
														  BYTE bAlpha /*=255*/ )
{
	float fStartX = fXPos;
	switch( eAlign )
	{
	case TAT_CENTER:
		fStartX -= m_fFullWidth * FLOAT05;
		break;
	case TAT_RIGHT:
		fStartX -= m_fFullWidth;
		break;
	}

	g_FontMgr.SetAlignType( TAT_LEFT );

	int iPieceCnt = m_vPieceList.size();
	for( int i=0 ; i<iPieceCnt ; i++ )
	{
		const TextPiece &rkPiece = m_vPieceList[i];

		g_FontMgr.SetTextColor( rkPiece.dwTextColor );
		g_FontMgr.SetBkColor( rkPiece.dwBkColor );
		g_FontMgr.SetTextStyle( rkPiece.eStyle );

		char szDst[MAX_PATH] = "";
		memset( szDst, 0, sizeof( szDst ) );
		Help::StringCutFun( rkPiece.fScale, fWidth, rkPiece.eStyle, szDst, sizeof(szDst), rkPiece.szText.c_str() );
		if( bAlpha == 255 )
		{
			g_FontMgr.PrintText( fStartX, fYPos + rkPiece.fYOffset, rkPiece.fScale, szDst );
		}
		else
		{
			g_FontMgr.PrintTextAlpha( fStartX, fYPos + rkPiece.fYOffset, rkPiece.fScale, bAlpha, szDst );
		}

		fStartX += rkPiece.fWidth;
		fWidth  -= rkPiece.fWidth;
	}
	return m_fFullWidth;
}

void ioComplexStringPrinter::ClearList()
{
	if( !m_vPieceList.empty()  )
	{
		m_vPieceList.clear();
	}

	m_fFullWidth = 0.0f;
}
