#ifndef __ioExcavationManager_h__
#define __ioExcavationManager_h__

#include "../Util/Singleton.h"
#include "../Util/IORandom.h"

class User;
class ioExcavationManager : public Singleton< ioExcavationManager >
{
protected: 
	enum ReservedTimeType
	{
		RTT_FIRST_CREATE = 0,
		RTT_CREATE       = 1,
		RTT_DELETE       = 2,
	};

	enum 
	{
		FAIL_ARTIFACT_START_TYPE   = 1000,
		EXTRAITEM_START_TYPE       = 10000,
	};
protected:  

	struct ArtifactInfo
	{
		DWORD m_dwIndex;
		DWORD m_dwDeleteTime;
		int   m_iX;
		int   m_iY;
		int   m_iZ;
    
		ArtifactInfo()
		{
			m_dwIndex      = 0;
			m_dwDeleteTime = 0;
			m_iX = 0;
			m_iY = 0;
			m_iZ = 0;
		}
	};

	typedef std::vector<ArtifactInfo> vArtifactInfoVector;

	struct ExcavationInfo
	{
		DWORD               m_iRoomIndex;
		int                 m_iModeSubType; // ¾î¶²¸ÊÀÎÁö Á¾·ù¸¦ ³ªÅ¸³¿
		vArtifactInfoVector m_vArtfactInfoVector;

		ExcavationInfo()
		{
			m_iRoomIndex  = 0;
			m_iModeSubType= 0;
			m_vArtfactInfoVector.clear();
		}

		ArtifactInfo *GetArtifactInfo( DWORD dwArtifactIndex )
		{
			int iSize = m_vArtfactInfoVector.size();
			for (int i = 0; i < iSize ; i++)
			{
				ArtifactInfo &rkArtifactInfo = m_vArtfactInfoVector[i];
				if( rkArtifactInfo.m_dwIndex == dwArtifactIndex )
				{
					return &rkArtifactInfo;
				}
			}

			return NULL;
		}

		int GetArtifactInfoSize()
		{
			return m_vArtfactInfoVector.size();
		}

		ArtifactInfo *GetArtifactInfoByArray( int iArray )
		{
			if( COMPARE( iArray, 0, (int) m_vArtfactInfoVector.size() ) )
				return &m_vArtfactInfoVector[iArray];
			return NULL;
		}
	};

	typedef std::vector<ExcavationInfo> vExcavationInfoVector;


	struct ItemInfo
	{
		DWORD m_dwRand;		// ÀÏ¹Ý È®·ü
		DWORD m_dwPCRand;   // ÇÇ¾¾¹æ È®·ü
		bool  m_bAlarm;
		int   m_iType;
		int   m_iValue1;

		ItemInfo()
		{
			m_dwRand	= 0;
			m_dwPCRand	= 0;
			m_bAlarm	= 0;
			m_iType		= 0;
			m_iValue1	= 0;
		}

		DWORD GetRandValue( bool bPCRoom )
		{
			if( bPCRoom )
				return m_dwPCRand;
			return m_dwRand;
		}
	};

	typedef std::vector<ItemInfo> vItemInfoVector;

	struct GradeInfo
	{
		int   m_iType;
		DWORD m_dwRand;		// ÀÏ¹Ý È®·ü
		DWORD m_dwPCRand;   // ÇÇ¾¾¹æ È®·ü
		float m_fRate;

		GradeInfo()
		{
			m_iType		= 0;
			m_dwRand	= 0;
			m_dwPCRand	= 0;
			m_fRate		= 0.0f;
		}

		DWORD GetRandValue( bool bPCRoom )
		{
			if( bPCRoom )
				return m_dwPCRand;
			return m_dwRand;
		}
	};

	typedef std::vector<GradeInfo> vGradeInfoVector;

	struct MapInfo
	{
		int   m_iModeSubType;
		DWORD m_dwLiveDelayTimeMin;
		DWORD m_dwLiveDelayTimeMax;
		int   m_iCollisionLength;
		float m_fRate;

		MapInfo()
		{
			m_iModeSubType              = 0;
			m_dwLiveDelayTimeMin        = 0;
			m_dwLiveDelayTimeMax        = 0;
			m_iCollisionLength          = 0;
			m_fRate                     = 0.0f;
		}
	};

	typedef std::vector<MapInfo> vMapInfoVector;

protected:
	struct LogInfo
	{
		int m_iType;
		int m_iGift;

		LogInfo()
		{
			m_iType = 0;
			m_iGift = 0;
		}
	};

	typedef std::vector<LogInfo> vLogInfoVector;

protected:
	vExcavationInfoVector m_vExcavationInfoVector;
	vItemInfoVector       m_vItemInfoVector;
	vMapInfoVector        m_vMapInfoVector;
	vGradeInfoVector      m_vGradeInfoVector;

	DWORD                 m_dwCurrentTime;

	IORandom              m_RandomTime;
	IORandom              m_RandomItem;
	IORandom              m_RandomGrade;	
	IORandom              m_RandomSuccess;

	DWORD                 m_dwRandomItemSeed;
	DWORD                 m_dwRandomItemPCRoomSeed;
	DWORD                 m_dwRandomGradeSeed;
	DWORD                 m_dwRandomGradePCRoomSeed;

	int                   m_iMaxArtifact;
	int                   m_iCreateTimeOffset;
	int                   m_iRealExcavatingUserTime;
	int                   m_iSuccessRatePerOneUser;
	int                   m_iMaxSuccessRate;
	int                   m_iExcavatingWaitUserTime;                 
	bool                  m_bSendArtifactEnterUser;
	
protected:
	DWORD GetReservedTime( int iModeSubType );
	int   GetRandomItemGradeType( bool bPCRoom );
	float GetItemGradeRate( int iType );
	bool  IsRandomSuccess( User *pUser, Room *pRoom );
	DWORD GetItemRandomValue( bool bPCRoom );
	DWORD GetGradeRandomValue( bool bPCRoom );

	ExcavationInfo *GetExcavationInfo( int iRoomIndex );
	MapInfo        *GetMapInfo( int iModeSubType );
	ItemInfo       *GetItemInfo( int iItemType );

	bool SendCreateArtifact( int iRoomIndex, DWORD dwArtifactIndex, bool bWait );
	void SendDeleteArtifact( int iRoomIndex, DWORD dwArtifactIndex );
	void SendGift( User *pUser, Room *pRoom, ExcavationInfo *pExcavationInfo, DWORD dwArtifactIndex );
	void SendExtraItem( User *pUser, Room *pRoom, DWORD dwArtifactIndex, int iItemType, int iAddExp, int iAddSoldierExp );

	void _OnArtifactPosition( User *pUser, SP2Packet &rkPacket );
	void _OnReseltJugement( User *pUser, SP2Packet &rkPacket);

public:
	void CreateExcavation( int iRoomIndex );
	void DeleteExcavation( int iRoomIndex );
	void ProcessExcavation();

	void EnterUser( User *pUser, int iRoomIndex );
	void CheckSendCreateArtifact( int iRoomIndex ); 

	void OnExcavationPacket( User *pUser, SP2Packet &rkPacket  );

	void CheckNeedReload();
	void LoadExcavation();

	int  GetRealExcavatingUserTime() const { return m_iRealExcavatingUserTime; }

public:
	static ioExcavationManager& GetSingleton();

public:
	ioExcavationManager(void);
	virtual ~ioExcavationManager(void);
};

#define g_ExcavationMgr ioExcavationManager::GetSingleton()

#endif // __ioExcavationManager_h__