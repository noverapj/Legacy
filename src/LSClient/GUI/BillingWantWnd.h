#pragma once

class ioPlayStage;
class BillingWantWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT					= 1,
		ID_GOLD_CHARGE			= 2,
		ID_CANCEL				= 3,
		ID_GOLD_CHARGE_PRESENT	= 4,
		ID_CANCEL_PRESENT		= 5,
	};

	// UI 사이즈 변경 
	enum
	{
		PRESENT_WIDTH			= 270,
		PRESENT_HEIGHT			= 318,
		PRESENT_X_OFFSET		= 236,

		NORMAL_WIDTH			= 250,
		NORMAL_HEIGHT			= 258,
		NORMAL_X_OFFSET			= 216,
	};

	enum
	{
		X_OFFSET				= 125, 
		X_PRESENT_OFFSET		= 135, 
		Y_OFFSET				= 144, 
		Y_LINE_GAP				= 20, 
		WIDTH_CUT				= 236, 

		ICON_X_OFFSET			= 125,
		ICON_Y_OFFSET			= 98,
		ICON_X_PRESENT_OFFSET	= 135, 
	};

protected:
	ioPlayStage*		m_pPlayStage;
	bool				m_bPresent;
	ioUIRenderFrame*	m_pBottomFrm;
	ioUIRenderImage*	m_pIcon;
	ioUIRenderImage*	m_pIconBack;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnRender();

protected:
	void ReSizeUI( bool bPresent );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void ShowPresent();

public:
	BillingWantWnd();
	virtual ~BillingWantWnd();
};