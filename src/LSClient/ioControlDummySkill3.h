#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioPlayStage;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioControlDummySkill3 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_ACTION,
		SS_LOOP,
		SS_END,
	};

	enum ReduceSkillGuageKey
	{
		RK_NONE,
		RK_ATTACK_KEY,
		RK_DEFENSE_KEY,
		RK_JUMP_KEY,
	};

	enum SkillSyncState
	{
		SSS_NONE,
		SSS_LOOP,
		SSS_CREATE_CONTROL_DUMMY,
		SSS_CREATE_SUB_DUMMY,
		SSS_DIR,
		SSS_END,
	};

	struct SubDummyInfo
	{
		ioHashString	DummyName;
		D3DXVECTOR3		vOffset;

		SubDummyInfo()
		{
			vOffset = ioMath::VEC3_ZERO;
		}
	};
	typedef std::vector<SubDummyInfo> SubDummyInfoList;

protected:
	SkillState					m_SkillState;

	IntVec						m_DummyIndexList;
	int							m_iDummyIndex;
	IntVec						m_vSubDmmyCharVec;

	D3DXVECTOR3					m_vCreatePos;

	// 스킬 게이지 감소 관련
	CEncrypt<bool>				m_bDisableGrowth;
	bool						m_bReduceSkillGuage;
	CEncrypt<float>				m_fGuagePerTic;
	CEncrypt<DWORD>				m_dwTicTime;
	DWORD						m_dwCheckTime;
	CEncrypt<float>				m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD>				m_dwTicTimeByGrowth;

	CEncrypt<DWORD>				m_dwFireStartTime;
	CEncrypt<DWORD>				m_dwSkillProtectTime;

	ioHashString				m_DummyCharName;
	D3DXVECTOR3					m_vDummyCharOffset;

	ioHashStringVec				m_vDestroyDummyCharList;

	BuffInfoList				m_OwnerBuffList;

protected:
	SubDummyInfoList			m_SubDummyCharInfoList;
	ioUserKeyInput::DirKeyInput	m_DirKeyInput;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	void LoadBuffList( ioINILoader &rkLoader );
	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();

public:
	bool IsDummyCharControl( ioBaseChar *pChar );
	bool IsDummyCharEnable( ioBaseChar *pChar );
	D3DXVECTOR3 GetDummyCharControlPos( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar* pChar, ioPlayStage *pStage );

public:
	void SetSkillEndState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void CreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage, int iCurIndex );
	void CreateSubDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessLoopRotate( ioBaseChar *pChar );
	void ProcessLoopPos( ioBaseChar *pChar );

public:
	void StartReduceGuage();

public:
	virtual bool IsNoDropState() const;

public:
	ioControlDummySkill3();
	ioControlDummySkill3( const ioControlDummySkill3 &rhs );
	virtual ~ioControlDummySkill3();
};

inline ioControlDummySkill3* ToControlDummySkill3( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONTROL_DUMMY3 )
		return NULL;

	return dynamic_cast< ioControlDummySkill3* >( pAttack );
}

