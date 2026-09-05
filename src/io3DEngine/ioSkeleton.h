
#ifndef _ioSkeleton_h_
#define _ioSkeleton_h_

class ioBiped;
class ioRcSkeleton;
class ioOrientBox;
class ioRopeSpringCurve;

struct SwingParam
{
	float m_fHSwingUP;
	float m_fHSwingDOWN;
	
	float m_fVSwingUP;
	float m_fVSwingDOWN;
};

typedef std::vector< SwingParam > SwingParamList;

struct RopeSpringParam
{
	float m_fGravity;
	float m_fBoxWeights;

	float m_fDamping;
	float m_fAngDamping;

	float m_fLengthFixRate;

	SwingParamList m_SwingList;
};

class __EX ioSkeleton
{
private:
	ioRcSkeleton	*m_pRcSkeleton;

	typedef std::vector< ioBiped* >	ioBipedVec;
	ioBipedVec m_vBipedList;

	typedef std::map< ioHashString, ioBiped* > BipedMap;
	BipedMap m_BipedSearchMap;	// °Ë»ö¿ë

private:
	typedef std::vector< ioRopeSpringCurve* > RopeSpringCurveList;
	RopeSpringCurveList m_SpringCurveList;

private:
	void GenerateBipedList();
	void ClearBipedList();

public:
	void UpdateSkeleton( float fTime );

	void BlendBiped( int iBipedID,
					 float fWeight,
					 const D3DXVECTOR3& vTrans,
					 const D3DXQUATERNION& qtRot );

	void ClearBlend();
	void LockBlend();

public:
	void SetExtraRotate( const ioHashString &kBipedName, const D3DXQUATERNION &qtRot, bool bLocal );
	void ReleaseExtraRotate( const ioHashString &kBipedName );
	void ReleaseAllExtraRotate();

public:
	bool HasRopeSpring() const;
	bool AddRopeSpringCurve( const ioHashString &rkBiped, const RopeSpringParam *pParam );
	bool RemoveRopeSpringCurve( const ioHashString &rkBiped );
	void ClearAllRopeSpringCurve();

	void AddWorldForce( const D3DXVECTOR3 &vForce );

private:
	void UpdateRopeSpringCurve( float fTime );

public:
	ioBiped* CreateBiped();
	ioBiped* CreateBiped( const ioHashString &kName );

public:
	const ioHashString& GetName() const;

	ioBiped* GetBiped( int id ) const;
	ioBiped* GetBiped( const ioHashString &name ) const;

	inline int GetBipedCnt() const { return m_vBipedList.size(); }
	int GetBipedIndex( const ioHashString &name ) const;

public:
	bool IsLinkedSkeleton() const;

public:
	void DebugRopeCollisionBox( std::vector< ioOrientBox > &vBoxList );

public:
	ioSkeleton( ioRcSkeleton *pRc );
	~ioSkeleton();
};

#endif
