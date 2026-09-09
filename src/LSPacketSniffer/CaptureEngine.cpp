#include "pch.h"
#include "CaptureEngine.h"
#include "PacketStore.h"

#include <pcap.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace
{
	DWORD ReadDwordLE(const BYTE* p)
	{
		return (DWORD)p[0] | ((DWORD)p[1] << 8) | ((DWORD)p[2] << 16) | ((DWORD)p[3] << 24);
	}

	WORD ReadWordLE(const BYTE* p)
	{
		return (WORD)(p[0] | (p[1] << 8));
	}

	bool IsLocalAddress(DWORD dwIPNetOrder)
	{
		if ((dwIPNetOrder & 0x000000FF) == 127) return true;
		return false;
	}
}

bool CaptureEngine::EnumerateDevices(std::vector<CaptureDeviceInfo>& out)
{
	out.clear();

	pcap_if_t* pAll = nullptr;
	char errbuf[PCAP_ERRBUF_SIZE] = "";

	if (pcap_findalldevs(&pAll, errbuf) == -1)
		return false;

	for (pcap_if_t* d = pAll; d; d = d->next)
	{
		CaptureDeviceInfo info;
		std::string name = d->name ? d->name : "";
		std::string desc = d->description ? d->description : "";
		info.sName.assign(name.begin(), name.end());
		info.sDesc.assign(desc.begin(), desc.end());
		if (info.sDesc.empty())
			info.sDesc = info.sName;
		out.push_back(info);
	}

	pcap_freealldevs(pAll);
	return !out.empty();
}

CaptureEngine::CaptureEngine(PacketStore* pStore)
	: m_pStore(pStore)
	, m_pPcap(nullptr)
	, m_pDumper(nullptr)
{
}

CaptureEngine::~CaptureEngine()
{
	Stop();
}

bool CaptureEngine::Start(const wchar_t* szDevice, const wchar_t* szBpfFilter, std::wstring& rsError)
{
	if (m_bRunning.load())
	{
		rsError = L"Sudah running";
		return false;
	}

	char errbuf[PCAP_ERRBUF_SIZE] = "";

	std::string dev;
	{
		int len = lstrlenW(szDevice);
		dev.resize(len);
		for (int i = 0; i < len; i++) dev[i] = (char)szDevice[i];
	}

	pcap_t* pPcap = pcap_create(dev.c_str(), errbuf);
	if (!pPcap)
	{
		rsError = L"pcap_create gagal: ";
		std::string e(errbuf);
		rsError += std::wstring(e.begin(), e.end());
		return false;
	}

	pcap_set_snaplen(pPcap, 65536);
	pcap_set_promisc(pPcap, 1);
	pcap_set_timeout(pPcap, 100);
	pcap_set_buffer_size(pPcap, 4 * 1024 * 1024);

	if (pcap_activate(pPcap) != 0)
	{
		rsError = L"pcap_activate gagal: ";
		std::string e(pcap_geterr(pPcap));
		rsError += std::wstring(e.begin(), e.end());
		pcap_close(pPcap);
		return false;
	}

	if (szBpfFilter && *szBpfFilter)
	{
		std::string filt;
		{
			int len = lstrlenW(szBpfFilter);
			filt.resize(len);
			for (int i = 0; i < len; i++) filt[i] = (char)szBpfFilter[i];
		}

		struct bpf_program fp;
		if (pcap_compile(pPcap, &fp, filt.c_str(), 1, PCAP_NETMASK_UNKNOWN) == -1)
		{
			rsError = L"pcap_compile gagal: ";
			std::string e(pcap_geterr(pPcap));
			rsError += std::wstring(e.begin(), e.end());
			pcap_close(pPcap);
			return false;
		}
		if (pcap_setfilter(pPcap, &fp) == -1)
		{
			rsError = L"pcap_setfilter gagal";
			pcap_freecode(&fp);
			pcap_close(pPcap);
			return false;
		}
		pcap_freecode(&fp);
	}

	if (!m_sSavePcapPath.empty())
	{
		std::string path;
		{
			int len = lstrlenW(m_sSavePcapPath.c_str());
			path.resize(len);
			for (int i = 0; i < len; i++) path[i] = (char)m_sSavePcapPath[i];
		}
		m_pDumper = pcap_dump_open(pPcap, path.c_str());
	}

	m_pPcap = pPcap;
	m_bRunning.store(true);

	m_thread = std::thread([this]()
	{
		RunThread();
	});

	return true;
}

void CaptureEngine::Stop()
{
	if (!m_bRunning.load() && !m_thread.joinable())
		return;

	m_bRunning.store(false);

	if (m_pPcap)
		pcap_breakloop((pcap_t*)m_pPcap);

	if (m_thread.joinable())
		m_thread.join();

	if (m_pDumper)
	{
		pcap_dump_flush((pcap_dumper_t*)m_pDumper);
		pcap_dump_close((pcap_dumper_t*)m_pDumper);
		m_pDumper = nullptr;
	}

	if (m_pPcap)
	{
		pcap_close((pcap_t*)m_pPcap);
		m_pPcap = nullptr;
	}

	std::lock_guard<std::mutex> lock(m_streamMutex);
	m_streams.clear();
}

void CaptureEngine::RunThread()
{
	pcap_loop((pcap_t*)m_pPcap, 0, PcapHandlerStub, (unsigned char*)this);
}

void CaptureEngine::PcapHandlerStub(unsigned char* user, const struct pcap_pkthdr* h, const unsigned char* bytes)
{
	CaptureEngine* pThis = (CaptureEngine*)user;
	if (pThis)
		pThis->OnRawPacket(h, bytes);
}

void CaptureEngine::OnRawPacket(const struct pcap_pkthdr* h, const unsigned char* bytes)
{
	m_dwRawSeen++;

	if (m_pDumper)
		pcap_dump((u_char*)m_pDumper, h, bytes);

	SYSTEMTIME tm;
	{
		FILETIME ft;
		SYSTEMTIME st;
		struct tm t;
		time_t tt = h->ts.tv_sec;
		localtime_s(&t, &tt);
		st.wYear = (WORD)(t.tm_year + 1900);
		st.wMonth = (WORD)(t.tm_mon + 1);
		st.wDayOfWeek = 0;
		st.wDay = (WORD)t.tm_mday;
		st.wHour = (WORD)t.tm_hour;
		st.wMinute = (WORD)t.tm_min;
		st.wSecond = (WORD)t.tm_sec;
		st.wMilliseconds = (WORD)(h->ts.tv_usec / 1000);
		SystemTimeToFileTime(&st, &ft);
		FileTimeToSystemTime(&ft, &tm);
		tm.wMilliseconds = st.wMilliseconds;
	}

	int nLink = pcap_datalink((pcap_t*)m_pPcap);

	const BYTE* ip = nullptr;

	if (nLink == DLT_EN10MB)
	{
		if (h->caplen < 14) return;
		WORD ethType = (WORD)((bytes[12] << 8) | bytes[13]);
		if (ethType != 0x0800) return;
		ip = bytes + 14;
	}
	else if (nLink == DLT_NULL)
	{
		if (h->caplen < 4) return;
		ip = bytes + 4;
	}
	else if (nLink == DLT_RAW)
	{
		ip = bytes;
	}
	else
		return;

	if (h->caplen < (u_int)(ip - bytes) + 20) return;

	int ihl = (ip[0] & 0x0F) * 4;
	if (ihl < 20) return;
	if (((ip[0] >> 4) & 0x0F) != 4) return;

	BYTE proto = ip[9];
	DWORD dwSrcIP, dwDstIP;
	memcpy(&dwSrcIP, ip + 12, 4);
	memcpy(&dwDstIP, ip + 16, 4);

	const BYTE* transport = ip + ihl;

	if (proto == 6)
	{
		if (h->caplen < (u_int)(transport - bytes) + 20) return;
		WORD wSrcPort = (WORD)((transport[0] << 8) | transport[1]);
		WORD wDstPort = (WORD)((transport[2] << 8) | transport[3]);
		BYTE dataOffset = (transport[12] >> 4) * 4;
		if (dataOffset < 20) return;

		int payloadLen = (int)h->caplen - (int)(transport - bytes) - dataOffset;
		if (payloadLen <= 0) return;

		OnTcpPayload(dwSrcIP, wSrcPort, dwDstIP, wDstPort,
		             transport + dataOffset, payloadLen, tm);
	}
	else if (proto == 17)
	{
		if (h->caplen < (u_int)(transport - bytes) + 8) return;
		WORD wSrcPort = (WORD)((transport[0] << 8) | transport[1]);
		WORD wDstPort = (WORD)((transport[2] << 8) | transport[3]);
		WORD wLen = (WORD)((transport[4] << 8) | transport[5]);
		int dataLen = (int)h->caplen - (int)(transport - bytes) - 8;
		if (wLen >= 8) dataLen = min(dataLen, wLen - 8);
		if (dataLen <= 0) return;

		OnUdpDatagram(dwSrcIP, wSrcPort, dwDstIP, wDstPort,
		              transport + 8, dataLen, tm);
	}
}

bool CaptureEngine::DetermineDirection(DWORD dwSrcIP, WORD wSrcPort, DWORD dwDstIP, WORD wDstPort,
                                       bool& bSrcIsServer, bool& bKnown) const
{
	if (IsLocalAddress(dwSrcIP) && IsLocalAddress(dwDstIP))
	{
		if (!m_vServerPorts.empty())
		{
			if (IsServerPort(wSrcPort)) { bSrcIsServer = true;  bKnown = true;  return true; }
			if (IsServerPort(wDstPort)) { bSrcIsServer = false; bKnown = true;  return true; }
		}
		bSrcIsServer = (wSrcPort < wDstPort);
		bKnown = true;
		return true;
	}

	if (IsLocalAddress(dwSrcIP)) { bSrcIsServer = false; bKnown = true;  return true; }
	if (IsLocalAddress(dwDstIP)) { bSrcIsServer = true;  bKnown = true;  return true; }

	if (!m_vServerPorts.empty())
	{
		if (IsServerPort(wSrcPort)) { bSrcIsServer = true;  bKnown = true;  return true; }
		if (IsServerPort(wDstPort)) { bSrcIsServer = false; bKnown = true;  return true; }
	}

	bKnown = false;
	bSrcIsServer = (wSrcPort < wDstPort);
	return true;
}

bool CaptureEngine::IsServerPort(WORD wPort) const
{
	for (size_t i = 0; i < m_vServerPorts.size(); i++)
	{
		if (m_vServerPorts[i] == wPort)
			return true;
	}
	return false;
}

void CaptureEngine::OnTcpPayload(DWORD dwSrcIP, WORD wSrcPort, DWORD dwDstIP, WORD wDstPort,
                                 const BYTE* payload, int len, const SYSTEMTIME& tm)
{
	StreamKey key;
	{
		UINT64 a = ((UINT64)dwSrcIP << 16) | wSrcPort;
		UINT64 b = ((UINT64)dwDstIP << 16) | wDstPort;
		if (a <= b)
		{
			key.dwIpA = dwSrcIP; key.wPortA = wSrcPort;
			key.dwIpB = dwDstIP; key.wPortB = wDstPort;
		}
		else
		{
			key.dwIpA = dwDstIP; key.wPortA = wDstPort;
			key.dwIpB = dwSrcIP; key.wPortB = wSrcPort;
		}
	}

	std::lock_guard<std::mutex> lock(m_streamMutex);

	StreamBuffer& sb = m_streams[key];
	sb.buf.insert(sb.buf.end(), payload, payload + len);

	while (true)
	{
		int nAvail = (int)sb.buf.size();
		int nSize = PacketDecoder::PeekTcpPacketSize(sb.buf.data(), nAvail);
		if (nSize == 0)
			break;

		if (nSize < 0)
		{
			m_dwDroppedTcp++;
			sb.buf.clear();
			break;
		}

		bool bSrcIsServer = false, bDirKnown = false;
		DetermineDirection(dwSrcIP, wSrcPort, dwDstIP, wDstPort, bSrcIsServer, bDirKnown);

		DecodedPacket dp;
		if (PacketDecoder::DecodeRaw(sb.buf.data(), nSize, false,
		                              dwSrcIP, wSrcPort, dwDstIP, wDstPort,
		                              !bSrcIsServer, bDirKnown, tm, 0, dp))
		{
			if (m_bStrictCS.load() && !dp.bChecksumValid)
				m_dwDroppedTcp++;
			else
				m_pStore->PushDecoded(dp);
		}
		else
		{
			m_dwDroppedTcp++;
		}

		sb.buf.erase(sb.buf.begin(), sb.buf.begin() + nSize);
	}

	if (sb.buf.size() > (LS_MAX_PACKET_SIZE * 4))
	{
		sb.buf.clear();
		m_dwDroppedTcp++;
	}
}

void CaptureEngine::OnUdpDatagram(DWORD dwSrcIP, WORD wSrcPort, DWORD dwDstIP, WORD wDstPort,
                                  const BYTE* data, int len, const SYSTEMTIME& tm)
{
	if (len < LS_PK_HEADER_SIZE)
	{
		m_dwDroppedUdp++;
		return;
	}

	bool bSrcIsServer = false, bDirKnown = false;
	DetermineDirection(dwSrcIP, wSrcPort, dwDstIP, wDstPort, bSrcIsServer, bDirKnown);

	DWORD dwTotal = ReadDwordLE(data + 4);
	if (dwTotal < LS_PK_HEADER_SIZE || (int)dwTotal > len || dwTotal > LS_MAX_PACKET_SIZE)
	{
		m_dwDroppedUdp++;
		return;
	}

	DecodedPacket dp;
	if (PacketDecoder::DecodeRaw(data, (int)dwTotal, true,
	                              dwSrcIP, wSrcPort, dwDstIP, wDstPort,
	                              !bSrcIsServer, bDirKnown, tm, 0, dp))
	{
		if (m_bStrictCS.load() && !dp.bChecksumValid)
			m_dwDroppedUdp++;
		else
			m_pStore->PushDecoded(dp);
	}
	else
	{
		m_dwDroppedUdp++;
	}
}
