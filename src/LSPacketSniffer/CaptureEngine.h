#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <thread>

#include "PacketDecoder.h"

#define CAP_DEFAULT_BPF "tcp or udp"

struct CaptureDeviceInfo
{
	std::wstring sName;
	std::wstring sDesc;
};

class PacketStore;

class CaptureEngine
{
public:
	CaptureEngine(PacketStore* pStore);
	~CaptureEngine();

	static bool EnumerateDevices(std::vector<CaptureDeviceInfo>& out);

	bool Start(const wchar_t* szDevice, const wchar_t* szBpfFilter, std::wstring& rsError);
	void Stop();

	bool IsRunning() const { return m_bRunning.load(); }

	void SetServerPorts(const std::vector<WORD>& vPorts) { m_vServerPorts = vPorts; }
	void SetStrictChecksum(bool b) { m_bStrictCS.store(b); }
	bool IsStrictChecksum() const { return m_bStrictCS.load(); }

	DWORD GetDroppedTcpCount() const { return m_dwDroppedTcp; }
	DWORD GetDroppedUdpCount() const { return m_dwDroppedUdp; }
	DWORD GetRawPacketsSeen() const { return m_dwRawSeen; }

private:
	struct StreamKey
	{
		DWORD dwIpA; WORD wPortA;
		DWORD dwIpB; WORD wPortB;

		bool operator<(const StreamKey& o) const
		{
			if (dwIpA != o.dwIpA) return dwIpA < o.dwIpA;
			if (wPortA != o.wPortA) return wPortA < o.wPortA;
			if (dwIpB != o.dwIpB) return dwIpB < o.dwIpB;
			return wPortB < o.wPortB;
		}
	};

	struct StreamBuffer
	{
		std::vector<BYTE> buf;
		bool bServerIsB = false;
		bool bDirectionKnown = false;
	};

	static void PcapHandlerStub(unsigned char* user, const struct pcap_pkthdr* h, const unsigned char* bytes);
	void OnRawPacket(const struct pcap_pkthdr* h, const unsigned char* bytes);
	void OnTcpPayload(DWORD dwSrcIP, WORD wSrcPort, DWORD dwDstIP, WORD wDstPort, const BYTE* payload, int len, const SYSTEMTIME& tm);
	void OnUdpDatagram(DWORD dwSrcIP, WORD wSrcPort, DWORD dwDstIP, WORD wDstPort, const BYTE* data, int len, const SYSTEMTIME& tm);
	void DrainStream(StreamKey& key, StreamBuffer& sb, DWORD dwSrcIP, WORD wSrcPort, DWORD dwDstIP, WORD wDstPort, const SYSTEMTIME& tm);

	bool DetermineDirection(DWORD dwSrcIP, WORD wSrcPort, DWORD dwDstIP, WORD wDstPort,
	                        bool& bSrcIsServer, bool& bKnown) const;
	bool IsServerPort(WORD wPort) const;

	void RunThread();

	PacketStore* m_pStore;
	void* m_pPcap;
	void* m_pDumper;
	std::thread m_thread;
	std::atomic<bool> m_bRunning{ false };
	std::wstring m_sSavePcapPath;

	std::vector<WORD> m_vServerPorts;

	std::mutex m_streamMutex;
	std::map<StreamKey, StreamBuffer> m_streams;

	std::atomic<DWORD> m_dwDroppedTcp{ 0 };
	std::atomic<DWORD> m_dwDroppedUdp{ 0 };
	std::atomic<DWORD> m_dwRawSeen{ 0 };
	std::atomic<bool> m_bStrictCS{ true };
};
