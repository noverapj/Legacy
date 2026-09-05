

#ifndef _ioMeshControlPoint_h_
#define _ioMeshControlPoint_h_

#include "io3DCommon.h"

class __EX ioMeshControlPoint
{
protected:
	ioHashString m_Type;
	int m_iIndex;

	ioHashString m_LinkedBiped;
	ioHashString m_ExtraInfo;
	
	D3DXVECTOR3 m_vPoint;

public:
	void SetTypeIndex( const char *szBuf );
	void SetTypeIndex( const char *szType, int iIndex );

	void SetLinkedBipedName( const char *szName );
	void SetExtraInfo( const char *szInfo );

	void SetPoint( const D3DXVECTOR3 &vPoint );

public:
	inline const ioHashString& GetType() const { return m_Type; }
	inline int GetIndex() const { return m_iIndex; }

	inline const ioHashString& GetLinkedBipedName() const { return m_LinkedBiped; }
	inline const ioHashString& GetExtraInfo() const { return m_ExtraInfo; }

	inline const D3DXVECTOR3& GetPoint() const { return m_vPoint; }

public:
	ioMeshControlPoint();
	virtual ~ioMeshControlPoint();
};

class CPSort : public std::binary_function< const ioMeshControlPoint*,
											const ioMeshControlPoint*,
											bool >
{
public:
	bool operator()( const ioMeshControlPoint *pLeft,
					 const ioMeshControlPoint *pRight ) const
	{
		if( pLeft->GetType() < pRight->GetType() )
			return true;
		else if( pLeft->GetType() == pRight->GetType() )
			return pLeft->GetIndex() < pRight->GetIndex();

		return false;
	}
};

typedef std::set< ioMeshControlPoint*, CPSort > CtrlPointSet;
typedef std::list< const ioMeshControlPoint* > ConstCtrlPointList;

#endif
