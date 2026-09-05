#pragma once

#include "../io3DEngine/io3DCommon.h"
#include "../io3DEngine/ioPrerequisites.h"
#include "../io3DEngine/ioSingleton.h"

#include "ioAnnounceLevel.h"

class ioItem;
class ioAnnounce;

class ioAnnounceManager : public Singleton< ioAnnounceManager >
{
protected:
	typedef std::list< ioAnnounce* > AnnounceList;
	AnnounceList m_AnnounceList;
	DWORD m_dwAnnounceInterval;

	struct AnnounceGreater
	{
		bool operator()( const ioAnnounce *a, const ioAnnounce *b ) const;
	};

	ioHashString m_ReLoadSound;
	ioHashString m_NeedMPSound;
	ioHashString m_TimeLimitSound;
	ioHashString m_ItemSupplySound;

	ioHashString m_SymbolDefenseMySound;
	ioHashString m_SymbolDefenseOtherSound;

public:
	void SetAnnounceInterval( DWORD dwInterval );

	void UpdateAnnouce();
	void RenderAnnounce();
	void ClearAnnounceList();

	bool IsNowAnnouncing() const;
	void DeleteAnnounce( int iLevel );

public:
	void GameReadyAnnounce( DWORD dwStartGapTime,
							TeamType eOwnerTeam,
							int iCurRound,
							int iHalfRound );

	void NormalAnnounce( const char *szTitle,
						 const char *szAnnounce,
						 DWORD dwTitleColor,
						 int iLevel = ANN_DEFAULT );

	void LevelupAnnounce( int iStr, int iDex, int iInt, int iVit );
	void GetItemAnnounce( const ioItem *pItem );
	void RevivalAnnounce( DWORD dwRevivalGapTime );

	void SymbolDefenseAnnounce( DWORD dwDefenseTime, bool bDefenseMode );
	void ExitPrisonerModeAnnounce( DWORD dwExitPrisonerModeTime, bool bExitPrisonerMode );
	void TimeLimitAnnounce( DWORD dwTimeLimit );

	void ItemSupplyAnnounce( const ioHashStringVec &vNameList );

	void NeedMoreMPAnnouce();
	void ReLoadAmmoAnnounce();
	void NeedMoreZenAnnouce();

protected:
	void AddNewAnnounce( ioAnnounce *pNewAnnounce, DWORD dwLiveTime );

public:
	static ioAnnounceManager& GetSingleton();

public:
	ioAnnounceManager();
	virtual ~ioAnnounceManager();
};

#define g_AnnounceMgr ioAnnounceManager::GetSingleton()

