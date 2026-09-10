#pragma once



/*
	struct INTF_example
	{
		int	 getNeighborCount( void* pCaller, const Node& node )							{ return 0; }
		float getNeighborAt( void* pCaller, const Node& node, int i, Node& outNeighbor )	{ return 0; }

		float getHeuristic( void* pCaller, const Node& node )								{ return 0; }
		bool isGoal( void* pCaller, const Node& node )										{ return true; }

		void  setInfo( void* pCaller, const Node& node, void* ptr )							{}
		void* getInfo( void* pCaller, const Node& node )									{ return NULL; }
	};
*/

template< typename INTF, typename Node >
class AStarSearcher
{
public:

	AStarSearcher( INTF* pIntf = NULL ) : m_openList( DNodeCostGreater() ), m_pIntf( pIntf ) {}
	~AStarSearcher() { clear(); }

	void setInterface( INTF* pIntf )
	{
		m_pIntf = pIntf;
	}

	bool findPath( const Node& start )	
	{ 
		clear();
		addStartNode( start, 0 );
		return doFindPath(); 
	}

	void clear()
	{
		m_pool.clear();
		m_openList.clear();
		m_resultPath.clear();
	}

	void addStartNode( const Node& start, float initialCost )
	{
		DNode* dnStart = getDN( start );
		dnStart->cost = initialCost;
		dnStart->total = dnStart->cost + m_pIntf->getHeuristic( this, start );
		dnStart->parent = NULL;
		dnStart->bOpen = true;
		m_openList.push( dnStart );
	}

	bool findPath()
	{
		return doFindPath();
	}

	int   getPathSize()							{ return int(m_resultPath.size()); }
	const Node& getPath( int i )				{ return *m_resultPath[ m_resultPath.size() - 1 - i ]; }

	int	  getLastTraverseCount()				{ return int(m_pool.get_unit_count()); }

private:

	INTF* m_pIntf;

	struct DNode
	{
		Node node;

		DNode* parent;
		float cost;
		float total;
		bool bOpen;
		bool bClose;

		DNode() { 
			parent = NULL; 
			cost = total = 0; 
			bOpen = bClose = false; 
		}
	};

	struct DNodeCostGreater {
		bool operator () ( const DNode* p, const DNode* q ) const		{ return p->total > q->total; }
	};

	void clearInfo()
	{
		for ( DNode* p = m_pool.get_first_unit(); p != NULL; p = m_pool.get_next_unit() )
			m_pIntf->setInfo( this, p->node, NULL );
	}

	bool doFindPath()
	{
		while ( !m_openList.empty() )
		{
			DNode* dn;
			m_openList.pop( dn );
			dn->bOpen = false;

			if ( m_pIntf->isGoal( this, dn->node ) ) {
				
				clearInfo();
				constructPath( dn ); 
				return true;

			} else {

				for ( int i = 0, n = m_pIntf->getNeighborCount( this, dn->node ); i < n; i++ )
				{
					Node nnode;
					float dist = m_pIntf->getNeighborAt( this, dn->node, i, nnode );
					DNode* dnn = getDN( nnode );

					float newCost = dn->cost + dist;
					if ( (dnn->bOpen || dnn->bClose) && (dnn->cost <= newCost) )
						continue;
					
					dnn->parent = dn;
					dnn->cost = newCost;
					dnn->total = dnn->cost + m_pIntf->getHeuristic( this, dnn->node );

					dnn->bClose = false;
					if ( dnn->bOpen )
						m_openList.adjust( dnn );
					else {
						dnn->bOpen = true;
						m_openList.push( dnn );
					}
				}
			}

			dn->bClose = true;
		}

		clearInfo();
		return false;
	}

	void constructPath( DNode* dn )
	{
		for (;; dn = dn->parent )
		{
			m_resultPath.push_back( &dn->node );
			if ( dn->parent == NULL )
				break;
		}
	}

	DNode* getDN( const Node& node )
	{
		DNode* pDN = reinterpret_cast< DNode* >( m_pIntf->getInfo( this, node ) );
		if ( pDN == NULL )
		{
			pDN = m_pool.alloc();
			pDN->node = node;
			m_pIntf->setInfo( this, node, pDN );
		}
		return pDN;
	}

	obj_heap< DNode > m_pool;
	PriorityQueue< DNode*, DNodeCostGreater > m_openList;

	std::vector< const Node* > m_resultPath;
};
