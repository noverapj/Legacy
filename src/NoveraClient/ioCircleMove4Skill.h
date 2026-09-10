#pragma once


#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

typedef struct tagElementCircleInfo
{
	float m_fCircleFr;
	float m_fCircleSi;
	ioHashString m_CircleReadyEff;
	ioHashString m_CircleEff;

	float m_fAttackFr;
	float m_fAttackSi;
	float m_fAttackHe;

	ioHashString m_FireEff;
	WeaponInfo m_FireAtt;

	ioHashString m_IceEff;
	WeaponInfo m_IceAtt;

	ioHashString m_LightEff;
	WeaponInfo m_LightAtt;

	UniqueObjID m_CircleEffID;
	UniqueObjID m_AttEffID;

	tagElementCircleInfo()
	{
		Init();
	}

	void Init()
	{
		m_fCircleFr = 0.0f;
		m_fCircleSi = 0.0f;

		m_fAttackFr = 0.0f;
		m_fAttackSi = 0.0f;
		m_fAttackHe = 0.0f;

		m_CircleEffID = -1;
		m_AttEffID = -1;
	}

	void InitID()
	{
		m_CircleEffID = -1;
		m_AttEffID = -1;
	}
} ElementCircleInfo;

typedef std::vector< ElementCircleInfo > ElementCircleInfoList;

class ioCircleMove4Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_ACTION,
		SSS_CHANGE,
	};

protected:
	SkillState m_State;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	// magic circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	// skill end
	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	DWORD m_dwProtectTime;

	// etc
	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

protected:
	CEncrypt<bool> m_bSetElementAtt;
	
	int m_iCurElement;
	ElementCircleInfoList m_ElementCircleInfoList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void LoadElementCircleInfo( ioINILoader &rkLoader );

	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void ClearElementInfo();

	void SetElementEff( ioBaseChar *pChar );
	void ClearElementEff( ioBaseChar *pChar );

	void SetElementAttEff( ioBaseChar *pChar );
	void SetElementAtt( ioBaseChar *pChar );

	void ChangeElement( ioBaseChar *pChar );

	void UpdateElementEff( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioCircleMove4Skill();
	ioCircleMove4Skill( const ioCircleMove4Skill &rhs );
	virtual ~ioCircleMove4Skill();
};

inline ioCircleMove4Skill* ToCircleMove4Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE4 )
		return NULL;

	return dynamic_cast< ioCircleMove4Skill* >( pSkill );
}

