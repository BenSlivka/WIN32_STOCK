/***    utility.c - utility routines
 *
 *  Author:
 *      Benjamin W. Slivka
 *
 *  History:
 *      04-Nov-1991 Initial version
 *	10-Feb-1992 Added SkipPath()
 */

#include <ctype.h>
#include <stddef.h>
#include "mem.h"
#include "utility.h"


BOOL IsPathChar(char ch);


#define	FALSE	0
#define	TRUE	1
typedef enum {ST_BLANK, ST_ARG} STATE;

/***    ParseCommandLine - Parse command line into argument array and count
 *
 *      Entry:
 *          pszIn   - Command line
 *          ppszArg - Pointer to receive array of command line args.
 *                    NOTE: Caller must call MemFree(*paszArg) to free this array!
 *          pcArg   - Pointer to receive count of command line args
 *
 *      Exit-Success:
 *          Returns TRUE.
 *
 *      Exit-Failure:
 *          Returns FALSE.
 */
BOOL ParseCommandLine(char *pszIn, char ***pppszArg, int *pcArg)
{
    int     cArgs=0;
    int     cbArray;
    int     cbIn;
    char   *pchBuffer;
    char   *pchDest;
    char   *pch;
    char  **ppch;
    STATE   state;

    // Count number of args, to compute amount of space needed

    state = ST_BLANK;
    for (pch=pszIn; *pch; pch++) {
        switch (state) {
            case ST_BLANK:
                if (!isspace(*pch)) {
                    state = ST_ARG;                     // Found start of argument
                    cArgs++;                            // Count another argument
                }
                break;
            case ST_ARG:
                if (isspace(*pch))
                    state = ST_BLANK;                   // Argument complete
                break;
        }
    }

    cbIn = pch - pszIn;                                 // Size of command line string

    // Allocate space for parse arguments
    //
    // How we compute size needed:
    //     Worst case is a command line like: "abcd"
    //     For this, we need room for one pointer (char *), room for the string (abcd), and
    //     room for a null terminator.
    cbArray = cArgs * sizeof(char *);                   // Size of char * array
    pchBuffer = MemAlloc(cbIn+cbArray+1);               // Allocate buffer
    if (pchBuffer == NULL) {                            //   Failed!
        *pppszArg = NULL;
        *pcArg = 0;
        return 0;
    }

    // Do copy

    pchDest = pchBuffer + cbArray;                      // Start of string area
    ppch = (char **)pchBuffer;                          // Start of array
    state = ST_BLANK;
    for (pch=pszIn; *pch; pch++) {
        switch (state) {
            case ST_BLANK:
                if (!isspace(*pch)) {
                    state = ST_ARG;                     // Found start of argument
                    *ppch++ = pchDest;                  // Start of argument
                    *pchDest++ = *pch;                  // Copy first character
                }
                break;
            case ST_ARG:
                if (isspace(*pch)) {
                    state = ST_BLANK;                   // Argument complete
                    *pchDest++ = '\0';                  // Terminate argument
                }
                else {
                    *pchDest++ = *pch;                  // Copy character
                }
                break;
        }
    }

    // Terminate final argument, if necessary
    if (state == ST_ARG)                                // Argument was at end of command line
        *pchDest++ = '\0';

    // Return pointer, count
    *pppszArg = (char **)pchBuffer;
    *pcArg = cArgs;
    return 1;
}




/***	SkipPath - Skip over path prefix in file name string
 *
 *      Entry:
 *	    psz  - File name path, which may or may not have path characters
 *		   [ : / \ ]
 *
 *      Exit-Success:
 *	    Returns pointer to first character of last component of path.
 *
 *      Exit-Failure:
 *          Returns FALSE.
 *
 *	NOTE:
 *	    This routine DOES NOT check for correctly formed paths!
 */
char *SkipPath(char *psz)
{
    BOOL    fLookingForNonPathChar = FALSE;  // Assume no path prefix
    char   *pchLast=psz;		// Point to start of component

    if (psz == NULL)
	return NULL;

    while (*psz != '\0') {		// Scan string
	if (IsPathChar(*psz))
	    fLookingForNonPathChar = TRUE;  // Look for component start
	else if (fLookingForNonPathChar) {
	    pchLast = psz;		// Got start of a component
	    fLookingForNonPathChar = FALSE; // No longer looking for component
	}
	psz++;
    }
    return pchLast;
}


/***	IsPathChar - Test if character is a path character
 *
 *      Entry:
 *	    ch	- characer to test
 *
 *      Exit-Success:
 *	    Returns TRUE if ch is one of [:/\].
 *	    Returns FALSE otherwise.
 */
BOOL IsPathChar(char ch)
{
    switch (ch) {
	case ':' :
	case '/' :
	case '\\' :
	    return TRUE;
    }
    return FALSE;
}
