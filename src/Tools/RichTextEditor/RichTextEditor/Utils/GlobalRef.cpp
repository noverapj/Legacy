
//----------------------------------------------------------------------------------
/*
Filename	: GlobalRef.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "GlobalRef.h"
#include "ioWndEXType.h"
#include "ioWnd.h"
#include "ioButton.h"
#include "ioScroll.h"
#include "ioEdit.h"
#include "ioProgressbar.h"
#include "ioWndEX.h"
#include "ioFrameWndEX.h"
#include "ioImageWndEX.h"
#include "ioLabelWndEX.h"
#include "ioButtonWndEX.h"
#include "ioCheckBoxEX.h"
#include "ioFlashPlayer.h"
#include "ioTabWndEX.h"
#include "ioScrollBarEX.h"
#include "ioRichLabel.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
GlobalRef
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

namespace GlobalRef
{
	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	Format : string을 만들어준다.
	*/
	std::string		Format( const char* format, ... )
	{	
		static const unsigned int FORMAT_BUFF_SIZE = 1024;
		static char buffer[ FORMAT_BUFF_SIZE ];
		va_list marker;
		va_start( marker, format );
		_vsntprintf_s( buffer, FORMAT_BUFF_SIZE, format, marker );
		va_end( marker );
		return buffer;
	};

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToString : color값을 string으로 구해줌
	*/
	std::string		ConvertToString( DWORD color )
	{	
		return Format( "(%d,%d,%d)", ( ( color & 0x00FF0000 ) >> 16 ), ( ( color & 0x0000FF00 ) >> 8 ), ( color & 0x000000FF ) );
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToTBT : 선택되어진 windowid를 bitmap type으로 converting
	*/
	int				ConvertToTBT( const wxWindowID& id, int def )
	{
		switch ( id )
		{
		case ID_TOOLBOX_WND:
			return TBT_WND;
		case ID_TOOLBOX_BUTTON:
			return TBT_BUTTON;
		case ID_TOOLBOX_RADIO:
			return TBT_RADIO;
		case ID_TOOLBOX_CHECK:
			return TBT_CHECK;
		case ID_TOOLBOX_EDIT:
			return TBT_EDIT;
		case ID_TOOLBOX_PROGRESSBAR:
			return TBT_PROGRESSBAR;
		case ID_TOOLBOX_SLIDE:
			return TBT_SLIDE;
		case ID_TOOLBOX_ACTIVESCREENBTN:
			return TBT_ACTIVESCREENBTN;
		case ID_TOOLBOX_SCROLL:
			return TBT_SCROLL;
		case ID_TOOLBOX_WNDEX:
			return TBT_WNDEX;
		case ID_TOOLBOX_FRAMEWND:
			return TBT_FRAMEWND;
		case ID_TOOLBOX_IMAGEWND:
			return TBT_IMAGEWND;
		case ID_TOOLBOX_LABELWND:
			return TBT_LABELWND;
		case ID_TOOLBOX_FRAMEBUTTON:
			return TBT_FRAMEBUTTON;
		case ID_TOOLBOX_IMAGEBUTTON:
			return TBT_IMAGEBUTTON;
		case ID_TOOLBOX_CHECKBOX:
			return TBT_CHECKBUTTON;
		case ID_TOOLBOX_FLASHPLAYER:
			return TBT_FLASHPLAYER;
		case ID_TOOLBOX_RADIOWND:
			return TBT_RADIOWND;
		case ID_TOOLBOX_RADIOFRAMEBTN:
			return TBT_RADIOFRAMEBUTTON;
		case ID_TOOLBOX_RADIOIMAGEBTN:
			return TBT_RADIOIMAGEBUTTON;
		case ID_TOOLBOX_TABFRAMEBUTTON:
			return TBT_TABFRAMEBUTTON;
		case ID_TOOLBOX_TABWND:
			return TBT_TABWND;
		case ID_TOOLBOX_SCROLLBAR:
			return TBT_SCROLLBAR;
		case ID_TOOLBOX_RICHLABEL:
			return TBT_RICHLABEL;
		}
		return def;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToTBT : window class명에 따라 bitmap type으로 converting
	*/
	int				ConvertToTBT( const char* classname, int def )
	{
#define CONVERT_NICKNAME_TO_TBT( ClassName, RetType ) \
		if ( strcmp( classname, ClassName::GetNickName() ) == 0 ) {	return RetType; }

		CONVERT_NICKNAME_TO_TBT( ioWnd, TBT_WND )
		CONVERT_NICKNAME_TO_TBT( ioButton, TBT_BUTTON )
		CONVERT_NICKNAME_TO_TBT( ioRadioButton, TBT_RADIO )
		CONVERT_NICKNAME_TO_TBT( ioCheckButton, TBT_CHECK )
		CONVERT_NICKNAME_TO_TBT( ioEdit, TBT_EDIT )
		CONVERT_NICKNAME_TO_TBT( ioProgressBar, TBT_PROGRESSBAR )
		CONVERT_NICKNAME_TO_TBT( ioSlide, TBT_SLIDE )
		CONVERT_NICKNAME_TO_TBT( ioActiveScreenBtn, TBT_ACTIVESCREENBTN )
		CONVERT_NICKNAME_TO_TBT( ioScroll, TBT_SCROLL )
		CONVERT_NICKNAME_TO_TBT( ioButton, TBT_BUTTON )
		CONVERT_NICKNAME_TO_TBT( ioWndEX, TBT_WNDEX )
		CONVERT_NICKNAME_TO_TBT( ioFrameWndEX, TBT_FRAMEWND )
		CONVERT_NICKNAME_TO_TBT( ioImageWndEX, TBT_IMAGEWND )
		CONVERT_NICKNAME_TO_TBT( ioLabelWndEX, TBT_LABELWND )
		CONVERT_NICKNAME_TO_TBT( ioFrameButtonWndEX, TBT_FRAMEBUTTON )
		CONVERT_NICKNAME_TO_TBT( ioImageButtonWndEX, TBT_IMAGEBUTTON )
		CONVERT_NICKNAME_TO_TBT( ioCheckBoxEX, TBT_CHECKBUTTON )
		CONVERT_NICKNAME_TO_TBT( ioFlashPlayer, TBT_FLASHPLAYER )
		CONVERT_NICKNAME_TO_TBT( ioRadioWndEX, TBT_RADIOWND )
		CONVERT_NICKNAME_TO_TBT( ioRadioFrameButtonEX, TBT_RADIOFRAMEBUTTON )
		CONVERT_NICKNAME_TO_TBT( ioRadioImageButtonEX, TBT_RADIOIMAGEBUTTON )
		CONVERT_NICKNAME_TO_TBT( ioTabFrameButtonEX, TBT_TABFRAMEBUTTON )
		CONVERT_NICKNAME_TO_TBT( ioTabWndEX, TBT_TABWND )
		CONVERT_NICKNAME_TO_TBT( ioScrollBarEX, TBT_SCROLLBAR )
		CONVERT_NICKNAME_TO_TBT( ioRichLabel, TBT_RICHLABEL )

		return def;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToTBT : 선택되어진 windowid를 이름으로 converting
	*/
	std::string		ConvertToString( int id, const std::string& def )
	{
		switch ( id )
		{
		case TBT_WND:
			return ioWnd::GetNickName();
		case TBT_BUTTON:
			return ioButton::GetNickName();
		case TBT_RADIO:
			return ioRadioButton::GetNickName();
		case TBT_CHECK:
			return ioCheckButton::GetNickName();
		case TBT_EDIT:
			return ioEdit::GetNickName();
		case TBT_PROGRESSBAR:
			return ioProgressBar::GetNickName();
		case TBT_SLIDE:
			return ioSlide::GetNickName();
		case TBT_ACTIVESCREENBTN:
			return ioActiveScreenBtn::GetNickName();
		case TBT_SCROLL:
			return ioScroll::GetNickName();
		case TBT_WNDEX:
			return ioWndEX::GetNickName();
		case TBT_FRAMEWND:
			return ioFrameWndEX::GetNickName();
		case TBT_IMAGEWND:
			return ioImageWndEX::GetNickName();
		case TBT_LABELWND:
			return ioLabelWndEX::GetNickName();
		case TBT_FRAMEBUTTON:
			return ioFrameButtonWndEX::GetNickName();
		case TBT_IMAGEBUTTON:
			return ioImageButtonWndEX::GetNickName();
		case TBT_CHECKBUTTON:
			return ioCheckBoxEX::GetNickName();
		case TBT_FLASHPLAYER:
			return ioFlashPlayer::GetNickName();
		case TBT_RADIOWND:
			return ioRadioWndEX::GetNickName();
		case TBT_RADIOFRAMEBUTTON:
			return ioRadioFrameButtonEX::GetNickName();
		case TBT_RADIOIMAGEBUTTON:
			return ioRadioImageButtonEX::GetNickName();
		case TBT_TABFRAMEBUTTON:
			return ioTabFrameButtonEX::GetNickName();
		case TBT_TABWND:
			return ioTabWndEX::GetNickName();
		case TBT_SCROLLBAR:
			return ioScrollBarEX::GetNickName();
		case TBT_RICHLABEL:
			return ioRichLabel::GetNickName();
		}
		return def;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToEngineColor : Tool에서 사용되는 컬러를 EngineColor로 변환
	*/
	DWORD			ConvertToEngineColor( DWORD color )
	{
		DWORD ret = 0xFF000000;		
		ret |= ( ( color & 0x000000FF ) << 16 );
		ret |= ( color & 0x0000FF00 );
		ret |= ( ( color & 0x00FF0000 ) >> 16 );
		return ret;
	}
	
	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToToolColor : EngineColor color를 Tool에서 사용되는 컬러로 변환
	*/
	DWORD			ConvertToToolColor( DWORD color )
	{
		return ConvertToEngineColor( color );
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	배열을 채워준다.
	*/
	void	GetArrayString( const char** strs, int num, wxArrayString& item )
	{
		if ( item.empty() ) {
			for ( int i = 0; i < num; ++i ) {
				item.Add( strs[ i ] );
			}
		}
	}
	void	GetArrayInt( int num, wxArrayInt& item )
	{
		if ( item.empty() ) {
			for ( int i = 0; i < num; ++i ) {
				item.Add( i );
			}
		}
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetRenderType To Array
	*/
	const wxArrayString&	GetRenderTypeString()
	{
		static wxArrayString item;
		GetArrayString( g_texturerendertype, TEXTURE_RENDERTYPE_NUM, item );
		return item;
	}

	const wxArrayInt&		GetRenderTypeInt()
	{
		static wxArrayInt item;
		GetArrayInt( TEXTURE_RENDERTYPE_NUM, item );
		return item;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetTextureFilterOptionType To Array
	*/
	const wxArrayString&	GetTextureFilterOptionTypeString()
	{
		static wxArrayString item;
		GetArrayString( g_texturefilteroption, TEXTURE_FILTER_OPTION_NUM, item );
		return item;
	}

	const wxArrayInt&		GetTextureFilterOptionTypeInt()
	{
		static wxArrayInt item;
		GetArrayInt( TEXTURE_FILTER_OPTION_NUM, item );
		return item;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetTextStyle To Array
	*/
	const wxArrayString&	GetTextStyleString()
	{
		static wxArrayString item;
		GetArrayString( g_enginetextstyle, TEXT_STYLE_NUM, item );
		return item;
	}

	const wxArrayInt&		GetTextStyleInt()
	{
		static wxArrayInt item;
		GetArrayInt( TEXT_STYLE_NUM, item );
		return item;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetVertAlign To Array
	*/
	const wxArrayString&	GetVertAlignString()
	{
		static wxArrayString item;
		GetArrayString( g_enginevertalign, VERTALIGN_NUM, item );
		return item;
	}
	
	const wxArrayInt&		GetVertAlignInt()
	{
		static wxArrayInt item;
		GetArrayInt( VERTALIGN_NUM, item );
		return item;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetHorzAlign To Array
	*/
	const wxArrayString&	GetHorzAlignString()
	{
		static wxArrayString item;
		GetArrayString( g_enginehorzalign, HORZALIGN_NUM, item );
		return item;
	}

	const wxArrayInt&		GetHorzAlignInt()
	{
		static wxArrayInt item;
		GetArrayInt( HORZALIGN_NUM, item );
		return item;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetButtonType To Array
	*/
	const wxArrayString&	GetButtonTypeString()
	{
		static wxArrayString item;
		GetArrayString( g_iobuttonextype, BEXT_MAX, item );
		return item;
	}

	const wxArrayInt&		GetButtonTypeInt()
	{
		static wxArrayInt item;
		GetArrayInt( BEXT_MAX, item );
		return item;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetAlignStyle To Array
	*/
	const wxArrayString&	GetAlignStyleString()
	{
		static wxArrayString item;
		GetArrayString( g_alignstyleex, ASE_MAX, item );
		return item;
	}

	const wxArrayInt&		GetAlignStyleInt()
	{
		static wxArrayInt item;
		GetArrayInt( ASE_MAX, item );
		return item;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetReserveFormat To Array
	*/
	const wxArrayString&	GetReserveFormatString()
	{
		static wxArrayString item;
		GetArrayString( g_reserverformattype, REVERSEFORMAT_TYPE_MAX, item );
		return item;
	}

	const wxArrayInt&		GetReserveFormatInt()
	{
		static wxArrayInt item;
		GetArrayInt( REVERSEFORMAT_TYPE_MAX, item );
		return item;
	}

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetLineFeedType To Array
	*/
	const wxArrayString&	GetLineFeedTypeString()
	{
		static wxArrayString item;
		GetArrayString( g_richlabellinefeedtype, RICHLABEL_LINEFEED_TYPE_NUM, item );
		return item;
	}

	const wxArrayInt&		GetLineFeedTypeInt()
	{
		static wxArrayInt item;
		GetArrayInt( RICHLABEL_LINEFEED_TYPE_NUM, item );
		return item;
	}
};