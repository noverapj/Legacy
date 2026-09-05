#pragma once

#include "../util/cSerialize.h"

class ioLogSerialize :public cSerialize
{
public:
	ioLogSerialize();
	~ioLogSerialize();

	void Init();
	void Destroy();

public:
	void Begin( const int logType );
	void End();

	void Append( uint8 value );
	void Append( uint16 value );
	void Append( uint32 value );
	void Append( uint64 value );
	void Append( short value );
	void Append( float value );
	void Append( double value );
	void Append( int value );
	void Append( int64 value );
	void Append( const char* value, uint32 length, BOOL saveLength = FALSE );
};