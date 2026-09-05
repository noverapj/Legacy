
//----------------------------------------------------------------------------------
/*
Filename	: ioRichLabel.h
Author		: megamegy
Date		: 2013.10.16
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _IORICHLABEL_H_
#define _IORICHLABEL_H_

#include "ioWndEX.h"
#include "ioComplexStringPrinterBase.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRichLabel
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

class __EX ioRichLabel : public ioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	자동 줄바꿈 타입
	*/
	enum LINEFEED_TYPE
	{
		LINEFEED_TYPE_NORMAL,	// 일반적으로 영역넘어가면 개행
		LINEFEED_TYPE_SPACE,	// 영역을 넘어가면 공백을 기준으로 개행
	};

	//----------------------------------------------------------------------------------
	/*
	TextBlock
	*/
	struct TextBlock
	{
		TextBlock( const std::string& txt, int size, TextStyle style, DWORD color )
			: m_text( txt ), m_size( size ), m_style( style ), m_color( color )
		{}

		std::string				m_text;
		int						m_size;
		TextStyle				m_style;
		DWORD					m_color;
	};
	typedef std::vector< TextBlock > TextBlockVec;

	//----------------------------------------------------------------------------------
	/*
	InputInfo : 입력정보
	*/
	struct InputInfo
	{
		void					Reset() { m_txtblock.clear(); m_param = ""; }
		bool					IsParamType() const { return m_txtblock.empty(); }

		TextBlockVec			m_txtblock;
		std::string				m_param;
	};
	
	//----------------------------------------------------------------------------------
	/*
	LineInfo
	*/
	struct Line
	{
		Line() 
			: m_maxheight( 0.0f )
		{}

		float						m_maxheight;
		ioComplexStringPrinterBase	m_printer;
	};	
	typedef std::vector< Line >		LineVec;

	byte							m_curralpharate;

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioRichLabel();
	ioRichLabel( IOWNDEX_TYPE type );
	virtual ~ioRichLabel();

	//----------------------------------------------------------------------------------
	/*
	ioWnd override
	*/
	virtual void				CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight );
	virtual void				ParseExtraInfo( ioXMLElement &xElement );
	virtual void				SetWindowAlpha( int iAlpha );

	//----------------------------------------------------------------------------------
	/*
	Type
	*/
	void						SetHorzAlignType( TextAlignType type ) { m_horztype = type; }
	TextAlignType				GetHorzAlignType() const { return m_horztype; }
	void						SetVertAlignType( TextVertAlign type ) { m_verttype = type; }
	TextVertAlign				GetVertAlignType() const { return m_verttype; }
	void						SetMSVertAlignType( TextVertAlign type ) { m_macroscopic_verttype = type; }
	TextVertAlign				GetMSVertAlignType() const { return m_macroscopic_verttype; }
	void						SetLineFeedType( LINEFEED_TYPE type ) { m_linefeedtype = type; }
	LINEFEED_TYPE				GetLineFeedType() const { return m_linefeedtype; }

	//----------------------------------------------------------------------------------
	/*
	Control
	*/
	void						Clear();
	void						ClearText();
	void						AddText( int size, TextStyle style, DWORD color, const char* txt, bool submitinputinfo = true );
	void						AddText( int size, TextStyle style, DWORD color, const std::string& txt, bool submitinputinfo = true );
	void						RemoveText( int i );
	void						SetText( ioXMLElement& xElement );
	void						SetText( const std::string& param );
	void						RecalcTextLine();
	float						GetTextHeight() const { return m_textheight; }
	void						SetVertGap( float gap ) { m_vertgap = gap; }
	float						GetVertGap() const { return m_vertgap; }
	int							GetNumInputInfo() const { return static_cast<int>( m_inputinfo.m_txtblock.size() ); }
	TextBlock*					GetInputInfo( int i );
	const char*					GetInputInfoText( int i );
	int							GetInputInfoSize( int i );
	TextStyle					GetInputInfoStyle( int i );
	DWORD						GetInputInfoColor( int i );
	void						SetInputInfoText( int i, const char* text );
	void						SetInputInfoSize( int i, int size );
	void						SetInputInfoStyle( int i, TextStyle style );
	void						SetInputInfoColor( int i, DWORD color );

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( RichLabel, ioRichLabel )
	virtual ioWnd*				_Clone() { return new ioRichLabel(); }	
	virtual void				_SetSaveData( ioXMLElement& extxml );
	virtual void				Copy( ioWnd* wnd );
	
protected:

	//----------------------------------------------------------------------------------
	/*
	ioWnd override
	*/
	virtual void				OnRender();

	//----------------------------------------------------------------------------------
	/*
	Text
	*/
	void						AddTextDetail( int size, TextStyle style, DWORD color, const std::wstring& txt );
	void						AddText( Line& line, int size, TextStyle style, DWORD color, const std::string& txt );

protected:

	TextAlignType				m_horztype;
	TextVertAlign				m_verttype;
	TextVertAlign				m_macroscopic_verttype; // 거시적 세로 정렬 타입 ( Lines를 하나로 보고 정렬 한다 )
	LINEFEED_TYPE				m_linefeedtype;

	float						m_vertgap;

	LineVec						m_lines;

	float						m_textheight;

	InputInfo					m_inputinfo;
};

#endif // _IORICHLABEL_H_