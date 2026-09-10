#ifndef _ioTwoMagicCircleSkill_h_
#define _ioTwoMagicCircleSkill_h_

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTwoMagicCircleSkill : public ioNormalSkill
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
		SECOND_CIRCLE,
		CIRCLE_COMPLETE,
	};

	enum SkillSync
	{
		NSS_CIRCLE_CHANGE,
		NSS_CREATE,
	};

protected:
	CEncrypt<int> m_State;
	CircleState m_CircleState;

	DWORD m_dwProtectTime;

	ioHashString m_SkillLoopMotion;
	float m_fSkillLoopMotionRate;

	ioHashString m_SkillMotion;
	float m_fSkillMototionRate;
	DWORD m_SkillMotionEndTime;

	ioHashString m_SkillEndMotion;
	float m_fSkillEndMototionRate;

	DWORD m_dwLoopDuration;
	DWORD m_dwLoopDurationStart;

	DWORD m_dwEndMotionEndTime;

	// First Circle info
	float m_fFirstCircleRange;
	float m_fFirstCircleMoveSpeed;
	float m_fFirstCircleHeightGap;
	float m_fFirstCircleOffSet;
	float m_fFirstMapHeight;
	
	ioHashString m_CurCircleEffect;

	bool m_bFirstOffCircle;
	bool m_bSecondOffCircle;
	bool m_bEnableSecnodCircle;

	// Second Circle info
	float m_fSecondCircleMaxRange;
	float m_fSecondCircleMinRange;
	float m_fSecondCircleMoveSpeed;
	float m_fSecondCircleHeightGap;
	float m_fSecondCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;
	D3DXVECTOR3 m_vFirstCirclePos;
	D3DXVECTOR3 m_vSecondCirclePos;

	float m_fCircleKeyOnHeightMin;
	float m_fCircleKeyOnHeightMax;

	float m_fCircleToCircleGap;

	ioHashString m_OnCircle;
	ioHashString m_OffCircle;
	ioHashString m_CircleEffect;

	ioHashString m_szStampCircleEffect;
	UniqueObjID m_dwFirstStampCircleEffect;
	UniqueObjID m_dwSecondStampCircleEffect;

	UniqueObjID m_dwCurMapEffect;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool m_bDropzone;
    
	ioHashString m_AreaWeaponName;

	DWORD m_dwCheckCallCircleCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );


protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );

protected:
	void ClearData();

	void SetActionState( ioBaseChar *pChar, ioPlayStage* pStage, CircleState cState );
	void SetLoopState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar, ioPlayStage* pStage );
		
	void CheckCircle(ioBaseChar *pChar, ioPlayStage *pStage );
	void ChangeCircleDropZoneAndHeightGap( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetCircleEffect(  ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vPos );
	
	void CheckCharMoveDir( ioBaseChar *pChar );
	void CreateAreaWeapon( ioBaseChar *pChar, ioPlayStage* pStage );

public:
	ioTwoMagicCircleSkill();
	ioTwoMagicCircleSkill( const ioTwoMagicCircleSkill &rhs );
	virtual ~ioTwoMagicCircleSkill();
};

inline ioTwoMagicCircleSkill* ToTwoMagicCircleSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TWO_CIRCLE )
		return NULL;

	return dynamic_cast< ioTwoMagicCircleSkill* >( pSkill );
}


#endif
