#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDummyAttachBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_CreateAreaWeapon,
		BS_CreateDummy,
		BS_CreateDummyWeapon,
		BS_PlayHornSound,
		BS_PlayAttackSound,
	};

	//더미 정보
	struct DummyInfo 
	{
		//인덱스
		int nIndex;
		//위치
		D3DXVECTOR3 vPos;
	};

	typedef std::vector<DummyInfo> vDummyInfoList;

protected:	
	//더미 데이터
	vDummyCharLoadInfoList m_DummyCharList;
	//삭제 더미 이름
	ioHashStringVec m_vDestroyDummyCharList;
	//더미 정보
	vDummyInfoList m_DummyInfoList;
	//활성 시간
	DWORD m_dwActiveDuration;
	//확인 시간
	DWORD m_dwCheckDuration;
	//활성 시간 사용 여부
	bool m_bSetActiveDuration;
	//무기 사용시 게이지 소모량
	float m_fAreaWeaponUseGauge;
	//무기 재사용 시간
	DWORD m_dwAreaWeaponTick;
	//무기 사용 시간 확인
	DWORD m_dwCheckAreaWeapon;
	//생성할 무기 이름
	ioHashString m_szAreaWeaponName;	
	//더미가 따라 다닐 것인가
	bool m_bSetDummyAttach;
	//게이지 감소 시작 설정
	bool m_bStartGauge;
	//Area 무기 사용
	bool m_bSetAreaWeapon;
	//더미 무기 사용
	bool m_bSetDummyWeapon;
	//무기 사용시 게이지 소모량
	float m_fDummyWeaponUseGauge;
	//무기 재사용 시간
	DWORD m_dwDummyWeaponTick;
	//무기 사용 시간 확인
	DWORD m_dwCheckDummyWeapon;
	//사운드 이름
	ioHashString m_szHornSoundName;
	//공격사운드 이름
	ioHashString m_szAttackSoundName;
	//사운드 재생 시간
	DWORD m_dwDelayHornSoundTime;
	//사운드 확인 시간
	DWORD m_dwCheckHornSoundTime;
	//사운드 재생 시간
	DWORD m_dwDelayAttackSoundTime;
	//사운드 확인 시간
	DWORD m_dwCheckAttackSoundTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	
public:
	void ApplyExtraBuffInfo( SP2Packet &rkPacket );
	
protected:
	//키입력
	void ProcessKeyInput();
	//무기 생성
	void CreateAreaWeapon();
	//소리 재생
	void PlayHornSound();
	void PlayAttackSound();
	//스킬게이지 설정
	void SetSkillGauge( float fSkillGauge );
	//더미 캐릭터 생성
	void CreateDummyChar();
	//더미 확인
	bool CheckDummy();
	//더미 연결
	void ProcessDummyAttach();
	//더미 정보 넣기
	void AddDummyInfo(int nIndex);
	//더미의 무기 생성
	void CreateDummyWeapon();

public:
	ioDummyAttachBuff();
	ioDummyAttachBuff( const ioDummyAttachBuff &rhs );
	virtual ~ioDummyAttachBuff();
};

inline ioDummyAttachBuff* ToDummyAttachBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DUMMY_ATTACH )
		return NULL;

	return dynamic_cast< ioDummyAttachBuff* >( pBuff );
}