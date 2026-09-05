//----------------------------------------------------------------------------------
/*
Filename	: ioWndEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOWNDEX_H_
#define _IOWNDEX_H_

#include "ioWnd.h"
#include "ioWndEXType.h"
#include "ioWndEXEventType.h"
#include "ioWndEXEventHandler.h"

//------------------------------------------------------------------------------------
/*
String Format
Desc : 주의! 멀티 쓰레드에서 안전하지 않음
*/
__EX const char*	Format( const char* format, ... );

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
RenderElement 관련 Set / Get 인터페이스를 반복적으로 정의하기 위한 매크로
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
#define DECLARE_RENDERELEMENT_INTERFACES( name, member ) \
	void SetRenderType##name(UIRenderType type) { if (member) member->SetRenderType(type); }\
	UIRenderType GetRenderType##name() const { if (member) return member->GetRenderType(); return UI_RENDER_NORMAL; }\
	void SetGrayRenderType##name(UIRenderType type) { if (member) member->SetGrayRenderType(type); }\
	UIRenderType GetGrayRenderType##name() const { if (member) return member->GetGrayRenderType(); return UI_RENDER_NORMAL; }\
	void SetTextureFilterOption##name(TextureFilterOption opt) { if (member) member->SetTextureFilterOption(opt); }\
	TextureFilterOption GetTextureFilterOption##name() const { if (member) return member->GetTextureFilterOption(); return TFO_NEAREST; }\
	void SetColor##name(DWORD color) { if (member) member->SetColor(color); }\
	DWORD GetColor##name() const { if (member) return member->GetColor(); return 0xFFFFFFFF; }

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioWndEX : 확장 컨트롤들이 반드시 상속받아야하는 클래스
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioWndEX : public ioWnd
{
protected:

	typedef std::vector< ioWndEXEventHandlerBase* >			HandlerVector;
	typedef std::map< WNDEX_EVENT_TYPE, HandlerVector >		ConnectMap;
	typedef std::map< WNDEX_EVENT_TYPE, std::string >		ConnectFunctionNameMap; // Tool을 위한 data

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioWndEX();
	ioWndEX( IOWNDEX_TYPE type );
	virtual ~ioWndEX();

	//----------------------------------------------------------------------------------
	/*
	Type
	*/
	IOWNDEX_TYPE			GetEXType() const { return m_extype; }
	virtual bool			IsEXType() const { return true; }

	//----------------------------------------------------------------------------------
	/*
	ioWnd override
	*/
	virtual void			ParseExtraInfo( ioXMLElement& xml );

	//----------------------------------------------------------------------------------
	/*
	ioWnd event override
	*/
	virtual void			iwm_create();
	virtual void			iwm_destroy();
	virtual void			iwm_lbuttondown( const ioMouse& mouse );
	virtual void			iwm_lbuttonup( const ioMouse& mouse );
	virtual void			iwm_lbuttonDBLCLK( const ioMouse& mouse );
	virtual void			iwm_rbuttondown( const ioMouse& mouse );
	virtual void			iwm_rbuttonup( const ioMouse& mouse );	
	virtual void			iwm_mouseover( const ioMouse& mouse );
	virtual void			iwm_mousemove( const ioMouse& mouse );
	virtual void			iwm_mouseleave( const ioMouse& mouse );
	virtual void			iwm_wheel( int zDelta );
	virtual void			iwm_show();	
	virtual void			iwm_hide();
	virtual bool			iwm_esc();
	virtual bool			iwm_spacebar();

	//----------------------------------------------------------------------------------
	/*
	Event
	*/
	void					ClearConnectors();
	void					Connect( WNDEX_EVENT_TYPE etype, ioWndEXEventHandlerBase* handler );
	void					Disconnect( WNDEX_EVENT_TYPE etype, ioWndEX* wnd );
	template < typename ET >
	void					SendEvent( WNDEX_EVENT_TYPE etype, ET& event )
	{
		ConnectMap::iterator iter = m_eventconnectors.find( etype );
		if ( iter == m_eventconnectors.end() ) {
			return;
		}

		HandlerVector::iterator iter2 = iter->second.begin();
		HandlerVector::iterator eiter2 = iter->second.end();
		for ( ; iter2 != eiter2; ++iter2 ) {
			(*iter2)->Call( this, event );
		}
	}

	//----------------------------------------------------------------------------------
	/*
	불러오기 가상함수
	*/
	virtual void			_ParseExtraInfo( ioXMLElement& extxml ) {}

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( WndEX, ioWndEX )
	virtual ioWnd*			_Clone() { return new ioWndEX( m_extype ); }
	virtual void			SetSaveData( ioXMLElement& xml );
	virtual void			_SetSaveData( ioXMLElement& extxml ) {}
	// event
	void					SetConnectFunctionName( WNDEX_EVENT_TYPE etype, const char* name );
	const char*				GetConnectFunctionName( WNDEX_EVENT_TYPE etype );

	//----------------------------------------------------------------------------------
	/*
	편의 함수들
	*/
	bool					IsInRect( ioWnd** childs, int cnum, int widthweight, int heightweight, const POINT& pos );
	static void				SetRenderElementSize( ioUIRenderElement* elem, int width, int height );
	static void				ChangeFrame( ioUIRenderFrame** frame, const char* templatename, ioGUIManager* mgr );
	static void				ChangeImageByFileName( ioUIRenderImage** image, const char* filename, ioGUIManager* mgr );
	static void				ChangeImageByImageSet( ioUIRenderImage** image, const char* imagesetname, const char* imagename, ioGUIManager* mgr );

protected:

	IOWNDEX_TYPE			m_extype;

	ConnectMap				m_eventconnectors;
	ConnectFunctionNameMap	m_connectfuncnames;
};


#endif // _IOWNDEX_H_