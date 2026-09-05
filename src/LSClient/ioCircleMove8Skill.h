#pragma once
#include "ioNormalSkill.h"

class ioCircleMove8Skill : public ioNormalSkill
{
private:
	struct DefaultGroundDirInfo
	{
		WeaponInfo DefaultWeaponInfo;
		float m_fChangeRange;
		float m_fChangeAngle;

		DefaultGroundDirInfo()
		{
			m_fChangeRange = 0.0f;
			m_fChangeAngle = 0.0f;
		}
	};
	typedef std::vector< DefaultGroundDirInfo > DefaultGroundDirInfoList;

	struct GroundAttackInfo
	{
		WeaponInfo		WeaopnData;
		ioHashString	szTargetName;
		D3DXVECTOR3		vPos;
		DWORD			dwEffectID;

		GroundAttackInfo()
		{
			WeaopnData.Init();
			szTargetName.Clear();
			vPos = ioMath::VEC3_ZERO;
		}
	};

	typedef std::vector< GroundAttackInfo > GroundAttackInfoList;

private:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_ACTION,
		SSS_END,
	};
private:
	SkillState					m_SkillState;

	//Loop
	ioHashString				m_szLoopAni;
	float						m_fLoopAniRate;
	DWORD						m_dwLoopStartTime;
	DWORD						m_dwLoopDuration;

	//Magic Circle
	ioHashString				m_ReadyCircle;
	ioHashString				m_RedGuidCircle;
	ioHashString				m_BlueGuidCircle;
	float						m_fCircleRange;
	float						m_fCircleMoveSpeed;
	float						m_fCircleHeightGap;
	float						m_fCircleOffSet;
	D3DXVECTOR3					m_vCirclePos;

	//Action
	DWORD						m_dwFireStartTime;
	//서칭 관련
	float						m_fSearchRadius;
	float						m_fSearchHeight;
	int							m_iSearchMaxTargetCount;
	WeaponInfo					m_TargetAttackWeaponInfo;

	//찾은 타겟 리스트
	BaseCharDistanceSqList		m_vSearchTargetNameList;
	//이펙트 이름
	ioHashString				m_szGroundEffectName;
	
	//attack 세팅 저장
	GroundAttackInfoList		m_GroundAttackInfoList;
	
	//default관련 세팅
	DefaultGroundDirInfoList	m_vGroundDefaultOffsetList;

private:
	void ClearData();

	void CreateMagicCircle( ioBaseChar *pOwner );
	void CreateTargetGroundEffect( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CreateTargetGroundWeapon( ioBaseChar *pOwner, ioPlayStage *pStage );

	void SetLoopState( ioBaseChar *pOwner );
	void SetActionState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pOwner );

	void SearchTarget( ioBaseChar *pChar, ioPlayStage *pStage );

	void DestroyTargetGroundEffect( ioBaseChar *pOwner, ioPlayStage *pStage );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	virtual bool OnSkillStart( ioBaseChar *pOwner );
	virtual void OnSkillEnd( ioBaseChar *pOwner );	
	virtual void OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void SetNormalSkill( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );
public:
	ioCircleMove8Skill(void);
	ioCircleMove8Skill( const ioCircleMove8Skill &rhs );
	~ioCircleMove8Skill(void);
};

inline ioCircleMove8Skill* ToCircleMove8Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE8 )
		return NULL;

	return dynamic_cast< ioCircleMove8Skill* >( pSkill );
}