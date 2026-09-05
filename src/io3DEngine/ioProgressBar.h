

#ifndef _ioProgressBar_h_
#define _ioProgressBar_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioWnd.h"

class __EX ioProgressBar : public ioWnd
{
public:
	enum ProgressAlign
	{
		PA_LEFT,
		PA_RIGHT,
	};

protected:
	int m_iCurPos;
	int m_iMaxPos;

	ProgressAlign m_FillAlign;

	ioUIRenderImage *m_pBack;
	ioUIRenderImage *m_pGraph;

	DWORD m_dwGraphColor;

public:
	void SetCurPosition( int iCurPos );
	void SetMaxPosition( int iMaxPos );

	void SetProgressAlign( ProgressAlign eAlign );
	void SetProgressAlign( const ioHashString &szAlign );

	void SetGraphColor( DWORD dwColor );
	
public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual bool IsCanMouseReaction() const;
	virtual DWORD GetWndType() const;

	//************************************************************************************************************************
	// Tool을 위한 인터페이스들
	virtual void SetSaveData( ioXMLElement& xml );
	DECL_NAME_INTERFACE_FOR_TOOL( ProgressBar, ioProgressBar )
	virtual ioWnd* _Clone() { return new ioProgressBar(); }
	virtual void Copy( ioWnd* wnd );
	DWORD GetGraphColor() const { return m_dwGraphColor; }
	int	GetProgressAlign() const { return m_FillAlign; }
	int GetMaxPos() const { return m_iMaxPos; }
	virtual int GetNumFixedRenderImage() const;
	virtual const char* GetFixedRenderImageType( int index );	
	virtual ioUIRenderElement** GetRenderImage( const char* type );
	//************************************************************************************************************************

protected:
	virtual void OnRender();

public:
	ioProgressBar();
	virtual ~ioProgressBar();
};

#endif