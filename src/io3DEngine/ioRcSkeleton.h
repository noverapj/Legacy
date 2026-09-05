

#ifndef _ioRcSkeleton_h_
#define _ioRcSkeleton_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioResource.h"
#include "ioRcBiped.h"

#include "ioVectorIterator.h"

class __EX ioRcSkeleton : public ioResource
{
protected:
	typedef std::vector< ioRcBiped > ioRcBipedVec;
	ioRcBipedVec m_vRcBipedList;

	ioRcSkeleton *m_pLinkedSkeleton;

public:
	void AllocBipedList( int iCount );
	void ClearBipedList();

public:
	ioRcBiped* GetRcBiped( const ioHashString &name );
	ioRcBiped* GetRcBiped( int iBipedID );
	inline int GetRcBipedCnt() const { return m_vRcBipedList.size(); }

public:
	void SetLinkedSkeleton( const ioHashString &rkName );
	void ReleaseLinkedSkeleton();

	inline ioRcSkeleton* GetLinkedSkeleton() const { return m_pLinkedSkeleton; }
	inline bool IsLinkedSkeleton() const { return ( m_pLinkedSkeleton != NULL ); }

public:
	ioRcSkeleton( const ioHashString& name );
	virtual ~ioRcSkeleton();
};

#endif

