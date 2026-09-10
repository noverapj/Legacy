#pragma once

#include "ioPlayMode.h"
#include "GUI/ResultInfo.h"

#define STPK_TEMP_PACKET_FAKE    0x5000

class SP2Packet;

class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class ioGrowthLevel;

class ioMyRoomMode : public ioPlayMode
{	
protected:
	enum
	{
		CHARACTER_POS_VER = 5,
	};
	typedef std::vector< ModeRecord > RecordList;
	RecordList m_vCharRecordList;

	ioHashString m_szReadySnd;
	ioHashString m_szPlaySnd;

	D3DXVECTOR3  m_vGaugeUpPos;
	float        m_fGaugeUpDistance;
	float        m_fGaugeUpRate;

	//
	DWORD        m_dwOwnerPosSaveTimer;
	DWORD        m_dwCurClassStartTimer;
	DWORD        m_dwClassStartTime;
	DWORD		 m_dwCurEventDuration;
	bool         m_bStartMotionEndOnShop;

	//
	Vector3Vec   m_vDefStartPos;
	Vector3Vec   m_vBackupDefStarPos;
	Vector3Vec   m_vDropFieldItemPos;
	D3DXVECTOR3    m_vRemoveExercisePos;
	D3DXQUATERNION m_qtRemoveExerciseRot;

	bool           m_bBuyExercise;
	bool           m_bOneMyChar;

	//
	int          m_iMyLobbyArray;
	DWORD        m_dwCurMyLobbyAlarmTime;
	DWORD        m_dwMyLobbyAlarmTime;
	ioHashString m_szMyLobbyAlarmMent_start;
	ioHashString m_szMyLobbyAlarmMent_current;

	static bool  m_bBlockNotice;
	static MyRoomType m_eMyRoomType;

protected:
	void LoadCharacterPos();
	void SaveCharacterPos();
	void ProcessOwnerPosSave( DWORD dwIndex );
	void ProcessOwnerDelayPosSave();
	void LoadSound();

protected:
	virtual void LoadCharacters();
	virtual void LoadNPC();
	virtual void LoadModeResource( ioINILoader &rkLoader );

	void SetMyCharEquipItem( ioBaseChar *pChar, int iCharArray );	
	void SetMyCharEquipMedalItem( ioBaseChar *pChar, int iCharArray );	
	void LoadDummyCharacter();
	void ResetOwnerAndName();	

	void InitSingleLobby();

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI();
	
	virtual void ReStartMode();
	virtual void ClearMode();

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName,
									SP2Packet &rkPacket, bool bDieCheck = false );

	virtual void SetCharSlotChange();
	virtual bool ShowSoldierSettingUI( ioBaseChar *pOwnerChar, int iArray ){ return false; }

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );

public:
	virtual int GetRoundWinCnt( TeamType eTeamType );
	virtual void SendOwnerDamageList( bool bLast = false ){}

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );
	ModeRecord *FindModeRecordCharIndex( const DWORD dwIndex );

protected:
	bool GetPartyStateInfo( int index, PartyStateInfo &rkInfo );
	virtual void UpdateRoundStateWnd();
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	virtual void UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker );

public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

	virtual void AddMySoldier( DWORD dwIndex, bool bBuySoldier = false  );
	virtual void RemoveMySoldier( DWORD dwIndex );
	virtual void ReLoadMySoldier( int iCharArray );
	virtual void ReLoadAllMySoldier();
	virtual void MySoldierDefPos();
	virtual void SetOneMyChar();
	virtual void SetAllMyChar();
	virtual bool IsOneMyChar();

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;

	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	virtual D3DXVECTOR3 GetStartPos( TeamType eTeamType, bool bRandom = true );

protected:
	void ProcessReadyState();
    void ProcessPlayState();

	void ProcessClassStartGOGO();
	void ProcessGUI();
	void ProcessRevival();
	void ProcessMyLobbyAlarm();
	
	void ProcessGaugeUpCheck();

	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );

	void ChangeOwnerChar( DWORD dwIndex, bool bBuySoldier );
	void ChangeOwnerCharByOneChar( DWORD dwIndex );
	void MySoldierDropStepPos();

public:    //±â¿©µµ.
	virtual void ContributeConvert();
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );

	virtual void ContributeResultSync( const ioHashString &szName, int iContribute );

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket );
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );
public:
	virtual bool IsNetworkMode(){ return false; }
	virtual bool IsCollisionCheck();

public:
	void TutorialDropFieldItem();
	void ReleaseTutorialDropFieldItem();

public:
	void SetMyLobbyArray( int iMyChannelArray );

protected:
	void OnChangeChar( SP2Packet &rkPacket );
	void OnJoinServerLobbyInfo( SP2Packet &rkPacket );
	void OnServerLobbyChat( SP2Packet &rkPacket );

protected:
	void ShowBlockNotice();

// For ExtraItem
public:
	virtual void SetChangeExtraItem( int iCharArray, int iSlot );

public:
	static void SetMyRoomType( MyRoomType eMyRoomType );
	static MyRoomType GetMyRoomType(){ return m_eMyRoomType; }

public:
	ioMyRoomMode( ioPlayStage *pCreator );
	virtual ~ioMyRoomMode();
};

inline ioMyRoomMode* ToMyRoomMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_MYROOM )
		return NULL;

	return dynamic_cast< ioMyRoomMode* >( pMode );
}
