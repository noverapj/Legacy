//////////////////////////////////////////////////////////////////////////
//2012.09.04
//작성자:윤태철
//타겟:제갈량 SD스킬
//설명:원하는 포인트에 타겟을 설정후 2개 이후부터 점에서 점으로 Weapon이 직선으로 지나간다.
//점은 n개로 설정한다.
//참고Class:ioMultiArea2Skill,ioTwoMagicCircleSkill

#ifndef _ioMultiMagicCircleSkill_h_
#define _ioMultiMagicCircleSkill_h_

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioMultiMagicCircleSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum CircleState
	{
		FIRST_CIRCLE,
		ADD_CIRCLE,
		CIRCLE_COMPLETE,
	};

	/*enum SkillSync
	{
		NSS_CIRCLE_CHANGE,
		NSS_CREATE,
	};*/

	enum SkillSyncState
	{
		SSS_KEY,
		SSS_ACTION,
		SSS_CREAT_CIRCLE_TRACE_DUMMY,
	};

	enum CreateWeaponStyle
	{
		CWS_NORMAL,
		CWS_MISSILE5,
		CWS_DUMMY_CREATE,
		CWS_CIRCLE_TRACE_DUMMY,
	};

protected:
	CEncrypt<int> m_State;
	Vector3Vec		m_vWeaponPosList;
	IntVec			m_vWeaponOffset;

	// weapon
	CEncrypt<int> m_iMaxWeaponCnt;
	CEncrypt<int> m_iCurWeaponCnt;

	ioHashString	m_szEnableCircle;
	DWORDVec		m_dwEnableCircleIDList;//바닥 사이클 이펙트 uniqueID

	CEncrypt<DWORD>	m_dwEnableKeyTime;
	DWORD			m_dwGapTime;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	// magic circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;

	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;

	ioHashString m_OnCircle;
	ioHashString m_OffCircle;

	//
	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString	m_LoopAnimation;
	CEncrypt<DWORD>	m_dwLoopProtectTime;
	DWORD			m_dwLoopStartTime;

	//
	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	//////////////////////////////////////////////////////////////////////////
	//추가 맴버
	bool m_bSafeCirclePos;//마법진을 설치 할 수 있는 안전한 상태
	ioHashString m_szStampCircleEffect;
	DWORD m_dwAttackAttribute;

	CreateWeaponStyle m_CreateWeaponStyle;

	//////////////////////////////////////////////////////////////////////////
	ioHashString m_stDummyName;
	ioHashString m_CurCircleEffect;
	bool		 m_bOffCircle;
	bool		 m_bEnableSummon;
	bool	     m_bUseEnableMagicCircle;

	bool		 m_bUseDecreaseActionGauge;
	float		 m_fReduceSkillGauge;

	bool		 m_bUseDestroyDummy;

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
	void CreateWeapon( ioBaseChar *pChar );
	void CreateMissle5Weapon( ioBaseChar *pChar );
	void CreateDummy( ioBaseChar *pChar );
	void CreateCircleTraceDummy( int iDummyIndex, ioBaseChar *pChar );

	void SetMagicCircle( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );
	void AddPosition( ioBaseChar *pChar, ioPlayStage *pStage );

	void ChangeCircleDropZoneAndPositionGap( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioMultiMagicCircleSkill();
	ioMultiMagicCircleSkill( const ioMultiMagicCircleSkill &rhs );
	virtual ~ioMultiMagicCircleSkill();
};


inline ioMultiMagicCircleSkill* ToMultiMagicCircleSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_MULTI_CIRCLE )
		return NULL;

	return dynamic_cast< ioMultiMagicCircleSkill* >( pSkill );
}


#endif
