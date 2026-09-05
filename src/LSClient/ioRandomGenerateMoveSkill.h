#pragma once
#include "ioAttackSkill.h"

class ioRandomGenerateMoveSkill :
	public ioAttackSkill
{
private:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_FIRE,
		SS_END,
	};
protected:
	SkillState m_SkillState;

protected:
	//Loop
	ioHashString m_SkillLoopMotion;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;
	float m_fSkillLoopMotionRate;

	//Magic Circle
	ioHashString m_ReadyCircle;
	
	float m_fMagicCircleOffSet;
	float m_fCurCircleOffset;

	float m_fMagicCircleMoveSpeed;
	float m_fMagicCircleHeightGap;
	float m_fMagicCircleRange;
	D3DXVECTOR3 m_vCirclePos;

	//웨폰 소환되는 마법진 반지름 값
	float m_fCircleRadius;
	float m_fCurCircleRadius;

	//무기 생성시작시간
	DWORD m_dwGenerateStartTime;
	//다음무기 생성 시작 시간
	DWORD m_dwNextGenerateTime;
	//무기 info list
	GenerateList m_vGenerateList;

	float m_fGenerateStartHeight;
	DWORD m_dwMinGenerateTime;
	DWORD m_dwMaxGenerateTime;

	//Effect
	ioHashString m_RedCircleEffect;
	ioHashString m_BlueCircleEffect;
	UniqueObjID m_dwCurMapEffect;

	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;
private:
	void ClearData();
protected:
	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar );
	void SetDeadState( ioBaseChar *pChar );

	void FireWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
protected:
	//Circle
	void CreateMagicCircle( ioBaseChar *pChar );
	bool UpdateMagicCircle( ioBaseChar *pChar );
	void CheckRotate( ioBaseChar *pChar );

	//Effect
	void CreateEffect( ioBaseChar *pChar );
	void UpdateEffect( ioBaseChar *pChar, ioPlayStage *pStage );
protected:
	//Weapn
	void GenerateWeapon( ioBaseChar *pChar );
	void OnGenerateList( ioBaseChar *pChar, SP2Packet &rkPacket );
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	ioRandomGenerateMoveSkill(void);
	ioRandomGenerateMoveSkill( const ioRandomGenerateMoveSkill &rhs );
	virtual ~ioRandomGenerateMoveSkill(void);
};

inline ioRandomGenerateMoveSkill* ToRandomGenerateMoveSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RANDOM_GENERATE_MOVE )
		return NULL;

	return dynamic_cast< ioRandomGenerateMoveSkill* >(pAttack);
}
