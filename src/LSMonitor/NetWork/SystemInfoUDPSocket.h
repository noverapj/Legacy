#pragma once

#define UDPSOCKET_PORT		    27000
#define WM_UDPSOCKET     	    (WM_USER +  200)

class SystemInfoUDPSocket : public CUDPSocket
{
public:
	SystemInfoUDPSocket(void);
	void EvaluateSocket(WPARAM wParam, LPARAM lParam);
	bool UDPReceiveProcess(WPARAM wParam);
};

