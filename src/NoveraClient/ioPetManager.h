#pragma once

class ioINILoader;
class ioPlayEntity;
class ioBaseChar;
class ioPlayMode;
class ioPlayStage;
class ioPet;
class SP2Packet;

class ioPetManager : public Singleton< ioPetManager >
{
protected:
	typedef std::vector< ioPet* > PetList;
	PetList m_PetList;
	PetBaseInfoList m_vPetBaseInfoList;

	ioPlayStage *m_pPlayStage;
	CurPetInfoList m_CurPetInfoList;
	
public:
	//기본 데이터 읽기
	void LoadPetBaseInfo();

protected:
	//모델 정보 읽기 (랭크별)
	void LoadModelInfo( ioINILoader &rkLoader, PetModelInfoList &vPetModelInfoList );
	//데이터 삭제
	void ClearPetBaseInfo();

public:
	//펫 생성
	ioPet* CreatePetEntity( ioHashString szModelName, ioPlayMode *pMode, PetTypeInfo sPetType );

protected:
	//메인 타입에 따른 서브 타입 펫 생성
	ioPet* CreateDefaultPet( ioEntityGroup *pGrp, ioPlayMode *pMode, int nSubType );
	
public:
	//현재 모든 펫 비우기
	void DestroyPetList();
	//엔드 상태 확인 후 제거
	void ClearDeadPet();
	
public:
	//기본 펫 정보 얻기
	bool GetPetBaseInfo( int nPetCode, PetBaseInfo &rkInfo );
	//해당 캐릭터의 펫 찾기
	ioPet* FindPet( ioBaseChar *pOwner );
	//해당 캐릭터의 펫 엔드 상태로 전환
	void SetPetDieState( ioBaseChar *pOwner );
				
public:
	//펫 모델 정보 얻기 (이름과 랭크에 따라)
	bool GetPetModelInfo( int nPetCode, PetRankType eCurPetRankType, PetModelInfo &sPetModelInfo );
	
public:
	//펫 생성
	ioPet* CreatePet( const ioHashString &szOwnerName, const D3DXVECTOR3 &vTargetPos );

protected:
	void AddCurPetInfo( ioHashString szCharName, CurPetInfo sCurPetInfo, bool bOnlyChange );

public:
	bool GetCurPetInfo( ioHashString szCharName, CurPetInfo &sCurPetInfo );

public:
	//펫 동기화
	void OnPetSync( SP2Packet &rkPacket );
	//펫 변경(슬롯만 변경)
	void OnPetChangeData( SP2Packet &rkPacket );
	//펫 장착(전투방)
	void OnPetEquipData( SP2Packet &rkPacket );
	//펫 변경(슬롯 제거)
	void OnPetRemoveData( SP2Packet &rkPacket );
	//펫 추가
	void OnPetAdd( SP2Packet &rkPacket );
	//펫 경험치 증가
	void OnPetNurture( SP2Packet &rkPacket );
	//펫 레벨 증가(전투방)
	void OnPetNurtureLevelUp( SP2Packet &rkPacket );
	//펫 합성
	void OnPetCompound( SP2Packet &rkPacket );

public:
	static ioPetManager& GetSingleton();

public:
	ioPetManager( ioPlayStage *pStage );
	virtual ~ioPetManager();
};

#define g_PetMgr ioPetManager::GetSingleton()