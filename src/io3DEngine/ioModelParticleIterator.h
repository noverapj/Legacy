

#ifndef _ioModelParticleIterator_h_
#define _ioModelParticleIterator_h_

class ioModelParticle;
class ioModelParicleSystem;

class ioModelParticleIterator
{
	friend class ioModelParticleSystem;

protected:
	std::list< ioModelParticle* >::iterator	m_Pos;
	std::list< ioModelParticle* >::iterator	m_Start;
	std::list< ioModelParticle* >::iterator	m_End;

	ioModelParticleIterator( std::list< ioModelParticle* >::iterator iStart,
							 std::list< ioModelParticle* >::iterator iEnd );

public:
	inline bool end() 
	{ 
		return (m_Pos == m_End );
	}

	inline ioModelParticle* GetNext()
	{
		return *m_Pos++;
	}
};

#endif
