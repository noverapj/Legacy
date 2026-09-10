#ifndef _ioTargetDebuffSkill5_h_
#define _ioTargetDebuffSkill5_h_

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioTargetDebuffSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetDebuffSkill5 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_ZERO_AIM_FIRE,
		SS_AIM_FIRE,
		SS_FIND_ENEMY,
		SS_WARP_ATTACK,
		SS_END,
	};	

	enum SkillSync
	{
		SSC_UPDATE_TARGETLIST,
		SSC_ZERO_AIM_FIRE,
		SSC_AIM_FIRE,
		SSC_END,
	};

protected:
	SkillState m_SkillState;

	bool m_bSkillZeroAimAttackGauge;

	//Aim
	DWORD m_dwSkillAimMotionEnd;
	AttackAttribute m_AimAttribute;
	AttackAttribute m_ZeroAimAttribute;
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

	//FindEnemyByBuff
	DWORD			m_dwFindEnemyMotionEnd;
	ioHashString	m_FindEnemyAnimation;
	float			m_fFindEnemyAnimationRate;

	float			m_fFindEnemyMaxLength;

	//AttackEnemyByBuff
	ioHashString	m_stEnemyFindBuff;
	float			m_fLookDist;

	//WarpAttack
	AttackAttribute m_WarpAttack;
	DWORD m_dwWarpAttackMotionEnd;
	DWORD m_dwWarpAttackMotionKeyReserveTime;
	bool m_bWarpAttackReserved;

	//실제 거리와 상관없이 과녁 정도로 사용될 거리
	CEncrypt<float> m_fTargetShowRange;

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

	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 m_vAttackDir;
	ioHashString m_szTargetName;

	DWORD m_dwLoopTime;
	DWORD m_dwCheckLoopTime;

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
	virtual bool IsNoDropState() const;
	virtual bool IsAttackEndState() const;

protected:
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetAimFireState( ioBaseChar *pChar, ioPlayStage *pStage, bool bZeroAim = false );
	void OnAimFireState( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurTime, bool bZeroAim = false );
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

	void FindEnemyMotionState( ioBaseChar *pChar, ioPlayStage *pStage );

	void FindEnemyByBuff( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetWarpAttack( ioBaseChar *pChar, ioPlayStage *pStage, ioBaseChar *pTarget );

protected:
	void Fire();

public:
	ioTargetDebuffSkill5();
	ioTargetDebuffSkill5( const ioTargetDebuffSkill5 &rhs );
	virtual ~ioTargetDebuffSkill5();
};

inline ioTargetDebuffSkill5* ToTargetDebuffSkill5( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_DEBUFF5 )
		return NULL;

	return dynamic_cast< ioTargetDebuffSkill5* >( pSkill );
}

#endif
