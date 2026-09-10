#pragma once

#include "ioCountSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCountLightSkill : public ioCountSkill
{
protected:
	enum SkillState
	{
		SS_None,
		SS_Action,
		SS_Loop,
		SS_Teleport,
		SS_Effect,
		SS_End,
	};

	enum UseEnable 
	{
		UE_DOWN,
		UE_JUMP,
		UE_JUMP_BLOW,
		UE_WOUND,
		UE_BLOW,
		UE_MAX,
	};

	struct EnableInfo
	{
		bool bUseEnable[UE_MAX];
		int nMinGauge;
		int nMaxGuage;
		ioHashString szEffect;
		float fDuration;
		float fRange;
		float fSpeed;
	};

	typedef std::vector< EnableInfo > InfoList;

protected:
	//정보 리스트
	InfoList m_InfoList;
	
	//마법진 설정
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	//이펙트 설정
	ioHashString m_ReadyCircle;

	//루프 애니 설정
	ioHashString m_szLoopAni;

	//유지시간
	DWORD m_dwLoopTime;

	//드랍존 오프셋
	float m_DropZoneOffset;

	//버프 리스트
	ioHashStringVec m_vCreateBuffList;
	ioHashStringVec m_vRemoveBuffList;

	//보호 시간
	DWORD m_dwSkillProtectTime;

protected:
	//스킬 상태
	SkillState m_SkillState;

	//마법진 위치
	D3DXVECTOR3 m_vCirclePos;
	
	//유지시간 확인
	DWORD m_dwCheckLoopTime;

	//드랍존 확인
	bool m_bIsDropZone;

	//보호시간 확인
	DWORD m_dwCurSkillProtectTime;

	UniqueObjID m_dwEffectID;
	int m_nGrade;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyEquipSlotSkillExtraInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	virtual void RecoveryCount( ioBaseChar *pChar );
	virtual void OnSkillItemRelease( ioBaseChar *pChar );

public:
	//스킬 사용 상태 얻기
	virtual bool IsCanDownSkill() const;
	virtual bool IsCanJumpingSkill() const;
	virtual bool IsCanJumpingSkillOnBlowWound() const;
	virtual bool IsCanWoundSkill() const;
	virtual bool IsCanBlowWoundSkill() const;
	virtual int  GetCurUseCount();
	virtual int  GetMaxUseCount();
	virtual int  GetNeedCount();

protected:
	void LoadEnableInfo( ioINILoader &rkLoader );
	void LoadBuffInfo( ioINILoader &rkLoader );
	bool FindUseEnable( UseEnable eType ) const;
	void SetSkillInfo( int nGrade );

protected:
	//상태 변경
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetTeleportState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

protected:
	//마법진 생성
	void CreateMagicCircle( ioBaseChar *pChar );
	//마법진 업데이트
	void UpdateMagicCircle( ioBaseChar *pChar );
	//마법진 위치얻기
	void GetMagicCirclePos( ioBaseChar *pChar );
	//마법진 방향으로 회전
	void CheckRotate( ioBaseChar *pChar );
	//키입력 확인
	void CheckInputKey( ioBaseChar *pChar );

protected:
	//버프 생성
	void CreateBuff( ioBaseChar *pChar );
	//버프 제거
	void RemoveBuff( ioBaseChar *pChar );
	//이펙트 설정
	void SetEffect( ioBaseChar *pOwner, ioHashString szEffectName );

public:
	ioCountLightSkill();
	ioCountLightSkill( const ioCountLightSkill &rhs );
	virtual ~ioCountLightSkill();
};

inline ioCountLightSkill* ToCountLightSkill( ioSkill *pSkill )
{
	ioCountSkill *pRange = ToCountSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != CST_LIGHT )
		return NULL;

	return dynamic_cast< ioCountLightSkill* >(pRange);
}