

#ifndef _ioStringConverter_h_
#define _ioStringConverter_h_

typedef std::vector< std::string > StringVector;

class ioStringConverter
{
private:
	static char	m_ConvertBuf[MAX_PATH];

public:
	static ioHashString toString( float val );

	static ioHashString toString( int val );

	static ioHashString toString( unsigned int val );

	static ioHashString toString( long val );

	static ioHashString toString( unsigned long val );

	// if bYesNo is true => yes/no, else true/false
	static ioHashString toString( bool val, bool bYesNo = false );

public:
	static float ParseFloat( const char *szVal );

	static int ParseInt( const char *szVal );

	static unsigned int ParseUnsignedInt( const char *szVal );

	static long ParseLong( const char *szVal );

	static unsigned long ParseUnsignedLong( const char *szVal );

	static bool ParseBool( const char *szVal );

public:
	static StringVector Split( const std::string &param,
							   const std::string &delims,
							   int iMaxSplit = 0 );

	static void toLowerCase( std::string &str );
	static void toUpperCase( std::string &str );
};

#endif
