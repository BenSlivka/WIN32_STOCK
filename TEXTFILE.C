/***    textfile.c - routines to handle text files efficiently
 *
 *	Author:
 *	    Benjamin W. Slivka
 *
 *	History:
 *	    29-Oct-1991 Initial version
 *	    16-Jan-1992 Fixed EOF bug; do not report EOF from TFEof until
 *			TFReadLine has encountered it!
 *
 *	Functions:
 *	    TFClose    - Close text file
 *	    TFEof      - Test text file for end-of-file
 *	    TFOpen     - Open text file
 *	    TFReadLine - Read line from text file
 */


#include <windows.h>

#include "mem.h"
#include "textfile.h"

typedef struct _TEXTFILE {  /* tf */
    int     hfile;	// File handle used with _lopen/_lread/etc.
    int     cchBuf;	// Buffer size
    NPSTR   pch;	// File buffer
    NPSTR   pchRead;    // Next byte to read from buffer
    NPSTR   pchLast;    // Last byte of buffer with valid data
    BOOL    fEOF;	// TRUE => file at EOF, buffer may have data!
    BOOL    fEOFtoRL;	// TRUE => EOF has been returned to TFReadLine call
} TEXTFILE;
typedef TEXTFILE *PTEXTFILE; /* ptf */

#define PTFfromHTF(htf) ((PTEXTFILE)(htf))
#define HTFfromPTF(ptf) ((ULONG)(ptf))


/***    TFClose - Close a text file
 *
 *	Close file opened by TFOpen.
 *
 *	Entry:
 *	    htf - Text file handle returned by TFOpen().
 *
 *	Exit-Success:
 *	    Returns TRUE.
 *
 *	Exit-Failure:
 *	    Returns FALSE.  htf was invalid.
 */
BOOL TFClose(HTF htf)
{
    PTEXTFILE   ptf;
    int 	rc;

    ptf = PTFfromHTF(htf);

    rc = _lclose(ptf->hfile);
    MemFree(ptf->pch);
    MemFree(ptf);

    return rc;
}


/***    TFEof - Test for EOF on a text file
 *
 *	NOTE: Does not return TRUE until TFReadLine has encountered EOF!
 *
 *	Entry:
 *	    htf - Text file handle returned by TFOpen().
 *
 *	Exit-Success:
 *	    Returns TRUE if at EOF.
 *	    Returns FALSE if not at EOF.
 *
 *	Exit-Failure:
 *	    Returns FALSE.  htf was invalid.
 */
BOOL TFEof(HTF htf)
{
    PTEXTFILE   ptf;

    ptf = PTFfromHTF(htf);

    return ptf->fEOFtoRL;   // Be consistent with what TFReadLine
}


/***    TFOpen - Open a text file for I/O
 *
 *	Entry
 *	    pszFile:   - File name.
 *	    iReadWrite - Read/Write/Sharing flags.
 *	    cbBuffer   - Size of read/write buffer.  0 specifies
 *		 default size.
 *
 *	Exit-Success:
 *	    Returns number of bytes read into buffer, *including* the
 *	    NULL terminating character.  This number may be smaller than
 *	    the parameter cb, in which case the file has been read to its
 *	    end.
 *
 *	Exit-Failure:
 *	    Returns 0.	The file is at its end, or the parameters are bad.
 */
HTF  TFOpen(NPSTR pszFile, int iReadWrite, int cbBuffer)
{
    PTEXTFILE   ptf;
    int         hf;
    NPSTR       pch;
    int         cb;

    // Open file

    hf = _lopen(pszFile,iReadWrite);

    if (hf == -1) { // Open failed
	// SetLastError(ERROR_FILE_NOT_FOUND);
	return 0;
    }

    // Allocate text file structure

    ptf = MemAlloc(sizeof(TEXTFILE));
    if (ptf == 0) {
	// SetLastError(ERROR_OUT_OF_MEMORY);
	return 0;
    }

    // Allocate input buffer
    cb = cbBuffer;
    pch = MemAlloc(cb);
    if (pch == 0) {
	MemFree(ptf);	// Free text structure
	// SetLastError(ERROR_OUT_OF_MEMORY);
	return 0;
    }

    // Fill in text file structure

    ptf->hfile	  = hf;
    ptf->cchBuf   = cb;
    ptf->pch	  = pch;
    ptf->pchRead  = pch;     // Force file read
    ptf->pchLast  = 0;	     // Force file read
    ptf->fEOF	  = FALSE;
    ptf->fEOFtoRL = FALSE;

    return HTFfromPTF(ptf); // Text file "handle"
}


/***    TFReadLine - Read a line from a text file
 *
 *	Read from current file position to end of line (as indicated by
 *	a carriage return and/or line feed).  File position is advanced
 *	to start of next line.
 *
 *	Entry:
 *	    htf - Text file handle returned by TFOpen().
 *	    ach - Buffer to recieve line from file.  The line
 *	      terminating characters are removed, and the
 *	      line is terminated with a NULL character.
 *	    cb	- Size of buffer, in bytes, on input.
 *
 *	Exit-Success:
 *	    Returns number of bytes read into buffer, *including* the
 *	    NULL terminating character.
 *
 *	Exit-Failure:
 *	    Returns 0.
 *	    If TFEof() returns TRUE, then file is at end.
 *	    If TFEof() returns FALSE, then parameters were bad.
 *
 *	NOTE:
 *	    A sequence of zero or more carriage returns ('\r') followed by
 *	    a single line feed ('\a') is interpreted as a line separator.
 *
 *	    Carriage returns embedded in a line are ignored.
 */
int TFReadLine(HTF htf, NPSTR lpBuffer, int cbBuffer)
{
    PTEXTFILE   ptf;
    int         cb;
    NPSTR       pch;

    ptf = PTFfromHTF(htf);
    pch = lpBuffer;

    while (cbBuffer > 0) {
	// Get what we can out of buffer
	while (ptf->pchRead <= ptf->pchLast) {	// Copy chars from buffer
	    if (*(ptf->pchRead) == '\r') {  // Found end of line
		ptf->pchRead++;     // Skip carriage return
	    }
	    else if (*(ptf->pchRead) == '\n') { // Found new line
		ptf->pchRead++;     // Skip newline
		*pch++ = '\0';	    // Terminate buffer
		return (pch - lpBuffer); // Bytes read, including NUL
	    }
	    else {			// Found a character to copy
		if (cbBuffer <= 1) {	// No room for NULL terminator
		    *pch = '\0';	// Terminate buffer, to be nice
		    // BUGBUG 29-Oct-1991 bens Should we read to end-of-line?
		    // SetLastError(ERROR_BUFFER_OVERFLOW)
		    return 0;		// Indicate failure
		}
		cbBuffer--;		// One less character to copy
		*pch++ = *(ptf->pchRead)++; // Copy character
	    }
	}

	// Get more data from file, if available

	if (ptf->fEOF) {		// At end of file
	    *pch++ = '\0';		// Terminate buffer, to be nice
	    ptf->fEOFtoRL = TRUE;	// We encountered EOF!
	    return 0;			// At EOF
	}
	cb = _lread(ptf->hfile,ptf->pch,ptf->cchBuf);
	if (cb != ptf->cchBuf)		// Did not fill buffer
	    ptf->fEOF = TRUE;		// Mark at end of file
	if (cb == 0) {			// No data left in file
	    *pch++ = '\0';		// Terminate buffer, to be nice
	    ptf->fEOFtoRL = TRUE;	// We encountered EOF!
	    return 0;
	}

	// Update pointers for more copying
	ptf->pchRead = ptf->pch;	// Start reading from front
	ptf->pchLast = ptf->pch + cb - 1; // Last byte in buffer
    }
}
