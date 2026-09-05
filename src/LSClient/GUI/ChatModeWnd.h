#pragma once

class ioUIRenderImage;
class ChatModeWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT    = 1,
		ID_CHANGE  = 2,
		ID_CLOSE   = 3,
	};

public:
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ChatModeWnd();
	virtual ~ChatModeWnd();
};

