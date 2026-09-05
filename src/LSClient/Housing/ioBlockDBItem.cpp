#include "stdafx.h"

#include "ioBlockDBItem.h"

ioBlockDBItem::ioBlockDBItem()
{	
	m_nCode			= 0;
	m_nUniqueIndex	= 0;
	m_nCellPos		= 0;
	m_nCellLevel	= 0;
	m_nTileMax		= 0;
	m_nRotate		= 0;	
}

ioBlockDBItem::ioBlockDBItem( int nCode, __int64 nUniqueIndex, int nCellPos, int nCellLevel, int nTileMax, int nRotate ) :
	m_nCode( nCode ),
	m_nUniqueIndex( nUniqueIndex ),
	m_nCellPos( nCellPos ),
	m_nCellLevel( nCellLevel ),
	m_nTileMax( nTileMax ),
	m_nRotate( nRotate )
{
}

ioBlockDBItem::~ioBlockDBItem()
{
}

DWORD ioBlockDBItem::GetCode() const
{
	return m_nCode;
}

DWORD ioBlockDBItem::GetType() const
{
	return m_nCode / HBT_CUT_TYPE;
}

__int64 ioBlockDBItem::GetUniqueIndex() const
{
	return m_nUniqueIndex;
}

DWORD ioBlockDBItem::GetSubType() const
{
	return m_nCode % HBT_CUT_TYPE;
}

int ioBlockDBItem::GetTilePosX() const
{
	return m_nCellPos % m_nTileMax;
}

int ioBlockDBItem::GetTilePosY() const
{
	return m_nCellLevel;
}

int ioBlockDBItem::GetTilePosZ() const
{
	return m_nCellPos / m_nTileMax;
}

TilePos ioBlockDBItem::GetTilePos() const
{
	return TilePos( GetTilePosX(), GetTilePosY(), GetTilePosZ() );
}

byte ioBlockDBItem::GetRotate() const
{
	return m_nRotate;
}

void ioBlockDBItem::FillItem( SP2Packet& rkPacket )
{
	rkPacket << m_nUniqueIndex;
	rkPacket << m_nCode;
	rkPacket << m_nCellPos;
	rkPacket << m_nCellLevel;
	rkPacket << m_nRotate;
}

void ioBlockDBItem::ApplyItem( SP2Packet& rkPacket )
{
	rkPacket >> m_nUniqueIndex;
	rkPacket >> m_nCode;	
	rkPacket >> m_nCellPos;
	rkPacket >> m_nCellLevel;
	rkPacket >> m_nRotate;
}

void ioBlockDBItem::SetTileMax( int nTileMax )
{
	m_nTileMax = nTileMax;
}