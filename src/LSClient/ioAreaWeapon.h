#pragma once


#include "ioAttackAttribute.h"

class SP2Packet;
class ioINILoader;

class ioPlayStage;
class ioPlayEntity;
class ioBaseChar;
class ioSkill;
class ioItem;
class ioWeapon;


struct ChangeMoveInfo
{
	DWORD m_dwChangeTime;

	float m_fChangeSpeed;
	float m_fChangeAngleH;
	float m_fChangeAngleV;

	ChangeMoveInfo()
	{
		m_dwChangeTime = 0;

		m_fChangeSpeed = 0.0f;
		m_fChangeAngleH = 0.0f;
		m_fChangeAngleV = 0.0f;
	}
};

struct CrossEquationInfo
{
	float m_fXCoefficient;
	float m_fYCoefficient;
	float m_fConstant;

	CrossEquationInfo()
	{
		m_fXCoefficient = 0.0f;
		m_fYCoefficient = 0.0f;
		m_fConstant = 0.0f;
	}
};

typedef std::vector< ChangeMoveInfo > ChangeMoveInfoList;

class ChangeMoveSort : std::binary_function< const ChangeMoveInfo&, const ChangeMoveInfo&, bool >
{
public:
	bool operator()( const ChangeMoveInfo &lhs , const ChangeMoveInfo &rhs ) const
	{
		if( lhs.m_dwChangeTime < rhs.m_dwChangeTime )
		{
			return true;
		}
		else if( lhs.m_dwChangeTime == rhs.m_dwChangeTime )
		{
			if( lhs.m_fChangeAngleV < rhs.m_fChangeAngleV )
				return true;
		}
		return false;
	}
};


class ioAreaWeapon
{
public:
	enum TargetType
	{
		TT_NONE,
		TT_OWNER,
		TT_TEAM,
		TT_ENEMY,
	};

	enum AreaType
	{
		AT_NONE,
		AT_TIC,
		AT_INOUT,
		AT_SCALE,
		AT_DOUBLE,
		AT_ATTACK,
		AT_DESTROY,
		AT_OVERLAP
	};

	enum AreaSubType
	{
		ST_NONE,
		ST_POINT_TO_POINT,
	};

	enum CreateType
	{
		CT_NORMAL,
		CT_STRUCT,
		CT_ATTACHED,
		CT_TARGET_ATTACHED,
		CT_MAP,
		CT_SKILL_DEPENDENCY,
	};

	enum AreaWeaponState
	{
		AWS_READY,
		AWS_ACTION,
	};

	enum AreaMoveType
	{
		AMT_NORMAL,
		AMT_OWNER,
		AMT_TRACE,
		AMT_OWNER_ROT,
	};

	enum AreaUsedType
	{
		AUT_NONE,
		AUT_USED,
	};

	enum AreaSubSyncType
	{
		ASST_POINT_TO_POINT,
	};

	enum VertDirType
	{
		VDT_NONE,
		VDT_UP,
		VDT_DOWN,
	};

	enum RandomGenerateFieldType
	{
		RFT_CIRCLE,
		RFT_RECTANGLE,
	};

protected:
	ioHashString m_AreaWeaponName;

	DWORD m_dwAreaWeaponIndex;
	DWORD m_dwAreaWeaponMapIndex;
	DWORD m_dwCreateWeaponIndex;

	CEncrypt<TeamType> m_TeamType;

	AreaWeaponState m_AreaWeaponState;	

	bool		 m_bHideEffect;			// AreaWeapon 자신과 해당 적에게만 보이도록 할때 사용 2020-10-28
	ioHashString m_OwnerName;			// AreaWeapon 생성자
	ioHashString m_TargetName;			// AreaWeapon 소유자(붙어다니는 경우)
	
	ioHashString m_AreaWeaponEffectBlue;
	ioHashString m_AreaWeaponEffectRed;
	ioHashString m_AreaWeaponReadyEffectBlue;
	ioHashString m_AreaWeaponReadyEffectRed;
	ioHashString m_AreaWeaponEndEffectBlue;
	ioHashString m_AreaWeaponEndEffectRed;

	bool m_bForceEnd;

	ioHashString m_TargetMapEffect;

	UniqueObjID m_dwAreaWeaponEffect;
	ioHashString m_AreaweaponSound;
	UniqueObjID m_AreaweaponSoundID;

	ioHashString m_OwnerSkill;
	ioHashString m_OwnerItem;

	D3DXQUATERNION m_qtFirstRotate;
	D3DXQUATERNION m_qtRotate;

	D3DXVECTOR3 m_vFirstPos;
	D3DXVECTOR3 m_vStartPos;
	D3DXVECTOR3 m_vPreStartPos;

	//////////////////////////////////////////////////////////////////////////
	bool		m_bUseOffSet;
	float		m_fOffSet;
	//////////////////////////////////////////////////////////////////////////

	CEncrypt<bool> m_bEnableAttacker;
	CEncrypt<bool> m_bEnableAttackCnt;
	CEncrypt<bool> m_bOneTimeUse;
	CEncrypt<bool> m_bUsedAreaWeapon;

	// 생성방식
	CreateType m_CreateType;

	// 적용방식
	AreaType m_AreaType;

	//상세 적용 방식
	AreaSubType m_SubAreaType;

	// 적용 조건
	TargetColType m_TargetColType;
	TargetWoundType m_TargetWoundType;

	CEncrypt<float> m_fTargetEnableHeight;

	// 유지시간
	CEncrypt<DWORD> m_dwLiveTime;
	CEncrypt<DWORD> m_dwLiveTimeByNPC;
	CEncrypt<DWORD> m_dwLiveTimeByFC;
	CEncrypt<DWORD> m_dwReadyTime;
	CEncrypt<DWORD> m_dwCreatedTime;

	// 게이지 사용
	float m_fNeedGauge;
	DWORD m_dwGaugeTicTime;
	DWORD m_dwGaugeCheckTime;

	// 버프 적용 주기
	CEncrypt<DWORD> m_dwCheckTicTime;
	DWORD m_dwCheckProcessTime;

	// 적용 범위
	CEncrypt<float> m_fCollisionRange;
	CEncrypt<float> m_fCollisionInRange;
	CEncrypt<float> m_fAreaAngle;

	CEncrypt<float> m_fUpHeight;
	CEncrypt<float> m_fUnderHeight;
	CEncrypt<float> m_fWidth;

	CEncrypt<float> m_fCurRange;
	CEncrypt<float> m_fCurInRange;

	// 확대 사용
	float m_fScaleRate;
	DWORD m_dwScaleSlerpTime;

	// 삭제 방식(추가:윤태철)
	bool m_bUseLengthKill;
	float m_fKillLength;

	// Force
	CEncrypt<float> m_fAreaForceAmt;
	CEncrypt<float> m_fAreaForceFriction;

	// Move
	AreaMoveType m_AreaMoveType;

	bool m_bOwnerMoveDir;
	CEncrypt<float> m_fAreaMoveSpeed;
	D3DXVECTOR3 m_vAreaMoveDir;

	// Move : Change
	DWORD m_dwPreFrameTime;
	CEncrypt<DWORD> m_dwChangeStartTime;
	CEncrypt<DWORD> m_dwChangeCheckTime;
	ChangeMoveInfoList m_ChangeMoveInfoList;

	// Move : Trace
	bool m_bUsedTrace;
	DWORD m_dwLostTraceTime;
	CEncrypt<DWORD> m_dwNotFindWaitTime;

	DWORD m_dwSencingStartTime;
	CEncrypt<DWORD> m_dwSencingWaitTime;

	ioHashString m_TraceTarget;
	CEncrypt<float> m_fTargetSearchRange;
	CEncrypt<float> m_fTargetTraceRange;

	// Move : LiveArea
	bool m_bUseLiveArea;
	D3DXVECTOR3 m_vMinLivePos;
	D3DXVECTOR3 m_vMaxLivePos;

	DWORD m_dwLiveAreaTime;
	DWORD m_dwLiveAreaEndTime;

	// Move : Owner Rot
	bool m_bFirstMove;
	DWORD m_dwRotateMaxTime;

	// Rotate
	float m_fRotationTime;

	// 버프
	ioHashStringVec m_vOwnerBuffList;
	//ioHashStringVec m_vTeamBuffList;
	//ioHashStringVec m_vEnemyBuffList;
	BuffInfoList m_vTeamBuffList;
	BuffInfoList m_vEnemyBuffList;
	ioHashStringVec m_vTeamBuffOnCreateWeaponList;

	// 다음에 발동시킬 웨폰
	WeaponInfoList m_vNextAttackAttributeByTeamBuff;
	// 위 웨폰을 팀원 수 에따라 발동 시키는게 아니라 한번만 발동시키는지 체크(true : 1번 발동, false : 팀 원 수에 따라 발동 )
	typedef std::map< int, bool > CheckOnceCallAttAttrMap;
	CheckOnceCallAttAttrMap m_CheckOnceCallAttAttrMap;

	// 다음 웨폰 생성 함수 한번만 호출되도록 
	bool m_bCallNextWeaponAttribute;

	bool m_bLive;

	BaseCharList m_vNewTargetList;
	ioHashStringVec m_vCurTargetList;
	ioHashStringVec m_vCurBlockList;

	float m_fExtraBuffDuration;

	// AttackAttribute
	WeaponInfo m_WeaponInfo;
	DWORD m_dwWeaponDuration;
	DWORD m_dwWeaponBaseIndex;

	//추가 yongjun
	//End AttackAttribute
	WeaponInfo m_EndWeaponInfo;
	DWORD m_dwEndWeaponDuration;
	ioHashString m_AreaweaponEndSound;
	//

	// Overlap AttackAttribute
	WeaponInfo m_OverlapWeaponInfo;
	DWORD m_dwOverlapWeaponDuration;
	
	CEncrypt<bool> m_bUseRandomAttack;
	CEncrypt<bool> m_bUseRandomAttackFixedStartPos;

	CEncrypt<DWORD> m_dwGenerateStartTime;
	CEncrypt<DWORD> m_dwMinGenerateTime;
	CEncrypt<DWORD> m_dwMaxGenerateTime;
	CEncrypt<DWORD> m_dwNextGenerateTime;
	
	CEncrypt<float> m_fGenerateHeight;

	GenerateList m_vGenerateList;
	CEncrypt<DWORD> m_dwGenerateSeed;
	D3DXVECTOR3 m_GeneratePos;

	VertDirType     m_VertDirType;
	RandomGenerateFieldType m_RandomGenerateFieldType;

	// double type
	CEncrypt<float> m_fDoubleInRange;

	ioHashStringVec m_vOwnerBuffList2;
	ioHashStringVec m_vTeamBuffList2;
	ioHashStringVec m_vEnemyBuffList2;
	ioHashStringVec m_WeaponEndBuffList;
		
	//only for point to point type data
	UniqueObjID m_dwFirstEffect;
	UniqueObjID m_dwSecondEffect;
	D3DXVECTOR3 m_vFirstPointPos;
	D3DXVECTOR3 m_vSecondPointPos;

	DWORD m_dwPointToPointDelayProcessTime;
	DWORD m_dwPointToPointStartProcessTime;

	float m_fEffectOffset;
	float m_fEffectHeightOffset;
	float m_fDropHeight;
	float m_fDropEnableRange;
	float m_fEnablePositionRange;
	float m_fEnableMaxPositionRange;
	
	float m_fPointToPointMoveRanage;
	float m_fPointToPointTotalMoveAmount;

	ioHashString m_pOwnerItemName;
	ioHashString szTargetIdentifyBuffName;

	// attack type
	D3DXVECTOR3 m_vTargetAttPos;
	DWORD m_dwTargetAttMaxCnt;
	DWORD m_dwTargetAttCurCnt;
	
	DWORD m_dwCheckTargetAttTime;
	CEncrypt<DWORD> m_dwTargetAttGap;

	ioHashStringVec m_TargetAttEffList;
	UniqueObjID m_dwCurTargetAttEff;

	CEncrypt<float> m_fInvisibleRange;

	// Refresh Collision list
	CEncrypt<DWORD> m_dwRefreshDuration;
	struct RefreshCollisionInfo
	{
		ioHashString m_CharName;
		DWORD        m_dwCollisionTime;
		RefreshCollisionInfo( const ioHashString &szName, DWORD dwCollisionTime )
		{
			m_CharName = szName;
			m_dwCollisionTime = dwCollisionTime;
		}
	};
	typedef std::vector<RefreshCollisionInfo> vRefreshCollisionList;
	vRefreshCollisionList m_RefreshCollisionList;

	bool m_bDelayCreateAreaAttack;
	DWORD m_dwDelayCreateAreaAttackTime;
	DWORD m_dwCheckCreateAreaAttackTime;

	bool m_bSetRepeatAttack;
	bool m_bTraceTargetCheckTeam;
	bool m_bCreateOnlyOne;

	ioHashStringVec m_vPassDummyList;
	typedef std::vector<ioWeapon::WeaponType> vDestroyWeaponList;
	vDestroyWeaponList m_vDestroyWeaponList;

	// 겹치는 에어리어 웨폰
	ioHashStringVec m_vOverlapAreaWeapon;
	TargetType m_OverlapTargetType;

public:
	ioAreaWeapon* Clone();
	void LoadProperty( ioINILoader &rkLoader );

public:
	void SetInitInfo( const ioHashString& szOwner,
					  const ioHashString& szTarget,
					  const ioHashString& szSkill,
					  ioPlayStage *pStage,
					  const D3DXVECTOR3 &vStartPos,
					  const D3DXQUATERNION &qtRot,
					  DWORD dwIndex,
					  int eCreateType );

	void ProcessState( ioPlayStage *pStage );
	void DestroyAreaWeapon( ioPlayStage *pStage, bool bActive = true );

	void SetLiveTime( DWORD dwLiveTime );

	DWORD GetCurLiveTime();
	void SetCurLiveTime( DWORD dwCurGapTime );
	void SetAreaReadyTime( DWORD dwReadyTime );

	void FillCurTargetList( SP2Packet &rkPacket );
	void ApplyCurTargetList( ioHashStringVec vTargetList );
	bool IsCreateByStruct();

	void SetAreaPosition( const D3DXVECTOR3& vNewPos, ioPlayStage *pStage );
	void SetAreaRotation( const D3DXQUATERNION &qtTargetRot );
	void SetAreaDirection( const D3DXVECTOR3& vDir );
	void SetAreaRange( float fRange ){ m_fCollisionRange = fRange; }
	void SetAreaWidth( float fWidth ){ m_fWidth = fWidth; }
	void SetAreaEffectScale( ioPlayStage *pStage, D3DXVECTOR3 vScale );
	
	float GetAreaRange(){ return m_fCollisionRange; }
	float GetAreaWidth(){ return m_fWidth; }

	inline bool IsLive() const { return m_bLive; }
	inline bool IsCreateOnlyOne()	{ return m_bCreateOnlyOne; }

	inline const ioHashString& GetAreaWeaponName() const { return m_AreaWeaponName; }
	inline int GetCreateType() const { return m_CreateType; }

	inline const ioHashString& GetOwnerName() const { return m_OwnerName; }
	inline const ioHashString& GetTargetName() const { return m_TargetName; }
	inline const ioHashString& GetOwnerSkillName() const { return m_OwnerSkill; }

	inline const D3DXVECTOR3& GetStartPos() const { return m_vStartPos; }
	inline const D3DXVECTOR3& GetPreStartPos() const { return m_vPreStartPos; }
	inline const D3DXQUATERNION& GetRotation() const { return m_qtRotate; }

	inline DWORD GetAreaWeaponIndex() const { return m_dwAreaWeaponIndex; }
	inline float GetAreaWeaponMoveSpeed() const { return m_fAreaMoveSpeed; }

	void ApplyMoveInfo( SP2Packet &rkPacket, ioPlayStage *pStage );
	void ApplySubInfo( SP2Packet &rkPacket, ioPlayStage *pStage );

	void SetAreaWeaponMapIndex( DWORD dwIndex );
	void SetUsedAreaWeapon( bool bUsed );
	void SetUsedTrace( bool bUsed );
	void SetTraceTarget( const ioHashString &szName );
	void SetCreateWeaponIndex( DWORD dwIndex );

	void SetPointToPointInfo( ioPlayStage* pStage,
							  D3DXVECTOR3 vFirstPos, 
							  D3DXVECTOR3 vSecondPos,
							  ioHashString szFirstEffectName,
							  ioHashString szSecondEffectName );

	void SetTargetAttInfo( const D3DXVECTOR3 &vTargetPos, DWORD dwMaxCnt );

	inline int GetAreaWeaponMoveType() const { return m_AreaMoveType; }
	inline DWORD GetAreaWeaponMapIndex() const { return m_dwAreaWeaponMapIndex; }
	inline const bool GetOneTimeUse() const { return m_bOneTimeUse; }
	inline const bool GetUsedAreaWeapon() const { return m_bUsedAreaWeapon; }
	inline const bool GetUsedTrace() const { return m_bUsedTrace; }
	inline const ioHashString& GetTraceTargetName() const { return m_TraceTarget; }
	inline DWORD GetCreateWeaponIndex() const { return m_dwCreateWeaponIndex; }

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void LoadTeamBuffList( ioINILoader &rkLoader );
	void LoadEnemyBuffList( ioINILoader &rkLoader );

	bool SetOwnerBuffList( ioBaseChar *pChar );
	bool SetTeamBuffList( ioBaseChar *pOwner, ioBaseChar *pChar, int iTeamUserCnt );
	bool SetEnemyBuffList( ioBaseChar *pOwner, ioBaseChar *pChar, ioPlayStage *pStage );
	bool SetTeamBuffOnCreateWeaponList( ioBaseChar *pOwner, ioBaseChar *pChar );

	bool SetOwnerBuffList2( ioBaseChar *pChar );
	bool SetTeamBuffList2( ioBaseChar *pOwner, ioBaseChar *pChar );
	bool SetEnemyBuffList2( ioBaseChar *pOwner, ioBaseChar *pChar, ioPlayStage *pStage );

	void LoadAttributeInfo( ioINILoader &rkLoader );

	//추가 yongjun end_attack 관련한 ini값 세팅
	void LoadEndAttributeInfo( ioINILoader &rkLoader );
	//AreaWeapon이 소멸되며 end_weapon 생성
	void CreateEndAreaAttack( ioPlayStage *pStage );

	void LoadOverlapAttributeInfo( ioINILoader &rkLoader );
	//
	void CreateAreaAttack( ioPlayStage *pStage );
	void CheckAreaAttack( ioPlayStage *pStage, bool bRefresh );
	void CheckAreaTargetTeleport( ioPlayStage *pStage );

	void CheckNeedGauge( ioPlayStage *pStage );
	void CheckAreaWeaponBuff( ioPlayStage *pStage );
	void CheckAreaWeaponTarget( ioPlayStage *pStage );
	void CheckAreaWeaponBuffByDouble( ioPlayStage *pStage );
	void CheckAreaWeaponBuffOnCreateWeapon( ioPlayStage *pStage );

	bool IsPassDummy( ioDummyChar *pDummy );
	void FindAndDestroyDummy( ioPlayStage *pStage );
	bool IsCorrectWeapon( ioWeapon *pWeapon );
	void FindAndWeapon( ioPlayStage *pStage );
	void CheckDestroy( ioPlayStage *pStage );

	void CheckTargetRotation();
	void CheckTargetPosition();

	void UpdateTargetListByInOut( ioPlayStage *pStage );
	void UpdateTargetListByScale( ioPlayStage *pStage );
	void UpdateTargetListByDouble( ioPlayStage *pStage );

	void UpdateCollisionRange();
	
	void GetTargetCharListBySphere( ioPlayStage *pStage, BaseCharList &rkCharList );
	void GetTargetCharListByCylinder( ioPlayStage *pStage, BaseCharList &rkCharList );
	void GetTargetCharListByBox( ioPlayStage *pStage, BaseCharList &rkCharList );

	void RemoveTargetBuff( ioBaseChar *pTarget, TargetType eType, bool bSend );
	bool IsCollisioned( ioBaseChar *pTarget, TargetType eType );
	bool IsBlocked( ioBaseChar *pTarget );

	bool FindTarget( const ioHashString &szTarget );

	void SetAreaForce( ioBaseChar *pChar );

	bool CheckIgnoreProtectBuff();

	// Move
	void LoadChangeMoveInfoList( ioINILoader &rkLoader );

	void ProcessAreaWeaponMove( ioPlayStage *pStage );

	// Move : Change
	void CheckChangeMove( ioPlayStage *pStage );
	void CheckChangeMove( ioPlayStage *pStage, DWORD dwGapTime );
	void RotateMoveDir( float fAngleH, float fAngleV, ioPlayStage *pStage );

	// Move : Trace
	void FindTraceTarget( ioPlayStage *pStage );
	bool CheckTraceTarget( ioPlayStage *pStage );

	void SetReposition( ioPlayStage *pStage );

	// Move : Area
	bool CheckLiveAreaInAreaWeapon( ioPlayStage *pStage );
	bool CheckLiveAreaInChar( ioBaseChar *pChar, ioPlayStage *pStage );

	// Move : Owner Rot
	void CheckOwnerRot( ioBaseChar *pChar, ioPlayStage *pStage, float fTimePerSec );

	// Double
    bool CheckDoubleRange( ioBaseChar *pChar );

	void CheckVisible( ioPlayStage *pStage );

protected:
	void LoadRandomAttack( ioINILoader &rkLoader );
	void GenerateRandomAttack( ioBaseChar *pChar );
	void ProcessRandomAttack( ioBaseChar *pChar );
	void ProcessTargetAttack( ioBaseChar *pChar );

	bool CheckRefreshCollisionList( const ioHashString &szName, ioPlayStage *pStage );
	bool CheckTargetEquipObjectBuff( const ioHashString& szBuffName, ioBaseChar* pTarget );

	void LoadNextCallAttributeList( ioINILoader &rkLoader );
	void CallNextWeaponAttribute( ioBaseChar *pOwner );

protected:
	bool CheckOverlapAreaWeapon( ioPlayStage* pStage );
	POINT GetIntersection( CrossEquationInfo crossInfo1, CrossEquationInfo crossInfo2 );

	void CreateOverlapWeapon( ioPlayStage *pStage );

public:
	DWORD GetLiveTime();

public:
	ioAreaWeapon();
	ioAreaWeapon( const ioAreaWeapon &rhs );
	virtual ~ioAreaWeapon();
};

