#pragma once

#include "PetEnumType.h"

class ioUserPet
{
public:
	enum
	{
		MAX_SLOT_CNT = 20,
	};

protected:
	PetSlotList m_PetSlotList;

public:
	//동기화
	void ApplyPetSlotData( SP2Packet &rkPacket );

public:
	//슬롯 추가
	void AddPetSlotData( const PetSlot &sNewSlot );
	//슬롯 삭제
	bool DeletePetSlotData( int nSlotIndex );
	//슬롯 변경
	void SetPetSlot( const PetSlot &sPetSlot );

public:
	//장착 설정
	void SetPetEquip( int nSlotIndex, bool bEquip );
	//장착 해제
	void ClearPetEquip();

public:
	//슬롯 얻기(슬롯 인덱스)
	bool GetPetSlot( int nSlotIndex, PetSlot &sPetSlot );
	//슬롯 얻기(슬롯 배열)
	bool GetPetSlotArray( int nArray, PetSlot &sPetSlot );
	//장착 중인 슬롯 얻기
	bool GetPetSlotEquip( PetSlot &sPetSlot );
	//슬롯 개수 얻기
	int GetPetSlotCnt();
	//슬롯 상태 확인
	bool IsSlotFull();
	//정렬
	bool SlotSort();

public:
	ioUserPet();
	virtual ~ioUserPet();
};

