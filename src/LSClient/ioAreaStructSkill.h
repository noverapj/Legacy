#pragma once


#include "ioNormalSkill.h"
#include "WeaponAttribute.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;

class ioAreaStructSkill : public ioNormalSkill
{
public:
	enum SyncState
	{
		SSC_AREA,
		SSC_DUMMY,
		SSC_AREA_WEAPONS,
	};

	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_ACTION,		
	};
protected:
	typedef struct _tagAreaWeapnStyle
	{
		ioHashString szAreaWeaponName;
		D3DXVECTOR3  vOffset;
		D3DXVECTOR3  vRotAngle;
		bool		 bChangeHeight;
	}AreaWeaponStyle;

	typedef std::vector<AreaWeaponStyle> AreaWeapnStyleList;
// Area
protected:
	ioHashString m_AreaWeaponName;

	AreaWeapnStyleList m_AreaWeapnStyleList;
	std::vector<DWORD> m_AreaWeaponIndexList;

	DWORD m_dwAreaWeaponIndex;

	float m_fOffSet;
	float m_fYOffSet;
	float m_fOffSet2;
	
	ioHashString m_RedCircle;
	ioHashString m_BlueCircle;
	UniqueObjID m_dwCurMapEffect;
	
	DWORD m_dwCircleDuration;
	DWORD m_dwCircleEnd;
	D3DXVECTOR3 m_vCreatePos;

	DWORD m_dwCreateTime;

	SkillState m_State;

	//for mutant accessory
	DWORD m_dwJumpReserveTime;
	DWORD m_dwCurJumpReserveTime;
	DWORD m_dwSkillJumpPower;

// Struct
protected:
	int m_iBildStruct;

	ioHashString m_BildEffect;

	int m_iMaxStructCnt;

// dummychar
	ioHashString m_DummyCharEffect;
	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;

// etc
protected:
	ForceInfoList m_vForceInfoList;

//
protected:
	DWORD m_dwAttackEndTime;

	bool m_bEnableCheckHeight;
	float m_fUpLimitHeight;
	float m_fDownLimitHeight;

	bool m_bUseItemCircle;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:

	void SetPreActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetActionState( ioBaseChar *pChar );
	void CheckActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	
	void CreateArea( ioBaseChar *pChar, ioPlayStage *pStage );
	void CreateAreaWeapons( ioBaseChar *pChar, ioPlayStage *pStage );
	void CreateStruct( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioAreaStructSkill();
	ioAreaStructSkill( const ioAreaStructSkill &rhs );
	virtual ~ioAreaStructSkill();
};

inline ioAreaStructSkill* ToAreaStructSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_AREA_STRUCT )
		return NULL;

	return dynamic_cast< ioAreaStructSkill* >( pSkill );
}

