/***    mem.h - memory definitions
 *
 *  Definitions of routines that layer C run-time library memory
 *  routines to provide debugging assistance.
 *
 *  History
 *      07-Mar-1990 bens    Initial version.
 *      08-Mar-1990 bens    Added heap check code
 *      12-May-1990 bens    Upper-case function names
 */

#ifdef CHECKASSERTS

void    MyAssert(void *pv,char *pszFile,UINT line);
void *  MyAlloc(UINT cb,char *pszFile,UINT line);
void    MyCheckHeap(char *pszFile,UINT line);
void    MyFree(void *pv,char *pszFile,UINT line);
char *  MyStrDup(char *pv,char *pszFile,UINT line);

#define MemAlloc(cb)   MyAlloc(cb,__FILE__,__LINE__)
#define MemAssert(pv)  MyAssert(pv,__FILE__,__LINE__)
#define MemCheckHeap() MyCheckHeap(__FILE__,__LINE__)
#define MemFree(pv)    MyFree(pv,__FILE__,__LINE__)
#define MemStrDup(pv)  MyStrDup(pv,__FILE__,__LINE__)

#else

#include <memory.h>
#include <malloc.h>
// No Debug Asserts
#define MemAlloc(cb)   malloc(cb)
#define MemAssert(pv)
#define MemCheckHeap()
#define MemFree(pv)    free(pv)
#define MemStrDup(pv)  strdup(pv)

#endif
