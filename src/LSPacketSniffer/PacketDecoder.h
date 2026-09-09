#pragma once

#include <windows.h>
#include <string>
#include <vector>

#define LS_PK_HEADER_SIZE   16
#define LS_MAX_PACKET_SIZE  (32768 * 2)

#define LS_CUPK_CONNECT      0x4001
#define LS_CUPK_SYNCTIME    0x4002
#define LS_CUPK_CHAT        0x4003
#define LS_CUPK_RANGE_LO    0x4001
#define LS_CUPK_RANGE_HI    0x4FFF

struct DecodedPacket
{
	bool        bUDP;
	bool        bToServer;
	bool        bDirectionKnown;
	DWORD       dwSrcIP;
	WORD        wSrcPort;
	DWORD       dwDstIP;
	WORD        wDstPort;
	SYSTEMTIME  time;
	DWORD       dwSeq;

	DWORD       dwPacketID;
	DWORD       dwTotalSize;
	DWORD       dwCheckSum;
	DWORD       dwFSMState;
	bool        bChecksumValid;
	bool        bChecksumMD5;

	std::vector<BYTE> body;

	bool        bRelayEnvelope;
	DWORD       dwRelayDestIP;
	DWORD       dwRelayDestPort;
};

class PacketDecoder
{
public:
	static void EncryptBytes(BYTE* p, int len);
	static void DecryptBytes(BYTE* p, int len);

	static DWORD MakeDigestMD5(const BYTE* p, int len);
	static DWORD MakeDigestHybrid(const BYTE* p, int len);

	static bool DecodeRaw(const BYTE* pRaw, int nRawLen, bool bUDP,
	                      DWORD dwSrcIP, WORD wSrcPort, DWORD dwDstIP, WORD wDstPort,
	                      bool bToServer, bool bDirectionKnown,
	                      const SYSTEMTIME& time, DWORD dwSeq,
	                      DecodedPacket& out);

	static int PeekTcpPacketSize(const BYTE* pStream, int nAvail);

	static std::wstring BodyToHexDump(const std::vector<BYTE>& body);
	static std::wstring ExtractStrings(const std::vector<BYTE>& body);
	static std::wstring IPToString(DWORD dwIPNetworkOrder);
	static DWORD       StringToIP(const wchar_t* szIP);

	static bool SelfTest(std::wstring& rsReport);
};
