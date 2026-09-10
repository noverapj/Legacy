#pragma once

#include "../io3DEngine/ioStringConverter.h"
#include "ioComplexStringPrinter.h"

class ioRenderSystem;
class ioUIRenderer;
class ioUIRenderImage;
class ioUIRenderFrame;

class ioGameAnnMgr : public Singleton< ioGameAnnMgr >
{
public:
	enum
	{
		BATTLE_STYLE		= 1,
		LADDER_STYLE		= 2,
		ALL_BATTLE_STYLE	= 3,
	};

protected:	
	ioUIRenderImage *m_pBackground;
	
public:
	struct AnnounceData
	{
		DWORD m_dwSeqIndex;
		DWORD m_dwStartDate;
		DWORD m_dwEndDate;
		DWORD m_dwRotateTime;
		DWORD m_dwCurRotateTime;
		DWORD m_dwRenderTime;

		int   m_iCheckModeType;              // 발생시 해당 모드 체크
		int   m_iCheckRoomStyle;             // 발생시 룸 스타일(전투or진영) 체크

		bool  m_bShutDownUser;
		int   m_iPriority;

#ifndef TEST_SUITE_ON
		ioComplexStringPrinter m_AnnounceText;
#endif
		AnnounceData()
		{
			m_dwSeqIndex      = 0;
			m_dwStartDate     = m_dwEndDate = m_dwRotateTime = 0;
			m_dwCurRotateTime = 0;
			m_dwRenderTime    = 0;

			m_iCheckModeType = -1;
			m_iCheckRoomStyle= -1;

			m_bShutDownUser = false;
			m_iPriority     = 99999999; // 낮을 수록 우선순위가 높음 
		}
	};
	typedef std::vector < AnnounceData > vAnnounceData;

	class DataSort : public std::binary_function< const ioGameAnnMgr::AnnounceData&, const ioGameAnnMgr::AnnounceData&, bool >
	{
	public:
		bool operator()( const ioGameAnnMgr::AnnounceData &lhs , const ioGameAnnMgr::AnnounceData &rhs ) const
		{
			if( lhs.m_iPriority < rhs.m_iPriority )
				return true;
			return false;
		}
	};

protected:
	vAnnounceData m_AnnounceList;
	vAnnounceData m_CurrentAnnLIst;

	DWORD m_dwCurProcessTime;

protected:
	ioHashString m_szStartSnd;
	float        m_fSecMoveWidth;
	int          m_iAddWidth;

public:
	static ioGameAnnMgr& GetSingleton();

public:
	void Initialize(); 

public:
	void Process( ModeType eModeType );
	void Render( ioUIRenderer *pUIRender );

public:
	bool IsCurrentList( DWORD dwSeqIndex );

public:
	void InsertServerAnnounce( const ioHashString &rkAnnounce, int iPriority );

public:
	ioGameAnnMgr();
	virtual ~ioGameAnnMgr();

};

#define g_GameAnnMgr ioGameAnnMgr::GetSingleton()

