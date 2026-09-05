#pragma once
#include "../../io3DEngine/ioStringConverter.h"

#include "BlockDefine.h"
#include "ioBlockDBItem.h"

class ioBlock;
class ioPlayStage;

class ioUserBlockStorage
{
public:
	enum
	{
		SYNC_TIME = 10000,
	};

	struct InvenBlockItem
	{
		ioHashString	m_szName;
		ioHashString	m_szIcon;
		int				m_nBlockCode;
		int				m_nOrder;
		int				m_nEtcCode;	
		int				m_nManualIdx;
		int				m_nCount;
		int				m_nItemCategory;

		InvenBlockItem()
		{
			m_nBlockCode	= 0;
			m_nEtcCode		= 0;			
			m_nManualIdx	= 0;
			m_nCount		= 0;
			m_nItemCategory	= 0;
		}
	};
	typedef std::vector<InvenBlockItem> InvenBlockItemList;

	class InvenBlockItemSort : public std::binary_function< const InvenBlockItem&, const InvenBlockItem&, bool >
	{
	public:
		bool operator()( const InvenBlockItem &lhs , const InvenBlockItem &rhs ) const
		{
			if( lhs.m_nOrder < rhs.m_nOrder )
				return true;

			return false;
		}
	};

protected:
	ioBlockDBItemContainer	m_ioBlockDBItemContainer;
	InvenBlockItemList		m_InvenBlockItemList;
	int						m_CurrInvenItemIndex;

	DWORD					m_dwBlockInvenSyncTime;

public:
	void Initialize();
	void UpdateInvenBlockItem();

public:
	ioBlockDBItemContainer* GetBlockDBItemContainer();

public:
	virtual void SendBlockInvenSync( bool bMust = false );

public:	
	virtual void InitInvenBlockItem();
	ioUserBlockStorage::InvenBlockItem* GetInvenBlockItem( int nIndex, bool bZeroNtoNull = false );
	ioUserBlockStorage::InvenBlockItem* GetInvenBlockItemByCode( int nCode, bool bZeroNtoNull = false );
	ioUserBlockStorage::InvenBlockItem* GetCurrInvenBlockItem();

	void GetInvenItemAll( OUT InvenBlockItemList& List );

public:
	void IncreaseItem( int nCode );
	void DecreaseItem( int nCode );
	
	void NextItem();
	void PrevItem();
	void SelectItem( int nCode );

	int GetCurrInvenIndex();
	int GetInvenItemCount();

public:	
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

public:
	ioUserBlockStorage();
	virtual ~ioUserBlockStorage();

};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


class ioGuildBlockStorage : public ioUserBlockStorage
{
protected:
	virtual void InitInvenBlockItem();
	virtual void SendBlockInvenSync( bool bMust = false );

public:	
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );

public:
	void ApplyBlcokDBItem( SP2Packet& rkPacket );
	void ApplyBlcokInvenItem( SP2Packet& rkPacket );

public:
	ioGuildBlockStorage();
	virtual ~ioGuildBlockStorage();
};


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


class ioHomeBlockStorage : public ioUserBlockStorage
{
public:
	void ClearBlock();

protected:
	virtual void InitInvenBlockItem();
	virtual void SendBlockInvenSync( bool bMust = false );

public:	
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );

public:
	void ApplyBlcokDBItem( SP2Packet& rkPacket );
	void ApplyBlcokInvenItem( SP2Packet& rkPacket );

public:
	ioHomeBlockStorage();
	virtual ~ioHomeBlockStorage();
};