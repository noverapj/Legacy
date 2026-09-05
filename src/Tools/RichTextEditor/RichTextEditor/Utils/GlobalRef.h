
//----------------------------------------------------------------------------------
/*
Filename	: GlobalRef.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _GLOBALREF_H_
#define _GLOBALREF_H_

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
GlobalRef
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

#include "../wxMainFrame.h"
#include "ioWnd.h"

namespace GlobalRef
{
	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ToolBox Bitmap 관련
	*/
	enum TOOLBOX_BITMAP_TYPE
	{
		TBT_WND,
		TBT_BUTTON,
		TBT_RADIO,
		TBT_CHECK,
		TBT_EDIT,
		TBT_PROGRESSBAR,
		TBT_SLIDE,
		TBT_ACTIVESCREENBTN,
		TBT_SCROLL,

		// EX
		TBT_WNDEX,
		TBT_FRAMEWND,
		TBT_IMAGEWND,
		TBT_LABELWND,
		TBT_FRAMEBUTTON,
		TBT_IMAGEBUTTON,
		TBT_CHECKBUTTON,
		TBT_FLASHPLAYER,
		TBT_RADIOWND,
		TBT_RADIOFRAMEBUTTON,
		TBT_RADIOIMAGEBUTTON,
		TBT_TABFRAMEBUTTON,
		TBT_TABWND,
		TBT_SCROLLBAR,
		TBT_RICHLABEL,

		TBT_MAX,
	};
	static const char*	g_toolboxbitmapfilenames[ TBT_MAX ] =
	{
		"ToolResources/UITool/Images/Wnd.bmp"
		,"ToolResources/UITool/Images/button.bmp"
		,"ToolResources/UITool/Images/Radio.bmp"
		,"ToolResources/UITool/Images/check.bmp"
		,"ToolResources/UITool/Images/Edit.bmp"
		,"ToolResources/UITool/Images/progressbar.bmp"
		,"ToolResources/UITool/Images/slide.bmp"
		,"ToolResources/UITool/Images/activebutton.bmp"
		,"ToolResources/UITool/Images/scroll.bmp"
		,"ToolResources/UITool/Images/Wnd.bmp"
		,"ToolResources/UITool/Images/framewnd.bmp"
		,"ToolResources/UITool/Images/imagewnd.bmp"
		,"ToolResources/UITool/Images/labelwnd.bmp"
		,"ToolResources/UITool/Images/framebutton.bmp"
		,"ToolResources/UITool/Images/imagebutton.bmp"
		,"ToolResources/UITool/Images/check.bmp"
		,"ToolResources/UITool/Images/flashplayer.bmp"
		,"ToolResources/UITool/Images/radiobox.bmp"
		,"ToolResources/UITool/Images/radiobutton.bmp"
		,"ToolResources/UITool/Images/radiobutton.bmp"
		,"ToolResources/UITool/Images/tabframebutton.bmp"
		,"ToolResources/UITool/Images/tabwnd.bmp"
		,"ToolResources/UITool/Images/scrollbar.bmp"
		,"ToolResources/UITool/Images/richlabel.bmp"
	};

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	엔진쪽 Window Style Text
	*/
	enum { IWS_NUM = 12 };
	static const char*	g_enginewindowstyletext[ IWS_NUM ] =
	{
		"IWS_START_HIDE"
		,"IWS_INACTIVE"
		,"IWS_BACKMOST"
		,"IWS_CANMOVE"
		,"IWS_MODAL"
		,"IWS_EXIT_ESC"
		,"IWS_PROCESS"
		,"IWS_DRAG_DROP"
		,"IWS_NO_MOUSE_RESPONSE"
		,"IWS_CHILD_ONLY_RESPONSE"
		,"IWS_TOPMOST"
		,"IWS_EXACTIVE"
	};
	enum { IWS_EX_NUM = 16 };
	static const char*	g_enginewindowexstyletext[ IWS_EX_NUM ] =
	{
		"IWS_EX_EDIT_PW"
		,"IWS_EX_EDIT_RIGHT"
		,"IWS_EX_EDIT_CENTER"
		,"IWS_EX_MULTI_EDIT"
		,"IWS_EX_NOTIFY_MOVED"
		,"IWS_EX_OVER_SCREEN"
		,"IWS_EX_PUSH_SCREEN"
		,"IWS_EX_HSCROLL"
		,"IWS_EX_VSCROLL"
		,"IWS_EX_PUSH_DOUBLE"
		,"IWS_EX_EDIT_NUM"
		,"IWS_EX_EDIT_CHECK_FOCUS"
		,"IWS_EX_PUSHED_OVER_SCREEN"
		,"IWS_EX_SHOW_NO_TOP"
		,"IWS_EX_SHOW_HIDE_ACTION"
		,"IWS_EX_NO_REND_EDIT_NUM"
	};
	enum { TEXT_STYLE_NUM = 15 };
	static const char*	g_enginetextstyle[ TEXT_STYLE_NUM ] =
	{
		"Normal"
		,"Shadow"
		,"Outline"
		,"Outline_2x"
		,"Outline_full"
		,"Outline_full_2x"
		,"Outline_full_3x"
		,"Outline_full_4x"
		,"Bold"
		,"Bold_shadow"
		,"Bold_outline"
		,"Bold_outline_full"
		,"Bold_outline_full_2x"
		,"Bold_outline_full_3x"
		,"Bold_outline_full_4x"
	};
	static const char*	g_enginewndalign[ ioWnd::UI_NONE_ALIGN ] =
	{
		"Left"
		,"Right"
		,"Center"
		,"Rate"
		,"Top"
		,"Bottom"
	};				   
	enum { HORZALIGN_NUM = 3 };
	static const char*	g_enginehorzalign[ HORZALIGN_NUM ] =
	{
		"Left"
		,"Center"
		,"Right"
	};
	enum { VERTALIGN_NUM = 3 };
	static const char*	g_enginevertalign[ HORZALIGN_NUM ] =
	{
		"Top"
		,"Center"
		,"Bottom"
	};

	enum { TEXTURE_RENDERTYPE_NUM = 10 };
	static const char*	g_texturerendertype[ TEXTURE_RENDERTYPE_NUM ] =
	{
		"UI_RENDER_NORMAL",
		"UI_RENDER_SCREEN",
		"UI_RENDER_GRAY",
		"UI_RENDER_ADD",
		"UI_RENDER_NORMAL_ADD_COLOR",
		"UI_RENDER_COLOR_ALPHA",
		"UI_RENDER_LIGHTEN",
		"UI_RENDER_DARKEN",
		"UI_RENDER_MULTIPLY",
		"UI_RENDER_GRAY_LIGHT"
	};

	enum { TEXTURE_FILTER_OPTION_NUM = 5 };
	static const char*	g_texturefilteroption[ TEXTURE_FILTER_OPTION_NUM ] =
	{
		"TFO_NEAREST",
		"TFO_BILINEAR",
		"TFO_TRILINEAR",
		"TFO_ANISOTROPIC",
		"TFO_BILINEAR_NOMIPMAP"
	};

	enum { RICHLABEL_LINEFEED_TYPE_NUM = 2 };
	static const char*	g_richlabellinefeedtype[ RICHLABEL_LINEFEED_TYPE_NUM ] =
	{
		"NORMAL",
		"SPACE"
	};

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	Format : string을 만들어준다.
	*/
	std::string				Format( const char* format, ... );

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToString : color값을 string으로 구해줌
	*/
	std::string				ConvertToString( DWORD color );

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToTBT : 선택되어진 windowid를 bitmap type으로 converting
	*/
	int						ConvertToTBT( const wxWindowID& id, int def );

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToTBT : window class명에 따라 bitmap type으로 converting
	*/
	int						ConvertToTBT( const char* classname, int def );

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToString : 선택되어진 TBT를 이름으로 converting
	*/
	std::string				ConvertToString( int id, const std::string& def );

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToEngineColor : Tool에서 사용되는 컬러를 EngineColor로 변환
	*/
	DWORD					ConvertToEngineColor( DWORD color );

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*
	ConvertToToolColor : EngineColor color를 Tool에서 사용되는 컬러로 변환
	*/
	DWORD					ConvertToToolColor( DWORD color );

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetRenderType To Array
	*/
	const wxArrayString&	GetRenderTypeString();
	const wxArrayInt&		GetRenderTypeInt();

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetTextureFilterOptionType To Array
	*/
	const wxArrayString&	GetTextureFilterOptionTypeString();
	const wxArrayInt&		GetTextureFilterOptionTypeInt();

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetTextStyle To Array
	*/
	const wxArrayString&	GetTextStyleString();
	const wxArrayInt&		GetTextStyleInt();

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetVertAlign To Array
	*/
	const wxArrayString&	GetVertAlignString();
	const wxArrayInt&		GetVertAlignInt();

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetHorzAlign To Array
	*/
	const wxArrayString&	GetHorzAlignString();
	const wxArrayInt&		GetHorzAlignInt();

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetButtonType To Array
	*/
	const wxArrayString&	GetButtonTypeString();
	const wxArrayInt&		GetButtonTypeInt();

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetAlignStyle To Array
	*/
	const wxArrayString&	GetAlignStyleString();
	const wxArrayInt&		GetAlignStyleInt();

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetReserveFormat To Array
	*/
	const wxArrayString&	GetReserveFormatString();
	const wxArrayInt&		GetReserveFormatInt();

	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	/*	
	GetLineFeedType To Array
	*/
	const wxArrayString&	GetLineFeedTypeString();
	const wxArrayInt&		GetLineFeedTypeInt();
};

#endif // _GLOBALREF_H_