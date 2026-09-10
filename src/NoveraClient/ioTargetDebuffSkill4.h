#ifndef _ioTargetDebuffSkill4_h_
#define _ioTargetDebuffSkill4_h_

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioTargetDebuffSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetDebuffSkill4 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_START,
		SS_DELAY,
		SS_AIM_FIRE,
		SS_AIM_MOVE_ATTACK,
		SS_JUMP_STATE,
		SS_END,
	};	

	enum SkillSync
	{
		SSC_UPDATE_TARGETLIST,
		SSC_UPDATE_ONLYONE_TARGET,
		SSC_AIM_FIRE,
		SSC_AIM_MOVE_ATTACK,
		SSC_JUMPPING,
		SSC_ROTATE,
		SSC_END,
	};

	enum AimTargetAttackType
	{
		ATAT_FIRE,
		ATAT_OWNER_MOVE,
		ATAT_TARGET_FIRE,
	};

protected:
	SkillState m_SkillState;
	AimTargetAttackType m_AimTargetAttackType;

	bool m_bSkillZeroAimAttackGauge;

	//Aim
	DWORD m_dwSkillAimMotionEnd;
	AttackAttribute m_AimAttribute;
	bool m_bSkillAimAttackGauge;
	//Aim Attack
	AttackAttribute m_AimAttackAttribute;
	D3DXVECTOR3	    m_vOwnerAttackDir;
	float			m_fOwnerDefaultAttackAngle;
	float			m_fMoveAttackSpeed;
	float			m_fCurMoveDist;
	float			m_fMaxMoveDist;
	DWORD			m_dwEnableAimAttackTime;
	DWORD			m_dwAimAttackStartTime;

	float			m_fReduceAttackSkillGauge;

	//Jump State
	ioHashString	m_NextJumppingAnimation;
	float			m_NextJumppingAniRate;

	//ZeroAim, Aim
	CEncrypt<DWORD> m_dwFireStartTime;
	CEncrypt<DWORD> m_dwKeyDurationTime;
	DWORD m_dwSetLoopCnt;
	DWORD m_dwCurAimMarkerChargeCnt;
	DWORD m_dwAimMarkerChargeEnableCnt;

	//TargetList
	TargetList m_TargetList;
	ioHashString m_OnlyOneTarget;
	ioHashString m_LoopDelayAnimation;
	ioHashString m_LoopMoveAnimation;
	bool m_bSetMoveAni;

	//Gauge Tic
	float		m_fExtraDecreaseGauge;
	DWORD		m_dwExtraDecreaseGaugeTic;
	DWORD		m_dwExtraDecreaseGaugeTicTime;
	float		m_fCurExtraDecreaseGauge;
	float		m_fGaugeDecreaseHeight;

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

	//////////////////////////////////////////////////////////////////////////

	float			m_fStartAngle;
	float			m_fStartMaxRange;
	DWORD			m_dwStartDuration;

	ioHashString	m_MoveAni;
	float			m_fMoveAniRate;

	float			m_fMoveSpeed;
	float			m_fUpDownSpeed;

	DWORD			m_dwFlyStartTime;
	float			m_fFlyCurRange;

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	DWORD		   m_dwSkillLifeTime; 

	float			m_fOffsetLook1;
	float			m_fOffsetY1;
	float			m_fOffsetX1;
	float			m_fOffsetLook2;
	float			m_fOffsetY2;
	float			m_fOffsetX2;

	bool			m_bSipping;//왼쪽 오른쪽 발사시 사용

	bool			m_bSkillInJump;
	ioBaseChar*		m_pTargetChar;

	float			m_fEnableJumppingHeight;
	float			m_fJumpAngle;
	float			m_fJumpMaxRange;
	DWORD			m_dwJumppingDuration;

	float			m_fCurJumpRange;

	DWORD			m_NoTragetAimEffectID;
	D3DXVECTOR3		m_fNoTragetAimOffset;

	bool			m_bUpDownCheckByAttackKey;

	int				m_iMaxAttackCount;
	int				m_iCurAttackCount;

	bool			m_bRemoveAllGauge;

	DWORD			m_dwStartAttackTime;

	DWORD			m_dwNoneTargetAimEffectID;
	D3DXVECTOR3		m_vNoneTargetAimOffset;
	
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
	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;

protected:
	void SetDelayState( ioBaseChar *pChar, ioPlayStage *pStage, bool bEraseTarget );

	void SetAimFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAimMoveAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void OnAimFireState( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurTime );
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );

	void FindTarget( ioBaseChar *pChar, ioPlayStage* pStage );
	void FindOneTarget( ioBaseChar *pChar, ioPlayStage* pStage );
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

	//Aim Move Attack
	bool CheckMove( ioBaseChar *pChar );
	void SetNextJumpState( ioBaseChar *pChar, ioPlayStage *pStage );
	bool ProcessJumpping( ioBaseChar *pChar, ioPlayStage *pStage );

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

	bool ProcessFlyStart( ioBaseChar *pChar );
	void ProcessFlyMove( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );

	void ChangeFlyMove( ioBaseChar *pOwner );

	void ClearTargetList( ioBaseChar *pChar, ioPlayStage *pStage );
	
protected:
	void Fire();

protected:
	void SetNoneTargetAimEffect( ioBaseChar *pChar, ioPlayStage *pStage );
	
public:
	ioTargetDebuffSkill4();
	ioTargetDebuffSkill4( const ioTargetDebuffSkill4 &rhs );
	virtual ~ioTargetDebuffSkill4();
};

inline ioTargetDebuffSkill4* ToTargetDebuffSkill4( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_DEBUFF4 )
		return NULL;

	return dynamic_cast< ioTargetDebuffSkill4* >( pSkill );
}

#endif
