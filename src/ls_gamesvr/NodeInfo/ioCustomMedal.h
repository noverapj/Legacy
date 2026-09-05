#pragma once

class user;
class CustomMedal;
class ioCustomMedal
{
public:
	enum
	{
		//MEDAL_STAT	 = 8,
		MAX_SLOT     = 10,
	};

	enum
	{
		PT_TIME		= 0,
		PT_MORTMAIN	= 1,
	};



	IORandom m_iPointRandom;

protected:
	typedef std::vector< CustomMedal* > CustomMedalItemList;
	CustomMedalItemList m_vCustomMedalItemList;
	typedef CustomMedalItemList::iterator CustomMedalIter;

protected:
	CustomMedal* CreateCustomMedal();

public:
	void Initialize( User* pUser );
	void Destroy();
	void DBtoData( CQueryResultData *query_data );	
	void SaveData();
	void FillMoveData( SP2Packet &rkPacket );

	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );
	CustomMedal* SetCustomMedal( int iPresentIndex, int iSlotIndex, int iItemCode, int iItemValue, IN SP2Packet &rkRecvPacket );
	CustomMedal* GetCustomMedal( int iIndex, int iItemCode );
	CustomMedal* GetCustomMedalPresentIndex( int iIndex, int iSlotIndex, int iItemCode );
	void GetCustomMedalEquipClass( int iClass, IntOfStatVec &kCustomMedal );
	void FillEquipCustomMedal( int iClass, int iMaxSlot, OUT SP2Packet &rkPacket );
	void FillEquipAllCustomMedal( int iClass, OUT SP2Packet &rkPacket );
	int GetEquipCustomMedalNum( int iClass );

	int  GetRandomPoint( int iItemType, CustomMedal* pInfo  );						// 커스텀 메달 스탯 포인트 계산
	void SendCustomMedalData( CustomMedal* pInfo, SP2Packet& rkPacket );			// 커스텀 메달 스탯 정보 전송
	void DeleteCustomMedal( int iIndex, int iCode );								// 커스텀 메달 삭제

	int	 ReleaseEquipcustomMedal( int iClassType );

	void SendPresentCustomMedalToClient(CQueryResultData *query_data);				// 선물함에서 커스텀 메달 획득한 경우

	void OnSellCustomMedal( SP2Packet &rkPacket );
	CustomMedal* OnChangeCustomMedalState( SP2Packet &rkPacket, int& iRoomClass, bool& bState );


	bool IsCustomMedal( int iItemType );
public:

protected:
	User* m_pUser;

public:
	ioCustomMedal(void);
	virtual ~ioCustomMedal(void);
};

enum CUSTOM_MEDAL_TYPE
{
	NORMAL_TYPE = 1,
	CUSTOM_TYPE = 2,
};
