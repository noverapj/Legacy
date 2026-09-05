

#ifndef _ioOggCallBack_h_
#define _ioOggCallBack_h_

size_t ReadOggCallBack(void *ptr, size_t size, size_t nmemb, void *datasource);
int    SeekOggCallBack(void *datasource, ogg_int64_t offset, int whence);
int    CloseOggCallBack(void *datasource);
long   TellOggCallBack(void *datasource);

DWORD ReadOggData(OggVorbis_File *pVF, char *pBuffer, DWORD dwReadSize, ioMemFile *pMemFile );

#endif