//----------------------------------------------------------------------------------
/*
Filename	: ioFrameWndEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOFRAMEWNDEX_H_
#define _IOFRAMEWNDEX_H_

#include "ioWndEX.h"
#include "ioUIRenderFrame.h"
#include "ioGUIManager.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
Frame 관련 Set / Get 인터페이스를 반복적으로 정의하기 위한 매크로
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
#define DECLARE_FRAME_INTERFACES( name, member ) \
	DECLARE_RENDERELEMENT_INTERFACES( name, member )\
	void SetFrameTemplateName##name(const char* templatename, ioGUIManager* mgr)\
	{\
		if (!mgr || !templatename) return;\
		DWORD color = 0xFFFFFFFF;\
		if (member) color = member->GetColor();\
		SAFEDELETE(member);\
		member = mgr->CloneFrame(templatename);\
		if (member) {\
			RECT rt = member->GetRenderRect();\
			rt.left = 0; rt.top = 0;\
			rt.right = GetWidth();\
			rt.bottom = GetHeight();\
			member->SetRenderRect(rt);\
			member->SetColor(color);\
			RecalcSize();\
		}\
	}\
	const char*	GetFrameTemplateName##name() const { if (member) return dynamic_cast<ioUIRenderFrame*>(member)->GetName().c_str(); return 0; }


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioFrameWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioFrameWndEX : public ioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioFrameWndEX( ioGUIManager* mgr );
	ioFrameWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioFrameWndEX();

	//----------------------------------------------------------------------------------
	/*
	ioWnd override
	*/
	virtual void			SetSize( int width, int height );
	//
	virtual void			OnRender();
	virtual void			CheckFrameReSize( int width, int height, int prevwidth, int prevheight );
	virtual void			SetWindowAlpha( int alpha );

	//----------------------------------------------------------------------------------
	/*
	Controls
	*/
	DECLARE_FRAME_INTERFACES( , m_frame )
	//
	void					SetNoMagicAlign( bool b ) { m_nomagicalign = b; }
	bool					GetNoMagicAlign() const { return m_nomagicalign; }

	//----------------------------------------------------------------------------------
	/*
	불러오기 가상함수
	*/
	virtual void			_ParseExtraInfo( ioXMLElement& extxml );
	
	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( FrameWnd, ioFrameWndEX )
	virtual ioWnd*			_Clone() { return new ioFrameWndEX( m_guimgr ); }
	virtual void			Copy( ioWnd* wnd );
	virtual void			_SetSaveData( ioXMLElement& extxml );

protected:

	ioGUIManager*			m_guimgr;

	ioUIRenderFrame*		m_frame;

	bool					m_nomagicalign;
};


#endif // _IOFRAMEWNDEX_H_