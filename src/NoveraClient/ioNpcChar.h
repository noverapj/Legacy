#pragma once

#include "ioNpcDefine.h"
#include "ioBaseChar.h"

#define NPC_NOT_INDEX 0

class ioAI;
class ioCallByNpcBuff;

class ioNpcAppearance
{
public:
	ioNpcAppearance(	int race,	
						int sex, 
						int beard,	
						int accessory, 
						int face, 
						int hair, 
						int skincolor, 
						int haircolor, 
						int underwear,
						std::string model, 
						std::string raceinfo )
		:	m_race( race ), m_sex( sex ),
			m_beard( beard ), m_accessory( accessory ), m_face( face ), 
			m_hair( hair ), m_skincolor( skincolor ), m_haircolor( haircolor ), 
			m_underwear( underwear ), m_model( model ), m_raceinfo( raceinfo )
	{
	}

	~ioNpcAppearance() {}

	// old system is 1 base while ioNpcAppearance is 0 base
	int		GetCharInfoSex()		{	return m_sex + 1;	}
	int		GetCharInfoKindred()	{	return m_race + 1;	}

public:
	int				m_race, m_sex;
	int				m_beard, m_accessory, m_face, m_hair, m_skincolor, m_haircolor, m_underwear;
	std::string		m_model;
	std::string		m_raceinfo;
};

class ioNpcChar : public ioBaseChar
{
public:
	enum
	{
		NBT_NORMAL	  = 0,
		NBT_HALF_BOSS = 1,
		NBT_LAST_BOSS = 2,
	};

	enum
	{
		NPT_NORMAL = 0,
		NPT_NODMGANI,
	};

protected:
	ioAI *m_pAI;
	DWORD m_dwNpcCode;

protected:
	IORandom m_GradeUpRandom;
	DWORD    m_dwGradeUpTime;

protected:
	CEncrypt<float> m_fMonsterDefaultHP;
	CEncrypt<float> m_fMonsterDefaultSpeed;

protected:
	DWORD m_dwCustomNoInputDelayTime;
	DWORD m_dwCustomLimiteDieFlyTime;
	DWORD m_dwDieAlphaTime;
	float m_fCustomDieActionRate;
	float m_fCustomDieDownActionRate;
	ioHashString m_AniCustomStartMotion;
	ioHashString m_AniCustomDieAction;
	ioHashString m_AniCustomBackDieAction;
	ioHashString m_AniCustomBlowDieAction;
	ioHashString m_AniCustomBackBlowDieAction;
	ioHashString m_AniCustomDownBlowDieAction;
	ioHashString m_AniCustomBackdownBlowDieAction;
	ioHashString m_AniCustomFalling;
	ioHashString m_AniCustomDropAction;
	ioHashString m_AniCustomName;
	ioHashString m_AniCustomBlowGetUp;
	ioHashString m_AniCustomBackBlowGetUp;

protected:
	ioHashString m_CustomEffect;

protected:
	DWORD m_dwCustomStartProtectionTime;
	DWORD m_dwCustomBlowProtectionTime;
	int m_nNpcProtectType;
	
protected:
	ioHashString m_szSyncUserName;

protected:
	ioHashString m_szNpcViewName; 
	ioHashString m_szGradeTitle;
	bool m_bShowGrade;

protected:
	DWORD m_dwCurrentStartTime;
	DWORD m_dwStartTime;
	DWORD m_dwStartReduceTime;
	float m_fStartXPos;
	float m_fStartZPos;
	D3DXVECTOR3 m_vStartPos;

	ioHashStringVec m_NPCStartBuffList;

protected:
	int m_iStuckActiveRand;
	int m_iStuckActiveSeed;
	DWORD m_dwStuckActiveTime;
	ioHashStringVec m_StuckBuffList;

	struct StuckExData
	{
		IntVec m_vWeaponTypeList;
		int    m_iStuckActiveRand;
		StuckExData()
		{
			m_iStuckActiveRand = 0;
		}
	};
	typedef std::vector< StuckExData > vStuckExData;
	vStuckExData m_StuckExData;

protected:
	struct MonsterMent
	{
		DWORD		 m_dwShowTime;
		DWORD        m_dwColor;
		float        m_fFontScale;
		ioHashString m_szMent;
		MonsterMent()
		{
			m_dwShowTime = m_dwColor = 0;
			m_fFontScale = 0.0f;
		}
	};
	MonsterMent m_MonsterMent;

protected:
	struct BalanceBuff
	{
		float m_fCheckHPRate;
		ioHashString m_szHPBuff;
		BalanceBuff()
		{
			m_fCheckHPRate = 0.0f;
		}
	};
	typedef std::vector< BalanceBuff > vBalanceBuff;
	vBalanceBuff m_BalanceBuff;
	float m_fPrevHPRate;

	ioCallByNpcBuff *m_pCallByNpcBuff;


protected:
	int  m_iBossMonsterType;
	bool m_bDeathMonster;

protected:
	bool m_bUseHeadMeshChange;

	typedef std::map< ioHashString, ioHashString > HeadMeshInfoMap;
	HeadMeshInfoMap m_HeadMeshInfoMap;

protected:
	ioHashString m_szMonsterIcon;
	ioHashString m_szBossMonsterEmoticon;
	ioHashString m_szImageName;

protected:
	bool m_bStartAICreateMotion;
	bool m_bClassLevelShow;
	bool m_bActionTrigger;


protected:
	DWORD m_dwDisplayMotionItem;
	DWORD m_dwDisplayMotionItemTime;

public:
	virtual void InitCustomInfo( ioINILoader &rkLoader );
	virtual void Initialize( const ITEM_DATA *pItem );

	void InitHeadMeshInfo( ioINILoader &rkLoader );

	virtual void InitCustomEffect( ioINILoader &rkLoader );
	
	void SetCustomEffect();
	void SetCustomEffectEnd();

public:
	void LoadMonsterAIInfo( ioINILoader &rkLoader );
	
	// for champion mode AI
	void LoadMonsterInfo( ioXMLElement* xmlNode );
	void LoadMonsterMent( ioXMLElement* xmlNode );

protected:
	virtual void InitUserStat();
	virtual void UpdateStatAttribute();

	virtual void CheckDieState();

public:
	virtual ControlType GetControlType() const { return CONTROL_AI; } 
	virtual NpcType GetNpcType() const;
	inline DWORD GetNpcCode(){ return m_dwNpcCode; }

public:
	virtual bool IsNeedProcess() const;
	virtual bool IsNeedSendNetwork() const;
	virtual bool IsDropDamage();
	virtual bool IsItemDrop();
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void CreateMagicCircle( const ioHashString &ReadyCircle, const ioHashString &EnableCircle, const ioHashString &GuidCircle, float fOffSet );

public:
	virtual DWORD GetNoInputDelayTime();
	virtual DWORD GetLimiteDieFlyTime();
	virtual float GetDieActionRate();
	virtual float GetDieDownActionRate();

public:
	virtual const ioHashString &GetAniStartAction( bool &bWeaponAni );
	virtual const ioHashString &GetAniDieAction();
	virtual const ioHashString &GetAniBackDieAction();
	virtual const ioHashString &GetAniBlowDieAction();
	virtual const ioHashString &GetAniBackBlowDieAction();
	virtual const ioHashString &GetAniDownBlowDieAction();
	virtual const ioHashString &GetAniBackdownBlowDieAction();
	virtual ioHashString &GetAniFalling();
	virtual ioHashString &GetDropDownAction();
	virtual ioHashString &GetAniBlowGetUp();
	virtual ioHashString &GetAniBackBlowGetUp();


// For HeadMeshChange
public:
	virtual void HeadMeshChange( const char *szMeshName, int iDuration );
	virtual void CheckHeadMeshChange();
	virtual void StopHeadMeshChange();

public:
	virtual DWORD GetStartProtectionTime(){ return m_dwCustomStartProtectionTime; }
	virtual DWORD GetBlowStateProtectionTime(){ return m_dwCustomBlowProtectionTime; }

protected:
	bool IsStartCheck();
	void SetNPCStartBuff();

protected:
	virtual void ProcessAI();
	virtual void ProcessBalanceCheck();
	virtual bool CehckStuckWeaponType( int iWeaponType, int iRandValue );
	virtual bool CheckStuckActive( int iWeaponType );
	void ProcessMotionCheck();

protected:
	void ProcessBossMonsterEmoticon();

public:
	void SetAI( const ioHashString &rkAI );
	void SetAI( ioAI *pAI );
	void SetNpcCode( DWORD dwCode ){ m_dwNpcCode = dwCode; }
	void SetDirectionKey( int iDirection, WORD wKeyData );
	void SetActionKey( WORD wDefenseKey, WORD wJumpKey, WORD wAttackKey );
	void SetNpcViewName( const ioHashString &szViewName );
	void SetNpcGradeTitle( const ioHashString &szGradeTitle );
	void SetSyncUser( const ioHashString &rkSyncName );
	void SetStuckInfo( int iStuckActiveRand, int iStuckActiveSeed, DWORD dwSturkActiveTime, ioHashStringVec &rkStackBuffList );
	void SetStuckExList( const ioHashString &rkWeaponType, int iStuckActiveRand );
	void SetFixingTarget( const ioHashString &rkTargetName, float fFixingTime );
	void SetDeathMonster( bool bDeathMonster ){ m_bDeathMonster = bDeathMonster; }
	void SetDisplayMotion( DWORD dwDisplayMotionItem );
	void SetCallByNpcBuff(const DWORD dwOccurNpc, bool bAlive);
	void SetNpcMaxHpRate( float fMaxHpRate );

public:
	int  GetGradeUpRandom( int iRandom );
	void SetGradeUpRandomSeed( DWORD dwRandomSeed );
	void ReSetGradeLevel( int iGradeLevel );
	void RandomizeGradeLevel();
	int  GetNpcProtectType() const { return m_nNpcProtectType; }

	bool GetNpcActionTrigger() { return m_bActionTrigger; }

public:	// use in AI
	bool IsComboAttackPossible();
	int	 GetComboAttackTimeline( unsigned int frontMargin = 0, unsigned int tailMargin = 0 );

public:
	void SetNpcGradeShow(bool bSet) { m_bShowGrade = bSet;}
	bool IsMonsterHalfBoss(){ return ( m_iBossMonsterType == NBT_HALF_BOSS ); }
	bool IsMonsterLastBoss(){ return ( m_iBossMonsterType == NBT_LAST_BOSS ); }
	DWORD GetMonsterMentTime(){ return m_MonsterMent.m_dwShowTime; }
	DWORD GetMonsterMentColor(){ return m_MonsterMent.m_dwColor; }
	float GetMonsterMentScale(){ return m_MonsterMent.m_fFontScale; }
	ioHashString &GetMonsterMent(){ return m_MonsterMent.m_szMent; }
	bool IsDeathMonster(){ return m_bDeathMonster; }
	bool IsEscapeType();
	float GetStartXPos(){ return m_fStartXPos; }
	float GetStartZPos(){ return m_fStartZPos; }
	D3DXVECTOR3	GetStartPos() { return m_vStartPos; }
	DWORD GetDisplayMotion(){ return m_dwDisplayMotionItem; }
	bool IsUseHeadMeshChange() { return m_bUseHeadMeshChange; }
	bool IsAI(){ return ( m_pAI != NULL ); }

public:
	const ioHashString &GetViewName(){ return m_szNpcViewName; }
	const ioHashString &GetSyncUser(){ return m_szSyncUserName; }
	const ioHashString &GetMonsterIcon(){ return m_szMonsterIcon; }
	const ioHashString &GetImgName(){ return m_szImageName; }


public:
	void AddItemBalance( float fAddArmorClass, float fAddSpeedClass );
	void ReEquipItem();

public:
	void ChangeItemSkill( EquipSlot eSlot, const ioHashString &rkSkillName );

public:
	void ExceptionStartAI();
	void StartAI( DWORD dwStartTime, float fStartXPos, float fStartZPos, bool bCreateMotion = false, DWORD dwIndex = 0  );
	void StopNPC();
	void StartReduceTime( DWORD dwStartReduceTime );

public:
	inline void SetClassLevelShow( bool bShow ){ m_bClassLevelShow = bShow; }

public:
	virtual void ActiveStuck();

public:
	bool RenderGaugeHide();
	void SetGaugeScale(float fScaleX, float fScaleY);
	virtual void RenderGauge();

public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );

public:
	ioNpcChar( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode );
	virtual ~ioNpcChar();
};

inline ioNpcChar* ToNpcChar( ioGameEntity *pObj )
{
	ioBaseChar *pChar = ToBaseChar( pObj );
	if( !pChar || pChar->GetControlType() != CONTROL_AI )
		return NULL;

	return dynamic_cast< ioNpcChar* >(pChar);
}

inline const ioNpcChar* ToNpcCharConst( const ioGameEntity *pObj )
{
	const ioBaseChar *pChar = ToBaseCharConst( pObj );
	if( !pChar || pChar->GetControlType() != CONTROL_AI )
		return NULL;

	return dynamic_cast< const ioNpcChar* >(pChar);
}

inline bool IsNpcChar( ioGameEntity *pObj )
{
	ioBaseChar *pChar = ToBaseChar( pObj );
	if( !pChar || pChar->GetControlType() != CONTROL_AI )
		return false;

	return true;
}
