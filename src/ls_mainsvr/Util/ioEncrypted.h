
#ifndef _ioEncrypted_h_
#define _ioEncrypted_h_

#define DATA_LEN 16
#define SEED_USER_KEY_LEN 16 

namespace ioEncrypted
{

bool Encode15( const char* szPlain, const char* szUserKey, char *szCipher);
bool Decode15(char *szCipher, char *szUserKey, char *szPlain);

} // end namespace ioEncrypted
#endif