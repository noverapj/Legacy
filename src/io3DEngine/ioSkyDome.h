

#ifndef _ioSkyDome_h_
#define _ioSkyDome_h_

class ioTexture;
class ioMeshData;
class ioCamera;

class __EX ioSkyDome
{
private:
	ioMeshData	*m_pMeshData;
	ioTexture	*m_pSkyTex;

	D3DXVECTOR3	 m_vPosition;

	ioHashString m_Name;

	float	m_fRadius;
	int		m_iSegment;
	float	m_fHeightScale;
	bool	m_bFogEnable;

private:
	void GenerateVertex();
	void GenerateIndex();

public:
	void Create( float fRadius, int iSegment, float fHeightScale );
	void Render( bool bWorldFog );
	void Destroy();

public:
	void SetSkyTexture( const ioHashString &name, bool bDefaultPath = true );
	void SetFogEnable( bool bEnable );
	void SetPosition( const D3DXVECTOR3 &vPosition );

	void NotifyCurrentCamera( ioCamera *pCamera );
	const ioHashString& GetName() const;
	
public:
	float GetDomeRadius() const;
	float GetDomeHeightScale() const;
	int GetDomeSegment() const;

	bool IsFogEnable() const;

public:
	ioSkyDome( const ioHashString &name );
	~ioSkyDome();
};

#endif
