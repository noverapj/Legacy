

#ifndef _ioEmitPointGenerator_h_
#define _ioEmitPointGenerator_h_

#include "io3DCommon.h"
#include "ioStringInterface.h"
#include "ioRandomCreator.h"

class __EX ioEmitPointGenerator : public ioStringInterface
{
protected:
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vDir;

	mutable ioRandomCreator m_RandomCreator;

public:
	virtual void ParametersCopy( const ioEmitPointGenerator *pSrc );
	virtual void SetDefaultPosition( const D3DXVECTOR3 &vPos );
	virtual void SetDefaultDirection( const D3DXVECTOR3 &vDir );
	virtual D3DXVECTOR3 GetStartPosition() const;

	virtual void ScaleGenerateArea( float fScale );

public:
	void SetRandomSeed( unsigned int uSeed );
	unsigned int GetRandomSeed() const;

public:
	float UnitRandom();
	float RangeRandom( float fMin, float fMax );
	float SymmetricRandom();

public:
	virtual bool IsSameGenerator( const ioEmitPointGenerator *pRhs ) const;
	virtual const ioHashString& GetType() const = 0;

public:
	inline const D3DXVECTOR3& GetDefaultPosition() const { return m_vPos; }
	inline const D3DXVECTOR3& GetDefaultDirection() const { return m_vDir; }

public:
	ioEmitPointGenerator();
	virtual ~ioEmitPointGenerator();
};

class __EX ioPointEmitGenerator : public ioEmitPointGenerator
{
public:
	virtual const ioHashString& GetType() const;

public:
	ioPointEmitGenerator();
	virtual ~ioPointEmitGenerator();
};

class __EX ioAreaEmitGenerator : public ioEmitPointGenerator
{
public:
	class CmdAreaSize : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

public:
	static CmdAreaSize	m_AreaSizeCmd;

protected:
	D3DXVECTOR3	m_vSize;
	D3DXVECTOR3 m_vRangeX, m_vRangeY, m_vRangeZ;

public:
	virtual void ParametersCopy( const ioEmitPointGenerator *pSrc );
	virtual void SetDefaultDirection( const D3DXVECTOR3 &vDir );
	virtual void ScaleGenerateArea( float fScale );
	virtual bool IsSameGenerator( const ioEmitPointGenerator *pRhs ) const;

public:
	void SetSize( const D3DXVECTOR3 &vSize );
	void SetSize( float fSizeX, float fSizeY, float fSizeZ );

	inline const D3DXVECTOR3& GetSize() const { return m_vSize; }

protected:
	bool InitDefaultParameters( const char *szType );
	void GenAreaAxis();

public:
	ioAreaEmitGenerator();
	virtual ~ioAreaEmitGenerator();
};

//--------------------------------------------------------------------

class __EX ioBoxEmitGenerator : public ioAreaEmitGenerator
{
public:
	virtual D3DXVECTOR3 GetStartPosition() const;
	virtual const ioHashString& GetType() const;

public:
	ioBoxEmitGenerator();
	virtual ~ioBoxEmitGenerator();
};

//--------------------------------------------------------------------

class __EX ioCylinderEmitGenerator : public ioAreaEmitGenerator
{
public:
	virtual D3DXVECTOR3 GetStartPosition() const;
	virtual const ioHashString& GetType() const;

public:
	ioCylinderEmitGenerator();
	virtual ~ioCylinderEmitGenerator();
};

//--------------------------------------------------------------------------

class __EX ioEllipseEmitGenerator : public ioAreaEmitGenerator
{
public:
	class CmdInnerSize : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	static CmdInnerSize m_InnerSizeCmd;

protected:
	float	m_fInnerSizeX;
	float	m_fInnerSizeZ;

public:
	virtual void ParametersCopy( const ioEmitPointGenerator *pSrc );
	virtual bool IsSameGenerator( const ioEmitPointGenerator *pRhs ) const;
	virtual D3DXVECTOR3 GetStartPosition() const;
	virtual const ioHashString& GetType() const;

public:
	void SetInnerSize( float fSize );	// 0.0f ~ 1.0f
	void SetInnerSize( float fSizeX, float fSizeZ );	// 0.0f ~ 1.0f
	
	float GetInnerSizeX() const { return m_fInnerSizeX; }
	float GetInnerSizeZ() const { return m_fInnerSizeZ; }

public:
	ioEllipseEmitGenerator();
	virtual ~ioEllipseEmitGenerator();
};

//-------------------------------------------------------------------------------

class __EX ioEllipsoidEmitGenerator : public ioAreaEmitGenerator
{
public:
	virtual D3DXVECTOR3 GetStartPosition() const;
	virtual const ioHashString& GetType() const;

public:
	ioEllipsoidEmitGenerator();
	virtual ~ioEllipsoidEmitGenerator();
};

//---------------------------------------------------------------------------------

class __EX ioHollowEllipsoidEmitGenerator : public ioAreaEmitGenerator
{
public:
	class CmdInnerSizeXYZ : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	static CmdInnerSizeXYZ m_InnerSizeXYZCmd;

protected:
	D3DXVECTOR3 m_vInnerSize;

public:
	virtual void ParametersCopy( const ioEmitPointGenerator *pSrc );
	virtual bool IsSameGenerator( const ioEmitPointGenerator *pRhs ) const;

	virtual D3DXVECTOR3 GetStartPosition() const;
	virtual const ioHashString& GetType() const;

public:
	void SetInnerSize( const D3DXVECTOR3 &vInnerSize );	// 0.0f ~ 1.0f
	void SetInnerSize( float fInnerX, float fInnerY, float fInnerZ );	// 0.0f ~ 1.0f
	
	inline const D3DXVECTOR3& GetInnerSize() const { return m_vInnerSize; }

public:
	ioHollowEllipsoidEmitGenerator();
	virtual ~ioHollowEllipsoidEmitGenerator();
};


#endif
