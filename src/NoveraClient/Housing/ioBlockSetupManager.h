#pragma once
#include "ioBlockEventAgency.h"
#include "BlockDefine.h"

class ioPlayStage;
class ioBaseChar;
class ioConstructHouseSpecialState;

struct SetupBlockItemInfo
{
	DWORD			dwCode;
	DWORD			dwCount;

	SetupBlockItemInfo()
	{
		dwCode		= 0;
		dwCount		= 0;
	}
};
typedef std::vector< SetupBlockItemInfo>  SetupBlockItemInfoList;


class ioBlockSetupManager : public Singleton< ioBlockSetupManager >
{
protected:
	ioPlayStage*			m_pStage;
	BlockModeType			m_BlockModeType;

	DWORD					m_dwSetUpItemCode;
	SetupBlockItemInfoList	m_SetupBlockItemInfoList;
	ioHashString			m_SetUpAdminUser;

protected:
	int						m_nSetUpItemListIndex;
	int						m_nBlockLimitCount;

public:
	void Initialize( ioPlayStage* pStage, BlockModeType eType );

public:
	void HousingSetUpProcess();
	void BeginSetUpState();
	void ApplySetUpState();
	void EndSetUpState();
	void ApplyEndSetUpState();

	bool IsSetUpState();
	ioConstructHouseSpecialState* GetAdminState();

public:
	void SendSetUpBlock( int nItemCode, int x, int y, int z, byte rotate );
	void ApplySetUpBlock();

	void SendRemoveBlock( __int64 nUniqueIndex );
	void ApplyRemoveBlock();

public:
	void ChangeToSetUp();	
	void ChangeToRemove();

public:
	virtual void Process();
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

public:
	void OnSetUpState( SP2Packet &rkPacket );
	void OnSetUpBlock( SP2Packet &rkPacket );
	void OnRemoveBlock( SP2Packet &rkPacket );

public:
	inline int GetBlockLimitCount(){ return m_nBlockLimitCount; }

public:
	static ioBlockSetupManager& GetSingleton();

public:
	ioBlockSetupManager();
	virtual ~ioBlockSetupManager();

};

#define g_BlockSetupManager ioBlockSetupManager::GetSingleton()

