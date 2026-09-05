#pragma once
#include "ioDummyGroup.h"


class ioWeapon;
class ioBaseChar;
class ioPlayEntity;
class SP2Packet;
class ioINILoader;
class ioPlayStage;
class ioDummyChar;
class ioPlayMode;

struct StDummyNoOverLapTracking
{
	ioHashString m_stCreaterName;
	ioHashString m_stTargetName;
};

class ioDummyCharManager : public Singleton< ioDummyCharManager > 
{
public:
	typedef std::vector< ioDummyChar* > DummyCharList;

protected:
	ioPlayStage*			m_pPlayStage;

	vDummyCharBaseInfoList	m_DummyCharBaseInfoList;	
	DummyCharList			m_DummyCharList;


protected:
	typedef std::vector< StDummyNoOverLapTracking > DummyNoOverLapTrackingList;
	DummyNoOverLapTrackingList	m_DummyNoOverLapTrackingList;
	int							m_iLocalIndex;
	ioDummyGroupList			m_DummyGroupList;

public:
	void LoadDummyCharBaseInfo();

	ioDummyChar* CreateDummyChar( ioHashString szModelName, ioPlayMode *pMode, int iType );
	ioDummyChar* CreateDummyCharByOwnerGroup( ioBaseChar *pOwner, ioPlayMode *pMode, int iType );

	void DestroyDummyCharList();
	
	void ClearDeadDummyChar();
	void ClearDummyCharByOwner( ioBaseChar *pChar );

	void ClearMapDummy();

protected:
	void LoadInfoByChar();
	void LoadInfoByNPC();

public:
	bool GetDummyCharBaseInfo( IN const ioHashString &szName, OUT DummyCharBaseInfo &rkInfo );

	ioDummyChar* FindDummyChar( ioBaseChar *pChar, int iIndex );
	ioDummyChar* FindDummyCharToName( ioBaseChar *pChar, const ioHashString &szDummyCharName );
	ioDummyChar* FindMyDummyCharToName( ioBaseChar *pChar, const ioHashString &szDummyCharName );
	void FindAllDummyCharToName( IN ioBaseChar *pChar, IN const ioHashString &szDummyCharName, OUT DummyCharList& List );

	int GetDummyCharCntToName( ioBaseChar *pChar, const ioHashString &szDummyCharName );
	void SetReserveAttackCode( ioBaseChar *pChar, int iCode );

	int GetCurLocalIndex( bool bIncreaseIndex = true );
	void SetCurLocalIndex( int iIndex );

	int GetDummyCharCnt( ioBaseChar *pChar, bool bSync = false );
	void FillDummyCharInfo( ioBaseChar *pChar, SP2Packet &rkPacket );	

	void FillDummyCharMidSync( ioBaseChar *pChar, SP2Packet &rkPacket );
	void ApplyDummyCharMidSync( ioBaseChar *pChar, SP2Packet &rkPacket );

	void DestroyDummyCharByNameList( ioBaseChar *pChar, ioHashStringVec &rvList );
	void DestroyDummyCharByName( ioBaseChar *pChar, const ioHashString &szName );

	bool GeDisablePosition( D3DXVECTOR3 vPos, float fRadius );

	void CheckEntityCollision( ioBaseChar *pChar, ioPlayEntity *pEntity, int iIndex, bool bDefense );

	bool HasMamaHahaDummy( ioBaseChar *pChar );
	void DestroyMamaHahaDummy( ioBaseChar *pChar );

	void ResetMoveState( ioBaseChar *pChar );
	void ResetDashState( ioBaseChar *pChar );

public:
	bool AddNoOverLapTrackingDummy( const ioHashString& stAttacker, const ioHashString& stTarget );
	bool EraseNoOverLapTrackingDummy( const ioHashString& stAttacker, const ioHashString& stTarget );
	bool IsNoOverLapTrackingDummy( const ioHashString& stAttacker, const ioHashString& stTarget );
	
public:
	int GetCurDummyCharCnt( ioBaseChar *pOwner, ioHashString szDummyName );
	bool CheckMaxCreateDummyCnt( ioBaseChar *pOwner, ioDummyChar *pDummy, int iMaxCnt );

public:
	std::vector< ioDummyChar* > GetDummyCharList() { return m_DummyCharList; }

public:
	int GetDummyGroupCount( const ioHashString& szOwnerName );
	ioDummyGroup* GetDummyGroup( const ioHashString& szOwnerName, const ioHashString& szIdentityName );
	ioDummyGroup* AddDummyGroup( const ioHashString& szOwnerName, const ioHashString& szIdentityName );

	void ProcessCheckDummyGroup();

	void FillDummyGroupInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void ApplyDummyGroupInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );

	bool IsDropZoneCollision( ioBaseChar *pChar );

public:
	static ioDummyCharManager& GetSingleton();

public:
	ioDummyCharManager( ioPlayStage *pStage );
	virtual ~ioDummyCharManager();
};

#define g_DummyCharMgr ioDummyCharManager::GetSingleton()

