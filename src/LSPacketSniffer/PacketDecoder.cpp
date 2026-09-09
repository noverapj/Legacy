#include "pch.h"
#include "PacketDecoder.h"
#include "OpcodeTable.h"
#include "cryption.h"
#include <cstdio>
#include <cstring>

namespace
{
	bool HexToBytes(const char* szHex, std::vector<BYTE>& out)
	{
		out.clear();
		while (*szHex)
		{
			while (*szHex == ' ') szHex++;
			if (!*szHex) break;
			int hi, lo;
			auto hexv = [](char c) -> int {
				if (c >= '0' && c <= '9') return c - '0';
				if (c >= 'a' && c <= 'f') return c - 'a' + 10;
				if (c >= 'A' && c <= 'F') return c - 'A' + 10;
				return -1;
			};
			hi = hexv(szHex[0]);
			if (hi < 0) return false;
			if (!szHex[1]) return false;
			lo = hexv(szHex[1]);
			if (lo < 0) return false;
			out.push_back((BYTE)((hi << 4) | lo));
			szHex += 2;
		}
		return !out.empty();
	}
}

int PacketDecoder::PeekTcpPacketSize(const BYTE* pStream, int nAvail)
{
	if (nAvail < 8)
		return 0;

	DWORD dwSize;
	memcpy(&dwSize, pStream + 4, 4);

	if (dwSize < LS_PK_HEADER_SIZE || dwSize > LS_MAX_PACKET_SIZE)
		return -1;

	if ((int)dwSize > nAvail)
		return 0;

	return (int)dwSize;
}

bool PacketDecoder::DecodeRaw(const BYTE* pRaw, int nRawLen, bool bUDP,
                              DWORD dwSrcIP, WORD wSrcPort, DWORD dwDstIP, WORD wDstPort,
                              bool bToServer, bool bDirectionKnown,
                              const SYSTEMTIME& time, DWORD dwSeq,
                              DecodedPacket& out)
{
	if (nRawLen < LS_PK_HEADER_SIZE || nRawLen > LS_MAX_PACKET_SIZE)
		return false;

	BYTE buf[LS_MAX_PACKET_SIZE];
	memcpy(buf, pRaw, nRawLen);
	Decrypt(buf + 8, nRawLen - 8);

	memcpy(&out.dwPacketID,  buf + 0,  4);
	memcpy(&out.dwTotalSize, buf + 4,  4);
	memcpy(&out.dwCheckSum,  buf + 8,  4);
	memcpy(&out.dwFSMState,  buf + 12, 4);

	if (out.dwTotalSize < LS_PK_HEADER_SIZE || (int)out.dwTotalSize > nRawLen)
		return false;

	out.body.assign(buf + LS_PK_HEADER_SIZE, buf + out.dwTotalSize);

	{
		memset(buf + 8, 0, 4);
		DWORD dwHybrid = MakeDigestSHA256(buf, (int)out.dwTotalSize);
		if (dwHybrid == out.dwCheckSum)
		{
			out.bChecksumValid = true;
			out.bChecksumMD5 = false;
		}
		else
		{
			DWORD dwMd5 = MakeDigest(buf, (int)out.dwTotalSize);
			out.bChecksumValid = (dwMd5 == out.dwCheckSum);
			out.bChecksumMD5 = out.bChecksumValid;
		}
	}

	out.bUDP = bUDP;
	out.bToServer = bToServer;
	out.bDirectionKnown = bDirectionKnown;
	out.dwSrcIP = dwSrcIP;
	out.wSrcPort = wSrcPort;
	out.dwDstIP = dwDstIP;
	out.wDstPort = wDstPort;
	out.time = time;
	out.dwSeq = dwSeq;

	out.bRelayEnvelope = false;
	out.dwRelayDestIP = 0;
	out.dwRelayDestPort = 0;
	if (bUDP && out.body.size() >= 8 &&
	    out.dwPacketID >= LS_CUPK_RANGE_LO && out.dwPacketID <= LS_CUPK_RANGE_HI)
	{
		bool bRegistration =
			out.dwPacketID == 0x4001 ||
			out.dwPacketID == 0x4002 ||
			out.dwPacketID == 0x4003 ||
			out.dwPacketID == 0x4006 ||
			out.dwPacketID == 0x4007 ||
			out.dwPacketID == 0x4008 ||
			out.dwPacketID == 0x4009;

		if (!bRegistration)
		{
			out.bRelayEnvelope = true;
			memcpy(&out.dwRelayDestIP,    &out.body[0], 4);
			memcpy(&out.dwRelayDestPort,  &out.body[4], 4);
		}
	}

	return true;
}

std::wstring PacketDecoder::IPToString(DWORD dwIPNetworkOrder)
{
	wchar_t buf[24];
	const BYTE* b = (const BYTE*)&dwIPNetworkOrder;
	swprintf_s(buf, L"%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
	return buf;
}

DWORD PacketDecoder::StringToIP(const wchar_t* szIP)
{
	DWORD parts[4] = { 0,0,0,0 };
	int idx = 0;
	while (*szIP && idx < 4)
	{
		if (*szIP == L'.') { idx++; szIP++; continue; }
		parts[idx] = parts[idx] * 10 + (*szIP - L'0');
		szIP++;
	}
	return (DWORD)((parts[0] & 0xff) | ((parts[1] & 0xff) << 8) | ((parts[2] & 0xff) << 16) | ((parts[3] & 0xff) << 24));
}

std::wstring PacketDecoder::BodyToHexDump(const std::vector<BYTE>& body)
{
	std::wstring s;
	wchar_t line[128];

	int total = (int)body.size();
	int rows = (total + 15) / 16;
	s.reserve((size_t)rows * 88);

	for (int r = 0; r < rows; r++)
	{
		int off = r * 16;
		int n = total - off;
		if (n > 16) n = 16;

		wchar_t hexPart[52] = L"";
		wchar_t ascPart[20] = L"";
		int hx = 0;

		for (int i = 0; i < 16; i++)
		{
			if (i < n)
			{
				BYTE b = body[off + i];
				hx += swprintf_s(hexPart + hx, _countof(hexPart) - hx, L"%02X ", b);
				ascPart[i] = (b >= 0x20 && b < 0x7f) ? (wchar_t)b : L'.';
			}
			else
			{
				hx += swprintf_s(hexPart + hx, _countof(hexPart) - hx, L"   ");
				ascPart[i] = L' ';
			}
		}
		ascPart[16] = 0;

		swprintf_s(line, L"%04X  %s %s\r\n", off, hexPart, ascPart);
		s += line;
	}
	return s;
}

std::wstring PacketDecoder::ExtractStrings(const std::vector<BYTE>& body)
{
	std::wstring s;
	int runStart = -1;
	int total = (int)body.size();
	for (int i = 0; i <= total; i++)
	{
		bool printable = (i < total) && (body[i] >= 0x20 && body[i] < 0x7f);
		if (printable && runStart < 0) runStart = i;
		if (!printable && runStart >= 0)
		{
			int len = i - runStart;
			if (len >= 4)
			{
				wchar_t off[32];
				swprintf_s(off, L"  @%04X: ", runStart);
				s += off;
				for (int j = runStart; j < i; j++) s += (wchar_t)body[j];
				s += L"\r\n";
			}
			runStart = -1;
		}
	}
	return s;
}

bool PacketDecoder::SelfTest(std::wstring& rsReport)
{
	rsReport.clear();

	struct Fixture
	{
		const char* szHeader;
		const char* szBody;
		DWORD dwExpectID;
		DWORD dwExpectChecksum;
		const char* szExpectString;
		const char* szLabel;
	};

	static const Fixture fx[] =
	{
		{
			"5F050000470000000C9C611FFFFFFFFF",
			"33313132373935356539613038376434656634386565323932316130333332333139666566323130323263666465643061323539643900",
			0x055F, 0x1F619C0C,
			"31127955e9a087d4ef48ee2921a0332319fef21022cfded0a259d9",
			"CTPK_CONNECT 0x055F (official client)"
		},
		{
			"014000001C00000012A136E440150000",
			"4E6F7665726144656D6F3100",
			0x4001, 0xE436A112,
			"NoveraDemo1",
			"CUPK_CONNECT 0x4001 (official client)"
		},
	};

	bool bAllOK = true;

	for (int k = 0; k < _countof(fx); k++)
	{
		std::vector<BYTE> hdr, body;
		bool bOK = true;
		std::wstring line;

		wchar_t buf[512];

		do
		{
			if (!HexToBytes(fx[k].szHeader, hdr) || hdr.size() != 16) { bOK = false; break; }
			if (!HexToBytes(fx[k].szBody, body)) { bOK = false; break; }

			BYTE sanity[8] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
			BYTE orig[8];
			memcpy(orig, sanity, 8);
			Encrypt(sanity, 8);
			Decrypt(sanity, 8);
			if (memcmp(sanity, orig, 8) != 0)
			{
				swprintf_s(buf, L"FAIL: Encrypt/Decrypt roundtrip gagal\r\n");
				line += buf;
				bOK = false;
				break;
			}

			std::vector<BYTE> wire(hdr);
			wire.insert(wire.end(), body.begin(), body.end());
			Encrypt(wire.data() + 8, (int)wire.size() - 8);

			SYSTEMTIME st = { 2026, 9, 0, 8, 15, 8, 22, 0 };
			DecodedPacket dp;
			if (!DecodeRaw(wire.data(), (int)wire.size(), fx[k].dwExpectID == 0x4001,
			               0x0100007F, 14011, 0x0100007F, 14031, true, true, st, 0, dp))
			{
				swprintf_s(buf, L"FAIL: DecodeRaw return false\r\n");
				line += buf;
				bOK = false;
				break;
			}

			if (dp.dwPacketID != fx[k].dwExpectID)
			{
				swprintf_s(buf, L"FAIL: ID 0x%04X != expect 0x%04X\r\n", dp.dwPacketID, fx[k].dwExpectID);
				line += buf;
				bOK = false;
			}

			if (!dp.bChecksumValid)
			{
				swprintf_s(buf, L"FAIL: checksum 0x%08X tidak valid (expect 0x%08X)\r\n", dp.dwCheckSum, fx[k].dwExpectChecksum);
				line += buf;
				bOK = false;
			}
			else if (dp.bChecksumMD5)
			{
				swprintf_s(buf, L"WARN: match via MD5 klasik, bukan hybrid\r\n");
				line += buf;
			}

			std::string got((const char*)dp.body.data(), dp.body.size());
			if (strstr(got.c_str(), fx[k].szExpectString) == nullptr)
			{
				swprintf_s(buf, L"FAIL: string expected tidak ketemu di body\r\n");
				line += buf;
				bOK = false;
			}
		} while (false);

		swprintf_s(buf, L"[%s] %s : ID=0x%04X CS=0x%08X\r\n",
		           bOK ? L"PASS" : L"FAIL", L"fixture", fx[k].dwExpectID, fx[k].dwExpectChecksum);
		rsReport = rsReport + line + buf;

		if (bOK == false) bAllOK = false;
	}

	rsReport += bAllOK ? L"SELF-TEST: SEMUA PASS\r\n" : L"SELF-TEST: ADA FAILURE\r\n";
	return bAllOK;
}
