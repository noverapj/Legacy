#pragma once

#include "../io3DEngine/ioMovingWnd.h"

class ioXMLElement;
class TimeGateSearch : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
	};

	enum
	{
		ST_PLAZA      = 1,
		ST_PARTY_JOIN = 2,
	};

protected:
	struct ScaleImage
	{
		ioUIRenderImage *m_pImage;
		DWORD m_dwScaleTime;
		float m_fCurScale;
	};

	typedef std::vector< ScaleImage* > ScaleImageList;
	ScaleImageList m_ScaleImageList;
	ioUIRenderImage *m_pScaleImageBack;

	static int  m_iReserveType;
	static bool m_bSafetyRoom;
	
	static DWORD m_dwTrainingStartTime;
	static DWORD m_dwPartyJoinStartTime;
	static DWORD m_dwPlazaStartTime;
	static DWORD m_dwPartyJoinTryCount;

protected:
	DWORD m_dwChangeDuration;
	float m_fBigScale;
	float m_fSmallScale;
	DWORD m_dwChangeTime;
	int   m_iCurBigIdx;	
	bool  m_bReverse;
	DWORD m_dwBattleJoinTry;
	DWORD m_dwBattleJoinTrySec;
	DWORD m_dwAutoSearchTime;

	ioHashString m_szCurTitle;

	int   m_iSearchType;

	bool  m_bPenaltyPeso;

	void SendSearch();
	void SendCancel();
public:
	inline void SetReserve() { m_iReserveType = m_iSearchType; }
	inline void ClearReserve() { m_iReserveType = 0; }
	inline int GetSearchType(){ return m_iSearchType; }
	void SetSearching( const ioHashString &szTitle, int eSearchType, bool bReserve=false, bool bSafetyRoom=false );
	void DoReserve();
	
protected:
	void ScaleIconImage();
	void AutoSearchProcess();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
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
	TimeGateSearch();
	virtual ~TimeGateSearch();
};
//////////////////////////////////////////////////////////////////////////
class TimeGateSearchResult : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_SEARCH_CORRECTION = 2,
		ID_RESEARCH = 3,
		ID_CLOSE = 4,
	};

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();

protected:
	virtual void OnRender();

public:
	TimeGateSearchResult();
	virtual ~TimeGateSearchResult();
};

