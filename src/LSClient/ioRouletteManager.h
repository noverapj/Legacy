
#pragma once

class ioRoulette;

class ioRouletteManager : public Singleton< ioRouletteManager >
{
public:
	enum RouletteType
	{
		RT_NONE,
		RT_WHEEL,
		RT_STAND,
		RT_ARROW,
		RT_PRIZE,
		RT_BUTTON,
	};

	enum ButtonType
	{
		BT_ON,
		BT_OFF,
	};

	enum RouletteState
	{
		RS_READY,
		RS_SPIN,
		RS_END,
	};

	enum ArrowState
	{
		AS_NONE,
		AS_STUMBLE,
		AS_RETURN,
	};

protected:
	struct RouletteInfo 
	{
		CEncrypt<int> m_iType;
		CEncrypt<int> m_iSubIndex;
		ioHashString  m_ModelName;
		D3DXVECTOR3   m_vOffset;
		D3DXVECTOR3   m_vScale;
		RouletteInfo()
		{
		}
		RouletteInfo( int iType, int iSubType, const ioHashString &szModelName, D3DXVECTOR3 vOffset, D3DXVECTOR3 vScale )
		{
			m_iType     = iType;
			m_iSubIndex = iSubType;
			m_ModelName = szModelName;
			m_vOffset   = vOffset;
			m_vScale    = vScale;
		}
	};
	typedef std::vector<RouletteInfo> RouletteInfoList;
	typedef std::vector<ioRoulette*> RouletteList;

protected:
	ioPlayStage *m_pPlayStage;
	RouletteInfoList m_vRouletteInfoList;
	RouletteList     m_vRouletteList;
	
protected:
	RouletteState m_RouletteState;
	ArrowState    m_ArrowState;

	struct ResultData
	{
		CEncrypt<float> m_fAngleMin;
		CEncrypt<float> m_fAngleMax;
		D3DXVECTOR3     m_vDirection;
		CEncrypt<int>   m_iRate;

		ResultData()
		{
		}
		ResultData( float fMax, float fMin, D3DXVECTOR3 vDir, int iRate )
		{
			m_fAngleMax  = fMax;
			m_fAngleMin  = fMin;
			m_vDirection = vDir;
			m_iRate      = iRate;
		}
	};
	typedef std::vector<ResultData> ResultDataList;
	struct ResultGroup
	{
		CEncrypt<int> m_iMinPlayers;
		CEncrypt<int> m_iMaxPlayers;
		ResultDataList m_vRouletteResultList;
		ResultGroup()
		{
		}
		ResultGroup( int iMax, int iMin )
		{
			m_iMaxPlayers = iMax;
			m_iMinPlayers = iMin;
		}
	};
	typedef std::vector<ResultGroup> ResultGroupList;
	ResultGroupList m_vResultGroupList;

	ioHashString m_EffectButton;
	ioHashString m_EffectDelay;
	ioHashString m_EffectResult;
	ioHashString m_EffectStart;
	DWORD m_dwEffectDelayID;
	DWORD m_dwEffectStartID;
	DWORD m_dwEffectResultID;

	ioHashString m_EventStartEffect;
	ioHashString m_EventEndEffect;

	ioHashStringVec m_vJoinUser;
	DWORD m_dwEnableSpinStartTime;
	DWORD m_dwSpinStartTime;
	DWORD m_dwSpinEndTime;
	CEncrypt<DWORD> m_dwSpinDuration;
	CEncrypt<DWORD> m_dwSpinStopTime;
	CEncrypt<DWORD> m_dwJoinEndTime;
	CEncrypt<DWORD> m_dwRestartTime;
	CEncrypt<float> m_fSpinSpeed;
	CEncrypt<float> m_fReduceRate;
	CEncrypt<float> m_fEnableRange;
	CEncrypt<float> m_fEnableAngle;
	CEncrypt<int>   m_iNeedCoin;
	float m_fCurAngle;
	float m_fCurSpinSpeed;
	float m_fResultMinAngle;
	float m_fResultMaxAngle;
	float m_fResultAngle;

	float m_fCurArrowAngle;

	ioRoulette *m_pWheel;
	ioRoulette *m_pArrow;

	bool m_bEnableJoin;

	D3DXQUATERNION m_qtOriginal;

	D3DXVECTOR3 m_vRoulettePosition;
	D3DXVECTOR3 m_vRouletteScale;

public:
	void SetPlayStage( ioPlayStage *pStage ) { m_pPlayStage = pStage; }
	void Initialize();
	void InitForRestart();
	void LoadRoulette();
	void CreateRouletteAllEntity( ioPlayMode* pMode );
	ioRoulette *CreateRouletteEntity( ioPlayMode* pMode, int iType, int iSubIndex );
	void LoadRouletteMapInfo( const char* szFileName );

	void ProcessRoulette();
	void ProcessWheel();
	void ProcessArrow();

protected:
	bool CheckArrowStumble();

public:
	void SetRouletteReady();
	void SetRouletteSpin( const ioHashString & szUserName, bool bSend );
	void SetRouletteEnd();

public:
	bool EnableRouletteSpin( const ioHashString & szUserName );
	bool CheckCharRange( const ioHashString & szUserName );

public:
	void OnRouletteStart( SP2Packet &rkPacket );
	void OnRouletteJoinEnd( SP2Packet &rkPacket );
	void OnRouletteEnd( SP2Packet &rkPacket );
	void OnRouletteException( SP2Packet &rkPacket );

	void ApplyRouletteInfo( SP2Packet &rkPacket );
	void RemoveJoinUser( const ioHashString &rkName );
	void RemoveJoinPlayer( const ioHashString &szUserName );

public:
	bool IsRouletteEvent();
	bool IsJoinRoulette( const ioHashString &szUserName );

public:
	D3DXVECTOR3 GetRoulettePos();
	D3DXVECTOR3 GetButtonPositon();
	float GetEnableStartRange() { return m_fEnableRange; }

public:
	void CheckRouletteStruct();
	void CheckRouletteStruct( bool bAlive );

protected:
	void ResetEntity( int iSubIndex = 0 );
	void EraseRouletteEntity( int iType, int iSubIndex );
	ioRoulette *GetRouletteEntity( int iType, int iSubIndex = 0 );
	bool IsExistRouletteEntity();

public:
	void DestroyRouletteList();

public:
	int GetCurResultGroupArray();
	int GetResultGroupMaxArray() { return static_cast<int>( m_vResultGroupList.size() ); }
	void GetResultGroupInfoByArray( IN int iArray, OUT int &iMax, OUT int &iMin, OUT int &iRate );
	int GetJoinCount() { return m_vJoinUser.size(); }
	float GetSpinSpeed() { return m_fSpinSpeed; }
	float GetCurSpinSpeed() { return m_fCurSpinSpeed; }

	int GetState() { return (int)m_RouletteState; }
	DWORD GetSpinStartTime() { return m_dwSpinStartTime; }
	DWORD GetSpinEndTime() { return m_dwSpinEndTime; }
	DWORD GetJoinEndTime() { return m_dwSpinStartTime+m_dwJoinEndTime; }

	int GetRewardRank();

	const D3DXQUATERNION &GetRouletteOrientation();
	const float &GetCurAngle() { return m_fCurAngle; }

public:
	static ioRouletteManager& GetSingleton();

public:
	ioRouletteManager();
	virtual ~ioRouletteManager();
};

#define g_RouletteMgr ioRouletteManager::GetSingleton()