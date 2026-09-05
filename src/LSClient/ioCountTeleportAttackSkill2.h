#pragma once

#include "ioCountSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCountTeleportAttackSkill2 : public ioCountSkill
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
		SS_TELEPORT_ATTACK,
		SS_TELEPORT_ATTACK_FIRE,
		SS_TELEPORT_AUTO,
		SS_TELEPORT_AUTO_ATTACK,
		SS_TELEPORT_AUTO_END,
	};
	enum TeleportType
	{
		TT_NONE,
		TT_NORMAL,
		TT_IMMEDIATELY,
		TT_NOT_USE,
		TT_AUTOTARGET,
		TT_MASS_TELEPORT,
		TT_ATTACK,
	};
	enum TargetState
	{
		TS_NONE,
		TS_NORMAL,
		TS_AIR,
	};

	struct TargetInfo
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3	vDir;
		TargetState	eState;

		TargetInfo()
		{
			vPos	= ioMath::VEC3_ZERO;
			vDir	= ioMath::VEC3_ZERO;
			eState	= TS_NONE;
		}
		
	};
	typedef std::vector<TargetInfo> TargetDirectionList;

	struct DefulatFire
	{
		float range;
		float dir;
		DefulatFire( float _range, float _dir )
		{
			range = _range;
			dir = _dir;
		}
	};
	typedef std::vector<DefulatFire> vDefulatFire;

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
	CEncrypt<DWORD> m_dwNormalTeleportDuration;

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

	TargetDirectionList	m_TargetDirectionList;
	vDefulatFire		m_DefaultFireList;
	float				m_fTargetMaxRange;
	float				m_fHeightOffset;
	AttackAttribute		m_NomralAttackByAir;
	AttackAttribute		m_JumpAttackByAir;

	float m_fTeleportOffset;
	float m_fTeleportNoTarget;
	float m_fJumpHeightGap;
	float m_fTeleportJumpPower;
	D3DXVECTOR3 m_vTargetPos;
	AttackAttribute m_TeleportAutoMove;
	AttackAttribute m_TeleportAutoJump;

	CEncrypt<bool> m_bTargetPosAttack;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

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
	void SetTeleportAttackSkill( ioBaseChar *pChar );

protected:
	void SetWaitState( ioBaseChar *pChar );
	void SetTeleportState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTeleportAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTeleportAutoTargetState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	D3DXVECTOR3 GetMagicCirclePos( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetMassTeleportState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTeleportAutoState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSendPacket );
	void CheckTeleportAttackState( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckTeleportTarget( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetExtraEndAniRate( float fRate );

	void FindTarget( ioBaseChar* pChar, ioPlayStage* pStage );
	void AttackFireToDir( ioBaseChar* pChar, ioPlayStage* pStage, D3DXVECTOR3& vDir, TargetState eState );
	void AttackFireToPos( ioBaseChar* pChar, ioPlayStage* pStage, D3DXVECTOR3& vPos, D3DXVECTOR3& vDir, TargetState eState );

	void ProcessTeleportAuto( ioBaseChar *pOwner, ioPlayStage *pStage );

	virtual int ReduceNeedCount( int iNeedCount );

public:
	ioCountTeleportAttackSkill2();
	ioCountTeleportAttackSkill2( const ioCountTeleportAttackSkill2 &rhs );
	virtual ~ioCountTeleportAttackSkill2();
};

inline ioCountTeleportAttackSkill2* ToCountTeleportAttackSkill2( ioSkill *pSkill )
{
	ioCountSkill *pRange = ToCountSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != CST_TELEPORT_ATTACK2 )
		return NULL;

	return dynamic_cast< ioCountTeleportAttackSkill2* >(pRange);
}