#pragma once
#include "../../../ioNpcChar.h"
#include "Practice.h"

class ioPlayStage;
class ioPlayEntity;
class ioBaseChar;
class SP2Packet;

class ioPracticeMode : public ioPlayMode
{
protected:
	Practice* m_pPractice;
	Vector3Vec m_vRevivalPos;
	bool m_bEnterMsgShow;
	bool m_bEnd;

protected:
	typedef std::vector< ModeRecord > RecordList;
	RecordList m_vCharRecordList;
	int	m_iPracticeIndex;
	int m_iResultStar;
	DWORD m_dwPracticeStartTime;

private:
	int m_iMacroStar;

public:
	void SetMacro(DWORD dwResult);
	void SetMacroState();			// 유저가 매크로 사용 확인되어 관련 UI 띄워졌을때 상태 값 변경
	void SetResultState();			// 유저가 매크로 사용 확인되어 관련 UI에서 퀴즈를 맞췄을 경우 상태 값 변경

public:
	virtual void InitMode( int iMapIndex );
	virtual void LoadMap();
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI(){};

	virtual void ReStartMode(){};
	virtual void RetryMode();
	virtual void ClearMode();

	virtual void LoadDisableSkillList();
	virtual void LoadStartBuffList();

public:
	virtual void ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck = false );
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );
	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordCharByUserIndex( DWORD dwUserIndex );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	//virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName ){};

protected:
	virtual void LoadModeResource( ioINILoader &rkLoader );

protected:
	void CreatePracticeType();
	void LoadPracticeINI();

protected:
	void UpdateCountUpWnd();

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate ){};
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate ){};

public:    //기여도.
	virtual void ContributeConvert(){};
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );
	virtual void ContributeResultSync( const ioHashString &szName, int iContribute ){};
	
	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket ){};
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket ){};
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket ){};

public:
/*
	virtual void LoadCharacters();
	virtual void LoadDisableSkillList();*/

protected:
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundStart( SP2Packet &rkPacket );
	//virtual void OnTurnStart( SP2Packet &rkPacket );//수련장 디버깅용 임시함수

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	void OnPracticeSelectRegular( SP2Packet &rkPacket );
	void OnPracticeResult( SP2Packet &rkPacket );
	void OnEscapePrisonerMode( SP2Packet &rkPacket );

public:
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );

protected:
	void GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargetPos );

public:
	void CreatePracticeNpc( Practice::PracticeNPC PracticeNpc );
	bool ApplyWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon );
	void CheckWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage );
	void CheckWeaponDefense( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage );
	void SelectRegularSoldier();
	void EscapePrisonerChar( ioBaseChar *pChar );
	void PracticeEndByDie();

	bool IsAllDie( TeamType eTeam );

	float GetExtraRecoveryGaugeRate();

	void ReadyPractice();
	void StartPractice();

	int GetTeamPlayerCount( TeamType eTeam, int &iDieCount );
	int GetPracticeIndex() { return m_iPracticeIndex; }

public:
	virtual void ProcessZoneCollision();

protected:
	void ProcessPlayState();
	void ProcessWaitState();
	void ProcessGUI();
	void ProcessWait();

protected:
	virtual void UpdateMiniMapSpotPoint();

public:
	ioPracticeMode( ioPlayStage *pCreator );
	virtual ~ioPracticeMode();
};

inline ioPracticeMode* ToPracticeMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_PRACTICE )
		return NULL;

	return static_cast< ioPracticeMode* >( pMode );
}

