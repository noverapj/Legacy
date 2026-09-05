#ifndef _ioTargetDebuffSkill_h_
#define _ioTargetDebuffSkill_h_

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

struct EffectInfo
{
	UniqueObjID m_dwAimEffectID;
	UniqueObjID m_dwChargeEffectID;
	UniqueObjID m_dwFireEffectID;
	ioHashString m_szTargetName;

	DWORD m_dwCreateTime;

	bool m_bAir;

	EffectInfo()
	{
		m_dwAimEffectID = -1;
		m_dwChargeEffectID = -1;
		m_dwFireEffectID = -1;

		m_szTargetName.Clear();

		m_dwCreateTime = 0;

		m_bAir = false;
	}
};

typedef std::map<ioHashString,EffectInfo> TargetList;

class ioTargetDebuffSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_ZERO_AIM_FIRE,
		SS_AIM_FIRE,
		SS_END,
	};	

	enum SkillSync
	{
		SSC_UPDATE_TARGETLIST,
		SSC_ZERO_AIM_FIRE,
		SSC_AIM_FIRE,
	};

protected:
	SkillState m_SkillState;

	//ZeroAim
	ioHashString m_SkillZeroAimMotion;
	float m_fSkillZeroAimMotionLate;

	DWORD m_dwSkillZeroAimMotionEnd;
	DWORD m_dwSkillZeroAimAttackCnt;
	DWORD m_dwCurSkillZeroAimAttackCnt;	
	bool m_bSkillZeroAimAttackGauge;
	
	//Aim
	DWORD m_dwSkillAimMotionEnd;
	AttackAttribute m_AimAttribute;
	bool m_bSkillAimAttackGauge;

	//ZeroAim, Aim
	CEncrypt<DWORD> m_dwFireStartTime;
	CEncrypt<DWORD> m_dwKeyDurationTime;
	DWORD m_dwSetLoopCnt;
	DWORD m_dwCurAimMarkerChargeCnt;
	DWORD m_dwAimMarkerChargeEnableCnt;

	//TargetList
	TargetList m_TargetList;
	ioHashString m_LoopDelayAnimation;
	ioHashString m_LoopMoveAnimation;
	bool m_bSetMoveAni;

	//Send List
	ioHashStringVec m_vAddTargetList;
	ioHashStringVec m_vRemoveTargetList;
	ioHashStringVec m_vRemoveNonTargetList;

	//Target Search
	TargetWoundType m_TargetWoundType;
	CEncrypt<float>	m_fTargetAngle;
	CEncrypt<float>	m_fTargetMaxRange;
	CEncrypt<float>	m_fTargetMinRange;
	CEncrypt<DWORD> m_dwTargetMaxCnt;

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;
	
	CEncrypt<DWORD> m_dwSkillProtectedTime;
	DWORD m_dwStartSkillProtectedTime;

	//Mark
	DWORD m_dwMarkCurCnt;

	ioHashString m_szRedGuideMarker;
	ioHashString m_szBlueGuideMarker;

	ioHashString m_szRedAimMarker;
	ioHashString m_szBlueAimMarker;

	ioHashString m_szRedChargeMarker;
	ioHashString m_szBlueChargeMarker;

	ioHashString m_szRedFireMarker;
	ioHashString m_szBlueFireMarker;

	UniqueObjID m_dwGuidMaker;	
	UniqueObjID m_dwChargeMaker;

	bool m_bFireMaker;
	bool m_bResetFireMarker;
	bool m_bResetAimMarker;
	
	TargetList::iterator m_CurFireIter;

	CEncrypt<float> m_fScalingSpeed;
	CEncrypt<float> m_fScalingRate;

	float m_fCurScaleRate;
	float m_dwCurScaleStartTime;
	
	BuffInfoList m_TargetAimAttackBuffList;

	ioHashString m_SkillMapEffect;
	float m_fCharEffectOffset;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

	void LoadBuffList( ioINILoader &rkLoader );

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
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetZeroAimFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void OnZeroAimFireState( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetAimFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void OnAimFireState( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurTime );
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );

	void FindTarget( ioBaseChar *pChar, ioPlayStage* pStage );
	void AddTarget( ioPlayStage* pStage,
					ioBaseChar* pOwner,
					ioBaseChar* pTarget,
					DWORD dwCurTime,
					bool bMarker,
					bool CheckList=false );

	void UpdateTarget( ioBaseChar* pOwner, ioBaseChar* pTarget,  ioPlayStage* pStage, bool bMarker );
	void RemoveTarget( ioBaseChar* pOwner, ioBaseChar* pTarget, bool CheckList=false );
	void RemoveTarget( ioBaseChar* pOwner, ioHashString szTargetName );

	void UpdateTargetList( ioBaseChar* pOwner, ioPlayStage* pStage );

	void ResetSendList();
	void SendList( ioBaseChar* pOwner, ioPlayStage* pStage );

	void CheckNormalMoveAni( ioBaseChar* pChar );

	void SetMove();
	void SetPreDelay();

	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bCheckProtected = false );

protected:
	void StartScaleInfo();
	void ResetScaleInfo( DWORD dwCurTime = 0 );
	void UpdateScaleInfo( ioBaseChar *pChar, ioPlayStage *pStage );

	void CreateGuideMarker( ioBaseChar *pChar, ioPlayStage *pStage );
	void UpdateGuideMarker( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyGuideMarker( ioBaseChar *pChar );

	void CreateFireMarker( ioBaseChar *pChar, ioPlayStage *pStage, EffectInfo& pInfo );
	void UpdateFireMarker( ioBaseChar *pTarget, ioPlayStage *pStage, UniqueObjID dwEffectID );

	void CreateAimMarker( ioBaseChar* pChar, ioBaseChar *pTarget, ioPlayStage* pStage, EffectInfo& pInfo );
	void UpdateAimMarker( ioBaseChar *pTarget, ioPlayStage *pStage, UniqueObjID dwEffectID );
		
	void CreateChargeMarker( ioBaseChar *pChar, ioBaseChar *pTarget, ioPlayStage* pStage, EffectInfo& pInfo );
	void UpdateChargeMarker( ioBaseChar *pTarget, ioPlayStage *pStage, UniqueObjID dwEffectID );

	void DestroyMarker( ioBaseChar *pChar, DWORD& dwMarker );
	void UpdateAllMarker( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyAllMarker( ioBaseChar *pChar, ioPlayStage *pStage );


public:
	ioTargetDebuffSkill();
	ioTargetDebuffSkill( const ioTargetDebuffSkill &rhs );
	virtual ~ioTargetDebuffSkill();
};

inline ioTargetDebuffSkill* ToTargetDebuffSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_DEBUFF )
		return NULL;

	return dynamic_cast< ioTargetDebuffSkill* >( pSkill );
}

#endif
