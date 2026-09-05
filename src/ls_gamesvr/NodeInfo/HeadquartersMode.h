
#ifndef _HeadquartersMode_h_
#define _HeadquartersMode_h_

#include "Mode.h"
#include "HeadquartersModeHelp.h"

class SP2Packet;
class User;

class HeadquartersMode : public Mode
{
public:
	enum
	{
		STATE_OPEN_CHAR		= 1,         // 용병 펼치기 상태 ( 손님 있을 때 불가 )
		STATE_UNITE_CHAR	= 2,         // 용병 합치기 상태 ( 손님 있을 때 불가 )
		STATE_DISPLAY_CHAR	= 3,         // 용병 진열 상태 ( 손님있을 때 고정 상태 ) 
	};

protected:
	HeadquartersRecordList m_vRecordList;
	HeadquartersCharRecordList m_vCharacterList;
	
	typedef std::map< DWORD , ioEquipSlot * > CharacterEquipSlotMap;
	CharacterEquipSlotMap m_CharacterEquipSlotMap;

	bool m_bCharacterCreate;
	int  m_iCharacterCreateNameCount;

	bool m_bJoinLock;

	IntVec m_TeamList;
	IntVec m_TeamPosArray;

	ioHashString m_szMasterName;

	DWORD m_dwCharState;

protected:
	struct MonsterTable
	{
		DWORDVec m_MonsterCodeList;
		int      m_iMonsterCreateCount;
		DWORD    m_dwDropItemIndex;

		FloatVec m_StartXPosList;
		FloatVec m_StartZPosList;

		MonsterTable()
		{
			m_iMonsterCreateCount = 0;
			m_dwDropItemIndex     = 0;
		}
	};
	MonsterTable m_MonsterTable;


public:
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy );

	virtual void SetStartPosArray();
	virtual int GetCurTeamUserCnt( TeamType eTeam ){ return 0; }

protected:
	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );

protected:
	virtual void ProcessPlay();
	virtual void RestartMode(){}    // 무한 플레이
	virtual void ProcessRevival();

	virtual void SetRoundEndInfo( WinTeamType eWinTeam ){} // 결과 없음

	void AddTeamType( TeamType eTeam );
	void RemoveTeamType( TeamType eTeam );

protected:
	void CheckCharacterDeleteByAI();
	void CheckCharacterCreate( bool bForceCreate );
	const ioHashString &SearchCharacterSyncUser();
	void RemoveRecordChangeCharacterSync( const ioHashString &rkRemoveName );
	void PlayCharacterSync( HeadquartersRecord *pSendRecord );
    
protected:
	void InsertEquipSlotMap( DWORD dwCharIndex );
	void DeleteEquipSlotMap( DWORD dwCharIndex );
	ioItem* EquipItem( DWORD dwCharIndex, ioItem *pItem );
	ioItem* EquipItem( DWORD dwCharIndex, int iSlot, ioItem *pItem );
	ioItem* ReleaseItem( DWORD dwCharIndex, int iSlot );
	ioItem* ReleaseItem( DWORD dwCharIndex, int iGameIndex, int iItemCode );

	void FillCharacterInfo( HeadquartersCharRecord &rkCharacter, bool bCreate, SP2Packet &rkPacket );
	void FillCharacterSimpleInfo( HeadquartersCharRecord &rkCharacter, bool bCreate, SP2Packet &rkPacket );
	void FillCharEquipItem( DWORD dwCharIndex , SP2Packet &rkPacket );

public:
	virtual void OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnDropItemNpc( User *pSendUser, const ioHashString &rkOwnerID, SP2Packet &rkPacket );
	virtual void OnDropMoveItemNpc( User *pSendUser, const ioHashString &rkOwnerID, SP2Packet &rkPacket );
	virtual bool OnModeChangeChar( User *pSend, int iCharArray, bool bWait, int iSelectCharArray, DWORD dwCharChangeIndex );
	virtual void OnModeChangeDisplayMotion( User *pSend, DWORD dwEtcItem, int iClassType );
	virtual void OnModeCharDecoUpdate( User *pSend, ioCharacter *pCharacter );
	virtual void OnModeCharExtraItemUpdate( User *pSend, DWORD dwCharIndex, int iSlot, int iNewIndex ); 
	virtual void OnModeCharMedalUpdate( User *pSend, DWORD dwCharIndex, int iMedalType, bool bEquip );
	virtual void OnModeCharCustomMedalUpdate( User *pSend, DWORD dwCharIndex, CustomMedal *pInfo );
	virtual void OnModeCharGrowthUpdate( User *pSend, int iClassType, int iSlot, bool bItem, int iUpLevel );
	virtual void OnModeCharInsert( User *pSend, ioCharacter *pCharacter );
	virtual void OnModeCharDelete( User *pSend, DWORD dwCharIndex );
	virtual void OnModeCharDisplayUpdate( User *pSend, DWORD dwCharIndex );
	virtual	void OnModeJoinLockUpdate( User *pSend, bool bJoinLock );
	virtual void OnModeLogoutAlarm( const ioHashString &rkMasterName );

public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck = false );
	virtual int GetRecordCnt() const;

	virtual const char* GetModeINIFileName() const;
	virtual TeamType GetNextTeamType();
	virtual void CheckRoundEnd( bool bProcessCall ){}    // 무한 플레이

	virtual void UpdateRoundRecord();

	virtual bool IsTimeClose(){ return false; }
	virtual void UpdateUserDieTime( User *pDier );

	void UpdateMonsterDieRecord( const ioHashString &szAttacker, const ioHashString &szBestAttacker );

public:
	void OnMonsterDropItemPos( Vector3Vec &rkPosList, float fRange );
	void OnMonsterDieToItemDrop( float fDropX, float fDorpZ, const ioHashString &rkDropName );

public:
	virtual void InitObjectGroupList();
	virtual Vector3 GetRandomItemPos(ioItem *pItem = NULL);

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );
	HeadquartersCharRecord* GetCharacterInfo( int iCharCode );
	void SetCharacterReinforceInfo( int iCode, int iReinforce );

public:
	void SetCharState( DWORD dwState );
	void SetHeadquartersMaster( const ioHashString &rkName ){ m_szMasterName = rkName; }
	void ReSetCreateCharacter( User *pUser );
	void CreateCharacter( User *pUser );
	void InsertCharacter( User *pUser, ioCharacter *pCharacter );

public:
	bool IsMasterJoin();
	const ioHashString &GetHeadquartersMaster(){ return m_szMasterName; }
	bool IsHeadquartersJoinLock(){ return m_bJoinLock; }

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	HeadquartersRecord* FindHeadquartersRecord( User *pUser );
	HeadquartersRecord* FindHeadquartersRecord( const ioHashString &rkName );
	HeadquartersRecord* FindHeadquartersRecordByUserID( const ioHashString &rkUserID );
	HeadquartersCharRecord* FindCharacterInfo( const ioHashString &rkName );

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

protected:
	void OnCharStateChange( User *pSend, SP2Packet &rkPacket );	
	void OnUserInvite( User *pSend, SP2Packet &rkPacket );

public:
	HeadquartersMode( Room *pCreator );
	virtual ~HeadquartersMode();
};

inline HeadquartersMode* ToHeadquartersMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_HEADQUARTERS )
		return NULL;

	return static_cast< HeadquartersMode* >( pMode );
}

#endif 
