#pragma once

class GrowthDownDoneWnd : public ioWnd
{
public: 
	enum 
	{
		ID_GO	= 1,
		ID_USE	= 2,
	};

protected:
	int m_iClassType;
	int m_iReturnPoint;

	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pPointImg;

	ioComplexStringPrinter m_Desc[2];

public:
	void ShowGrowthDownDone( int iClassType, int iReturnPoint );

protected:
	void ReBatchButton();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	GrowthDownDoneWnd(void);
	virtual ~GrowthDownDoneWnd(void);
};
//////////////////////////////////////////////////////////////////////////////////////
class GrowthAllDownDoneWnd : public ioWnd
{
public: 
	enum 
	{
		ID_GO	= 1,
		ID_USE	= 2,
	};

	enum 
	{
		MAX_DESC = 6,
	};

protected:
	int m_iClassType;
	int m_iReturnPoint;
	__int64 m_iReturnPeso;

	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pBackEffect;

	ioComplexStringPrinter m_Desc[MAX_DESC];

public:
	void ShowGrowthAllDownDone( int iClassType, int iReturnPoint, __int64 iReturnPeso );

protected:
	void ReBatchButton();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	GrowthAllDownDoneWnd(void);
	virtual ~GrowthAllDownDoneWnd(void);
};