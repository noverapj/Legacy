#pragma once

class ioXMLElement;

#define SEARCH_TIME_LIMIT              30000
#define CHANGE_DURATION_PROCESS_CIRCLE 200
#define CHANGE_DURATION_MODE_IMAGE     600

class TimeGateSearchShuffle : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_CANCEL = 2,
	};

	enum
	{
		SUN_    = 100,
		MON_    = 101,
		TUE_    = 102,
		WED_    = 103,
		THU_    = 104,
		FRI_    = 105,
		SAT_    = 106,
		DEFAULT = 107, //날짜를 못찾을때 출력		
	};

protected:
	ioUIRenderImage *m_pProcessCircle[2];	
	ioUIRenderImage *m_pModeImageBack;
	typedef std::vector<ioUIRenderImage*> UIRenderImageVec;
	UIRenderImageVec m_pModeImageVec;

	ioUIRenderImage *m_pCurProcessCircle;
	ioUIRenderImage *m_pCurModeImage;



protected:
	DWORD m_dwChangeDuration;
	DWORD m_dwChangeTime;

	DWORD m_dwChangeProcessCicleTime;
	DWORD m_dwChangeModeImageTime;

	int m_iProcessArray;
	int m_iModeArray;

protected:
	DWORD m_dwCurrSerachTime;

protected:
	void SendSearch();
	void SendCancel();
	void ChangeProcessCircle();
	void ChangeModeImage();

public:
	void SetSearching();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	
public:
	TimeGateSearchShuffle();
	virtual ~TimeGateSearchShuffle();
};

//////////////////////////////////////////////////////////////////////////
class TimeGateSearchShuffleResult : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_RESEARCH = 2,
		ID_CLOSE = 3,
	};

	enum
	{
		SUN_    = 100,
		MON_    = 101,
		TUE_    = 102,
		WED_    = 103,
		THU_    = 104,
		FRI_    = 105,
		SAT_    = 106,
		DEFAULT = 107, //날짜를 못찾을때 출력		
	};

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();

public:
	TimeGateSearchShuffleResult();
	virtual ~TimeGateSearchShuffleResult();
};

