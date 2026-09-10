#pragma once
#include "BlockDefine.h"

class ChatMacroMgr;
class ioBlockDBItem
{

protected:	
	int				m_nCode;
	__int64			m_nUniqueIndex;

	int				m_nCellPos;
	int				m_nCellLevel;
	int				m_nTileMax;
	byte			m_nRotate;

public:
	DWORD GetCode() const;
	DWORD GetType() const;
	DWORD GetSubType() const;

public:
	__int64 GetUniqueIndex() const;

public:
	int GetTilePosX() const;	
	int GetTilePosY() const;
	int GetTilePosZ() const;
	TilePos GetTilePos() const;

	byte GetRotate() const;

public:
	void FillItem( SP2Packet& rkPacket );
	void ApplyItem( SP2Packet& rkPacket );

public:
	void SetTileMax( int nTileMax );

public:
	ioBlockDBItem();
	ioBlockDBItem( int nCode, __int64 nUniqueIndex, int nCellPos, int nCellLevel, int nTileMax, int nRotate );
	virtual ~ioBlockDBItem();
};
typedef std::vector<ioBlockDBItem> ioBlockDBItemList;

struct ioBlockDBItemContainer
{
	ioBlockDBItemList ItemList;

	ioBlockDBItemContainer()
	{
	}
};
typedef std::map< ioHashString, ioBlockDBItemContainer> BlockDBItemMap;