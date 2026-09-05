

#ifndef _ioEnumDisplayMode_h_
#define _ioEnumDisplayMode_h_

class ioEnumDisplayMode
{
public:
	struct MonitorMode
	{
		UINT uWidth;
		UINT uHeight;
		UINT uPixel;

		typedef std::vector<UINT> RefreshList;
		RefreshList vRefresh;
	};

	struct AdapterMode
	{
		UINT uWidth;
		UINT uHeight;

		bool operator<( const AdapterMode &rhs ) const
		{
			if( uWidth < rhs.uWidth )
				return true;
			else if( uWidth == rhs.uHeight )
			{
				if( uHeight < rhs.uHeight )
					return true;
			}

			return false;
		}
	};

	enum
	{
		SAFE_REFRESH = 60,
		MIN_HEIGHT = 480,
		MIN_WIDTH = 640,
	};

protected:
	typedef std::vector< MonitorMode* > MonitorModeList;
	MonitorModeList m_MonitorModeList;

	typedef std::vector< AdapterMode > AdapterModeList;
	AdapterModeList m_16BitsAdapters;
	AdapterModeList m_32BitsAtapters;

	UINT m_uMinWidth;
	UINT m_uMinHeight;
	UINT m_uMaxWidth;
	UINT m_uMaxHeight;

	UINT m_uStartRefresh;

public:
	void SetMinWidthHeight( UINT uMinWidth, UINT uMinHeight );
	void CheckEnableDisplayMode( IDirect3D9 *pD3D, D3DFORMAT e16Bit, D3DFORMAT e32Bit );

protected:
	void ClearAllEnumInfo();

	void CheckCurrentDisplayMode();
	void CheckMonitorDisplayMode();
	void InsertMonitorMode( const DEVMODE &rkMode );

	void CheckAdapterDisplayMode( IDirect3D9 *pD3D, D3DFORMAT eFormat );
	bool IsMonitorEnableMode( const D3DDISPLAYMODE &rkMode ) const;

	AdapterModeList* GetAdapterModeListByFormat( D3DFORMAT eFormat );

public:
	int  GetNumDisplayMode( D3DFORMAT eFormat );
	bool GetResolution( D3DFORMAT eFormat, int iIndex, UINT &uWidth, UINT &uHeight );
	UINT GetRefresh( D3DFORMAT eFormat, UINT uWidth, UINT uHeight );
	UINT GetStartRefresh() const { return m_uStartRefresh; }

public:
	ioEnumDisplayMode();
	~ioEnumDisplayMode();
};

#endif