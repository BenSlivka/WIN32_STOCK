/***    textfile.h - routines to handle text files efficiently
 *
 *  Author:
 *      Benjamin W. Slivka
 *
 *  History:
 *          29-Oct-1991 Initial version
 */

typedef unsigned long ULONG;

typedef ULONG HTF;  // Handle to text file

extern BOOL TFClose(HTF htf);
extern BOOL TFEof(HTF htf);
extern HTF  TFOpen(NPSTR pszFile, int iReadWrite, int cbBuffer);
extern int  TFReadLine(HTF htf, NPSTR lpBuffer, int cbBuffer);



// extern BOOL TFFlush(HTF htf);
