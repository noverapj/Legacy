// HeroRankManager.h: interface for the HeroRankManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HERORANKMANAGER_H__38D8F28B_C7F4_4DA8_8278_70870304D390__INCLUDED_)
#define AFX_HERORANKMANAGER_H__38D8F28B_C7F4_4DA8_8278_70870304D390__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQueryResultData;
using namespace std;
typedef struct tagHeroRankData
{
	int   m_iRank;
	DWORD m_dwUserIndex;
	int   m_iGradeLevel;
	ioHashString m_szPublicID;
	int   m_iHeroTitle;
	int   m_iHeroWin;
	int   m_iHeroLose;
	int   m_iCampPos;
	int   m_iHeroExpert;
	bool  m_bLogIn;
	tagHeroRankData()
	{
		m_dwUserIndex = 0;
		m_iRank = m_iGradeLevel = m_iHeroTitle = m_iHeroWin = m_iHeroLose = m_iCampPos = m_iHeroExpert = 0;
		m_bLogIn = false;
	}
}HeroRankData;
typedef vector<HeroRankData> vHeroRankData;

class HeroRankManager
{
private:
	static HeroRankManager *sg_Instance;
	
public:
	static HeroRankManager &GetInstance();
	static void ReleaseInstance();
//////////////////////////////////////////////////////////////////////////
protected:
	vHeroRankData m_HeroRankList;

protected:
	DWORD m_dwCurrentTime;
	DWORD m_dwTop100SyncDate;
	DWORD m_dwUserDataSyncDate;

public:
	void DBtoData( CQueryResultData *query_data );	

public:
	void UpdateProcess();

public:
	void OnSelectHeroRank( DWORD dwUpdateDate = 0 );

public:
	void CheckLogIn( DWORD dwUserIndex, const ioHashString &rkPublicID );
	void CheckLogOut( DWORD dwUserIndex );
	void CheckCamp( DWORD dwUserIndex, int iCampPos );

public:
	void SendCurTop100Data( User *pUser, int iCurPage, int iMaxCount );

public:
	int GetTop100UserExpert( int iRank );

private:     	/* Singleton Class */
	HeroRankManager();
	virtual ~HeroRankManager();
};
#define g_HeroRankManager HeroRankManager::GetInstance()
#endif // !defined(AFX_HERORANKMANAGER_H__38D8F28B_C7F4_4DA8_8278_70870304D390__INCLUDED_)
