#pragma once

#include "NamedTitleEnumType.h"

class ioUserNamedTitle
{
protected:
	NamedTitleSlotList m_NamedTitleSlotList;

public:
	//동기화
	void ApplyNamedTitleSlotData( SP2Packet &rkPacket );

public:
	//슬롯 추가
	bool AddNamedTitleSlotData( const NamedTitleSlot &sNewSlot );
	//슬롯 삭제
	bool DeleteNamedTitleSlotData( DWORD dwCode );
	//슬롯 변경
	bool SetNamedTitleSlot( const NamedTitleSlot &sNamedTitleSlot );

public:
	//장착 설정
	void SetNamedTitleEquip( DWORD dwCode );

	//등급 설정
	void SetNamedTitleGrade( DWORD dwCode, int nLevel );
	
	//프리미엄 설정
	void SetNamedTitlePremium( DWORD dwCode );

public:
	void ClearNamedTitleEquip();
	void ClearNamedTitleNew();

public:
	//장착 중인 슬롯 얻기
	bool GetNamedTitleSlotEquip( NamedTitleSlot &sNamedTitleSlot );
	//슬롯 얻기(슬롯 코드)
	bool GetNamedTitleSlot( DWORD dwCode, NamedTitleSlot &sNamedTitleSlot );
	//슬롯 얻기(슬롯 배열)
	bool GetNamedTitleSlotArray( int nArray, NamedTitleSlot &sNamedTitleSlot );

public:
	//슬롯 개수 얻기
	int GetNamedTitleSlotCnt();
	//새로운 칭호
	bool IsNewNamedTitle();
	//프리이엄 가능 칭호
	bool IsEnablePremiumNamedTitle();

public:
	ioUserNamedTitle();
	virtual ~ioUserNamedTitle();
};

