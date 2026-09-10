#pragma once
class ioPlayStage;

class ioAccessoryInfoManager : public Singleton< ioAccessoryInfoManager >
{
private:
	float m_fMortmainItemSellPrice;
	float m_fTimeItemSellPrice;

	ioHashString m_szCompoundErrMsg[3];
	ioHashString m_szReinforceErrMsg;
	ioHashString m_szDeleteMsg;
	ioHashString m_szAccessoryChangeErrMsg[2];
	AccessoryCompoundInfoMap m_CompoundInfo;

public:
	void OnAccessoryBuyResult( SP2Packet &rkPacket );
	void OnAccessorySellResult( SP2Packet &rkPacket );
	void OnAccessoryPresentResult( SP2Packet &rkPacket );
	void OnAccessoryAdd( SP2Packet &rkPacket, int &iCode );
	void OnAccessoryChange( SP2Packet &rkPacket, ioPlayStage* pPlayStage );
	void OnAccessoryCompound( SP2Packet &rkPacket );
	void OnAccessoryReinforce( SP2Packet &rkPacket );

public:
	void LoadAccessoryInfo();

public:
	const float& GetResellMortmainAccessoryPeso() const;
	const float& GetResellTimeAccessoryTimePeso() const;
	AccessoryCompoundInfo& FindCompoundInfo( int iCompoundCode );

public:
	static ioAccessoryInfoManager& GetSingleton();

public:
	ioAccessoryInfoManager();
	virtual ~ioAccessoryInfoManager();
};

#define g_AccessoryInfoMgr ioAccessoryInfoManager::GetSingleton()