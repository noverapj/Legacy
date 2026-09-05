#pragma once

#include "Accessory.h"

class User;
class ioCharacter;

class ioUserAccessory
{
public:
	ioUserAccessory();
	~ioUserAccessory();

	void Init(User* pUser);
	void Destroy();

public:
	void DBtoData( CQueryResultData *query_data, int& iLastIndex );	
	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	void SetEquipInfo(int& iClassType, int iAccessoryCode, int iAccessoryIndex, int iAccessoryValue, int iComposeCode, int iComposeValue);

	void AddAccessoryItem(Accessory& AccessoryInfo);
	bool DeleteAccessoryItem(DWORD& dwIndex);

	void EquipAccessory(const int iClassType, Accessory* pAccessory);

	void ReleaseAccessoryWithAccessoryIndex(DWORD dwIndex);
	void ReleaseAccessoryWithClassType(int iClassType);
	void ReleaseAccessory(Accessory* pAccessory);

	void GetAccessoryPassedDate(IntVec &vDeleteIndex, IntVec &vDeleteCode);

	int ChangeEquipInfo(int iClassArray, DWORD& dwTargetIndex, int iEquipPos, bool bEquip, Accessory* pAccessory);

	void OnCompose( int iAccessoryIndex, int iMaterialIndex1, int iMaterialIndex2, int iMaterialIndex3 );
	int OnReinforce( int iAccessoryIndex, int iMaterialIndex );

	bool IsEnableAdd();
	bool IsEmpty();
	bool IsCompose( int iAccessoryIndex );

	Accessory* GetAccessory(DWORD dwIndex);
	int GetAccessoryEquipPos(int iCode);

	void SendAllAccessoryInfo();

protected:
	typedef boost::unordered_map<DWORD, Accessory> UserAccessoryItem;	//<인덱스, 해당아이템>

protected:
	User* m_pUser;
	UserAccessoryItem m_mUserAccessoryMap;
};