#include "StdAfx.h"

#include "uniheap.h"
#include "priorityq.h"
#include "astar.h"
#include "boxgraph.h"
#include "boxgraphloader.h"

static inline float getXYDist( const Vec3& p, const Vec3& q )
{
	float dx = abs( p.x - q.x ), dy = abs( p.y - q.y );
	if ( dx > dy ) 
		return dy*1.414f + (dx-dy);
	else
		return dx*1.414f + (dy-dx);
}

static inline float getDist( const Vec3& p, const Vec3& q, float heightFactor )
{
	return abs(p.z - q.z) * heightFactor + getXYDist( p, q );
}

static inline float getDist( const Vec3& pos, const BoxSpace::Box& box, float heightFactor )
{
	float dx = 0, dy = 0, dz = 0;

	if ( pos.x < box.posMin.x )
		dx = box.posMin.x - pos.x;
	else if ( box.posMax.x < pos.x )
		dx = pos.x - box.posMax.x;

	if ( pos.y < box.posMin.y )
		dy = box.posMin.y - pos.y;
	else if ( box.posMax.y < pos.y )
		dy = pos.y - box.posMax.y;

	if ( pos.z < box.posMin.z )
		dz = box.posMin.z - pos.z;
	else if ( box.posMax.z < pos.z )
		dz = pos.z - box.posMax.z;

	if ( dx > dy )
		return dy * 1.414f + (dx-dy) + dz * heightFactor;
	else
		return dx * 1.414f + (dy-dx) + dz * heightFactor;
}

static inline float max2( float a, float b ) 
{ 
	return a > b ? a : b; 
}

static inline float min2( float a, float b ) 
{ 
	return a < b ? a : b; 
}

static inline bool insideBox( const Vec3& pos, const Vec3& boxMin, const Vec3& boxMax )
{
	if ( pos.x < boxMin.x || pos.x > boxMax.x ) 
		return false;
	if ( pos.y < boxMin.y || pos.y > boxMax.y ) 
		return false;
	if ( pos.z < boxMin.z || pos.z > boxMax.z ) 
		return false;

	return true;
}

static inline float calcTransferCost( const Vec3& from, const Vec3& to, char moveType )
{
	if ( moveType == MOVETYPE_JUMP ) {
		return getXYDist( from, to ) + 50;

	} else if ( moveType == MOVETYPE_JUMP2 ) {
		return getXYDist( from, to ) + 210;

	} else
		return getXYDist( from, to );
}

// =======================================================================


int BoxSpace::getNeighborCount( void* pCaller, Box* pNode )
{
	return m_bUseJump ? pNode->links.size() : pNode->walkCount;
}

float BoxSpace::getNeighborAt( void* pCaller, BoxSpace::Box* pNode, int i, BoxSpace::Box*& pOutNeighbor )	
{ 
	Link& link = pNode->links[ i ];
	pOutNeighbor = &m_boxes[ link.toBox ];
	return link.cost;
}

float BoxSpace::getHeuristic( void* pCaller, BoxSpace::Box* pNode )	
{ 
	return getDist( pNode->posCenter, m_pGoal->posCenter, 1 );
}

static void calcTransferPoints( const BoxSpace::Box& boxFrom, const BoxSpace::Box& boxTo, char dir, Vec3& outPosFrom, Vec3& outPosTo )
{
	if ( boxTo.posMax.x < boxFrom.posMin.x || boxFrom.posMax.x < boxTo.posMin.x ) {

		if ( boxTo.posMax.x < boxFrom.posMin.x )
			outPosFrom.x = boxFrom.posMin.x, outPosTo.x = boxTo.posMax.x;
		else
			outPosFrom.x = boxFrom.posMax.x, outPosTo.x = boxTo.posMin.x;

		outPosFrom.y = outPosTo.y = (min2( boxFrom.posMax.y, boxTo.posMax.y ) + max2( boxFrom.posMin.y, boxTo.posMin.y )) * FLOAT05;

	} else if ( boxTo.posMax.y < boxFrom.posMin.y || boxFrom.posMax.y < boxTo.posMin.y ) {

		if ( boxTo.posMax.y < boxFrom.posMin.y )
			outPosFrom.y = boxFrom.posMin.y, outPosTo.y = boxTo.posMax.y;
		else
			outPosFrom.y = boxFrom.posMax.y, outPosTo.y = boxTo.posMin.y;

		outPosFrom.x = outPosTo.x = (min2( boxFrom.posMax.x, boxTo.posMax.x ) + max2( boxFrom.posMin.x, boxTo.posMin.x )) * FLOAT05;

	} else if ( boxTo.posMax.z < boxFrom.posMin.y ) {

		// vertical fall

		switch ( dir ) {
			case 'x':

				outPosFrom.x = outPosTo.x = boxFrom.posMin.x;
				outPosFrom.y = outPosTo.y = (min2( boxFrom.posMax.y, boxTo.posMax.y ) + max2( boxFrom.posMin.y, boxTo.posMin.y )) * FLOAT05;
				break;

			case 'X':

				outPosFrom.x = outPosTo.x = boxFrom.posMax.x;
				outPosFrom.y = outPosTo.y = (min2( boxFrom.posMax.y, boxTo.posMax.y ) + max2( boxFrom.posMin.y, boxTo.posMin.y )) / 2;
				break;

			case 'y':

				outPosFrom.y = outPosTo.x = boxFrom.posMin.y;
				outPosFrom.x = outPosTo.y = (min2( boxFrom.posMax.x, boxTo.posMax.x ) + max2( boxFrom.posMin.x, boxTo.posMin.x )) / 2;
				break;

			case 'Y':

				outPosFrom.y = outPosTo.x = boxFrom.posMax.y;
				outPosFrom.x = outPosTo.y = (min2( boxFrom.posMax.x, boxTo.posMax.x ) + max2( boxFrom.posMin.x, boxTo.posMin.x )) / 2;
				break;

			default:
				assert(0 && "invalid dir");
				LOG.PrintTimeAndLog(0, "boxgraph invalid dir");
		}
	}

	outPosFrom.z = (boxFrom.posMax.z + boxFrom.posMin.z) / 2;
	outPosTo.z = (boxTo.posMax.z + boxTo.posMin.z) / 2;
}

struct Fn_WalkLess
{
	bool operator () ( const BoxSpace::Link& a, const BoxSpace::Link& b )
	{
		return a.type == MOVETYPE_WALK && b.type != MOVETYPE_WALK;
	}
};

bool BoxSpace::build()
{
	clear();

	for ( int i = 0, n = int(m_boxes.size()); i < n; i++ ) 
	{
		Box& box = m_boxes[ i ];
		box.pAStarInfo = NULL;

		box.walkCount = 0;
		std::stable_sort( box.links.begin(), box.links.end(), Fn_WalkLess() );

		for ( int il = 0, nl = int(box.links.size()); il < nl; il++ ) 
		{
			Link& link = box.links[ il ];
			Box& boxTo = m_boxes[ link.toBox ];

			calcTransferPoints( box, boxTo, link.dir, link.posFrom, link.posTo );

			link.cost = getXYDist( box.posCenter, link.posFrom ) + getXYDist( link.posTo, boxTo.posCenter ) 
					  + calcTransferCost( link.posFrom, link.posTo, link.type );

			if ( link.type == MOVETYPE_WALK )
				box.walkCount++;
		}
	}

	m_bFixed = true;
	return true;
}

void BoxSpace::clear()
{
	m_bFixed = false;
	m_astar.clear();
}

int BoxSpace::addBox( const char* pName, const Vec3& posMin, const Vec3& posMax )
{
	if ( m_bFixed )
		return -1;

	Box newBox;
	newBox.name = std::string( pName );
	newBox.posMin = posMin;
	newBox.posMax = posMax;
	newBox.posCenter.x = (posMin.x + posMax.x) / 2;
	newBox.posCenter.y = (posMin.y + posMax.y) / 2;
	newBox.posCenter.z = (posMin.z + posMax.z) / 2;
	m_boxes.push_back( newBox );
	return int(m_boxes.size())-1;
}

bool BoxSpace::addLink( short node, short linkBox, char dir, char linkType )
{
	if ( m_bFixed )
		return false;

	if ( node < 0 || node >= int(m_boxes.size()) || linkBox < 0 || linkBox >= int(m_boxes.size()) )
		return false;

	Box& nodeData = m_boxes[ node ];
	Link linkData;
	linkData.toBox = linkBox;
	linkData.dir = dir;
	linkData.type = linkType;

	nodeData.links.push_back( linkData );

	return true;
}

int BoxSpace::findBox( const Vec3& pos ) const
{
	// temporary

	for ( int i = 0, n = int(m_boxes.size()); i < n; i++ )
	{
		const Box& node = m_boxes[ i ];
		if ( insideBox( pos, node.posMin, node.posMax ) )
			return i;
	}
//	LOG.PrintTimeAndLog(0, "can't find %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
	return -1;
}

int BoxSpace::findNearestBox( const Vec3& pos ) const
{
	// temporary

	if ( m_boxes.size() == 0 )
	{
//		LOG.PrintTimeAndLog(0, "can't find nearestbox%.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
		return -1;
	}

	float minDist = getDist( pos, m_boxes.front(), 1 );
	int minBox = 0;

	for ( int i = 1, n = int(m_boxes.size()); i < n; i++ )
	{
		const Box& box = m_boxes[ i ];
		float dist = getDist( pos, box, 1 );
		if ( dist < minDist ) {
			minDist = dist;
			minBox = i;
			if ( dist == 0 )
				break;
		}
	}

	return minBox;
}

int	BoxSpace::findBoxPath( const Vec3& posFrom, const Vec3& posTo, BoxPath& outPath )
{
	m_bUseJump = outPath.useJump;

	short boxFrom = findBox( posFrom );
	outPath.bInBox = (boxFrom != -1);
	if ( boxFrom == -1 )
		boxFrom = findNearestBox( posFrom );

	short boxTo = findBox( posTo );
	if ( boxTo == -1 )
		boxTo = findNearestBox( posTo );
	
	outPath.path.clear();
	outPath.index = 0;

	if ( boxFrom == boxTo )
		return 0;

	if ( boxFrom == -1 || boxTo == -1 )
		return -1;

	m_pGoal = &m_boxes[ boxTo ];

	m_astar.clear();

	Box* pFrom = &m_boxes[ boxFrom ];
	if ( outPath.bInBox ) 
		for ( int i = 0, n = pFrom->links.size(); i < n; i++ ) {
			Link& link = pFrom->links[ i ];
			if ( !outPath.useJump && (link.type == MOVETYPE_JUMP || link.type == MOVETYPE_JUMP2) ) 
				continue;
			float cost = link.cost - getXYDist( pFrom->posCenter, link.posFrom ) + getXYDist( posFrom, link.posFrom );
			m_astar.addStartNode( &m_boxes[ link.toBox ], cost );
		}
	else 
		m_astar.addStartNode( pFrom, 0 );
	
	if ( !m_astar.findPath() )
		return -1;

	outPath.path.reserve( m_astar.getPathSize() + 2 );

	if ( outPath.bInBox )
		outPath.path.push_back( boxFrom );

	for ( int i = 0, n = m_astar.getPathSize(); i < n; i++ )
		outPath.path.push_back( m_astar.getPath( i ) - &m_boxes.front() );

	return int(outPath.path.size());
}

static inline void getNearestPointInBox( const Vec3& posFrom, const Vec3& posMin, const Vec3& posMax, Vec3& outPos, float xymargin )
{
	outPos = posFrom;
	float xmargin = min2( xymargin, (posMax.x - posMin.x) / 2 );
	float ymargin = min2( xymargin, (posMax.y - posMin.y) / 2 );

	if ( posFrom.x <= posMin.x + xmargin ) 
		outPos.x = posMin.x + xmargin; 
	else if ( posFrom.x >= posMax.x - xmargin ) 
		outPos.x = posMax.x - xmargin;

	if ( posFrom.y <= posMin.y + ymargin ) 
		outPos.y = posMin.y + ymargin; 
	else if ( posFrom.y >= posMax.y - ymargin ) 
		outPos.y = posMax.y - ymargin;

	if ( posFrom.z <= posMin.z ) 
		outPos.z = posMin.z; 
	else if ( posFrom.z >= posMax.z ) 
		outPos.z = posMax.z;
}

static inline void getJumpValues( float jumpCurveH, float jumpCurveV, float vdist, 
								  float& outDistTotal, float& outDistOver )
{
	float ff = sqrt( 1 - vdist / jumpCurveV );
	outDistOver = 2 * jumpCurveH * ff;
	outDistTotal = jumpCurveH * ( 1 - ff ) + outDistOver;
}

static inline void adjustForJump( const Vec3& pos, const Vec3& fromMin, const Vec3& fromMax, const Vec3& toMin, const Vec3& toMax, 
								  float jumpCurveH, float jumpCurveV, float xymargin,
								  Vec3& outPos )
{
	float xmargin = min2( xymargin, (fromMax.x - fromMin.x) / 2 ), ymargin = min2( xymargin, (fromMax.y - fromMin.y) / 2 );

	float heightDiff = (toMax.z + toMin.z) * FLOAT05 - (fromMax.z + fromMin.z) * FLOAT05;
	outPos = pos;

	if ( heightDiff <= 0 )
		return;

	float d_total, d_over;
	getJumpValues( jumpCurveH, jumpCurveV, heightDiff, d_total, d_over );

	if ( toMax.x <= fromMin.x ) {

		if ( d_over > toMax.x - toMin.x ) {
			if ( d_total > xmargin + fromMax.x - toMin.x )
				outPos.x = fromMax.x;
			else
				outPos.x = toMin.x + d_total + xmargin;
		}

	} else if ( fromMax.x <= toMin.x ) {

		if ( d_over > toMax.x - toMin.x ) {
			if ( d_total > xmargin + toMax.x - fromMin.x )
				outPos.x = fromMin.x;
			else
				outPos.x = toMax.x - d_total - xmargin;
		}

	} else if ( toMax.y <= fromMin.y ) {

		if ( d_over > toMax.y - toMin.y ) {
			if ( d_total > ymargin + fromMax.y - toMin.y )
				outPos.y = fromMax.y;
			else
				outPos.y = toMin.y + d_total + ymargin;
		}

	} else if ( fromMax.y <= toMin.y ) {

		if ( d_over > toMax.y - toMin.y ) {
			if ( d_total > ymargin + toMax.y - fromMin.y )
				outPos.y = fromMin.y;
			else
				outPos.y = toMax.y - d_total - ymargin;
		}
	} 
}

char BoxSpace::getLinkType( short from, short to ) const
{
	const Box& box = m_boxes[ from ];
	for ( std::vector< Link >::const_iterator it = box.links.begin(); it != box.links.end(); it++ )
		if ( (*it).toBox == to ) {
			return (*it).type;
			break;
		}
	return 0;
}

bool BoxSpace::getNextWaypoint( BoxPath& path, const Vec3& posCur, const Vec3& posGoal, Vec3& outWaypoint, char& outType ) const
{
	outType = MOVETYPE_WALK;

	if ( path.index == path.path.size() ) {
		outWaypoint = posGoal;
		return false;
	}

	if ( path.bInBox ) 
	{

		const Box& boxCur = m_boxes[ path.path[ path.index ] ];

		if ( path.index == path.path.size() - 1 ) 
		{

			getNearestPointInBox( posGoal, boxCur.posMin, boxCur.posMax, outWaypoint, path.margin );

			if ( insideBox( posGoal, boxCur.posMin, boxCur.posMax ) )
				return false;

		} 
		else 
		{

			short nextBoxIndex = path.path[ path.index+1 ];
			const Box& boxNext = m_boxes[ nextBoxIndex ];

			Vec3 p;
			getNearestPointInBox( posCur, boxNext.posMin, boxNext.posMax, p, path.margin );

			if ( getLinkType( path.path[ path.index ], nextBoxIndex ) == MOVETYPE_WALK ){

				// path straightening

				if ( path.index == path.path.size() - 2 ) 
				{
					float d1 = getXYDist( p, posGoal ) + getXYDist( p, posCur );
					getNearestPointInBox( posGoal, boxNext.posMin, boxNext.posMax, outWaypoint, path.margin );
					getNearestPointInBox( outWaypoint, boxCur.posMin, boxCur.posMax, outWaypoint, path.margin );
					float d2 = getXYDist( p, posGoal ) + getXYDist( p, posCur );
					if ( d1 < d2 ) 
						outWaypoint = p;
				} 
				else 
				{
					const Box& boxNN = m_boxes[ path.path[ path.index+2 ] ];
					getNearestPointInBox( p, boxNN.posMin, boxNN.posMax, p, path.margin );
					getNearestPointInBox( p, boxNext.posMin, boxNext.posMax, p, path.margin );
					getNearestPointInBox( p, boxCur.posMin, boxCur.posMax, outWaypoint, path.margin );
				}
			
			} else {

				getNearestPointInBox( p, boxCur.posMin, boxCur.posMax, outWaypoint, path.margin );

				// calc jump distance

				adjustForJump( outWaypoint, boxCur.posMin, boxCur.posMax, boxNext.posMin, boxNext.posMax, 
							   path.jumpH, path.jumpV, path.margin, outWaypoint );
			}
		}

		path.bInBox = false;
		path.index++;
	} 
	else 
	{

		short boxIndex = path.path[ path.index ];

		if ( path.index > 0 ) {
			outType = getLinkType( path.path[ path.index-1 ], boxIndex );
			assert( outType != 0 && "link is not found!!" );
			if( outType == 0)
				LOG.PrintTimeAndLog(0, "Link is not Found!!, %d - %d", path.path[path.index-1], boxIndex );
		}

		const Box& box = m_boxes[ boxIndex ];
		getNearestPointInBox( posCur, box.posMin, box.posMax, outWaypoint, path.margin );
		path.bInBox = true;
	}

	return true;
}

bool BoxSpace::ls_getNextWaypoint( BoxPath& path, const Vec3& posCur, const Vec3& posGoal, Vec3& outWaypoint, char& outType ) const
{
	outType = MOVETYPE_WALK;

	if ( path.index == path.path.size() ) {
		outWaypoint = posGoal;
		return false;
	}

	if ( true || path.bInBox ) 
	{

		const Box& boxCur = m_boxes[ path.path[ path.index ] ];

		if ( path.index == path.path.size() - 1 ) 
		{
			return false;

// 			getNearestPointInBox( posGoal, boxCur.posMin, boxCur.posMax, outWaypoint, path.margin );
// 
// 			if ( insideBox( posGoal, boxCur.posMin, boxCur.posMax ) )
// 				return false;

		} 
		else 
		{

			short nextBoxIndex = path.path[ path.index+1 ];
			const Box& boxNext = m_boxes[ nextBoxIndex ];

			outWaypoint.x = boxNext.posCenter.x;
			outWaypoint.y = boxNext.posCenter.y;
		}

		path.bInBox = false;
		path.index++;
	} 
	else 
	{

		short boxIndex = path.path[ path.index ];

		if ( path.index > 0 ) {
			outType = getLinkType( path.path[ path.index-1 ], boxIndex );
			assert( outType != 0 && "link is not found!!" );
			if( outType == 0)
				LOG.PrintTimeAndLog(0, "Link is not Found!!, %d - %d", path.path[path.index-1], boxIndex );
		}

		const Box& box = m_boxes[ boxIndex ];
//		getNearestPointInBox( posCur, box.posMin, box.posMax, outWaypoint, path.margin );

		outWaypoint.x = box.posCenter.x;
		outWaypoint.y = box.posCenter.y;


		path.bInBox = true;
	}

	return true;
}


