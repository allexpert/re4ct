// re4ct.cpp : main project file.

// MS Windows service: react to changes on a given file

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <regex> 

#include <strsafe.h>

/* Parameters to pass to service:
	HKEY_LOCAL_MACHINE/SYSTEM/CurrentControlSet/Services/re4ct/ImagePath

	<service.exe> <containing folder> <file name>

	e.g.:
	C:\tmp\re4ct.exe C:\Users\dummy\AppData\Roaming\DWP_PAC LocalPac
*/

int RefreshDirectory(LPTSTR);
void RefreshTree(LPTSTR);
void WatchDirectory(LPTSTR);

SERVICE_STATUS        g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

LPTSTR wdir= L"C:\\tmp\\";
LPTSTR wfil= L"re4ct.demo";
LPTSTR rdir= L"C:\\tmp\\";
LPTSTR rfil= L"re4ct.demo.template";

VOID WINAPI ServiceMain (DWORD argc, LPTSTR *argv);
VOID WINAPI ServiceCtrlHandler (DWORD);
DWORD WINAPI ServiceWorkerThread (LPVOID lpParam);

FILE *flog= NULL;

#define SERVICE_NAME  _T("re4ct")
//#define DEBUG

void vaOutputDebugString0( LPCTSTR sFormat, ... )
{
    va_list argptr;      
    va_start( argptr, sFormat ); 
    TCHAR buffer[ 2000 ];
    HRESULT hr = StringCbVPrintf( buffer, sizeof( buffer ), sFormat, argptr );
    if ( STRSAFE_E_INSUFFICIENT_BUFFER == hr || S_OK == hr )
//		WriteEventLogEntry(buffer,  EVENTLOG_INFORMATION_TYPE);
		OutputDebugString( buffer );
    else
        OutputDebugString( _T("StringCbVPrintf error.") );
}

void vaOutputDebugString( LPCTSTR sFormat, ... )
{
    va_list argptr;      
    va_start( argptr, sFormat ); 
    TCHAR buffer[2000];
    HRESULT hr = StringCbVPrintf( buffer, sizeof( buffer ), sFormat, argptr );
    if ( STRSAFE_E_INSUFFICIENT_BUFFER == hr || S_OK == hr )
#ifdef DEBUG
		fprintf(flog, "%ls\n", buffer);
#else
		OutputDebugString( buffer );
#endif
    else
#ifdef DEBUG
		fprintf(flog, "StringCbVPrintf error.");
#else
        OutputDebugString( _T("StringCbVPrintf error.") );
#endif
		fflush(flog);

}

int _tmain (int argc, TCHAR *argv[])
{
char buf[20]; 
    const char *first = "axayaz"; 
    const char *last = first + strlen(first); 
    std::regex rx("a"); 
    std::string fmt("A"); 
    std::regex_constants::match_flag_type fonly = 
        std::regex_constants::format_first_only; 
 
    *std::regex_replace(&buf[0], first, last, rx, fmt) = '\0'; 
    std::cout << "replacement == " << &buf[0] << std::endl; 
 
    *std::regex_replace(&buf[0], first, last, rx, fmt, fonly) = '\0'; 
    std::cout << "replacement == " << &buf[0] << std::endl; 
 
    std::string str("adaeaf"); 
    std::cout << "replacement == " 
        << std::regex_replace(str, rx, fmt) << std::endl; 
 
    std::cout << "replacement == " 
        << std::regex_replace(str, rx, fmt, fonly) << std::endl; 
 
    return (0); 
#ifdef DEBUG
	flog= fopen("C:\\tmp\\re4ct.log", "a");
#endif

    vaOutputDebugString(_T("re4ct: Main: Entry argc=%d"), argc);

	if (argc > 0) {
		vaOutputDebugString(_T("tmain arg[0] %s"), argv[0]);
	}
	if (argc > 1) {
		wdir= argv[1];
		vaOutputDebugString(_T("tmain arg[1] %s"), argv[1]);
	}
	if (argc > 2) {
		wfil= argv[2];
		vaOutputDebugString(_T("tmain arg[2] %s"), argv[2]);
	}
	if (argc > 3) {
		rdir= argv[3];
		vaOutputDebugString(_T("tmain arg[3] %s"), argv[3]);
	}
	if (argc > 4) {
		rfil= argv[4];
		vaOutputDebugString(_T("tmain arg[4] %s"), argv[4]);
	}

    SERVICE_TABLE_ENTRY ServiceTable[] = 
    {
        {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) ServiceMain},
        {NULL, NULL}
    };

	if (StartServiceCtrlDispatcher (ServiceTable) == FALSE)
    {
       OutputDebugString(_T("re4ct: Main: StartServiceCtrlDispatcher returned error"));
       return GetLastError ();
    }

    OutputDebugString(_T("re4ct: Main: Exit"));
    return 0;
}

// registry: HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\re4ct\params

VOID WINAPI ServiceMain (DWORD argc, LPTSTR *argv)
{
    DWORD Status = E_FAIL;

	vaOutputDebugString(_T("re4ct: ServiceMain: Entry argc=%d"), argc);

	if (argc > 0) {
		vaOutputDebugString(_T("re4ct arg[0] %s"), argv[0]);
	}
	if (argc > 1) {
		vaOutputDebugString(_T("re4ct arg[1] %s"), argv[1]);
	}
	if (argc > 2) {
		vaOutputDebugString(_T("re4ct arg[2] %s"), argv[2]);
	}
	if (argc > 3) {
		vaOutputDebugString(_T("re4ct arg[3] %s"), argv[3]);
	}
	if (argc > 4) {
		vaOutputDebugString(_T("re4ct arg[4] %s"), argv[4]);
	}

	g_StatusHandle = RegisterServiceCtrlHandler (SERVICE_NAME, ServiceCtrlHandler);

    if (g_StatusHandle == NULL) 
    {
        OutputDebugString(_T("re4ct: ServiceMain: RegisterServiceCtrlHandler returned error"));
        goto EXIT;
    }

    // Tell the service controller we are starting
    ZeroMemory (&g_ServiceStatus, sizeof (g_ServiceStatus));
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE) 
    {
        vaOutputDebugString(_T("re4ct: ServiceMain: SetServiceStatus returned error"));
    }

    /* 
     * Perform tasks neccesary to start the service here
     */
    vaOutputDebugString(_T("re4ct: ServiceMain: Performing Service Start Operations"));

    // Create stop event to wait on later.
    g_ServiceStopEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL) 
    {
        OutputDebugString(_T("re4ct: ServiceMain: CreateEvent(g_ServiceStopEvent) returned error"));

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;

        if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
	    {
		    vaOutputDebugString(_T("re4ct: ServiceMain: SetServiceStatus returned error"));
	    }
        goto EXIT; 
    }    

    // Tell the service controller we are started
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
	    vaOutputDebugString(_T("re4ct: ServiceMain: SetServiceStatus returned error"));
    }

    // Start the thread that will perform the main task of the service
    HANDLE hThread = CreateThread (NULL, 0, ServiceWorkerThread, wdir, 0, NULL);

    vaOutputDebugString(_T("re4ct: ServiceMain: Waiting for Worker Thread to complete"));

    // Wait until our worker thread exits effectively signaling that the service needs to stop
    WaitForSingleObject (hThread, INFINITE);

    vaOutputDebugString(_T("re4ct: ServiceMain: Worker Thread Stop Event signaled"));

    /* 
     * Perform any cleanup tasks
     */
    vaOutputDebugString(_T("re4ct: ServiceMain: Performing Cleanup Operations"));

    CloseHandle (g_ServiceStopEvent);

    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;

    if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
	    vaOutputDebugString(_T("re4ct: ServiceMain: SetServiceStatus returned error"));
    }
    
    EXIT:
    vaOutputDebugString(_T("re4ct: ServiceMain: Exit"));

    return;
}

/*
void OnStart(DWORD argc, LPWSTR *argv)
{
    HANDLE hThread = CreateThread (NULL, 0, ServiceWorkerThread, wdir, 0, NULL);
	vaOutputDebugString(_T("OnStart: "), argv[0]);
}
*/

VOID WINAPI ServiceCtrlHandler (DWORD CtrlCode)
{
    OutputDebugString(_T("re4ct: ServiceCtrlHandler: Entry"));

    switch (CtrlCode) 
	{
     case SERVICE_CONTROL_STOP :

        OutputDebugString(_T("re4ct: ServiceCtrlHandler: SERVICE_CONTROL_STOP Request"));

        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
           break;

        /* 
         * Perform tasks neccesary to stop the service here 
         */
        
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        g_ServiceStatus.dwWin32ExitCode = 0;
        g_ServiceStatus.dwCheckPoint = 4;

        if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
		{
			OutputDebugString(_T("re4ct: ServiceCtrlHandler: SetServiceStatus returned error"));
		}

        // This will signal the worker thread to start shutting down
        SetEvent (g_ServiceStopEvent);

        break;

     default:
         break;
    }

    OutputDebugString(_T("re4ct: ServiceCtrlHandler: Exit"));
}


DWORD WINAPI ServiceWorkerThread (LPVOID lpParam)
{
	DWORD dwWaitStatus; 
	HANDLE dwChangeHandles[2]; 
	TCHAR lpDrive[4];
	TCHAR lpFile[_MAX_FNAME];
	TCHAR lpExt[_MAX_EXT];
	int iret= 0;
	LPTSTR lpDir= (LPTSTR) lpParam;

	OutputDebugString(_T("re4ct: ServiceWorkerThread: Entry"));

	_tsplitpath_s(lpDir, lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

	lpDrive[2] = (TCHAR)'\\';
	lpDrive[3] = (TCHAR)'\0';

	// Watch the directory for file creation and deletion. 

	dwChangeHandles[0] = FindFirstChangeNotification( 
		lpDir,                         // directory to watch 
		FALSE,                         // do not watch subtree 
		FILE_NOTIFY_CHANGE_LAST_WRITE); // watch file name changes 

	if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) 
	{
		printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		ExitProcess(GetLastError()); 
	}

	// Watch the subtree for directory creation and deletion. 

	dwChangeHandles[1] = FindFirstChangeNotification( 
		lpDrive,                       // directory to watch 
		TRUE,                          // watch the subtree 
		FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir name changes 

	if (dwChangeHandles[1] == INVALID_HANDLE_VALUE) 
	{
		printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		ExitProcess(GetLastError()); 
	}


	// Make a final validation check on our handles.

	if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL))
	{
		printf("\n ERROR: Unexpected NULL from FindFirstChangeNotification.\n");
		ExitProcess(GetLastError()); 
	}

	// Change notification is set. Now wait on both notification 
	// handles and refresh accordingly. 

    //  Periodically check if the service has been requested to stop
    while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
    {        
        /* 
         * Perform main service function here
         */
		// Wait for notification.

		printf("\nWaiting for notification...\n");

		dwWaitStatus = WaitForMultipleObjects(2, dwChangeHandles, 
			FALSE, INFINITE); 

		switch (dwWaitStatus) {
		case WAIT_OBJECT_0: 

			// A file was changed in the directory.
			// Refresh this directory and restart the notification.

			iret= RefreshDirectory(lpDir); 
			if ( FindNextChangeNotification(dwChangeHandles[0]) == FALSE )
			{
				printf("\n ERROR: FindNextChangeNotification function failed.\n");
				ExitProcess(GetLastError()); 
			}
			if (!iret) {
				dwChangeHandles[0] = FindFirstChangeNotification( 
					lpDir,                         // directory to watch 
					FALSE,                         // do not watch subtree 
					FILE_NOTIFY_CHANGE_LAST_WRITE); // watch file name changes 
				if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) {
					printf("\n ERROR: 2. FindFirstChangeNotification function failed.\n");
					ExitProcess(GetLastError()); 
				}
			}
			break; 

		case WAIT_OBJECT_0 + 1: 

			// A directory was created, renamed, or deleted.
			// Refresh the tree and restart the notification.

			RefreshTree(lpDrive); 
			if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE )
			{
				printf("\n ERROR: FindNextChangeNotification function failed.\n");
				ExitProcess(GetLastError()); 
			}
			break; 

		case WAIT_TIMEOUT:

			// A timeout occurred, this would happen if some value other 
			// than INFINITE is used in the Wait call and no changes occur.
			// In a single-threaded environment you might not want an
			// INFINITE wait.

			printf("\nNo changes in the timeout period.\n");
			break;

		default: 
			printf("\n ERROR: Unhandled dwWaitStatus.\n");
			ExitProcess(GetLastError());
			break;
		}
	}

    OutputDebugString(_T("re4ct: ServiceWorkerThread: Exit"));

    return ERROR_SUCCESS;
}

void ErrorDisplay(void) 
{ 
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	printf("Failed with error %d: %s", dw, lpMsgBuf); 
}

int RefreshDirectory(LPTSTR lpDir)
{
	// This is where you might place code to refresh your
	// directory listing, but not the subtree because it
	// would not be necessary.

//	_tprintf(TEXT("Directory (%s) changed.\n"), lpDir);
	vaOutputDebugString(_T("Dir (%s) changed"), lpDir);

	TCHAR path_wfil_bkp[512] = _T("");
	StringCchCat(path_wfil_bkp, 512, rdir); 
	StringCchCat(path_wfil_bkp, 512, wfil);
	StringCchCat(path_wfil_bkp, 512, L".bkp");
	if (!DeleteFile(path_wfil_bkp)) {
		wchar_t buf[256];
		DWORD erc= GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, erc, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		vaOutputDebugString(_T("1.Delete error %d: %ls\n"), erc, buf);
	}

	TCHAR path_rfil_tmp[512] = _T("");
	StringCchCat(path_rfil_tmp, 512, rdir); 
	StringCchCat(path_rfil_tmp, 512, rfil);
	StringCchCat(path_rfil_tmp, 512, L".new");
	if (!DeleteFile(path_rfil_tmp)) {
		wchar_t buf[256];
		DWORD erc= GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, erc, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		vaOutputDebugString(_T("2.Delete error %d: %ls\n"), erc, buf);
	}

	TCHAR path_wfil[512] = _T("");
	StringCchCat(path_wfil, 512, wdir); 
	StringCchCat(path_wfil, 512, wfil);
	if (!MoveFile(path_wfil, path_wfil_bkp)) {
		wchar_t buf[256];
		DWORD erc= GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, erc, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		vaOutputDebugString(_T("1.Move error %d: %ls\n"), erc, buf);
	}

	TCHAR path_rfil[512] = _T("");
	StringCchCat(path_rfil, 512, rdir); 
	StringCchCat(path_rfil, 512, rfil);
	while (!CopyFile(path_rfil, path_rfil_tmp, FALSE)) {
		wchar_t buf[256];
		DWORD erc= GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, erc, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		vaOutputDebugString(_T("!!!Copy error %d: %ls\n"), erc, buf); // 2do: make a fix
		ExitProcess(GetLastError());
	}

	printf("Move %ls to %ls\n", path_rfil_tmp, path_wfil);
	while (!MoveFile(path_rfil_tmp, path_wfil)) {
		wchar_t buf[256];
		DWORD erc= GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, erc, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		vaOutputDebugString(_T("2.Move error %d: %ls\n"), erc, buf);
		DeleteFile(path_wfil);
	}
	vaOutputDebugString(_T("Copied!\n"));
	return(0);
}

void RefreshTree(LPTSTR lpDrive)
{
	// This is where you might place code to refresh your
	// directory listing, including the subtree.

	_tprintf(TEXT("Directory tree (%s) changed.\n"), lpDrive);
}
