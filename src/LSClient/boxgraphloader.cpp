#include "StdAfx.h"
#include "uniheap.h"
#include "priorityq.h"
#include "astar.h"
#include "boxgraph.h"
#include "boxgraphloader.h"


static bool LoadBoxSpace( BoxSpace& space, ioXMLDocument& doc )
{
	stdext::hash_map< std::string, int > nodemap;

	ioXMLElement* pNodes = &doc.GetRootElement().FirstChild("nodes");


	if ( pNodes == NULL )
		return false;

	space.clear();

	for ( ioXMLElement* pNode = &pNodes->FirstChild( "box" ); !pNode->IsEmpty(); pNode = &pNode->NextSibling("box")) 
	{
		const char* pName = pNode->GetStringAttribute_e( "name" ); 
		const char* pVMin = pNode->GetStringAttribute( "min" );
		const char* pVMax = pNode->GetStringAttribute( "max" );

		if ( pName == NULL || pVMin == NULL || pVMax == NULL )
			return false;

		Vec3 posMin, posMax;
		sscanf( pVMin, "%f,%f,%f", &posMin.x, &posMin.y, &posMin.z );
		sscanf( pVMax, "%f,%f,%f", &posMax.x, &posMax.y, &posMax.z );

		int index = space.addBox( pName, posMin, posMax );

		nodemap[ pName ] = index;
	}

	ioXMLElement* pLinks = &doc.GetRootElement().FirstChild( "links" );
	if ( pLinks == NULL )
		return false;

	for ( ioXMLElement* pLink = &pLinks->FirstChild( "link" ); !pLink->IsEmpty(); pLink = &pLink->NextSibling( "link" ) )
	{
		const char* pFrom = pLink->GetStringAttribute_e( "from" );
		const char* pTo = pLink->GetStringAttribute_e( "to" );
		const char* pType = pLink->GetStringAttribute_e( "type" );

		if ( pFrom == NULL || pTo == NULL || pType == NULL )
			return false;

		if ( nodemap.find( pFrom ) == nodemap.end() || nodemap.find( pTo ) == nodemap.end() )
			return false;

		int iFrom = nodemap[ pFrom ];
		int iTo = nodemap[ pTo ];

		space.addLink( iFrom, iTo, ' ', *pType );
	}

	space.build();

	return true;
}

bool LoadBoxSpaceFromFile( BoxSpace& space, const char* pFilename )
{
	ioBinaryStream kStream;

	if( !g_ResourceLoader.LoadStream( pFilename, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioUIFrameManager::LoadFile - %s Load Failed", pFilename );
		return false;
	}
	
	ioXMLDocument doc;

	if ( !doc.LoadFromMemory( (char*)kStream.GetPtr() ) )
		return false;

	return LoadBoxSpace( space, doc );
}

bool LoadBoxSpaceFromMem( BoxSpace& space, const void* pBuf, int size )
{
	ioXMLDocument doc;
	
	if ( !doc.LoadFromMemory( reinterpret_cast< const char* >( pBuf ) ))
		return false;

	return LoadBoxSpace( space, doc );
}
