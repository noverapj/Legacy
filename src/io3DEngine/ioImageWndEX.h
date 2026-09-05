//----------------------------------------------------------------------------------
/*
Filename	: ioImageWndEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOIMAGEWNDEX_H_
#define _IOIMAGEWNDEX_H_

#include "ioWndEX.h"
#include "ioUIRenderImage.h"
#include "ioGUIManager.h"
#include "ioUIImageSetManager.h"
#include "ioStringConverter.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
Image 관련 Set / Get 인터페이스를 반복적으로 정의하기 위한 매크로
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
#define DECLARE_IMAGE_INTERFACES( name, member ) \
	DECLARE_RENDERELEMENT_INTERFACES( name, member )\
	void SetReverseFormatting##name( ioUIRenderImage::ReverseFormatting format ) { if (member) dynamic_cast<ioUIRenderImage*>(member)->SetReverseFormatting( format ); }\
	ioUIRenderImage::ReverseFormatting	GetReverseFormatting##name() const { if (member) return dynamic_cast<ioUIRenderImage*>(member)->GetReverseFormatting(); return ioUIRenderImage::RF_NONE_REVERSE; }\
	void SetOffsetX##name( int x ) { if (member) dynamic_cast<ioUIRenderImage*>(member)->SetEXOffsetX(x); }\
	void SetOffsetY##name( int y ) { if (member) dynamic_cast<ioUIRenderImage*>(member)->SetEXOffsetY(y); }\
	int GetOffsetX##name() const { if (member) return dynamic_cast<ioUIRenderImage*>(member)->GetEXOffsetX(); return 0; }\
	int GetOffsetY##name() const { if (member) return dynamic_cast<ioUIRenderImage*>(member)->GetEXOffsetY(); return 0; }\
	bool IsCreationFromFileImage##name() { if (member) return dynamic_cast<ioUIRenderImage*>(member)->IsFromFile(); return false; }\
	void SetFileName##name( const char* filename, ioGUIManager* mgr )\
	{\
		if (!filename || !mgr) return;\
		DWORD color = 0xFFFFFFFF;\
		if (member) color = member->GetColor();\
		SAFEDELETE(member);\
		const char* setimagename = NULL;\
		const char* imagename = NULL;\
		StringVector vparam = ioStringConverter::Split( std::string(filename), "#", 2 );\
		if( vparam.size() == 2 ) {\
			if( strcmp( vparam[0].c_str(), "" ) != 0 ){\
			setimagename = vparam[0].c_str();\
			}\
			if( strcmp( vparam[1].c_str(), "" ) != 0 ){\
				imagename = vparam[1].c_str();\
			}\
		}\
		if( !setimagename || !imagename ) return;\
		member = mgr->CreateImage( setimagename, imagename );\
		if (member) {\
			RECT rt = member->GetRenderRect();\
			rt.left = 0; rt.top = 0;\
			rt.right = GetWidth();\
			rt.bottom = GetHeight();\
			member->SetRenderRect( rt );\
			member->SetColor( color );\
			dynamic_cast<ioUIRenderImage*>(member)->SetImageSetName( setimagename );\
			dynamic_cast<ioUIRenderImage*>(member)->SetImageName( imagename );\
			dynamic_cast<ioUIRenderImage*>(member)->SetImageFileName( filename );\
			dynamic_cast<ioUIRenderImage*>(member)->SetAutoSize( true );\
			dynamic_cast<ioUIRenderImage*>(member)->SetUseOffset( false );\
			RecalcSize();\
		}\
	}\
	const char*	GetFileName##name() const { if (member) return dynamic_cast<ioUIRenderImage*>(member)->GetImageFileName().c_str(); return 0; }\
	void SetImageSet##name( const char* imagesetname, const char* imagename, ioGUIManager* mgr )\
	{\
		if (!imagesetname || !imagename || !mgr) return;\
		DWORD color = 0xFFFFFFFF;\
		if (member) color = member->GetColor();\
		SAFEDELETE(member);\
		member = mgr->CreateImage( imagesetname, imagename );\
		if (member) {\
			RECT rt = member->GetRenderRect();\
			rt.left = 0; rt.top = 0;\
			rt.right = GetWidth();\
			rt.bottom = GetHeight();\
			member->SetRenderRect( rt );\
			member->SetColor( color );\
			dynamic_cast<ioUIRenderImage*>(member)->SetImageName( imagename );\
			dynamic_cast<ioUIRenderImage*>(member)->SetImageSetName( imagesetname );\
			dynamic_cast<ioUIRenderImage*>(member)->SetImageFileName( imagesetname, imagename );\
			dynamic_cast<ioUIRenderImage*>(member)->SetAutoSize( true );\
			dynamic_cast<ioUIRenderImage*>(member)->SetUseOffset( false );\
			RecalcSize();\
		}\
	}\
	void SetImageSet##name( const char* fullname )\
	{\
		if(!fullname) {\
			return;\
		}\
		DWORD color = 0xFFFFFFFF;\
		if ( member ) color = member->GetColor();\
		SAFEDELETE( member );\
		const char* setimagename = NULL;\
		const char* imagename = NULL;\
		StringVector vparam = ioStringConverter::Split( std::string(fullname), "#", 2 );\
		if( vparam.size() == 2 ){\
			if( strcmp( vparam[0].c_str(), "" ) != 0 ){\
			setimagename = vparam[0].c_str();\
			}\
			if( strcmp( vparam[1].c_str(), "" ) != 0 ){\
				imagename = vparam[1].c_str();\
			}\
		}\
		if( !setimagename || !imagename ) return;\
		member = g_UIImageSetMgr.CreateImage( setimagename, imagename );\
		if(member) {\
			RECT rt = member->GetRenderRect();\
			rt.left = 0;\
			rt.top = 0;\
			rt.right = GetWidth();\
			rt.bottom = GetHeight();\
			member->SetRenderRect( rt );\
			member->SetColor( color );\
			dynamic_cast<ioUIRenderImage*>(member)->SetImageSetName( setimagename );\
			dynamic_cast<ioUIRenderImage*>(member)->SetImageName( imagename );\
			dynamic_cast<ioUIRenderImage*>(member)->SetImageFileName( fullname );\
			dynamic_cast<ioUIRenderImage*>(member)->SetAutoSize( true );\
			dynamic_cast<ioUIRenderImage*>(member)->SetUseOffset( false );\
			RecalcSize();\
		}\
	}\
	const char*	GetImageSetName##name() const { if (member) return dynamic_cast<ioUIRenderImage*>(member)->GetImageSetName().c_str(); return 0; }\
	const char*	GetImageName##name() const { if (member) return dynamic_cast<ioUIRenderImage*>(member)->GetImageName().c_str(); return 0; }


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioImageWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioImageWndEX : public ioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioImageWndEX( ioGUIManager* mgr );
	ioImageWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioImageWndEX();

	//----------------------------------------------------------------------------------
	/*
	ioWnd override
	*/
	virtual void						SetSize( int width, int height );
	//
	virtual void						OnRender();
	virtual void						CheckFrameReSize( int width, int height, int prevwidth, int prevheight );
	virtual void						SetWindowAlpha( int alpha );
	virtual bool						OnRenderShowAndHideByTop( DWORD dwCheckTime );

	//----------------------------------------------------------------------------------
	/*
	Controls
	*/
	DECLARE_IMAGE_INTERFACES( , m_image )
	//
	void								SetNoMagicAlign( bool b ) { m_nomagicalign = b; }
	bool								GetNoMagicAlign() const { return m_nomagicalign; }
	//
	void								SetImage( ioUIRenderImage* image );

	//----------------------------------------------------------------------------------
	/*
	불러오기 가상함수
	*/
	virtual void						_ParseExtraInfo( ioXMLElement& extxml );
	
	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( ImageWnd, ioImageWndEX )
	virtual ioWnd*						_Clone() { return new ioImageWndEX( m_guimgr ); }
	virtual void						Copy( ioWnd* wnd );
	virtual void						_SetSaveData( ioXMLElement& extxml );

protected:

	ioGUIManager*						m_guimgr;
	ioUIRenderImage*					m_image;
	bool								m_nomagicalign;
};


#endif // _IOIMAGEWNDEX_H_