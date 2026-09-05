#ifndef _SP2Packet_h_
#define _SP2Packet_h_

class CPacket;
class CQueryData;
class CQueryResultData;
struct CHARACTER;
struct GAMESERVERINFO;

class SP2Packet : public CPacket
{
	public:
	SP2Packet();
	SP2Packet( const SP2Packet &rhs );
	SP2Packet(DWORD ID);
	SP2Packet(char *buffer,int size);
	SP2Packet( DWORD dwUserIndex, SP2Packet &rhs );
	virtual ~SP2Packet();

	const char* GetData() const;
	int GetDataSize() const;
	const char* GetBuffer() const;	
	int   GetBufferSize() const;
	void SetDataAdd( char *buffer, int size, bool bCurPosReSet = false );
	
	//operator
public:
	SP2Packet& operator =  ( const SP2Packet& packet );
	SP2Packet& operator << (BYTE arg);
	SP2Packet& operator << (bool arg);
	SP2Packet& operator << (int arg);
	SP2Packet& operator << (LONG arg);
	SP2Packet& operator << (WORD arg);
	SP2Packet& operator << (DWORD arg);
	SP2Packet& operator << (__int64 arg);
	SP2Packet& operator << (LPTSTR arg);
	SP2Packet& operator << (double arg);	
	SP2Packet& operator << (float arg);
	SP2Packet& operator << (short arg);
	SP2Packet& operator << ( const ioHashString &arg );
	SP2Packet& operator << ( const Vector3 &arg );
	SP2Packet& operator << ( const Quaternion &arg );
	SP2Packet& operator << (CQueryData &arg);
	SP2Packet& operator << (CQueryResultData &arg);
	SP2Packet& operator << ( const GAMESERVERINFO& arg );
	SP2Packet& operator << ( const MAINSERVERINFO& arg );
	SP2Packet& operator << ( SendRelayInfo_& arg);

	SP2Packet& operator >> (BYTE &arg);
	SP2Packet& operator >> (bool &arg);
	SP2Packet& operator >> (int &arg);
	SP2Packet& operator >> (LONG &arg);
	SP2Packet& operator >> (WORD &arg);
	SP2Packet& operator >> (DWORD &arg);
	SP2Packet& operator >> (__int64 &arg);
	SP2Packet& operator >> (LPTSTR arg);
	SP2Packet& operator >> (double &arg);	
	SP2Packet& operator >> (float &arg);	
	SP2Packet& operator >> (short &arg);
	SP2Packet& operator >> ( ioHashString &arg );
	SP2Packet& operator >> ( Vector3 &arg );
	SP2Packet& operator >> ( Quaternion &arg );
	SP2Packet& operator >> (CQueryData &arg);
	SP2Packet& operator >> (CQueryResultData &arg);
	SP2Packet&	operator >> ( GAMESERVERINFO& arg );
	SP2Packet&	operator >> ( MAINSERVERINFO& arg );
	SP2Packet& operator >> ( SendRelayInfo_& arg);

public:
	bool Write(bool arg);
	bool Write(int arg);
	bool Write(LONG arg);
	bool Write(DWORD arg);
	bool Write(__int64 arg);
	bool Write(LPTSTR arg);
	bool Write(double arg);
	bool Write(float arg);
	bool Write(const ioHashString &arg);
	bool Write(CQueryData &arg);
	bool Write(CQueryResultData &arg);
	bool Write(SYSTEMTIME& arg);

	bool Read(bool& arg);
	bool Read(int& arg);
	bool Read(LONG& arg);
	bool Read(DWORD& arg);
	bool Read(__int64& arg);
	bool Read(const int nLength, LPTSTR arg);
	bool Read(double& arg);
	bool Read(float& arg);
	bool Read(ioHashString& arg);
	bool Read(CQueryData& arg);
	bool Read(CQueryResultData& arg);

};
#endif