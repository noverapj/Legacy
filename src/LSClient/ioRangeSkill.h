#pragma once

#include "ioSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioRangeSkill : public ioSkill
{
public:
	enum RangeState
	{
		RS_NONE,
		RS_ACTION,
		RS_ACTION2,
		RS_LOOP,
		RS_LOOP2,
		RS_END,
		RS_END2,
	};

	enum EnemyBuffType
	{
		EBT_NORMAL,
		EBT_RANDOM,
		EBT_DIRECTION
	};

protected:
	TargetColType m_TargetColType;
	TargetWoundType m_TargetWoundType;

	RangeState m_RangeState;
	DWORD m_dwDuration;
	
	DWORD m_dwLoopProtectDuration;
	DWORD m_dwProtectTime;

	bool m_bEnableAttacker;
	bool m_bEnableAttackCnt;
	bool m_bCheckCollisionLine;

	float m_fSkillRange;
	float m_fSkillAngle;
	float m_fOffSet;

	float m_fUpHeight;
	float m_fUnderHeight;
	float m_fWidth;

	ioHashString m_RangeEffect;
	D3DXVECTOR3 m_vEffectDir;

	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	ioHashString m_EndAnimation;
	float m_fEndAniRate;
	DWORD m_dwEndAniStartTime;
	DWORD m_dwEndAniEndTime;
	WeaponInfoList m_vEndAttackWeaponList;
	ForceInfoList m_vEndAttackForceList;

	BuffInfoList m_OwnerBuffList;
	BuffInfoList m_TeamBuffList;
	BuffInfoList m_EnemyBuffList;
	BuffInfoList m_DirectionBuffFrontList;
	BuffInfoList m_DirectionBuffBackList;

	int m_iEnemyRandomBuffCount;
	std::vector<BuffInfoList> m_EnemyRandomBuffList;

	ioHashStringVec m_vCollisionedList;
	ioHashStringVec m_vBlockedList;

	bool			m_bNotUseKeyReserve;

protected:
	ioHashString m_StartHandMesh;
	ioHashString m_LoopHandMesh;
	ioHashString m_EndHandMesh;

protected:
	CEncrypt<bool> m_bIgnoreProtect;

protected:
	CEncrypt<bool>	m_bStateEscapeOnStand;
	CEncrypt<bool>	m_bStateEscapeOnAir;
	CEncrypt<bool>	m_bStateEscapeOnDown;

protected:
	EnemyBuffType m_EnemyBuffType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	virtual int GetTypeID() const;

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

protected:
	virtual void SetActionState( ioBaseChar *pChar );
	virtual void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void SetEndState( ioBaseChar *pChar );

	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void LoadTeamBuffList( ioINILoader &rkLoader );
	void LoadEnemyBuffList( ioINILoader &rkLoader );
	void LoadEnemyRandomBuffList( ioINILoader &rkLoader );
	void LoadDirectionBuffList( ioINILoader &rkLoader );
	void LoadEndAttackList( ioINILoader &rkLoader );

	bool SetOwnerBuffList( ioBaseChar *pChar );
	bool SetTeamBuffList( ioBaseChar *pOwner, ioBaseChar *pChar );
	virtual bool SetEnemyBuffList( ioBaseChar *pOwner, ioBaseChar *pChar );
	virtual bool SetEnemyRandomBuffList( ioBaseChar *pOwner, ioBaseChar *pChar );
	virtual bool SetEnemyDirectionBuffList( ioBaseChar *pOwner, ioBaseChar *pChar );

	bool IsCollisioned( ioBaseChar *pChar );
	bool IsBlocked( ioBaseChar *pChar );

	bool IsFrontSide( ioBaseChar *pOwner, ioBaseChar *pChar );

public:
	virtual bool IsCanStateEscape() const;
	virtual bool IsCanStateEscapeOnStand( ioBaseChar *pChar ) const;
	virtual bool IsCanStateEscapeOnAir( ioBaseChar *pChar ) const;
	virtual bool IsCanStateEscapeOnDown( ioBaseChar *pChar ) const;

public:
	ioRangeSkill();
	ioRangeSkill( const ioRangeSkill &rhs );
	virtual ~ioRangeSkill();
};

inline ioRangeSkill* ToRangeSkill( ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != ST_RANGE )
		return NULL;

	return dynamic_cast< ioRangeSkill* >( pSkill );
}

