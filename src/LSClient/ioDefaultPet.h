#pragma once

class ioBaseChar;
class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDefaultPet : public ioPet
{
protected:
	enum PetState
	{
		DPS_None,
		DPS_Special_Loop,
		DPS_Owner_Attach,
	};

protected:
	//펫의 상태
	PetState m_PetState;
	
	//위치 설정
	D3DXVECTOR3 m_vAttachOffset;

public:
	//데이터 읽기
	virtual void LoadProperty( ioINILoader &rkLoader );
	//동기화 처리
	virtual void ApplyPetSync( ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	//펫 상태 설정 (캐릭터의 스테이트를 통해)
	virtual void SetPetState( int eCharState ); 
	//시작 상태
	virtual bool OnStartState();
	//엔드 상태
	virtual void OnDeadState();

protected:
	//루프
	virtual void OnProcessState();
	
protected:
	//엔드 확인
	void CheckDie();
	//하이드 확인
	void CheckVisible();
	
protected:
	//이동 설정
	void OwnerAttachMove();

public:
	ioDefaultPet( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDefaultPet();
};

inline ioDefaultPet* ToDefaultPet( ioPet *pPet )
{
	if( !pPet || pPet->GetTypeID() != DPST_DEFAULT )
		return NULL;

	return dynamic_cast< ioDefaultPet* >( pPet );
}