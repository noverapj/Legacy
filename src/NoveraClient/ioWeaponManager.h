#pragma once

class ioWeapon;
class ioBaseChar;
class ioPlayEntity;
class ioSceneManager;
class SP2Packet;
class ioINILoader;
class ioPlayStage;
class ioMapEffect;
class ioSummonMissileWeapon;
class ioMineWeapon;
class ioMineWeapon2;

#include "../io3DEngine/ioListIterator.h"
#include "WeaponAttribute.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct tagSummonWeaponInfo
{
	DWORD m_dwCreateTime;
	ioSummonMissileWeapon *m_pWeapon;

	tagSummonWeaponInfo()
	{
		m_dwCreateTime = 0;
		m_pWeapon = NULL;
	}
} SummonWeaponInfo;

typedef std::vector< SummonWeaponInfo > SummonWeaponInfoList;

class SummonSort : public std::binary_function< const SummonWeaponInfo&, const SummonWeaponInfo&, bool >
{
public:
	bool operator()( const SummonWeaponInfo &lhs , const SummonWeaponInfo &rhs ) const
	{
		if( lhs.m_dwCreateTime < rhs.m_dwCreateTime )
		{
			return true;
		}

		return false;
	}
};

struct exReserveWeaponCollisionInfo
{
	ioHashString m_szTarget;
};

typedef struct tagCallWeaponInfo
{
	ioHashString m_CreateChar;
	WeaponInfo m_WeaponInfo;
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vDir;	
	DWORD m_dwGenerateTime;
	DWORD m_dwOwnerWeaponIdx;
	bool m_bSend;
	exReserveWeaponCollisionInfo m_exCollisionInfo;
} CallWeaponInfo;

typedef std::vector< CallWeaponInfo > CallWeaponInfoList;

class CallWeaponInfoSort : public std::binary_function< const CallWeaponInfo&, const CallWeaponInfo&, bool >
{
public:
	bool operator()( const CallWeaponInfo &lhs , const CallWeaponInfo &rhs ) const
	{
		if( lhs.m_dwGenerateTime < rhs.m_dwGenerateTime )
		{
			return true;
		}

		return false;
	}
};

typedef struct tagApplyWeaponInfo
{
	ioHashString m_CharName;
	DWORD m_dwWeaponIndex;
	DWORD m_dwApplyTime;
	int m_iApplyType;

	tagApplyWeaponInfo()
	{
		m_dwWeaponIndex = 0;
		m_dwApplyTime = 0;
		m_iApplyType = 0;
	}
} ApplyWeaponInfo;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioWeaponManager : public Singleton< ioWeaponManager >
{
public:
	typedef std::list< ioWeapon* > ActiveWeaponList;
	typedef ioListIterator< ActiveWeaponList >WeaponIterator;
	typedef std::vector< ioINILoader* > GroupLoaderList;
	typedef std::vector< ioMineWeapon2* > TraceMineWeaponList;

	typedef std::map< int, ioINILoader* > GroupLoaderMap;

	//
	typedef std::vector< ApplyWeaponInfo > ApplyWeaponInfoList;
	typedef std::map< ioHashString, int > ApplyWeaponChecker;
	typedef std::map< DWORD, WeaponEventType > WeaponEventMap;

protected:
	WeaponAttributeList m_vWeaponAttributeList;
	ActiveWeaponList	m_vWeaponList;

	ActiveWeaponList	m_vMineBufferList;
	ActiveWeaponList	m_vTrackingMineBufferList;
	ActiveWeaponList	m_vTracking3MineBufferList;
	ActiveWeaponList	m_vAirMineBufferList;

	GroupLoaderList m_vGroupLoaderList;
	GroupLoaderMap m_GroupLoaderMap;

	ioSceneManager *m_pSceneMgr;
	ioPlayStage *m_pPlayStage;

	typedef std::set< DWORD > MissingMineIndex;
	MissingMineIndex m_MissingMineIndex;

	CallWeaponInfoList m_vCallWeaponInfoList;

	//
	ApplyWeaponInfoList m_CheckWeaponInfoList;
	ApplyWeaponInfoList m_ApplyWeaponInfoList;

	ApplyWeaponChecker m_ApplyWeaponChecker;

protected:
	WeaponEventMap m_WeaponEventMap;

	int			   m_iLocalIndex;

public:
	int GetCurLocalIndex();

public:
	void InitManager();
	void SetPlayStage( ioPlayStage *pStage );

	void ProcessWeaponList( float fTimePerSec );
	void ClearDeadWeaponList();
	void DestroyWeaponList();

	void ClearWeaponInfo();

	inline ioPlayStage* GetCreator() { return m_pPlayStage; }

protected:
	void LoadMercenaryInfo();
	void LoadNPCInfo();
	void LoadAccessoryInfo();

public:
	ioWeapon* CreateWeapon( int iAttackIndex, ioBaseChar *pOwner );
	void DestroyReserveOwnerWeapon( const ioHashString &rkOwner );
	void DestroyReserveContactWeapon( const ioHashString &rkOwner );
	void DestroyReserveZoneWeapon( const ioHashString &rkOwner );
	void DestroyDummyCharWeapon( const ioHashString &rkOwner, int iDummyIdx );
	void DestroyReserveOwenrWeaponExceptCharChange( const ioHashString &rkOwner );

	void DestroyReserveAdhesive3( const ioHashString &rkOwner );
	void DestroyReserveAdhesive4( const ioHashString &rkOwner );
	void CheckAdhesiveWeapon3( const ioHashString &rkOwner );
	void CheckAdhesiveWeapon4( const ioHashString &rkOwner );
	void CheckRotateAttachCharWeapon( const ioHashString &rkOwner );

public:
	const WeaponAttribute* FindAttribute( int iAttributeIdx );
	void CheckMaxWeaponCnt( ioWeapon *pWeapon, int iMaxCnt );
	bool CheckRotationWeaponByItem( const ioHashString &szName, int iItemIndex );
	void RemoveRotationWeaponByWound( const ioHashString &szName );
	void RemoveAllTrackingWeapon( ioPlayStage* pStage, bool bSend );
	void RemoveAllTracking3Weapon( ioPlayStage* pStage, bool bSend );

	void CheckMapAttachWeaponAction( const ioHashString &szName );
	void CheckMaxMapAttachWeaponCnt( ioWeapon *pWeapon, int iMaxCnt );
	int GetCurMapAttachWeaponCnt( ioBaseChar *pOwner );

	void CheckMaxSummonWeaponCnt( ioWeapon *pWeapon, int iMaxCnt );
	void CheckControlMissileWeaponClear( const ioHashString &szOwnerName );

	bool HasSkillDependencyWeapon( const ioHashString &szOwnerName, const ioHashString &szSkillName );
	void CheckSkillDependencyWeaponAction( const ioHashString &szOwnerName, const ioHashString &szSkillName );
	void CheckSkillDependencyWeaponDead( const ioHashString &szOwnerName, const ioHashString &szSkillName );

	void CheckLobeliaWeaponAction( const ioHashString &szName );
	void CheckMaxLobeliaWeaponCnt( ioWeapon *pWeapon, int iMaxCnt );
	int GetCurLobeliaWeaponCnt( ioBaseChar *pOwner );

	int CheckSetWeaponSylphid( ioBaseChar *pOwner );
	void CheckSylphidMissileWeapon( ioWeapon *pWeapon );

	bool CheckGraffitiWeaponAction( const ioHashString &szName );
	void CheckMaxGraffitiWeaponCnt( ioWeapon *pWeapon, int iMaxCnt );
	int GetCurGraffitiWeaponCnt( ioBaseChar *pOwner );

	void SetReserveCallWeaponInfo( const ioHashString &rkName, const WeaponInfo &rkWeaponInfo,
								   const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir,
								   DWORD dwGenerateTime, DWORD pOwnerWeaponIdx,
								   bool bSend = false );

	void SetReserveCallWeaponCollisionInfo( const ioHashString &rkName, const WeaponInfo &rkWeaponInfo,
										    const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir,
										    DWORD dwGenerateTime, DWORD pOwnerWeaponIdx,
										    exReserveWeaponCollisionInfo exCollisionInfo,
											bool bSend = false );

	void ProcessCallWeaponInfo();

	void SetReturnPingpong( ioBaseChar *pOwner );

protected:
	void ParseWeaponAttribute( ioINILoader &rkLoader, int iAttributeIndex );
	DWORD ParseWeaponFeature( ioINILoader &rkLoader );
	void ParseBlowWoundAniInfo( BlowWoundAniInfo &rkInfo, ioINILoader &rkLoader );

	void LoadActionStop( ioINILoader &rkLoader );
	ActionStopType ParseActionStop( const char *szType );

	// m_WeaponList 에서 반복자를 제거하지 않으므로 반드시 이함수 호출후 제거시켜줘야한다.
	void DestroyWeapon( ioWeapon *pWeapon );

public:
	ioBaseChar* GetWeaponOwnerChar( const ioHashString &szName );
	ioWeapon* FindWeapon( ioBaseChar *pAttacker, DWORD dwWeaponIdx );
	void FindWeaponByAttributeIndex( IN ioBaseChar *pAttacker, IN DWORD dwAttrIndex, OUT DWORDVec& CurrList );
	void FindWeaponByAttributeType( IN ioBaseChar *pAttacker, IN DWORD dwAttrType, OUT DWORDVec& CurrList );

	WeaponIterator GetMineWeaponIterator();
	WeaponIterator GetTrackingMineWeaponIterator();
	WeaponIterator GetTracking3MineWeaponIterator();
	WeaponIterator GetAirMineWeaponIterator();
	DWORD GetCurTrackingMineCnt( ioBaseChar* pOwner );

	WeaponIterator GetWeaponIterator();
	int GetAttackAttributeCnt() const;

public:
	int  GetCurMineWeaponCnt( ioBaseChar *pOwner, MineType eType );
	int GetWeaponCnt( ioBaseChar *pOwner, int nType );

	void FillMineInfo( ioBaseChar *pOwner, SP2Packet &rkPacket, MineType eType );
	void FillMapAttachInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void RegisterMissingMineIndex( DWORD dwIndex );
	bool CheckMissingIngMineIndex( DWORD dwIndex );
	void ClearMissingMineIndex();

	bool HasControlReturnWeapon( ioBaseChar *pOwner );
	bool HasWeaponType( ioBaseChar *pOwner, int iType );

	void AddApplyWeaponChecker( const ioHashString &szCharName );

	void AddCheckWeaponInfo( const ioHashString &szCharName, ioWeapon *pWeapon, int iType );
	void ProcessApplyWeaponInfoList();

	void CheckApplyWeaponWounded( const ioHashString &szCharName, DWORD dwWeaponIndex, DWORD dwAttributeIndex );
	void CheckApplyWeaponDefense( const ioHashString &szCharName, DWORD dwWeaponIndex, DWORD dwAttributeIndex );

	void ClearApplyWeaponInfo();

	WeaponEventType GetWeaponEvent( DWORD dwWeaponCreateIdx );
	void AddWeaponEvent( DWORD dwWeaponCreateIdx, WeaponEventType eType );
	void WeaponMapCollisionExecProcedure( ioPlayStage* pStage, ioWeapon* pWeapon );
public:
	static ioWeaponManager& GetSingleton();

public:
	ioWeaponManager( ioSceneManager *pSceneMgr );
	virtual ~ioWeaponManager();
};

#define g_WeaponMgr ioWeaponManager::GetSingleton()

