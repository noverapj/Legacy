#pragma once

class ioWnd;

class ShuffleRankWnd : public ioWnd
{
protected:
	struct Rank
	{
		ioHashString szName;
		int iCurrStar;
		int iTeamType;

		Rank()
		{
			iCurrStar = 0;
			iTeamType = 0;
		}
	};
	typedef std::vector<Rank> vRank;

	class RankOrder : public std::binary_function< const Rank&, const Rank&, bool >
	{
	public:
		bool operator()( const Rank &lhs , const Rank &rhs ) const
		{
			if( lhs.iCurrStar > rhs.iCurrStar )
				return true;

			return false;
		}
	};

protected:
	enum
	{
		STATE_DELAY   = 1,
		STATE_FADEIN  = 2,
		STATE_VIEW    = 3,
		STATE_FADEOUT = 4,
	};

	enum
	{
		TCT_EX_YEELOW = D3DCOLOR_XRGB( 253, 193, 3 ),
		TCT_EX_RICE   = D3DCOLOR_XRGB( 255, 239, 205 ),
		TCT_EX_BROWN  = D3DCOLOR_XRGB( 136, 98, 17 ),
		TCT_EX_BLUE   = D3DCOLOR_XRGB( 1, 122, 201 ),
	};

protected:
	vRank m_vRank;

	DWORD m_dwState;	
	DWORD m_dwCurrentTime;
	DWORD m_dwFadeTime;

	int m_iHieght;
	int m_iAlphaRate;
	bool m_bSort;
	
protected:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	
protected:
	void SetState( DWORD dwState );
	D3DCOLOR GetRankColor( int iRank );

public:
	void ClearRank();
	void AddRankUser( const ioHashString& szName, int iTeamType, int iCurrStar );
	void Show();
	void Hide();

protected:
	virtual void OnRender();

	void OnRankRender( int iXPos, int iYPos, int iTeamType, ioHashString szName, int iRank, int iStar, bool bBottom );

public:
	ShuffleRankWnd();
	virtual ~ShuffleRankWnd();	
};