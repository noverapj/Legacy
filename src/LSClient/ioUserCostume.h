#pragma once

#include "CostumeEnumType.h"

class ioUserCostume
{
public:
	enum
	{
		MAX_SLOT_CNT = 500,
	};

protected:
	CostumeSlotList m_CostumeSlotList;

public:
	//동기화
	void ApplyCostumeSlotData( SP2Packet &rkPacket );

public:
	//슬롯 추가
	void AddCostumeSlotData( const CostumeSlot &sNewSlot );
	//슬롯 삭제
	bool DeleteCostumeSlotData( int nSlotIndex );
	//슬롯 변경
	void SetCostumeSlot( const CostumeSlot &sCostumeSlot );

public:
	//장착 설정
	void SetCostumeEquip( int nSlotIndex, bool bEquip );

public:
	//슬롯 얻기(슬롯 인덱스)
	bool GetCostumeSlot( int nSlotIndex, CostumeSlot &sCostumeSlot );
	//슬롯 얻기(슬롯 배열)
	bool GetCostumeSlotArray( int nArray, CostumeSlot &sCostumeSlot );
	//슬롯 개수 얻기
	int GetCostumeSlotCnt();
	//슬롯 상태 확인
	bool IsSlotFull();
	//정렬
	bool SlotSort();

	int GetCostumeCode( int nSlotIndex );
	void GetUseInfo( int nSlotIndex, ioHashString &rsInfo );

public:
	ioUserCostume();
	virtual ~ioUserCostume();
};

