#ifndef _ioTargetTeleportSkill2_h_
#define _ioTargetTeleportSkill2_h_

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetTeleportSkill2 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE,
		SS_LOOP,
		SS_TELEPORT_S,
		SS_TELEPORT_E,
		SS_NO_TARGET_FIRE,
		SS_NORMAL_FIRE,
		SS_SPECIAL_FIRE,
		SS_END,
	};

	enum SkillSubState
	{
		SSS_NONE,
		SSS_NORMAL_TARGET,
		SSS_SPECIAL_TARGET,
	};

	enum SkillSync
	{
		SSC_NONE,
		SSC_UPDATE_TARGETLIST,
		SSC_TELEPORT,
		SSC_FIRE,
	};

	struct TargetInfo
	{
		ioHashString m_TargetName;
		UniqueObjID m_dwTargetMarkerID;

		D3DXVECTOR3 m_vAttackDir;
		DWORD m_dwWeaponIndex;

		TargetInfo()
		{
			Init();
		}

		void Init()
		{
			m_TargetName.Clear();
			m_dwTargetMarkerID = 0;

			m_vAttackDir = ioMath::VEC3_ZERO;
			m_dwWeaponIndex = 0;
		}
	};

	typedef struct tagTargetSearchInfo
	{
		TargetWoundType m_TargetWoundType;
		float m_fTargetAngle;
		float m_fTargetMaxRange;
		float m_fTargetMinRange;

		tagTargetSearchInfo()
		{
			Init();
		}

		void Init()
		{
			m_TargetWoundType = TWT_ALL;
			m_fTargetAngle = 0.0f;
			m_fTargetMaxRange = 0.0f;
			m_fTargetMinRange = 0.0f;
		}
	} TargetSearchInfo;

protected:
	SkillState m_SkillState;
	SkillSubState m_SkillSubState;

	//Target Search
	TargetSearchInfo m_NormalTargetSearch;
	TargetSearchInfo m_JumpTargetSearch;

	// target info
	TargetInfo m_CurTargetInfo;

	// marker
	UniqueObjID m_dwNoTargetMarkerID;

	ioHashString m_RedNoTargetMarker;
	ioHashString m_RedNormalTargetMarker;
	ioHashString m_RedSpecialTargetMarker;
	ioHashString m_RedFireTargetMarker;

	ioHashString m_BlueNoTargetMarker;
	ioHashString m_BlueNormalTargetMarker;
	ioHashString m_BlueSpecialTargetMarker;
	ioHashString m_BlueFireTargetMarker;

	CEncrypt<float> m_fMaxScaleRate;
	CEncrypt<float> m_fMinScaleRate;

	DWORD m_dwNormalTargetDuration;
	DWORD m_dwSpecialTargetDuration;
	DWORD m_dwChangeSubStateTime;

	// target teleport
	ioHashString m_TargetTeleportStartAni;
	float m_fTargetTeleportStartAniRate;

	ioHashString m_TargetTeleportEndAni;
	float m_fTargetTeleportEndAniRate;

	DWORD m_dwTargetTeleportStartTime;
	DWORD m_dwTargetTeleportEndTime;

	CEncrypt< float > m_fTeleportOffSet;

	// attack
	CEncrypt<int> m_iCurComboCnt;

	AttributeList m_NoTargetAttackCombo;
	AttributeList m_NormalTargetAttackCombo;
	AttributeList m_SpecialTargetAttackCombo;

	DWORD m_dwNextComboTime;
	DWORD m_dwComboEndTime;

	// loop state
	ioHashString m_LoopDelayAnimation;
	float m_fLoopDelayAnimationRate;

	ioHashString m_JumpLoopDelayAnimation;
	float m_fJumpLoopDelayAnimationRate;

	ioHashString m_LoopMoveAnimation;
	bool m_bSetMoveAni;

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;
	
	// 무적시간
	CEncrypt<DWORD> m_dwSkillProtectedTime;
	DWORD m_dwStartSkillProtectedTime;

	ioHashString m_SkillMapEffect;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

	virtual bool IsNoDropState() const;
	virtual bool IsAttackEndState() const;

	virtual float GetSkillMoveRate() const;

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void CheckNormalMoveAni( ioBaseChar* pChar );

	bool CheckSkillGauge( ioBaseChar *pChar );

	void CheckSkillSubState( ioBaseChar *pChar );
	void ChangeSkillSubState( ioBaseChar *pChar, SkillSubState eState );

	void FindTarget( ioBaseChar *pChar, ioPlayStage* pStage );
	void FindTargetList( ioBaseChar *pChar, ioPlayStage* pStage, OUT BaseCharDistanceSqList &vTargetList );
	void UpdateTargetList( ioBaseChar* pOwner, ioPlayStage* pStage );

	void UpdateMarker( ioBaseChar *pChar, ioPlayStage *pStage );

	void CreateNoTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage );
	void UpdateNoTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage );

	void ChangeTargetMarker( ioBaseChar *pChar );
	void UpdateTargetMarker( ioBaseChar *pTarget, UniqueObjID eEffectID, ioPlayStage *pStage );
	void UpdateTargetMarkerScale( ioBaseChar *pChar, ioPlayStage *pStage );

	void DestroyAlllMaker( ioPlayStage *pStage );

protected:
	void SetTargetTeleportStart( ioBaseChar *pChar, ioPlayStage *pStage );
	void OnTargetTeleportStart( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetTargetTeleportEnd( ioBaseChar *pChar, ioPlayStage *pStage );
	void OnTargetTeleportEnd( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetTargetComboState( ioBaseChar *pChar, ioPlayStage *pStage );
	void OnTargetComboState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetNextTargetComboState( ioBaseChar *pChar, ioPlayStage *pStage );

	void ChangeTarget( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );

	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bCheckProtected = false );
	bool CheckRangeAngle( ioBaseChar *pChar, ioBaseChar *pTarget );

// normal 과 점프 구분처리용
protected:
	const ioHashString& GetLoopDelayAni();
	float GetLoopDelayAniRate();

	void GetTargetSearchInfo( OUT TargetSearchInfo &rkSearchInfo );

public:
	ioTargetTeleportSkill2();
	ioTargetTeleportSkill2( const ioTargetTeleportSkill2 &rhs );
	virtual ~ioTargetTeleportSkill2();
};

inline ioTargetTeleportSkill2* ToTargetTeleportSkill2( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_TELEPORT2 )
		return NULL;

	return dynamic_cast< ioTargetTeleportSkill2* >( pSkill );
}

#endif
