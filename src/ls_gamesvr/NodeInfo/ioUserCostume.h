#pragma once

#include "Costume.h"

class User;
class ioCharacter;

class ioUserCostume
{
public:
	ioUserCostume();
	~ioUserCostume();

	void Init(User* pUser);
	void Destroy();

public:
	void DBtoData( CQueryResultData *query_data, int& iLastIndex );	
	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	void SetEquipInfo(int& iClassType, int iCostumeCode, int iCostumeIndex);

	void AddCostumeItem(Costume& costumeInfo);
	bool DeleteCostumeItem(DWORD& dwIndex);

	void EquipCostume(const int iClassType, Costume* pCostume);

	void ReleaseCostumeWithCostumeIndex(DWORD dwIndex);
	void ReleaseCostumeWithClassType(int iClassType);
	void ReleaseCostume(Costume* pCostume);

	void DeleteCostumePassedDate(IntVec &vDeleteIndex);

	int ChangeEquipInfo(int iClassArray, DWORD& dwTargetIndex, int iEquipPos, bool bEquip, Costume* pCostume);
	//bool ChangeCostume(ioCharacter* pCharacter, DWORD dwClassIndex, int iEquipPos, DWORD dwTargetIndex, DWORD dwCostumeCode, bool bEquip);

	bool IsEnableAdd();
	bool IsEmpty();

	Costume* GetCostume(DWORD dwIndex);
	int GetCostumeEquipPos(int iCode);

	void SendAllCostumeInfo();

protected:
	typedef boost::unordered_map<DWORD, Costume> UserCostumeItem;	//<인덱스, 해당아이템>

protected:
	User* m_pUser;
	UserCostumeItem m_mUserCostumeMap;
	//ioEquipCostumeInfo m_EquipCostumeInfo;
};