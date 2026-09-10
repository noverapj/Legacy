#pragma once


// ==== move types

const char MOVETYPE_WALK = 'L';
const char MOVETYPE_JUMP = 'J';
const char MOVETYPE_JUMP2 = '2';

/*
struct Vec3
{
	float x,y,z;

	Vec3()									 {}
	Vec3( float _x, float _y, float _z )	 { x = _x, y = _y, z = _z; }
	Vec3( const Vec3& a )					 { x = a.x, y = a.y, z = a.z; }
	Vec3& operator = ( const Vec3& a )		 { x = a.x, y = a.y, z = a.z; return *this; }
	bool operator == ( const Vec3& a ) const { return x == a.x && y == a.y && z == a.z; }
};
*/


typedef D3DXVECTOR3 Vec3;


struct BoxPath
{
	// set by BoxSpace::findBoxPath() & BoxSpace::getNextWaypoint()

	std::vector< short > path;
	short index;
	bool bInBox;

	// optional parameters. set by user.

	float margin;			// stop margin
	float jumpH, jumpV;		// jump curve parameters
	bool  useJump;			// 

	BoxPath()	{ margin = 0; jumpH = 0; jumpV = 1; useJump = true; }
};



class BoxSpace
{
public:

	BoxSpace()		{ m_astar.setInterface( this ); m_bFixed = false; }
	~BoxSpace()		{}

	//

	int		findBoxPath( const Vec3& posFrom, const Vec3& posTo, BoxPath& outPath );
	bool	getNextWaypoint( BoxPath& path, const Vec3& posCur, const Vec3& posGoal, Vec3& outWaypoint, char& outType ) const;
	bool	ls_getNextWaypoint( BoxPath& path, const Vec3& posCur, const Vec3& posGoal, Vec3& outWaypoint, char& outType ) const;

	int		findBox( const Vec3& pos ) const; 
	int		findNearestBox( const Vec3& pos ) const; 

	// build methods

	void	clear();

	int		addBox( const char* pName, const Vec3& posMin, const Vec3& posMax );
	bool	addLink( short boxFrom, short boxTo, char dir, char linkType );

	bool	build();

	// 

	struct Link 
	{
		short toBox;
		char dir;
		char type;

		float cost;
		Vec3 posFrom, posTo;
	};

	struct Box
	{
		std::string name;
		Vec3 posMin, posMax;
		std::vector< Link > links;
		short walkCount;

		Vec3 posCenter;
		void* pAStarInfo;
	};

	const Box* getBoxData( short index ) const		{ return &m_boxes[ index ]; }
	int	GetBoxSize() {return m_boxes.size();}

	int getLastTraverseCount() const				{ return m_astar.getLastTraverseCount(); }

private:

	char getLinkType( short from, short to ) const;

	std::vector< Box > m_boxes;

	friend class AStarSearcher< BoxSpace, Box* >;
	mutable AStarSearcher< BoxSpace, Box* > m_astar;

	bool m_bFixed;
	bool m_bUseJump;

	// A* interface

	Box* m_pGoal;

	inline int	 getNeighborCount( void* pCaller, Box* pNode );
	inline float getNeighborAt( void* pCaller, Box* pNode, int i, Box*& pOutNeighbor );
	inline float getHeuristic( void* pCaller, Box* pNode );
	inline bool  isGoal( void* pCaller, Box* pNode )									{ return pNode == m_pGoal; }
	inline void  setInfo( void* pCaller, Box* pNode, void* ptr )						{ pNode->pAStarInfo = ptr; }
	inline void* getInfo( void* pCaller, Box* pNode )									{ return pNode->pAStarInfo; }
};	

