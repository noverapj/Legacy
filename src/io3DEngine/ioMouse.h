
#ifndef _ioMouse_h_
#define _ioMouse_h_

class ioDragItem;

class __EX ioMouse
{
public:
	enum MouseState
	{
		MS_NORMAL,
		MS_OVER,
		MS_CLICK,
		MS_BUSY,
		MS_DROP_ENABLE,
		MS_DROP_DISABLE,
	};

protected:
	MouseState	m_MouseState;
	POINT	m_ptMousePos;

	ioDragItem  *m_pDragItem;
	POINT	m_ptDragOffset;

	bool	m_bLBtnPress;
	bool	m_bLBtnDown;
	bool	m_bLBtnUp;
	bool	m_bLBtnDBLCLK;

	bool	m_bMBtnPress;
	bool	m_bMBtnDown;
	bool	m_bMBtnUp;
	bool	m_bMBtnDBLCLK;

	bool	m_bRBtnPress;
	bool	m_bRBtnDown;
	bool	m_bRBtnUp;
	bool	m_bRBtnDBLCLK;

	bool	m_bMouseMoved;

	int     m_iWheelDelta;

protected:
	static int m_iMaxWidth;
	static int m_iMaxHeight;

protected:
	int m_iCurrScreenWidth;
	int m_iCurrScreenHeight;
protected:
	DWORD	m_dwMouseBusyStartTime;
	DWORD	m_dwPacketID;

	bool	m_bCompulsionUnMouseBusy;

public:
	static void SetMaxWindowSize( int iWidth, int iHeight );

public:
	void SetCurrScreenSize( int iWidth, int iHeight );

public:
	void SetMousePos( int xPos, int yPos );
	void ClearCheckOnlyOnceStatus();
	void ClearAllBtnStatus();

public:
	void SetLBtnDown( int xPos, int yPos );
	void SetLBtnUp( int xPos, int yPos );
	void SetLBtnDBLCLK( int xPos, int yPos );

	void SetMBtnDown( int xPos, int yPos );
	void SetMBtnUp( int xPos, int yPos );
	void SetMBtnDBLCLK( int xPos, int yPos );

	void SetRBtnDown( int xPos, int yPos );
	void SetRBtnUp( int xPos, int yPos );
	void SetRBtnDBLCLK( int xPos, int yPos );

	void SetMouseMove( int xPos, int yPos );
	void SetMouseWheel( int xPos, int yPos, int zDelta );

public:
	void SetMouseState( MouseState eState );

	void SetBusy();
	void SetUnBusy();

	virtual void SetPacketID( DWORD dwPacketID );

	bool IsBusy() const;

	void SetCompulsionUnMouseBusy( bool bUnMouseBusy ){ m_bCompulsionUnMouseBusy = bUnMouseBusy; }
	bool GetCompulsionUnMouseBusy(){ return m_bCompulsionUnMouseBusy; }

	DWORD GetPacketID(){ return m_dwPacketID; }

	virtual void SetDragDropStart( ioDragItem *pDragItem, const POINT &ptOffset );
	virtual void ClearDragDropState();

	virtual void ProcessMouse();

public:
	inline bool IsLBtnPress() const { return m_bLBtnPress; }
	inline bool IsLBtnDown() const { return m_bLBtnDown; }
	inline bool IsLBtnUp()	const { return m_bLBtnUp; }
	inline bool IsLBtnDBLCLK() const { return m_bLBtnDBLCLK; }

	inline bool IsMBtnPress() const { return m_bMBtnPress; }
	inline bool IsMBtnDown() const { return m_bMBtnDown; }
	inline bool IsMBtnUp()	const { return m_bMBtnUp; }
	inline bool IsMBtnDBLCLK() const { return m_bMBtnDBLCLK; }

	inline bool IsRBtnPress() const { return m_bRBtnPress; }
	inline bool IsRBtnDown() const { return m_bRBtnDown; }
	inline bool IsRBtnUp() const { return m_bRBtnUp; }
	inline bool IsRBtnDBLCLK() const { return m_bRBtnDBLCLK; }

	inline bool IsMoved() const { return m_bMouseMoved; }
	inline int  GetWheelDelta() const { return m_iWheelDelta; }

	inline const POINT& GetMousePos() const { return m_ptMousePos; }
	inline MouseState GetMouseState() const { return m_MouseState; }

	inline int GetCurrScreenWidth()  const { return m_iCurrScreenWidth; }
	inline int GetCurrScrrenHeight() const { return m_iCurrScreenHeight; }

	// Draw관련 함수는 상속을 해서 만들기 바람.

public:
	ioMouse();
	virtual ~ioMouse();
};

#endif