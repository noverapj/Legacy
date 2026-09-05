#pragma once


class ioWeapon;
class ioBaseChar;
class ioPlayEntity;
class SP2Packet;
class ioINILoader;
class ioPlayStage;
class ioPushStruct;
class ioPlayMode;

class ioPushStructListManager : public Singleton< ioPushStructListManager >
{
protected:
	ioINILoader m_StructLoader;

	typedef std::vector< ioPushStruct* > PushList;
	PushList m_vPushStructList;

	ioPlayStage *m_pPlayStage;

	int m_iLocalIndex;

protected:
	int m_iBlockMaxCnt;
	int m_iBlockMyRoomMaxCnt;
	int m_iBlockEtcMaxCnt;
	ioHashString m_szBlockCreateLoopAni;
	ioHashString m_szBlockCreateActionAni;
	float m_fBlockCreateActionAniRate;

	float m_fBlockGuidSpeed;
	DWORD m_dwBlockCreateDelayTime;

protected:
	void LoadBlockInfo();

public:
	inline const ioINILoader& GetINILoader() const { return m_StructLoader; }

	ioPushStruct* CreatePushStruct( ioHashString szStructName, ioPlayMode *pMode, bool bTestCreate = false );

	void GetPushStructInfo( SP2Packet &rkPacket );
	void ApplyPushStructInfo( SP2Packet &rkPacket );

	void DestroyStructList();
	
	void ClearDeadStructList();
	void ClearStructListByOwner( ioBaseChar *pChar );
	void ApplyStructListByOwner( const ioHashString &szName );

	ioPushStruct* CheckMaxStructCnt( ioBaseChar *pOwner, int iStructNum, int iMaxCnt );
	ioPushStruct* CheckMaxStructCntByNum( ioBaseChar *pOwner, int iStructNum, int iMaxCnt );

public:
	ioPushStruct* FindPushStruct( int iArray );
	ioPushStruct* GetPushStruct( int iIndex );
	int FindUserPushStruct( ioBaseChar *pOwner );

	ioPushStruct* FindFishingGround( ioHashString szName, int iStructNum );
	ioPushStruct* FindFishingGroundByEtcCode( ioHashString szName, DWORD dwEtcCode );
	void DeleteAllFishingGround( ioBaseChar* pChar );
	void DeleteSelectFishingGround( ioBaseChar* pChar, DWORD dwEtcItemCode );

public:
	inline int GetPushStructCnt() const { return m_vPushStructList.size(); }
	inline int GetCurLocalIndex() const { return m_iLocalIndex; }

	inline int GetBlockMaxCnt() const { return m_iBlockMaxCnt; }
	inline int GetBlockMyRoomMaxCnt() const { return m_iBlockMyRoomMaxCnt; }
	inline int GetBlockEtcMaxCnt() const { return m_iBlockEtcMaxCnt; }

	const ioHashString& GetBlockCreateLoopAni();
	const ioHashString& GetBlockCreateActionAni();
	float GetBlockCreateActionAniRate();
	float GetBlockGuidSpeed();
	DWORD GetBlockCreateDelayTime();

public:
	int GetCurPushStructCntByType( PushStructType eType );
	int GetCurPushStructCntByEtcCode( ioBaseChar *pChar, DWORD dwEtcCode );

	bool CheckMaxStructCntForRoom();
	bool CheckMaxStructCntForMyRoom();
	bool CheckMaxStructCntForEtcCode( ioBaseChar *pChar, DWORD dwEtcCode );

	bool GeDisablePosition( D3DXVECTOR3 vPos, float fRadius );

public:
	static ioPushStructListManager& GetSingleton();

public:
	ioPushStructListManager( ioPlayStage *pStage );
	virtual ~ioPushStructListManager();
};

#define g_PushStructListMgr ioPushStructListManager::GetSingleton()

