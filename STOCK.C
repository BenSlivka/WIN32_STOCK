***    stock - draw a stock chart
 *
 *      Author:
 *	    (c) 1986-1994, Benjamin W. Slivka
 *
 *      History:
 *	    27-Mar-1989 bens	Initial version (from PM and QB ancestry)
 *          06-Apr-1989 bens    Always repaint on WM_SIZE.
 *          26-Apr-1990 bens    Get it limping along
 *          03-May-1990 bens    Add file open
 *          11-May-1990 bens    Printing works!
 *          06-Jun-1990 bens    Finish job property handling
 *          07-Jun-1990 bens    Fix bugs in JP stuff, spiff up user interface
 *          14-Dec-1990 bens    Increase cptMax to 2000 (from 1200)
 *          29-Oct-1991 bens    Ported to Windows!
 *	    16-Jan-1992 bens	More complete statistics.
 *	    07-Feb-1992 bens    Add File/Print common dialogs!
 *	    12-Feb-1992 bens	Add font/textout display
 *          26-May-1994 bens    Port to Win32
 */

#ifndef WIN32
typedef unsigned int UINT;
#endif

#include <windows.h>
#include <commdlg.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ids.h"
#include "mem.h"
#include "textfile.h"
#include "utility.h"



/**************
 *** Macros *****************************************************************
 **************/

#define MIR(x)  MAKEINTRESOURCE(x)  // Make DialogBox lines shorter

#define AssertMsg(x)        // BUGBUG - Enable this for DEBUG build

#define Message(psz,hwnd)                                \
MessageBox(hwnd,psz,"Internal Error",MB_APPLMODAL|MB_OK|MB_ICONEXCLAMATION)



/*****************
 *** Constants **************************************************************
 *****************/

#define cchMax     80                   // Maximum line length

#define iH      0                       // index of High
#define iL      1                       // index of Low
#define iC      2                       // index of Close
#define NV      3                       // Number of vectors

#define cptMax   4000                   // maximum data points

#define csplitMax  10                   // Maximum number of stock splits

#define CCHMAXPATH  256


/************************
 *** Type Definitions *******************************************************
 ************************/

/***    SPLIT - stock split record
 *
 */
typedef struct _split {
    int     ipt;        // Point index of split (into apt[][i])
    float   factor;     // Split factor
    char *  pszDate;	// Date of split
} SPLIT;

/***    GLOBAL - global data
 *
 */
typedef struct {    /* g */
    int     cpt;                // Number of points
    int     cSplit;             // Count of splits
    char *  pszFile;            // Data file
    float   rMax[NV];           // Maximum prices for high, low, close
    float   rMin[NV];           // Minimum prices for high, low, close
    float   rMaxOverall;	// Overall maximum price
    float   rMinOverall;	// Overall minimum price
    float   factorOverall;      // Overall split factor from IPO to present
    HWND    hwnd;               // Client window
    int     cxMain;             // X width of main window
    int     cyMain;             // Y width of main window
    int     cxClient;           // X width of client area of window
    int     cyClient;           // Y width of client area of window
    BOOL    fZeroOrigin;        // TRUE => show origin at 0
    HANDLE  hInstance;          // App instance handle
    int     cShowBusy;		// ShowBusy nest count
    FARPROC lpfnAboutDlgProc;   // About DlgProc instance function pointer
    FARPROC lpfnStatsDlgProc;   // Stats DlgProc instance function pointer
    SPLIT   split[csplitMax];   // Splits
    char    achMisc[CCHMAXPATH];// Temp buffer for formating
    char    achTitle[CCHMAXPATH]; // Stock data title
    char    achDateFirst[10];	// First date of stock data
    char    achDateLast[10];	// Last date of stock data
} GLOBAL;


/*****************
 *** Variables **************************************************************
 *****************/

GLOBAL g;   // Globals


float       apt[NV][cptMax];


/***************************
 *** Function Prototypes ****************************************************
 ***************************/

int PASCAL WinMain(HANDLE hInstance,
                   HANDLE hPrevInstance,
                   LPSTR lpszCmdLine,
                   int nCmdShow);

BOOL FAR PASCAL AboutDlgProc(HWND hDlg,UINT msg,UINT wParam,LONG lParam);
BOOL FAR PASCAL StatsDlgProc(HWND hDlg,UINT msg,UINT wParam,LONG lParam);
long FAR PASCAL WndProc(HWND hDlg,UINT msg,UINT wParam,LONG lParam);

VOID   AddSplit(GLOBAL *pg,float split,char *pszDate);
BOOL   BeginApp(HANDLE hInstance,HANDLE hPrevInstance);
BOOL   ComputeDataStats(VOID);
VOID   CopyToClipboard(HWND hwnd);
VOID   DoCommand(HWND hwnd,UINT wParam,LONG lParam);
BOOL   DoMouse(HWND hwnd,UINT msg,UINT wParam,LONG lParam);
VOID   EndApp(VOID);
VOID   NotImplemented(HWND hwnd, char *psz);
BOOL   OpenDataFile(HWND hwnd);
VOID   PrintView(HWND hwnd);
VOID   PlotPrice(HDC hdc,RECT *prect);
VOID   ProcessCommandLine(LPSTR lpszCmdLine);
BOOL   ReadData(VOID);
VOID   SetMenuState(VOID);
VOID   ShowBusyEnd(VOID);
VOID   ShowBusyStart(VOID);


// VOID   DumpResults(VOID);


/***************
 *** WinMain ****************************************************************
 ***************/

int PASCAL WinMain(HANDLE hInstance,
                   HANDLE hPrevInstance,
                   LPSTR lpszCmdLine,
                   int nCmdShow)
{
    MSG     msg;

    if (!BeginApp(hInstance,hPrevInstance))
        exit(1);

    // Set menu state
    SetMenuState();

    //  Show window

    ShowWindow(g.hwnd,nCmdShow);
    UpdateWindow(g.hwnd);

    ProcessCommandLine(lpszCmdLine);

    if (g.pszFile) {
	ShowBusyStart();
        ReadData();
        ComputeDataStats();
        InvalidateRect(g.hwnd,NULL,TRUE);   //  Force window paint
	ShowBusyEnd();
    }

    while(GetMessage(&msg,NULL,0,0))
        {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }

    EndApp();

    return msg.wParam;
}


/************************************************
 *** Functions - Listed in Alphabetical Order *******************************
 ************************************************/

/***    AboutDlgProc - About Dialog Box Procedure
 *
 */
BOOL FAR PASCAL AboutDlgProc(HWND hdlg,UINT msg,UINT wParam,LONG lParam)
{
    switch (msg)
        {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            switch (wParam)
                {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hdlg, 0);
                    return TRUE;
                }
            break;
        }
    return FALSE;
}


/***    AddSplit - Add split record
 *
 */
VOID AddSplit(GLOBAL *pg,float split,char *pszDate)
{
    if (g.cSplit >= csplitMax) {
	sprintf(g.achMisc,"Maximum number of splits (%d) exceeded",csplitMax);
	Message(g.achMisc,g.hwnd);

    // BUGBUG 16 Jan 92 bens - Max number of splits exceeded
    // We should either stop reading data and display what we have, or
    // we should dynamically grow the split array, or we should abort
    // altogether.  Growth would be best.

	return;
    }

    pg->split[g.cSplit].ipt = g.cpt;
    pg->split[g.cSplit].factor = split;
    pg->split[g.cSplit].pszDate = MemStrDup(pszDate);
    pg->cSplit++;
}


/***	BeginApp - Initialize application
 *
 */
BOOL BeginApp(HANDLE hInstance,HANDLE hPrevInstance)
{
    long        lStyle;
    static char szAppName [] = "Stock";
    WNDCLASS    wndclass;


    // Init misc global variables
    g.cShowBusy = 0;			// Not busy

    // Save hInstance
    g.hInstance = hInstance;

    // Register window class, if necessary
    if (!hPrevInstance)
        {
        wndclass.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc   = WndProc;
        wndclass.cbClsExtra    = 0;
        wndclass.cbWndExtra    = 0;
        wndclass.hInstance     = g.hInstance;
        wndclass.hIcon         = LoadIcon(hInstance, szAppName);
	wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wndclass.lpszMenuName  = szAppName;
        wndclass.lpszClassName = szAppName;

        RegisterClass(&wndclass);
        }

    // Create main window

    LoadString(g.hInstance,IDS_APP_TITLE,g.achMisc,sizeof(g.achMisc));
    lStyle = WS_OVERLAPPEDWINDOW;
    g.hwnd = CreateWindow(
                szAppName,          // Window class
		g.achMisc,	    // Window Title
                lStyle,             // Window Style
                0,                  // Left edge of window
                0,                  // Top edge of window
		500,		    // Width of window
		400,		    // Height of window
                NULL,               // Parent window
                NULL,               // Control ID
                hInstance,          // App instance
                NULL                // lpCreateStruct
             );

    // Create dialog procedure instance(s)
    g.lpfnAboutDlgProc = MakeProcInstance(AboutDlgProc,g.hInstance);
    g.lpfnStatsDlgProc = MakeProcInstance(StatsDlgProc,g.hInstance);

    return TRUE;
}


/***    ComputeDataStats - Apply splits, compute min/max prices
 *
 */
BOOL ComputeDataStats(VOID)
{
    float   factor;
    int     i;
    int     iptFirst;
    int     iptLast;
    int     iSplit;
    int     j;

    // Apply split factors
    //
    // We traverse the splits backwards in time, so that we need only
    // multiply each price at most once.  So, all prices after the last
    // split are unmodified.
    //
    // This is much faster than an earlier implementation, where we applied
    // split factors to accumulated data as we encountered splits.  This
    // Caused many more multiplications!

    factor = 1.0f;
    for (iSplit=g.cSplit-1; iSplit >= 0; iSplit--) {
        factor = factor/g.split[iSplit].factor; // Price factor

        // Locate first price to multiply for this split

        if (iSplit == 0)                // Oldest split
            iptFirst = 0;               // Start with first price
        else                            // Not oldest split
            iptFirst = g.split[iSplit-1].ipt; // Starting with previous split

        // Locate last price to multiply for this split

        iptLast = g.split[iSplit].ipt;  // Last is first of this split

        // Process all points for this split

        for (i=iptFirst; i<iptLast; i++) {
            for (j=0; j<NV; j++) {   // Process prices
                apt[j][i] *= factor;
                // dP(iV, j%) *= factor; // QB remnant, if we ever do volumes
            }
        }
    }
    g.factorOverall = 1.0f/factor;           // Save total factor

    // Find max and min prices

    // Speed win: Initialize min/max with actual values.
    //  This allows the if..else if below to work reliably.
    for (j=0; j<NV; j++) {
    	g.rMax[j] = apt[iH][0];
    	g.rMin[j] = apt[iL][0];
    }

    // Find highest and lowest prices for each set of High, Low, and Close prices
    for (i=0; i<g.cpt; i++) {
        for (j=0; j<NV; j++) {
            if (apt[j][i] > g.rMax[j])
                g.rMax[j] = apt[j][i];
            else if (apt[j][i] < g.rMin[j])
                g.rMin[j] = apt[j][i];
        }
    }

    // Find overall high and low
    g.rMaxOverall = g.rMax[0];
    g.rMinOverall = g.rMin[0];
    for (j=1; j<NV; j++) {
    	if (g.rMax[j] > g.rMaxOverall)
    	    g.rMaxOverall = g.rMax[j];
    	if (g.rMin[j] < g.rMinOverall)
    	    g.rMinOverall = g.rMin[j];
    }

    return TRUE;
}


/***    CopyToClipboard - Copy graph to clipboard
 *
 */
VOID   CopyToClipboard(HWND hwnd)
{
    HDC             hdc;
    GLOBALHANDLE    hgmem;
    HANDLE          hmf;
    LPMETAFILEPICT  lpMFP;
    RECT            rect;

    // Create Metafile

    hdc = CreateMetaFile(NULL);     // Memory metafile

    rect.top    = 0;                // Arbitrary rectangle
    rect.left   = 0;
    rect.bottom = 1200;
    rect.right  = 1600;
    PlotPrice(hdc,&rect);           // Draw into metafile

    hmf = CloseMetaFile(hdc);       // Complete metafile

    // Copy to the clipboard

    hgmem = GlobalAlloc(GHND,(DWORD)sizeof(METAFILEPICT));
    lpMFP = (LPMETAFILEPICT) GlobalLock(hgmem);

    lpMFP->mm = MM_HIMETRIC;        // Try this?
    lpMFP->yExt = 1200; 	    // units are 0.01 mm
    lpMFP->xExt = 1600;
    lpMFP->hMF = hmf;

    GlobalUnlock(hgmem);

    OpenClipboard(hwnd);
    EmptyClipboard();
    SetClipboardData(CF_METAFILEPICT,hgmem);
    CloseClipboard();
}


/***    DoCommand - Handle COMMAND messages
 *
 */
VOID DoCommand(HWND hwnd,UINT wParam,LONG lParam)
{
    switch (wParam) {
        case IDM_ABOUT:
            DialogBox(g.hInstance,MIR(IDD_ABOUT),hwnd,g.lpfnAboutDlgProc);
	    break;

        case IDM_EDIT_COPY:
            CopyToClipboard(hwnd);
	    break;

	case IDM_FILE_OPEN:
	    OpenDataFile(hwnd);
	    break;

	case IDM_FILE_PRINT:
	    PrintView(hwnd);
	    break;

	case IDM_FILE_PRINTER_SETUP:
	    NotImplemented(hwnd,"File.Printer Setup...");
	    break;

	case IDM_FILE_EXIT:
	    SendMessage(hwnd, WM_CLOSE, 0, 0L);
	    break;

	case IDM_OPTIONS_ORIGIN:
	    // Toggle origin, update menu, and force repaint
	    g.fZeroOrigin = !g.fZeroOrigin;
	    SetMenuState();
	    InvalidateRect(g.hwnd,NULL,TRUE);	//  Force window paint
	    break;

	case IDM_OPTIONS_STATS:
            DialogBox(g.hInstance,MIR(IDD_STATS),hwnd,g.lpfnStatsDlgProc);
	    break;
    }
    return;
}


/***	EndApp - End application
 *
 */
VOID EndApp(VOID)
{
    // Free dialog procedure instances

    FreeProcInstance(g.lpfnAboutDlgProc);
    FreeProcInstance(g.lpfnStatsDlgProc);
}


/***	NotImplemented - show not-implemented
 *
 *
 */
VOID NotImplemented(HWND hwnd, char *psz)
{
    MessageBox(hwnd, "Not Implemented!", psz,
    	MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
}


/***	OpenDataFile - open data file
 *
 */
BOOL OpenDataFile(HWND hwnd)
{
    BOOL    f;
    OPENFILENAME ofn;
    char *szFilter[] = {
	"DAT Files(*.DAT)",	"*.dat",
	"CSV Files(*.CSV)",	"*.csv",
	"Stock Files(*.STK)",	"*.stk",
	""
	};

    //** Initialize the OPENFILENAME members
    g.achMisc[0] = '\0';

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter[0];
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nFilterIndex = 1L;
    ofn.lpstrFile= g.achMisc;
    ofn.nMaxFile = sizeof(g.achMisc);
    ofn.lpstrFileTitle = NULL;		// Do not care about file title
    ofn.lpstrInitialDir = NULL; 	// Use current drive/directory
    ofn.lpstrTitle = (LPSTR) NULL;
    ofn.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "dat";

    // Get file name from user

    if (GetOpenFileName(&ofn)){
	g.pszFile = MemStrDup(g.achMisc);  // Copy file name to global
	ShowBusyStart();
	f = ReadData(); 		// Read data file
	if (f) {			// Read was successfull
	    ComputeDataStats(); 	// Compute stats
	    InvalidateRect(g.hwnd,NULL,TRUE);	//  Force window paint
	}
	ShowBusyEnd();
	return f;
    }
    else {
	// BUGBUG 10-Feb-1992 bens Handle GetOpenFileName(...) failure
	return FALSE;
    }
}


/***    PlotPrice - plot stock price
 *
 *      Entry:
 *          hdc   - DC to paint
 *          prect - Bounding rectangle to paint
 *
 *	    apt[][]	  = Filled in with price data
 *	    g.cpt	  = Count of points read
 *	    g.rMaxOverall = Maximum price
 *	    g.rMinOverall = Minimum price
 *          g.fZeroOrigin = TRUE => make Y-origin be 0.
 *
 *      Exit:
 *          Stock chart plotted
 */
VOID PlotPrice(HDC hdc,RECT *prect)
{
    int 	cxBar;
    int 	cxMargin = 4;
    int 	cy;
    int 	cyFlip;
    int 	cyMargin = 4;
    int 	cyFont;
    HFONT	hfont;
    HFONT	hfontSave;
    HPEN	hpenSave;
    HPEN	hpenHiLo;
    HPEN	hpenClose;
    int 	i;
    TEXTMETRIC	tm;
    int 	x;
    float	xFactor;
    int		y;
    int 	yC;
    float	yFactor;
    int 	yH;
    int 	yL;
    float	yOrigin;

    if (g.cpt == 0)                     // No points to draw
        return;

    // Compute useful constants

    if (g.fZeroOrigin)
        yOrigin = (float)0;
    else
	yOrigin = (float)g.rMinOverall;

    yFactor = (float)(prect->bottom - prect->top - (LONG)(2*cyMargin)) /
	      (float)(g.rMaxOverall-yOrigin);
    xFactor = (float)(prect->right - prect->left - (LONG)(2*cxMargin)) /
              (float)(g.cpt);

    cxBar = (int)(xFactor/2.0);
    if (cxBar < 1)
        cxBar = 1;

    cyFlip = prect->bottom - cyMargin;

    // We're going to hog the system for a bit
    ShowBusyStart();

    // Save original pen
    hpenSave = SelectObject(hdc,GetStockObject(BLACK_PEN));

    // Draw vertical bars indicating splits
    for (i=0; i<g.cSplit; i++) {
	// Draw split line half-way between two data points
	x = (int)((g.split[i].ipt-0.5)*(float)xFactor) + cxMargin;
	MoveToEx(hdc,x,prect->top,NULL);
	LineTo(hdc,x,prect->bottom);
    }

    cyFont = (prect->bottom - prect->top)/30;	// 30 is a random hack value
    cyFont = max(cyFont,14);	// At least 12 pels!
    // BUGBUG 11-Feb-1992 bens Compute appropriate font size!
    //	Size should be based on grid and device size.

    // Create Font
    hfont = CreateFont(
	    cyFont,			// Font height
	    0,				// Default width
	    0,				// Escapement: horizontal
	    0,				// Orientation: horizontal
	    FW_NORMAL,			// Font Weight
	    0,				// Not italic
	    0,				// Not underlined
	    0,				// Not strikeout
	    DEFAULT_CHARSET,		// Character set: default
	    OUT_TT_PRECIS,		// Get a TrueType font!
	    CLIP_DEFAULT_PRECIS,	// Default clipping
	    PROOF_QUALITY,		// Get a nice-looking font
	    VARIABLE_PITCH | FF_SWISS,	// Pitch and family
	    "Arial"			// Face name
	    );

    if (hfont) {
	hfontSave = SelectObject(hdc,hfont);
	x = 10;
	y = 10;
	GetTextMetrics(hdc,&tm);
	cy = tm.tmExternalLeading + tm.tmHeight;  // line spacing

	TextOut(hdc,x,y,g.achTitle,strlen(g.achTitle));

	y += cy;
	sprintf(g.achMisc,"File: %s",g.pszFile);
	TextOut(hdc,x,y,g.achMisc,strlen(g.achMisc));

	y += cy;
	sprintf(g.achMisc,"From: %s To: %s",g.achDateFirst,g.achDateLast);
	TextOut(hdc,x,y,g.achMisc,strlen(g.achMisc));

	y += cy;
	sprintf(g.achMisc,"Days: %d",g.cpt);
	TextOut(hdc,x,y,g.achMisc,strlen(g.achMisc));

	y += cy;
	sprintf(g.achMisc,"Splits: %d, Factor: %7.3f",g.cSplit,g.factorOverall);
	TextOut(hdc,x,y,g.achMisc,strlen(g.achMisc));

	y += cy;
	sprintf(g.achMisc,"Highest High: %7.3f, Highest Close: %7.3f",
				  g.rMaxOverall,g.rMax[iC]);
	TextOut(hdc,x,y,g.achMisc,strlen(g.achMisc));
	y += cy;

	sprintf(g.achMisc,"Lowest Low:  %7.3f, Lowest Close:  %7.3f",
				  g.rMinOverall,g.rMin[iC]);
	TextOut(hdc,x,y,g.achMisc,strlen(g.achMisc));

	SelectObject(hdc,hfontSave);
	DeleteObject(hfont);
    }

    // Create new pens
    hpenHiLo = CreatePen(PS_SOLID,1,RGB(0x00,0x00,0xFF));
    hpenClose = CreatePen(PS_SOLID,1,RGB(0xFF,0x00,0x00));
    // BUGBUG 3 Nov 1991  bens Need to check for failure to create pens
    // BUGBUG 11-Feb-1992 bens Create pens serialy, to reduce resource usage

    // Draw vertical (low to high) bars
    SelectObject(hdc,hpenHiLo);         // Set color for high-low bar
    for (i=0; i<g.cpt; i++) {
        x = (int)(i*(float)xFactor) + cxMargin;
        yH = cyFlip - (int)((float)(apt[iH][i]-yOrigin)*yFactor);
        yL = cyFlip - (int)((float)(apt[iL][i]-yOrigin)*yFactor);
        MoveToEx(hdc,x,yL,NULL);
        LineTo(hdc,x,yH);
    }

    // Draw horizontal (close) bar
    SelectObject(hdc,hpenClose);        // Set color for close bar
    for (i=0; i<g.cpt; i++) {
            x = (int)(i*(float)xFactor) + cxMargin;
            yC = cyFlip - (int)((float)(apt[iC][i]-yOrigin)*yFactor);
            MoveToEx(hdc,x,yC,NULL);
            LineTo(hdc,x+cxBar,yC);
    }

    // Restore original pen
    SelectObject(hdc,hpenSave);

    // Done with pens
    DeleteObject(hpenHiLo);
    DeleteObject(hpenClose);

    // We're done hoging the system
    ShowBusyEnd();
}


/***	PrintView - Print stock view
 *
 *      Entry:
 *	    hwnd - parent window, for dialogs
 *
 *      Exit:
 */
VOID   PrintView(HWND hwnd)
{
    HDC      hdc;
    PRINTDLG pd;
    RECT     rect;

    // Initialize PRINTDLG structure

    pd.lStructSize = sizeof(PRINTDLG);
    pd.hwndOwner = hwnd;
    pd.hDevMode = (HANDLE) NULL;
    pd.hDevNames = (HANDLE) NULL;
    // Only one page, so no page #'s!
    pd.Flags = PD_RETURNDC |
    	       PD_NOPAGENUMS |
    	       PD_NOSELECTION |
    	       PD_USEDEVMODECOPIES;
    pd.nFromPage = 0;
    pd.nToPage = 0;
    pd.nMinPage = 0;
    pd.nMaxPage = 0;
    pd.nCopies = 0;
    pd.hInstance = (HANDLE) NULL;

    // Prompt for print info, and print if successfull

    if (PrintDlg(&pd) != 0) {

	hdc = pd.hDC;

	rect.left   = 0;
	rect.top    = 0;
	rect.right  = GetDeviceCaps(hdc,HORZRES);
	rect.bottom = GetDeviceCaps(hdc,VERTRES);

	Escape(hdc,STARTDOC,strlen(g.achTitle),g.achTitle,NULL);

	PlotPrice(hdc,&rect);

	Escape(hdc,NEWFRAME,0,NULL,NULL);
	Escape(hdc,ENDDOC,0,NULL,NULL);
	DeleteDC(hdc);
	if (pd.hDevMode)
	   GlobalFree(pd.hDevMode);
	if (pd.hDevNames)
	   GlobalFree(pd.hDevNames);
    }
    else {
	// Some kind of failure
	// BUGBUG 10-Feb-1992 bens Handle PrintDlg(...) failure
    }
}


/***    ProcessCommandLine - Process command line arguments
 *
 *      Entry:
 *          lpszCmdLine - Command line string
 *
 *      Exit:
 */
VOID ProcessCommandLine(LPSTR lpszCmdLine)
{
    LPSTR   lpch;
    char  **ppszArg;
    int     cArg=0;                     // Assume no arguments
    char   *pch;
    char   *pchTmp;

    // Find end of string

    for (lpch=lpszCmdLine; *lpch; lpch++)
        ;

    // Allocate near buffer

    pchTmp = MemAlloc(lpch-lpszCmdLine+1);
    if (pchTmp != NULL) {

	// Copy far buffer to near buffer
	lpch = lpszCmdLine;
	pch = pchTmp;
	while (*pch++ = *lpch++)
	    ;

	// Parse the command line into argument array and count
	ParseCommandLine(pchTmp,&ppszArg,&cArg);
    }

    // If at least one argument, take that as data file name
    // NOTE: cArg is initialized to 0, so if MemAlloc fails above,
    //	     g.pszFile is set to NULL below.

    if (cArg >= 1)
        g.pszFile = MemStrDup(ppszArg[0]);
    else
        g.pszFile = NULL;
}


/***    ReadData - read data file
 *
 */
BOOL ReadData(VOID)
{
    static char   ach[CCHMAXPATH];
    char	  achDate[10];	// BUGBUG 12-Feb-1992 bens date strlen magic #
    int           cf;
    float         rClose;
    float         rHigh;
    float         rLow;
    char *        pch;
    char *        pch1;
    HTF           htf;
    ULONG         ulVol;

    g.cpt = 0;
    g.cSplit = 0;

    htf = TFOpen(g.pszFile,OF_READ,2048);
    if (htf == 0) {
        sprintf(g.achMisc,"Could not open \"%s\"",g.pszFile);
        Message(g.achMisc,g.hwnd);
        return FALSE;
    }

    // Get title line
    TFReadLine(htf,ach,cchMax);         // Skip stock data name line
    pch = strchr(ach,'"')+1;            // pch  -> Stock Title",....
    pch1 = strchr(pch,'"');             // pch1 ->            ",....
    *pch1 = '\0';                       // Remove trailing quote
    strcpy(g.achTitle,pch);

    // Skip column header line
    TFReadLine(htf,ach,cchMax);

    TFReadLine(htf,ach,cchMax);         // Get first data line
    while (!TFEof(htf)) {
        if (g.cpt >= cptMax) {          // Already read as max points
            sprintf(g.achMisc,"Too many data points. Max is %d.",cptMax);
            Message(g.achMisc,g.hwnd);
            return FALSE;
            }

        // Skip date
        pch = strchr(ach,'"')+1;        // pch  -> 04/24/60",....
        pch1 = strchr(pch,'"');         // pch1 ->         ",....
	memcpy(achDate,pch,pch1-pch+1); // Save date, for split record
        *pch1 = '\0';                   // Split string at end of date
        pch1 += 2;                      // Skip ",

        if (g.cpt == 0)
            strcpy(g.achDateFirst,pch);	// Save initial date

        cf = sscanf(pch1," %f, %f, %f, %ld",
              &rHigh,&rLow,&rClose,&ulVol);

        if (cf != 4) {
            // Adjust count for two header lines, plus base 0
            sprintf(g.achMisc,"Line %d is bad: [%s]",g.cpt+3,ach);
            Message(g.achMisc,g.hwnd);
            return FALSE;
            }

        if (stricmp(pch,"SPLIT") == 0) {
            // Store split information, used later for scaling
	    AddSplit(&g,rHigh,achDate);
        }
        else {
            apt[iH][g.cpt] = rHigh;
            apt[iL][g.cpt] = rLow;
            apt[iC][g.cpt] = rClose;
            g.cpt++;
        }
        TFReadLine(htf,ach,cchMax); 	// Get next data line
    }
    strcpy(g.achDateLast,pch);		// Save last date
    TFClose(htf);

    // Set Window Title

    LoadString(g.hInstance,IDS_APP_TITLE,g.achMisc,sizeof(g.achMisc));
    // BUGBUG 10-Feb-1992 bens Should have localizable title separator?
    strcat(g.achMisc," - ");
    strcat(g.achMisc,SkipPath(g.pszFile));
    SetWindowText(g.hwnd,g.achMisc);

    return TRUE;
}


/***	SetMenuState - Set menu enable/disable check/uncheck states
 *
 */
VOID   SetMenuState(VOID)
{
    HMENU   hmenu;

    hmenu = GetMenu(g.hwnd);
    CheckMenuItem(hmenu,IDM_OPTIONS_ORIGIN,
	(g.fZeroOrigin ? MF_CHECKED : MF_UNCHECKED));
}

/***	ShowBusyStart - tell user we are busy
 *
 */
VOID ShowBusyStart(VOID)
{
    g.cShowBusy++;
    if (g.cShowBusy == 1) {
	SetCursor(LoadCursor(NULL,IDC_WAIT));
    }
}


/***	ShowBusyEnd - tell user we are no longer busy
 *
 */
VOID ShowBusyEnd(VOID)
{
    g.cShowBusy--;
    if (g.cShowBusy == 0) {
	SetCursor(LoadCursor(NULL,IDC_ARROW));
    }
}


/***    StatsDlgProc - Stats Dialog Box Procedure
 *
 */
BOOL FAR PASCAL StatsDlgProc(HWND hdlg,UINT msg,UINT wParam,LONG lParam)
{
    switch (msg)
        {
        case WM_INITDIALOG:
            SetDlgItemText(hdlg,IDC_TITLE,g.achTitle);
            SetDlgItemText(hdlg,IDC_FILE,g.pszFile);
            SetDlgItemText(hdlg,IDC_DATE_FIRST,g.achDateFirst);
            SetDlgItemText(hdlg,IDC_DATE_LAST,g.achDateLast);
            SetDlgItemInt(hdlg,IDC_DAYS,g.cpt,0);
            SetDlgItemInt(hdlg,IDC_SPLITS,g.cSplit,0);

            sprintf(g.achMisc,"%7.3f",g.factorOverall);
            SetDlgItemText(hdlg,IDC_SPLIT_FACTOR,g.achMisc);

	    sprintf(g.achMisc,"%7.3f",g.rMin[iC]);
	    SetDlgItemText(hdlg,IDC_LOW_CLOSE,g.achMisc);

	    sprintf(g.achMisc,"%7.3f",g.rMax[iC]);
	    SetDlgItemText(hdlg,IDC_HIGH_CLOSE,g.achMisc);

	    sprintf(g.achMisc,"%7.3f",g.rMinOverall);
	    SetDlgItemText(hdlg,IDC_LOW_OVERALL,g.achMisc);

	    sprintf(g.achMisc,"%7.3f",g.rMaxOverall);
	    SetDlgItemText(hdlg,IDC_HIGH_OVERALL,g.achMisc);
            return TRUE;

        case WM_COMMAND:
            switch (wParam)
                {
                case IDOK:
                    EndDialog(hdlg, 0);
                    return TRUE;
                }
            break;
        }
    return FALSE;
}


/***    WndProc - Main Window Procedure
 *
 */
long FAR PASCAL WndProc(HWND hwnd,UINT msg,UINT wParam,LONG lParam)
{
    HDC         hdc;
    RECT        rect;
    PAINTSTRUCT ps;

    switch(msg) {
        case WM_CREATE:
            return 0;

        case WM_COMMAND:
            DoCommand(hwnd,wParam,lParam);
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rect);
	    PlotPrice(hdc,&rect);
            EndPaint(hwnd,&ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
}


#if 0

VOID DumpResults(VOID)
{
    int     i;

    printf("Trading Days:  %d\n",g.cpt);
    printf("Lowest Close:  %9.5f\n",g.rMinOverall);
    printf("Highest Close: %9.5f\n",g.rMaxOverall);
    printf("\n");
    printf("Splits: %2d\n",g.cSplit);
    printf("----------\n");
    printf("\n");
    printf("Date      Factor  Previous closing price\n");
    printf("--------  ------  ----------------------\n");

    for (i=0; i<g.cSplit; i++) {
        printf("%s  %6.3f  %7.3f\n",
            "xx/xx/xx",
            g.split[i].factor,
            apt[iC][g.split[i].ipt-1]);
    }
}

#endif
