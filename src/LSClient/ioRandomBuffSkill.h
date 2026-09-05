#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

enum BuffTargetType
{
	BUFFTT_NONE,
	BUFFTT_OWNER,
	BUFFTT_TEAM,
	BUFFTT_ENEMY_TEAM,
	BUFFTT_ENEMY_ONE,
};

typedef struct tagSectionInfoStruct
{
	int m_iMin, m_iMax;
	BuffTargetType m_iBuffTargetType;
	float m_fSkillRange;
	BuffInfoList m_BuffList;

	tagSectionInfoStruct()
	{
		Init();
	}

	void Init()
	{
		m_iMin = 0;
		m_iMax = 0;
		m_iBuffTargetType = BUFFTT_NONE;
		m_fSkillRange = 0;
		m_BuffList.clear();
	}
} SectionInfo;

typedef std::vector< SectionInfo > SectionInfoList;

class ioRandomBuffSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_END,
	};
	
	enum SkillSyncState
	{
		SSS_CREATE_RANDOM,
	};

protected:
	SkillState m_SkillState;

	CEncrypt<int> m_iSectionCnt;
	CEncrypt<int> m_iRandomLimitCnt;

	SectionInfoList m_SectionInfoList;
	SectionInfo m_SectionInfo;

	BaseCharList m_BuffCharList;

	int m_iValue;
	
	ioHashStringVec m_MapEffectList;

	float m_fEffectYGap;
	float m_fEffectXGap;
	DWORD m_dwEndTime;
	UniqueObjID m_MapEffectID[2];

	DWORD m_dwAddBuffTime;

	TargetWoundType m_TargetWoundType;

	ioHashString m_EndAnimation;
	float m_fEndAniRate;
	DWORD m_dwEndAniStartTime;
	DWORD m_dwEndAniEndTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	void ClearData();

protected:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetRandomBuff( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSend = false );
	void SetCharBuffList( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSend = false );
	void AddTargetBuff( ioBaseChar *pOwner, ioBaseChar *pChar );

	void CreateMapEffect( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyMapEffect( ioBaseChar *pChar );
public:
	ioRandomBuffSkill(void);
	ioRandomBuffSkill( const ioRandomBuffSkill &rhs );
	virtual ~ioRandomBuffSkill();
};

inline ioRandomBuffSkill* ToRandomBuffSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_RANDOM_BUFF )
		return NULL;

	return dynamic_cast< ioRandomBuffSkill* >( pSkill );
}

