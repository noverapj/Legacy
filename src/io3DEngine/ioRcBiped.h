

#ifndef _ioRcBiped_h_
#define _ioRcBiped_h_

#include "io3DCommon.h"

#define ROOT_NODE	"NoParent"

class ioRcSkeleton;

class __EX ioRcBiped
{
private:
	D3DXVECTOR3		m_vTrans;
	D3DXQUATERNION	m_qtRotation;
	D3DXMATRIX		m_BipedSpaceTM;

	D3DXMATRIX		m_ObjectTM;
	D3DXMATRIX		m_ObjectOffsetTM;
	float			m_fLinkOffsetRate;

	ioHashString	m_Name;
	ioHashString	m_ParentName;
	ioHashStringVec	m_vChildList;

public:
	void SetName( const char *szName );
	void SetParentName( const char *szName );
	void SetTranslation( const D3DXVECTOR3 &vTrans );
	void SetRotation( const D3DXQUATERNION &qtRot );
	void SetBipedSpaceTM( const D3DXMATRIX &kBipedSpaceTM );
	void SetObjectTM( const D3DXMATRIX &rkObjectTM );

	void MakeOffsetTM( const ioRcBiped *pLinked );

public:
	void ReserveChildCount( int iCount );
	void AddChild( const ioHashString &name );

public:
	inline const D3DXVECTOR3& GetTranslation() const { return m_vTrans; }
	inline const D3DXQUATERNION& GetRotation() const { return m_qtRotation; }
	inline const D3DXMATRIX& GetBipedSpaceTM() const { return m_BipedSpaceTM; }

	inline const D3DXMATRIX& GetObjectTM() const { return m_ObjectTM; }
	inline const D3DXMATRIX& GetObjectOffsetTM() const { return m_ObjectOffsetTM; }

	inline float GetLinkOffsetRate() const { return m_fLinkOffsetRate; }

	inline const ioHashString& GetName() const { return m_Name; }
	inline const ioHashString& GetParentName() const { return m_ParentName; }
	inline int GetChildCnt() const { return m_vChildList.size(); }

	const ioHashString& GetChildName( int id ) const;

public:
	ioRcBiped();
	ioRcBiped( const ioHashString &name );
	~ioRcBiped();
};

#endif
