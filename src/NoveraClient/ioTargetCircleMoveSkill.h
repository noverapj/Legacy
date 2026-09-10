#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"


class ioTargetCircleMoveSkill : public ioNormalSkill
{
public:
	struct TargetInfo
	{
		ioHashString	m_TargetName;
		float			m_fLenth;

		TargetInfo()
		{
			m_TargetName.Clear();
			m_fLenth = 0.0f;
		}
	};
	typedef std::vector<TargetInfo> TargetInfoList;

	class ChangeLenthSort : std::binary_function< const TargetInfo&, const TargetInfo&, bool >
	{
	public:
		bool operator()( const TargetInfo &lhs , const TargetInfo &rhs ) const
		{
			if( lhs.m_fLenth < rhs.m_fLenth )
			{
				return true;
			}
			return false;
		}
	};

	enum SendType
	{
		SNT_CHANGE	= 1,
		SNT_ATTACK	= 2,
		SNT_CIRCLE_MOVE = 3,
		SNT_CIRCLE_ACTION = 4,
		SNT_CIRCLE_END = 5,
	};

	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_FIRE,
		SS_CIRCLE_MOVE,
		SS_CIRCLE_ACTION,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_SkillLoopMotion;
	ioHashString m_CircleMoveLoopMotion;

	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	D3DXVECTOR3 m_vAttackDir;

	DWORD m_dwFireStartTime;
	DWORD m_dwAttackEndTime;

	// Limit Angle
	CEncrypt<float> m_fMaxAngle;
	CEncrypt<float> m_fMinAngle;
	CEncrypt<float>	m_fAimRange;
	CEncrypt<float>	m_fAimOffset;

	int m_iCurFireCount;
	int m_iMaxFireCount;

	ioHashString	m_szAimMark;
	UniqueObjID		m_dwAimMarkID;
	AttackAttribute m_AimAttack;
	ioHashString m_szAimAttackBuff;

	// TargetList
	TargetInfoList	m_vTargetInfoList;
	ioHashString	m_szTargetName;
	bool			m_bJumpKey;
	bool			m_bDefenseKey;

	//마법진 설정
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;
	DWORD m_dwCircleMoveTime;
	DWORD m_dwCircleActionTime;
	ioHashString m_ReadyCircle;
	D3DXVECTOR3 m_vCirclePos;

	AttackAttribute m_CircleActionAttack;
	ioHashString m_szCircleActionBuff;
	ioHashString m_szCircleMoveBuff;
	float m_fCircleActionReturnCheckLength;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();
	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	
protected:
	void SetLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetFireState( ioBaseChar *pOwner );
	void SetCircleMoveState( ioBaseChar *pChar );
	void SetCircleActionState( ioBaseChar *pChar, D3DXVECTOR3 vCirclePos );
	void SetCircleActionEndState( ioBaseChar *pChar );
	void WaitTargetReturn( ioBaseChar *pOwner );

	void CheckAimTime();
	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckVDir( ioBaseChar *pOwner );
	void CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage );	
	void CheckRotate( ioBaseChar *pChar );
	bool CheckTargetBuff( ioBaseChar *pChar, ioHashString szBuff );

	void UpdateAimMark( ioBaseChar *pChr, ioPlayStage *pStage );
	void DestroyAimMark( ioBaseChar *pChar );

	void CreateMagicCircle( ioBaseChar *pOwner );
	void UpdateMagicCircle( ioBaseChar *pOwner );

	void ApplyCircleActionEndState( ioBaseChar *pChar, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir );

public:
	ioTargetCircleMoveSkill();
	ioTargetCircleMoveSkill( const ioTargetCircleMoveSkill &rhs );
	virtual ~ioTargetCircleMoveSkill();
};

inline ioTargetCircleMoveSkill* ToTargetCircleMoveSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormalSkill = ToNormalSkill( pSkill );
	if( !pNormalSkill || pNormalSkill->GetSubSkillType() != NST_TARGET_CIRCLE_MOVE )
		return NULL;

	return dynamic_cast< ioTargetCircleMoveSkill* >( pNormalSkill );
}


