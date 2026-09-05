#pragma once

class CPacket;

class SP2Packet : public CPacket
{
public:
	SP2Packet();
	SP2Packet(DWORD ID);
	SP2Packet(char *buffer,int size);
	virtual ~SP2Packet();
	
public:
	SP2Packet& operator= (SP2Packet& packet);

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
	bool Write(LOGMessage& arg);
	bool Write(LOGMessageHeader& arg);
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
	bool Read(LOGMessage& arg);
	bool Read(LOGMessageHeader& arg);
	bool Read(SYSTEMTIME& arg);

	bool Ref(QueryHeader& arg);
};