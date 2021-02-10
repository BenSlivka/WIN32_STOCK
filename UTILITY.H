/***    utility.h - utility routines
 *
 *  Author:
 *      Benjamin W. Slivka
 *
 *  History:
 *      04-Nov-1991 Initial version
 *	10-Feb-1992 Added SkipPath()
 */

#ifndef BOOL
typedef int BOOL;
#endif

BOOL ParseCommandLine(char *pszIn, char ***pppszArg, int *pcArg);
char *SkipPath(char *psz);
