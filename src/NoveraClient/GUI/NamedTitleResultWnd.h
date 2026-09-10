#pragma once

class NamedTitleResultWnd : public ioWnd
{
public: 
	enum 
	{
		ID_GO	= 1,

		MAXLINE = 10,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pBackEffect;

	ioComplexStringPrinter m_Desc[MAXLINE];

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	void ShowResult( DWORD dwCode, int nLevel, BYTE byPremium );

public:
	NamedTitleResultWnd();
	virtual ~NamedTitleResultWnd();
};

class NamedTitlePremiumResultWnd : public ioWnd
{
public: 
	enum 
	{
		ID_GO	= 1,
		ID_USE	= 2,

		MAXLINE = 10,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pBackEffect;

	ioComplexStringPrinter m_Desc[MAXLINE];

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

protected:
	void ReBatchButton();

public:
	void ShowResult( DWORD dwCode, int nLevel, BYTE byPremium );

public:
	NamedTitlePremiumResultWnd();
	virtual ~NamedTitlePremiumResultWnd();
};