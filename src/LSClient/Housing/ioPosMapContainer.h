#pragma once 

template <typename T>
class ioPosMapContainer
{
public:
	virtual void Destroy(){}
	virtual void Init(){}
	virtual void AddElement( T pEntity, TilePos Pos ){}
	virtual void AddElement( T pEntity, int Index ){}
	virtual T GetElement( TilePos Pos ){ return NULL; }
	virtual T GetElement( int Index ){ return NULL; }
	virtual void RemoveElement( int Index ){}
	virtual int GetMax(){ return 0; }
};

enum PosMapConinerSize
{
	PMCS_32 = 32,
	PMCS_64 = 64,
};

template <typename T>
class ioPos32MapContainer : public ioPosMapContainer<T>
{
protected:
	T m_PosMap[PMCS_32 * PMCS_32];

public:
	virtual void Destroy()
	{
		for( int nZ = 0; nZ < PMCS_32; ++nZ )
		{
			for( int nX = 0; nX < PMCS_32; ++nX )
			{	
				SAFEDELETE( m_PosMap[nX + nZ * PMCS_32] );
			}
		}
	}

	virtual void Init()
	{
		for( int nZ = 0; nZ < PMCS_32; ++nZ )
		{
			for( int nX = 0; nX < PMCS_32; ++nX )
			{	
				m_PosMap[nX + nZ * PMCS_32] = NULL;
			}
		}
	}

public:
	virtual void AddElement( T pEntity, TilePos Pos )
	{
		AddElement( pEntity, Pos.x + Pos.z * PMCS_32 );
	}

	virtual void AddElement( T pEntity, int Index )
	{
		if( COMPARE( Index, 0, GetMax() ) )
			m_PosMap[Index] = pEntity;
	}

	virtual T GetElement( TilePos Pos )
	{
		return GetElement( Pos.x + Pos.z * PMCS_32 );
	}

	virtual T GetElement( int Index )
	{
		if( COMPARE( Index, 0, GetMax() ) )
		{
			return m_PosMap[Index];
		}

		return NULL;
	}

	virtual void RemoveElement( int Index )
	{
		if( COMPARE( Index, 0, GetMax() ) )
		{
			m_PosMap[Index] = NULL;
		}
	}

	virtual int GetMax(){ return PMCS_32 * PMCS_32; }

public:
	ioPos32MapContainer(){ Init(); }
	virtual ~ioPos32MapContainer(){}
};

template <typename T>
class ioPos64MapContainer : public ioPosMapContainer<T>
{
protected:
	T m_PosMap[PMCS_64 * PMCS_64];

public:
	virtual void Destroy()
	{
		for( int nZ = 0; nZ < PMCS_64; ++nZ )
		{
			for( int nX = 0; nX < PMCS_64; ++nX )
			{	
				SAFEDELETE( m_PosMap[nX + nZ * PMCS_64] );
			}
		}
	}

	virtual void Init()
	{
		for( int nZ = 0; nZ < PMCS_64; ++nZ )
		{
			for( int nX = 0; nX < PMCS_64; ++nX )
			{	
				m_PosMap[nX + nZ * PMCS_64] = NULL;
			}
		}
	}

public:
	virtual void AddElement( T pEntity, TilePos Pos )
	{
		AddElement( pEntity, Pos.x + Pos.z * PMCS_64 );
	}

	virtual void AddElement( T pEntity, int Index )
	{
		if( COMPARE( Index, 0, GetMax() ) )
			m_PosMap[Index] = pEntity;
	}

	virtual T GetElement( TilePos Pos )
	{
		return GetElement( Pos.x + Pos.z * PMCS_64 );
	}

	virtual T GetElement( int Index )
	{
		if( COMPARE( Index, 0, GetMax() ) )
		{
			return m_PosMap[Index];
		}

		return NULL;
	}

	virtual void RemoveElement( int Index )
	{
		if( COMPARE( Index, 0, GetMax() ) )
		{
			m_PosMap[Index] = NULL;
		}
	}

	virtual int GetMax(){ return PMCS_64 * PMCS_64; }

public:
	ioPos64MapContainer(){ Init(); }
	virtual ~ioPos64MapContainer(){}
};
