#pragma once

#include "ioNormalSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTeleportSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_TELEPORT,
		SS_ACTION,
		SS_MASS_TELEPORT,
		SS_END,
		SS_CANCEL,
	};

	enum TeleportType
	{
		TT_NONE,
		TT_NORMAL,
		TT_IMMEDIATELY,
		TT_NOTMOVE,
		TT_AUTOTARGET,
		TT_MASS_TELEPORT,
	};

protected:
	CEncrypt<int> m_State;
	CEncrypt<int> m_TeleportType;
	TargetWoundType m_TargetWoundType;

	float m_fExtraEndAniRate;

	CEncrypt<float> m_fCircleRange;
	CEncrypt<float> m_fCircleMoveSpeed;
	CEncrypt<float> m_fCircleHeightGap;
	CEncrypt<float> m_fCircleOffSet;

	CEncrypt<float> m_fTeleportRange;
	CEncrypt<float> m_fUpHeight;
	CEncrypt<float> m_fUnderHeight;

	bool m_bReverseChange;

	CEncrypt<DWORD> m_dwWaitStartTime;
	CEncrypt<DWORD> m_dwJumpTeleportDuration;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetRedCircleEffect;
	ioHashString m_TargetBlueCircleEffect;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;
	UniqueObjID m_dwCurMapEffect;

	ioHashString m_EnemyRedEffect;
	ioHashString m_EnemyBlueEffect;
	UniqueObjID m_dwCurEnemyMapEffect;

	UniqueObjID m_dwTargetCircleEffect;

	ioHashString m_SkillStartMotion;
	float m_fStartMotionRate;
	ioHashString m_SkillEndMotion;
	float m_fEndMotionRate;

	ioHashString m_TeleportStartEffect;
	ioHashString m_TeleportEndEffect;
	ioHashString m_TeleportCameraBuff;

	CEncrypt<float> m_fBackRange;
	CEncrypt<float> m_fHeightRange;

	CEncrypt<float> m_fEnableGapHeight;
	CEncrypt<float> m_fEnableRange;
	CEncrypt<int> m_iEnableAngleCnt;

	DWORD m_dwPreDelayProtectTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	inline int GetSkillState() const { return m_State; }

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetWaitState( ioBaseChar *pChar );
	void SetTeleportState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	D3DXVECTOR3 GetMagicCirclePos( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetMassTeleportState( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckTeleportTarget( ioBaseChar *pChar, ioPlayStage *pStage );
	void InfinityRangeTeleportTarget( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetExtraEndAniRate( float fRate );

public:
	ioTeleportSkill();
	ioTeleportSkill( const ioTeleportSkill &rhs );
	virtual ~ioTeleportSkill();
};

inline ioTeleportSkill* ToTeleportSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TELEPORT )
		return NULL;

	return dynamic_cast< ioTeleportSkill* >( pSkill );
}

