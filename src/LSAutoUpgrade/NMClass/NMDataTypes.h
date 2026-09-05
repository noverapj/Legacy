#ifndef __NMDATATYPES_H_C9C3D6C1_CC9F_4C22_8388_24CB34669356__
#define __NMDATATYPES_H_C9C3D6C1_CC9F_4C22_8388_24CB34669356__

//
//	Windows header files
//
#include <Windows.h>

#ifndef STRSAFE_NO_DEPRECATE
#define STRSAFE_NO_DEPRECATE
#endif
#include <strsafe.h>

//
//	STL header files
//
#include <functional>

///////////////////////////////////////////////////////////////////////////////
//
//	Primary data types
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _NMIDCODE_DEFINED
#define _NMIDCODE_DEFINED
	typedef	UINT64	NMIDCode;
#endif

#ifndef _NMGAMECODE_DEFINED
#define _NMGAMECODE_DEFINED
	typedef UINT32	NMGameCode;
#endif

#ifndef _NMGUILDCODE_DEFINED
#define _NMGUILDCODE_DEFINED
	typedef UINT32	NMGuildCode;
#endif

#ifndef _NMGUILDKEY_DEFINED
#define _NMGUILDKEY_DEFINED
	typedef UINT64	NMGuildKey;
#endif

#ifndef _NMGAMESERVERCODE_DEFINED
#define _NMGAMESERVERCODE_DEFINED
	typedef UINT32	NMGameServerCode;
#endif

///////////////////////////////////////////////////////////////////////////////
//
//	Main classes
//
///////////////////////////////////////////////////////////////////////////////

namespace Nexon
{
	namespace Platform
	{
		///////////////////////////////////////////////////////////////////////
		//
		//	Primary messenger data types
		//
		///////////////////////////////////////////////////////////////////////

		inline NMIDCode MakeNMIDCode( INT32 nNexonSN, INT32 nGameCode = 0x00010000 )
		{
			return ( ( ( static_cast< NMIDCode >( nNexonSN ) ) << 32 ) | nGameCode );
		}

		inline INT32 GetNexonSN( NMIDCode uIDCode )
		{
			return ( static_cast< INT32 >( uIDCode >> 32 ) );
		}

		inline INT32 GetServerCode( NMGuildKey const & guildKey )
		{
			return static_cast< INT32 >( guildKey >> 32 );
		}

		inline INT32 GetGuildSN( NMGuildKey const & guildKey )
		{
			return static_cast< INT32 >( guildKey );
		}

		inline NMGuildKey MakeNMGuildKey( INT32 nServerCode, INT32 nGuildSN )
		{
			return ( ( ( static_cast< NMGuildKey >( nServerCode ) ) << 32 ) | nGuildSN );
		}

		template< class T >
		struct IsSameNexonSN : public std::unary_function< T, bool >
		{
			IsSameNexonSN( INT32 nNexonSN ) : m_nNexonSN( nNexonSN )
			{
			}
			IsSameNexonSN( NMIDCode uIDCode ) : m_nNexonSN( GetNexonSN( uIDCode ) )
			{
			}
			result_type operator () ( argument_type const & value ) const
			{
				return ( value.GetNexonSN() == m_nNexonSN );
			}
		private:
			INT32 m_nNexonSN;
		};

		///////////////////////////////////////////////////////////////////////
		//
		//	NMGuildID structure
		//
		///////////////////////////////////////////////////////////////////////

		struct NMGuildID
		{
			INT32	nGameCode;
			INT32	nServerCode;
			INT32	nGuildSN;

			NMGuildID()
				: nGameCode( 0 )
				, nServerCode( 0 )
				, nGuildSN( 0 ) 
			{
			}
			NMGuildID( INT32 nGameCode, NMGuildKey guildKey )
				: nGameCode( nGameCode )
				, nServerCode( GetServerCode( guildKey ) )
				, nGuildSN( GetGuildSN( guildKey ) )
			{
			}
			NMGuildID( INT32 nGameCode, INT32 nServerCode, INT32 nGuildSN )
				: nGameCode( nGameCode )
				, nServerCode( nServerCode )
				, nGuildSN( nGuildSN )
			{
			}
			NMGuildKey GetGuildKey() const
			{
				return MakeNMGuildKey( this->nServerCode, this->nGuildSN );
			}
			bool operator == ( NMGuildID const & rhs ) const
			{
				return ( this->nGameCode == rhs.nGameCode && this->nServerCode == rhs.nServerCode && this->nGuildSN == rhs.nGuildSN );
			}
		};

		struct NMGuildIDLess : public std::binary_function< NMGuildID, NMGuildID, bool >
		{
			bool operator () ( NMGuildID const & lhs, NMGuildID const & rhs ) const
			{
				return ( lhs.nGameCode < rhs.nGameCode || lhs.nServerCode < rhs.nServerCode || lhs.nGuildSN < rhs.nGuildSN );
			}
		};

		template< class T >
		struct IsSameGuildID : public std::unary_function< T, bool >
		{
			IsSameGuildID( NMGuildID const & guildID ) : m_guildID( guildID )
			{
			}
			result_type operator () ( argument_type const & data ) const
			{
				return ( data.GetGuildID() == this->m_guildID );
			}
		private:
			NMGuildID m_guildID;
		};

		///////////////////////////////////////////////////////////////////////
		//
		//	StringZip template class
		//
		///////////////////////////////////////////////////////////////////////

		template< size_t N >
		class StringZip
		{
		public:
			typedef TCHAR		value_type;
			typedef LPTSTR		pointer;
			typedef LPCTSTR		const_pointer;
			typedef size_t		size_type;

		public:
			StringZip()
				: m_pData( m_szEmpty )
			{
			}

			StringZip( StringZip const & rhs )
				: m_pData( m_szEmpty )
			{
				this->CopyString( rhs.m_pData );
			}

			StringZip( const_pointer pszText )
				: m_pData( m_szEmpty )
			{
				this->CopyString( pszText );
			}

			//
			//	Workaround for Visual Studio 6.0 template compiling bug.
			//
#if defined( _MSC_VER ) && ( 1310 <= _MSC_VER )
			template< size_type N2 >
			StringZip< N >( StringZip< N2 > const & rhs )
				: m_pData( m_szEmpty )
			{
				this->CopyString( rhs );
			}
#endif

			StringZip & operator = ( StringZip const & rhs )
			{
				this->CopyString( rhs );
				return *this;
			}

			StringZip & operator = ( const_pointer pszText )
			{
				this->CopyString( pszText );
				return *this;
			}

			//
			//	Workaround for Visual Studio 6.0 template compiling bug.
			//
#if defined( _MSC_VER ) && ( 1310 <= _MSC_VER )
			template< size_type N2 >
			StringZip< N > & operator = ( StringZip< N2 > const & rhs )
			{
				this->CopyString( rhs );
				return *this;
			}
#endif

			~StringZip()
			{
				this->DeleteString();
			}

			bool IsEmpty() const
			{
				return ( this->m_pData == NULL || this->m_pData[ 0 ] == 0 );
			}

			void Empty()
			{
				this->DeleteString();
			}

			size_type GetSize() const
			{
				if ( this->m_pData )
				{
					size_t cchSize = 0;
					if ( SUCCEEDED( ::StringCchLength( this->m_pData, N, &cchSize ) ) )
					{
						return ( static_cast< size_type >( cchSize ) );
					}
				}
				return 0;
			}

			UINT32 GetCapacity() const
			{
				return static_cast< UINT32 >( N );
			}

			pointer GetBuffer()
			{
				this->AllocateString();
				return ( this->m_pData );
			}

			operator const_pointer () const
			{
				return ( this->m_pData );
			}

			template< size_type N2 >
			bool operator == ( StringZip< N2 > const & rhs ) const
			{
				return ( this->CompareString( this->m_pData, rhs ) == CSTR_EQUAL );
			}

			bool operator == ( const_pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_EQUAL );
			}

			bool operator == ( pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_EQUAL );
			}

			template< size_type N2 >
			bool operator != ( StringZip< N2 > const & rhs ) const
			{
				return ( this->CompareString( this->m_pData, rhs.m_pData ) != CSTR_EQUAL );
			}

			bool operator != ( const_pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) != CSTR_EQUAL );
			}

			bool operator != ( pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) != CSTR_EQUAL );
			}

			template< size_type N2 >
			bool operator < ( StringZip< N2 > const & rhs ) const
			{
				return ( this->CompareString( this->m_pData, rhs.m_pData ) == CSTR_LESS_THAN );
			}

			bool operator < ( const_pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_LESS_THAN );
			}

			bool operator < ( pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_LESS_THAN );
			}

			template< size_type N2 >
			bool operator > ( StringZip< N2 > const & rhs ) const
			{
				return ( this->CompareString( this->m_pData, rhs.m_pData ) == CSTR_GREATER_THAN );
			}

			bool operator > ( const_pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_GREATER_THAN );
			}

			bool operator > ( pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_GREATER_THAN );
			}

			template< size_type N2 >
			bool operator <= ( StringZip< N2 > const & rhs ) const
			{
				return !( *this > rhs );
			}

			bool operator <= ( const_pointer pszText ) const
			{
				return !( *this > pszText );
			}

			bool operator <= ( pointer pszText ) const
			{
				return !( *this > pszText );
			}

			template< size_type N2 >
			bool operator >= ( StringZip< N2 > const & rhs ) const
			{
				return !( *this < rhs );
			}

			bool operator >= ( const_pointer pszText ) const
			{
				return !( *this < pszText );
			}

			bool operator >= ( pointer pszText ) const
			{
				return !( *this < pszText );
			}

		protected:

			bool AllocateString()
			{
				if ( this->m_pData == NULL || this->m_pData == this->m_szEmpty )
				{
					try
					{
						this->m_pData = new value_type[ N ];
						this->m_pData[ 0 ] = 0;
					}
					catch (...)
					{
						this->m_pData = this->m_szEmpty;
					}
				}
				return ( this->m_pData != NULL && this->m_pData != this->m_szEmpty );
			}

			void DeleteString()
			{
				if ( this->m_pData && this->m_pData != this->m_szEmpty )
				{
					delete this->m_pData;
					this->m_pData = this->m_szEmpty;
				}
			}

			void CopyString( const_pointer pszText )
			{
				if ( pszText )
				{
					if ( this->AllocateString() )
					{
						::StringCchCopy( this->m_pData, N, pszText );
					}
				}
				else
				{
					this->DeleteString();
				}
			}

			int CompareString( LPCTSTR pszLeft, LPCTSTR pszRight ) const
			{
				if ( pszLeft != NULL && pszRight != NULL )
				{
					return ::CompareString( LOCALE_USER_DEFAULT, 0, pszLeft, -1, pszRight, -1 );
				}
				else if ( pszLeft == NULL && pszRight != NULL && pszRight[ 0 ] != 0 )
				{
					return CSTR_LESS_THAN;
				}
				else if ( pszLeft != NULL && pszLeft[ 0 ] != 0 && pszRight == NULL )
				{
					return CSTR_GREATER_THAN;
				}
				return CSTR_EQUAL;
			}

		private:
			pointer	m_pData;

		private:
			static value_type m_szEmpty[];
		};

		template< size_t N >
		typename StringZip< N >::value_type StringZip< N >::m_szEmpty[] = _T("");

		template< size_t N > bool operator == ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs == pszText );
		}

		template< size_t N > bool operator == ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs == pszText );
		}

		template< size_t N > bool operator != ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs != pszText );
		}

		template< size_t N > bool operator != ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs != pszText );
		}

		template< size_t N > bool operator < ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs > pszText );
		}

		template< size_t N > bool operator < ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs > pszText );
		}

		template< size_t N > bool operator > ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs < pszText );
		}

		template< size_t N > bool operator > ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs < pszText );
		}

		template< size_t N > bool operator <= ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs >= pszText );
		}

		template< size_t N > bool operator <= ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs >= pszText );
		}

		template< size_t N > bool operator >= ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs <= pszText );
		}

		template< size_t N > bool operator >= ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs <= pszText );
		}

		///////////////////////////////////////////////////////////////////////
		//
		//	String copy function
		//
		///////////////////////////////////////////////////////////////////////

		STRSAFEAPI StringCopy( LPTSTR pszTarget, UINT32 cbyMaxSize, LPCTSTR pszSource )
		{
			return ::StringCbCopyEx( pszTarget, cbyMaxSize, pszSource, NULL, NULL, STRSAFE_IGNORE_NULLS );
		}

		#define StringArrayCopy( target, source )	StringCopy( target, sizeof( target ), source )

		///////////////////////////////////////////////////////////////////////
		//
		//	ServerAddress class
		//
		///////////////////////////////////////////////////////////////////////

		class ServerAddress
		{
		public:
			enum
			{
				IP_ADDRESS_SIZE = 32,
			};

			typedef StringZip< IP_ADDRESS_SIZE >	IPAddressType;
			typedef IPAddressType::const_pointer	IPAddressInputType;
			typedef UINT16							PortType;

		public:
			ServerAddress()
				: m_port( 0 )
			{
			}

			ServerAddress( IPAddressInputType ipAddress, PortType port )
				: m_ipAddress( ipAddress )
				, m_port( port )
			{
			}

			ServerAddress( ServerAddress const & rhs )
				: m_ipAddress( rhs.m_ipAddress )
				, m_port( rhs.m_port )
			{
			}

			ServerAddress & operator = ( ServerAddress const & rhs )
			{
				this->m_ipAddress	= rhs.m_ipAddress;
				this->m_port		= rhs.m_port;

				return *this;
			}

			bool IsEmpty() const
			{
				return ( this->m_ipAddress.IsEmpty() || this->m_port == 0 );
			}

			void Empty()
			{
				this->m_ipAddress.Empty();
				this->m_port		= 0;
			}

			IPAddressType const & GetIPAddress() const
			{
				return ( this->m_ipAddress );
			}

			IPAddressType GetIPAddress()
			{
				return ( this->m_ipAddress );
			}

			void SetIPAddress( IPAddressInputType ipAddress )
			{
				this->m_ipAddress = ipAddress;
			}

			PortType GetPort() const
			{
				return ( this->m_port );
			}

			void SetPort( PortType port )
			{
				this->m_port = port;
			}

			void SetAddress( IPAddressInputType ipAddress, PortType port )
			{
				this->m_ipAddress	= ipAddress;
				this->m_port		= port;
			}

			bool operator == ( ServerAddress const & rhs ) const
			{
				return ( this->m_ipAddress == rhs.m_ipAddress && this->m_port == rhs.m_port );
			}

			bool operator != ( ServerAddress const & rhs ) const
			{
				return !( this->operator == ( rhs ) );
			}

		public:
			IPAddressType	m_ipAddress;
			PortType		m_port;
		};

		///////////////////////////////////////////////////////////////////////
		//
		//	ServerInfo
		//
		///////////////////////////////////////////////////////////////////////

		class ServerInfo
		{
		public:
			typedef INT32								IDType;
			typedef ServerAddress::IPAddressType		IPAddressType;
			typedef ServerAddress::IPAddressInputType	IPAddressInputType;
			typedef ServerAddress::PortType				PortType;

			enum ServerType
			{
				UNDEFINED_SERVER	= 0,

				LOGIN_SERVER		= 1,
				SESSION_SERVER		= 2,
				STAT_SERVER			= 3,
				__REMOVED_4			= 4,
				__REMOVED_5			= 5,
				CHANNEL_SERVER		= 6,
				RELAY_SERVER		= 7,

				AUTH_SERVER			= 16,
				AUTH_BACKUP_SERVER	= 17,

				NOTE_SERVER			= 255, // 서버 숫자는 관계 없음
			};

			static ServerType ToType( LPCTSTR pszType )
			{
				StringZip< 32 > szType( pszType );

				if		( szType == _T("LOGIN_SERVER") )		return LOGIN_SERVER;
				else if ( szType == _T("SESSION_SERVER") )		return SESSION_SERVER;
				else if ( szType == _T("STAT_SERVER") )			return STAT_SERVER;
				else if ( szType == _T("CHANNEL_SERVER") )		return CHANNEL_SERVER;
				else if ( szType == _T("RELAY_SERVER") )		return RELAY_SERVER;
				else if ( szType == _T("AUTH_SERVER") )			return AUTH_SERVER;
				else											return UNDEFINED_SERVER;
			}

			static LPCTSTR ToString( ServerType type )
			{
				switch ( type )
				{
				case LOGIN_SERVER:					return _T("LOGIN_SERVER");
				case SESSION_SERVER:				return _T("SESSION_SERVER");
				case STAT_SERVER:					return _T("STAT_SERVER");
				case CHANNEL_SERVER:				return _T("CHANNEL_SERVER");
				case RELAY_SERVER:					return _T("RELAY_SERVER");
				case AUTH_SERVER:					return _T("AUTH_SERVER");
				default:							return _T("UNDEFINED_SERVER");
				}
			}

		public:
			ServerInfo()
				: m_id( 0 )
				, m_type( UNDEFINED_SERVER )
			{
			}

			ServerInfo( IDType id, ServerType type, ServerAddress const & address )
				: m_id( id )
				, m_type( type )
				, m_address( address )
			{
			}

			ServerInfo( IDType id, ServerType type, IPAddressInputType ipAddress, PortType port )
				: m_id( id )
				, m_type( type )
				, m_address( ipAddress, port )
			{
			}

			ServerInfo( ServerType type, IPAddressInputType ipAddress, PortType port )
				: m_id( 0 )
				, m_type( type )
				, m_address( ipAddress, port )
			{
			}

			ServerInfo( ServerInfo const & rhs )
				: m_id( rhs.m_id )
				, m_type( rhs.m_type )
				, m_address( rhs.m_address )
			{
			}

			ServerInfo & operator = ( ServerInfo const & rhs )
			{
				this->m_id		= rhs.m_id;
				this->m_type	= rhs.m_type;
				this->m_address	= rhs.m_address;

				return *this;
			}

			IDType GetID() const
			{
				return ( this->m_id );
			}

			void SetID( IDType id )
			{
				this->m_id = id;
			}

			ServerType GetType() const
			{
				return ( this->m_type );
			}

			void SetType( ServerType type )
			{
				this->m_type = type;
			}

			ServerAddress const & GetAddress() const
			{
				return ( this->m_address );
			}

			ServerAddress GetAddress()
			{
				return ( this->m_address );
			}

			void SetAddress( ServerAddress const & address )
			{
				this->m_address = address;
			}

			void SetAddress( IPAddressInputType ipAddress, PortType port )
			{
				this->m_address.SetIPAddress( ipAddress );
				this->m_address.SetPort( port );
			}

			IPAddressType const & GetIPAddress() const
			{
				return ( this->m_address.GetIPAddress() );
			}

			IPAddressType GetIPAddress()
			{
				return ( this->m_address.GetIPAddress() );
			}

			void SetIPAddress( IPAddressInputType ipAddress )
			{
				this->m_address.SetIPAddress( ipAddress );
			}

			PortType GetPort() const
			{
				return ( this->m_address.GetPort() );
			}

			void SetPort( PortType port )
			{
				this->m_address.SetPort( port );
			}

		private:
			IDType			m_id;
			ServerType		m_type;
			ServerAddress	m_address;
		};

		typedef std::vector< ServerInfo >	ServerInfoList;
	};
};

#endif	//	#ifndef __NMDATATYPES_H_C9C3D6C1_CC9F_4C22_8388_24CB34669356__