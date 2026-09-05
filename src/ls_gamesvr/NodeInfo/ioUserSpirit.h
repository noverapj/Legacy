#pragma once
#include "../Util/IORandom.h"

class ioUserSpirit : public ioDBDataController
{
private:
	enum SpiritState
	{
		SPIRIT_STATE_NONE,
		SPIRIT_STATE_NEW,		// ½Å±Ô È¹µæ (DB INSERT)
		SPIRIT_STATE_CHANGED,	// ¼ö·® º¯°æ (DB UPDATE)
	};

	struct UserSpiritInfo
	{
		int item_code;
		int quantity;
		int state;
		UserSpiritInfo()
		{
			Init();
		}
		void Init()
		{
			item_code = 0;
			quantity = 0;
			state = SPIRIT_STATE_NONE;
		}
	};
	typedef std::vector<UserSpiritInfo> vUserSpiritInfo;

	vUserSpiritInfo m_vSpiritInfo;
	IORandom m_SpiritRandom;

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

private:
	void SendUserSpiritData();

public:
	int GetSpiritQuantity( int iItemCode );
	void IncreaseSpirit( int iItemCode, int iQuantity );
	void DecreaseSpirit( int iItemCode, int iQuantity );

public:
	void OnComposeSpirit( SP2Packet &rkPacket );
	void OnDecomposeSpirit( SP2Packet &rkPacket );
	void OnConversionSpirit( SP2Packet &rkPacket );
	void OnSellSpirit( SP2Packet &rkPacket );
	void OnPresentDecomposeSpirit( int iClassType );

private:
	int ComposeSpirit( int iSpiritCode, int iSpiritQuantity, int iSpecialSpiritCode, int iSpecialSpiritQuantity );
	int DecomposeSpirit( int iClassType, bool &bCritical );
	int ConversionSpirit( int iConsumeSpiritCode, int iConsumeSpiritQuantity, int iCreateSpiritCode, bool &bCritical  );

public:
	ioUserSpirit();
	virtual ~ioUserSpirit();
};