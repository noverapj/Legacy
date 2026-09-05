#ifndef _SP2Packet_h_
#define _SP2Packet_h_

#include "Packet.h"
#include "ioHashString.h"


class SP2Packet : public CPacket
{
	public:
	SP2Packet();
	SP2Packet( const SP2Packet &rhs );
	SP2Packet(DWORD ID);
	SP2Packet(const uint8* buffer,int size);
	SP2Packet( DWORD dwUserIndex, SP2Packet &rhs );
	virtual ~SP2Packet();

	const uint8* GetData() const;
	int GetDataSize() const;
	const uint8* GetBuffer() const;	
	int   GetBufferSize() const;
	void  SetDataAdd( char *buffer, int size, bool bCurPosReSet = false );
	void  SetDataAddCreateUDP( DWORD dwIP, DWORD dwPort, char *buffer, int size, bool bCurPosReSet = false );
	// >> 쪽으로 current_pos 이동
	void  MovePointer( DWORD dwMoveBytes );
	void  SetPosBegin();

	//operator
public:
	SP2Packet&  operator =  ( const SP2Packet& packet );
	SP2Packet&  operator << (BYTE arg);
	SP2Packet&  operator << (bool arg);
	SP2Packet&  operator << (int arg);
	SP2Packet&  operator << (LONG arg);
	SP2Packet&  operator << (WORD arg);
	SP2Packet&  operator << (DWORD arg);
	SP2Packet&  operator << (__int64 arg);
	SP2Packet&  operator << (LPTSTR arg);
	SP2Packet&  operator << (double arg);	
	SP2Packet&  operator << (float arg);
	SP2Packet&  operator << (short arg);
	//SP2Packet&  operator << ( CHARACTER &arg );
	SP2Packet&  operator << ( const ioHashString &arg );

	SP2Packet&  operator << (CQueryData &arg);
	SP2Packet&  operator << (CQueryResultData &arg);
	
	SP2Packet&  operator >> (BYTE &arg);
	SP2Packet&  operator >> (bool &arg);
	SP2Packet&  operator >> (int &arg);
	SP2Packet&  operator >> (LONG &arg);
	SP2Packet&  operator >> (WORD &arg);
	SP2Packet&  operator >> (DWORD &arg);
	SP2Packet&  operator >> (__int64 &arg);
	SP2Packet&  operator >> (LPTSTR arg);
	SP2Packet&  operator >> (double &arg);	
	SP2Packet&  operator >> (float &arg);	
	SP2Packet&  operator >> (short &arg);
	//SP2Packet&  operator >> (CHARACTER &arg);
	SP2Packet&  operator >> ( ioHashString &arg );

	SP2Packet&  operator >> (CQueryData &arg);
	SP2Packet&  operator >> (CQueryResultData &arg);

};
#endif