#pragma once

#include "../io3DEngine/ioStringConverter.h"

class ioUIRenderImage;
class SoldierLimitPopupWnd : public ioWnd
{
public:
	enum
	{
		ID_SHOP		= 1,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;

protected:
	ioPlayStage *m_pPlayStage;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	void SetPlayStage( ioPlayStage *pPlayStage ){ m_pPlayStage = pPlayStage; }

public:
	SoldierLimitPopupWnd();
	virtual ~SoldierLimitPopupWnd();
};
//////////////////////////////////////////////////////////////////////////
class RentalSoldierLimitPopupWnd : public ioWnd
{
public:
	enum
	{
		ID_SOLDIER_SELECT		= 1,
	};

	enum
	{
		LIMIT_SEC = 30,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pCharIcon;

protected:
	ioPlayStage *m_pPlayStage;

protected:
	int		m_iCharType;
	DWORD	m_dwRoomOutTime;
	DWORD   m_dwProcessCheckTime;
	int m_nGradeType;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

protected:
	void ProcessRentalCheck();

public:
	void SetPlayStage( ioPlayStage *pPlayStage ){ m_pPlayStage = pPlayStage; }

public:
	RentalSoldierLimitPopupWnd();
	virtual ~RentalSoldierLimitPopupWnd();
};
