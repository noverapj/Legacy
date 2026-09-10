#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioMultiTeleportSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_CONTROL,
		SS_TELEPORT,
		SS_CANCEL,
		SS_END,
	};

	enum GrapplingTargetState
	{
		GTS_NONE,
		GTS_STAND_FRONT,
		GTS_STAND_BACK,
	};

	enum SkillSync
	{
		SSC_GRAPPLING,
		SSC_CANCEL,
		SSC_TELEPORT,
		SSC_ESCAPE_TARGET,
	};

	struct TargetDistInfo
	{
		ioHashString	m_TargetName;
		float			m_fLenth;

		TargetDistInfo()
		{
			m_TargetName.Clear();
			m_fLenth = 0.0f;
		}
	};
	typedef std::vector<TargetDistInfo>	TargetDistInfoList;

	struct GrapplingTargetInfo
	{
		ioHashString				m_TargetName;
		D3DXVECTOR3					m_vPos;
		std::vector< UniqueObjID >	m_EffectList;
		GrapplingTargetState		m_GrapplingTargetState;
		DWORD						m_dwEnableCancelTime;
		DWORD						m_dwEnableCancelEndTime;
		DWORD						m_dwCancelValue;

		GrapplingTargetInfo()
		{
			m_TargetName.Clear();
			m_vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_EffectList.clear();
			m_GrapplingTargetState = GTS_NONE;
			m_dwEnableCancelTime = 0;
			m_dwEnableCancelEndTime = 0;
			m_dwCancelValue = 0;
		}
	};
	typedef std::vector<GrapplingTargetInfo>	GrapplingTargetInfoList;

protected:
	SkillState				m_SkillState;
	GrapplingTargetState	m_GrapplingTargetState;

	ioHashString	m_EndAnimation;
	CEncrypt<float>	m_fEndAniRate;

	DWORD			m_dwFireStartTime;
	CEncrypt<DWORD>	m_dwSkillProtectTime;

	// Circle
	DWORD			m_dwControlStartTime;
	CEncrypt<DWORD>	m_dwCircleDuration;
	CEncrypt<float>	m_fCircleRange;
	CEncrypt<float>	m_fCircleMoveSpeed;
	CEncrypt<float>	m_fCircleHeightGap;
	CEncrypt<float>	m_fCircleOffSet;

	ioHashString m_ReadyCircle;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwTargetCircleEffect;

	D3DXVECTOR3 m_vCirclePos;

	// skill loop animation
	ioHashString	m_SkillLoopMotion;
	CEncrypt<float>	m_fSkillLoopMotionRate;
	CEncrypt<DWORD>	m_dwSkillLoopLoopDuration;

	// Äµ½½ °ü·Ã
	CEncrypt<DWORD>	m_dwFastValue;
	CEncrypt<DWORD>	m_dwGrapplingMaxValue;
	CEncrypt<DWORD>	m_dwEnableGrapplingCancelTime;
	CEncrypt<DWORD>	m_dwEnableGrapplingCancelEndTime;
	bool			m_bPreLeft;

	ioHashString	m_szTargetCancelMotion;
	ForceInfoList	m_vTargetCancelForce;

	// Å¸°Ù Á¤º¸
	CEncrypt<int>	m_iMaxTarget;
	ioHashString	m_TargetName;
	ioHashStringVec	m_SearchTargetNameList;
	GrapplingTargetInfoList	m_GrapplingTargetInfoList;

	// Å¸°Ù °Ë»ö
	CEncrypt<float>	m_fTargetRange;
	CEncrypt<float>	m_fTargetAngle;
	TargetWoundType	m_TargetWoundType;
	CEncrypt<bool>	m_bDisableColGrap;

	// Guide Effect
	ioHashString	m_GuideEffect;
	CEncrypt<int>	m_iMaxEffectCount;
	float			m_fMaxEffectRange;

	// Å¸°Ù ¿öÇÁ
	CEncrypt<int>	m_iEnablePositionCnt;
	CEncrypt<float>	m_fEnablePositionRange;
	CEncrypt<float>	m_fEnableMaxPositionRange;
	CEncrypt<bool>	m_bCheckDropZone;
	CEncrypt<float>	m_fHightOffset;
	CEncrypt<float>	m_fTargetHightOffset;

	ioHashString	m_TeleportStartEffect;
	DWORD			m_dwTeleportStartEffectID;
	DWORDVec		m_dwTeleportStartEffectIDList;

	// Å¸°Ù ¹öÇÁ
	ioHashStringVec	m_TargetBuffList;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	void LoadForceInfo( const char *szForce, ForceInfoList &rkForceInfoList, ioINILoader &rkLoader );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pAttacker, ioBaseChar *pTarget );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void FindTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	void SetSkillActionState( ioBaseChar *pChar );
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillControlState( ioBaseChar *pChar );
	void SetSkillTeleportState( ioBaseChar *pChar );
	void SetEscapeTarget( ioBaseChar *pChar, ioBaseChar *pTarget );

	void ProcessWoundGrapplingState( ioBaseChar *pChar );
	void CheckTargetState( ioBaseChar *pChar );

	void AddTargetBuff( ioBaseChar *pChar);

public:
	bool isEnableCancel( ioBaseChar *pTarget );
	void SetSkillEndState( ioBaseChar *pChar );

// Guide Effect
protected:
	void ProcessGuideEffect( ioBaseChar *pOwner );
	void DestroyGuideEffect();
	void DestroyGuideEffect( ioHashString szTargetName );
	bool CheckGuideEffectCreated( ioBaseChar *pOwner );
	void ArrangeGuideEffect( ioBaseChar *pOwner );

public:
	ioMultiTeleportSkill();
	ioMultiTeleportSkill( const ioMultiTeleportSkill &rhs );
	virtual ~ioMultiTeleportSkill();

protected:
	class DistSort : std::binary_function< const TargetDistInfo&, const TargetDistInfo&, bool >
	{
	public:
		bool operator()( const TargetDistInfo &lhs , const TargetDistInfo &rhs ) const
		{
			if( lhs.m_fLenth < rhs.m_fLenth )
			{
				return true;
			}

			return false;
		}
	};
};

inline ioMultiTeleportSkill* ToMultiTeleport( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MULTI_TELEPORT )
		return NULL;

	return dynamic_cast< ioMultiTeleportSkill* >( pAttack );
}
