#pragma once

class CServerInfo
{
public:
	CServerInfo()
	{
	}

public:
	CString m_name;
	CString m_IP;
	UINT m_port;
};

const UINT MAX_TRANSFER_SIZE = 1024 * 50;