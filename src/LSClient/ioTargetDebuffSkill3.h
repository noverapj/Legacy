#ifndef _ioTargetDebuffSkill3_h_
#define _ioTargetDebuffSkill3_h_

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetDebuffSkill3 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE,
		SS_LOOP,
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
			m_dwTargetMarkerID = 0;

			m_vAttackDir = ioMath::VEC3_ZERO;
			m_dwWeaponIndex = 0;
		}
	};

	typedef std::map<ioHashString,TargetInfo> TargetInfoMap;

	struct TargetFireInfo
	{
		AttackAttribute m_TargetAttack;
		ioHashStringVec m_vTargetBuffList;

		TargetFireInfo()
		{
			m_TargetAttack.Init();
			m_vTargetBuffList.clear();
		}
	};

protected:
	SkillState m_SkillState;
	SkillSubState m_SkillSubState;

	// target cnt
	CEncrypt<int> m_iMaxTargetCnt;

	//Target Search
	TargetWoundType m_TargetWoundType;
	CEncrypt<float>	m_fTargetAngle;
	CEncrypt<float>	m_fTargetMaxRange;
	CEncrypt<float>	m_fTargetMinRange;

	// target info
	TargetInfoMap m_TargetInfoMap;

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

	// target fire normal & special
	TargetFireInfo m_NoTargetFireInfo;
	TargetFireInfo m_NormalTargetFireInfo;
	TargetFireInfo m_SpecialTargetFireInfo;

	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;

	// loop state
	ioHashString m_LoopDelayAnimation;
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
	void UpdateTargetList( ioBaseChar* pOwner, ioPlayStage* pStage );

	void UpdateMarker( ioBaseChar *pChar, ioPlayStage *pStage );

	void CreateNoTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage );
	void UpdateNoTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage );

	void ChangeTargetMarker( ioBaseChar *pChar );
	void UpdateTargetMarker( ioBaseChar *pTarget, UniqueObjID eEffectID, ioPlayStage *pStage );
	void UpdateTargetMarkerScale( ioBaseChar *pChar, ioPlayStage *pStage );

	void DestroyAlllMaker( ioPlayStage *pStage );

protected:
	void SetTargetFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void OnTargetFireState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );

	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bCheckProtected = false );

public:
	ioTargetDebuffSkill3();
	ioTargetDebuffSkill3( const ioTargetDebuffSkill3 &rhs );
	virtual ~ioTargetDebuffSkill3();
};

inline ioTargetDebuffSkill3* ToTargetDebuffSkill3( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_DEBUFF3 )
		return NULL;

	return dynamic_cast< ioTargetDebuffSkill3* >( pSkill );
}

#endif
