//--------------------------------------------------------------------------------------
// File: PC2Android.cpp
// Desc: A bridge connection from PC to Snapdragon SDK samples. Allows PC events to be
//       sent over to the samples for control (i.e. buttons 0-9, mouse movement etc.)
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>

#pragma comment(lib, "ws2_32.lib")

#define PROCESS_TIMEOUT_DEFAULT    60000 // 60 seconds

const static char *sDeviceMgr_ADB	= "adb.exe";
const static char *sHostName		= "localhost";
const static int   sPortNum			= 54321;

bool InitWinsock();

BOOL SpawnProcess( LPSTR szCommandLine, 
				  BOOL bShow = TRUE, 
				  BOOL bBlock = TRUE, 
				  INT nTimeoutMS = PROCESS_TIMEOUT_DEFAULT, 
				  LPSTR szOutput = NULL, 
				  DWORD dwOutputBytes = 0, 
				  LPCSTR szWorkingDir = NULL );

bool ADBStartServer();
bool ADBKillServer( bool bRestart = false );
bool ADBForward( const char *szSerialNum, unsigned int iLocalPort, unsigned int iDevicePort );


void myexit()
{
	printf("PRESS A KEY TO EXIT...\n");
	while(!_kbhit()){} exit(0);
}


//-----------------------------------------------------------------
// main
//-----------------------------------------------------------------

int main(int argc, char *argv[])
{
	printf("PC BRIDGE TO ANDROID SNAPDRAGON SAMPLES FRAMEWORK\n");
	printf("COPYRIGHT QUALCOM 2011\n\n");

	// Test for presence of ADB (we won't need to kill 
	if( ADBStartServer() )
	{
		// ADB is installed, forward TCP packets from device to PC
		ADBForward(NULL, sPortNum, sPortNum);
	}
	else
	{
		printf("ERROR ADB services are not present. Please check your Android development environment.");
		myexit();
	}

	// In windows, we must initialize the TCP-IP stack
	// This would be done different on a linux machine
	if(InitWinsock() == false)
	{
		myexit();
	}

	int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	char buffer[256];

	// Open a socket to write to
	sockfd = (int)socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
        printf("ERROR opening socket");
		myexit();
	}

	// Make sure to allow for socket re-use
	BOOL bOptVal = TRUE;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &bOptVal, sizeof bOptVal);

	server = gethostbyname(sHostName);

	if (server == NULL) 
	{
        printf("ERROR, no such host\n");
        myexit();;
    }
    
	memset( &serv_addr, 0, sizeof(serv_addr));
    
	serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(sPortNum);
    
	if (connect(sockfd,(struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
	{
		printf("ERROR, cannot attach to Android device.\n");
		myexit();
	}

	HANDLE hIn;
	HANDLE hOut;
	bool Continue = TRUE;
	INPUT_RECORD InRec;
	DWORD NumRead;

	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	printf("Press a key (0-9) OR \n");
	printf("Use mouse (Left-key only)\n\n");
	printf("Press 'q' to quit...\n\n");

	// Keep track of the mouse state to send the right data over
	bool bMouseDown  = false;
	bool bMouseMoved = false;
	SHORT sMouseX   = 0;
	SHORT sMouseY   = 0;

	while (Continue)
	{
		bMouseMoved = false;

		ReadConsoleInput(hIn, &InRec, 1, &NumRead);

		switch (InRec.EventType)
		{
		case KEY_EVENT:
			if (InRec.Event.KeyEvent.uChar.AsciiChar == 'q')
			{
				Continue = FALSE;
			}
			else
			{
				// BUILD MSG
				buffer[0] = 'S';
				buffer[1] = '2';
				buffer[2] = 'D';
				buffer[3] = 'K';									// MAGIC VALUE
				buffer[4] = 0;										// TYPE: KEY
				buffer[5] = InRec.Event.KeyEvent.uChar.AsciiChar;	// VALUE OF KEY
				
				if(InRec.Event.KeyEvent.bKeyDown)			
					buffer[6] = 1;									// KEY DOWN OR UP
				else
					buffer[6] = 0;
				
				buffer[7] = 0;										// END MSG

				// send it ouuut
				n =  send(sockfd, buffer, 8, 0);

				if (n <= 0) 
				{
					printf("ERROR writing to socket\n");
					myexit();
				}
			}

			break;

 		case MOUSE_EVENT:
			
			// Mouse State
			if (InRec.Event.MouseEvent.dwEventFlags == 0)
			{
				if (InRec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
				{
					bMouseDown = true;			
				}
				else
				{
					bMouseDown = false;			
				}
				
				// Calculate mouse location
				CONSOLE_SCREEN_BUFFER_INFO sbINFO;
				GetConsoleScreenBufferInfo(hOut, &sbINFO);

				SHORT sMouseXT = InRec.Event.MouseEvent.dwMousePosition.X - sbINFO.srWindow.Left;
				SHORT sMouseYT = InRec.Event.MouseEvent.dwMousePosition.Y - sbINFO.srWindow.Top;
				float x = (float)sMouseXT / (float)(sbINFO.srWindow.Right - sbINFO.srWindow.Left);
				x *= 200.0f; 
				float y = (float)sMouseYT / (float)(sbINFO.srWindow.Bottom - sbINFO.srWindow.Top);
				y *= 200.0f;

				sMouseX = (SHORT)x;
				sMouseY = (SHORT)y;

				bMouseMoved = false; // to pick up on first time click
			}
			else
			{
				if (InRec.Event.MouseEvent.dwEventFlags == MOUSE_MOVED)
				{
					// Calculate mouse location
					CONSOLE_SCREEN_BUFFER_INFO sbINFO;
					GetConsoleScreenBufferInfo(hOut, &sbINFO);

					SHORT sMouseXT = InRec.Event.MouseEvent.dwMousePosition.X - sbINFO.srWindow.Left;
					SHORT sMouseYT = InRec.Event.MouseEvent.dwMousePosition.Y - sbINFO.srWindow.Top;
					float x = (float)sMouseXT / (float)(sbINFO.srWindow.Right - sbINFO.srWindow.Left);
					x *= 200.0f; 
					float y = (float)sMouseYT / (float)(sbINFO.srWindow.Bottom - sbINFO.srWindow.Top);
					y *= 200.0f;

					// Check to see if it's the same as before
					if (((SHORT)x == sMouseX) && ((SHORT)y == sMouseY))
						continue;

					sMouseX = (SHORT)x;
					sMouseY = (SHORT)y;

					bMouseMoved = true;
				}
			}

			// Send the msg only on these conditions
			if ((InRec.Event.MouseEvent.dwEventFlags == 0) || bMouseMoved)
			{
				// BUILD MSG
				buffer[0] = 'S';
				buffer[1] = '2';
				buffer[2] = 'D';
				buffer[3] = 'K';									// MAGIC VALUE
				buffer[4] = 1;										// TYPE: MOUSE
				buffer[5] =	bMouseDown;
				buffer[6] = bMouseMoved;
				buffer[7] = (char)sMouseX;
				buffer[8] = (char)sMouseY;
				buffer[9] = 0;

				// send it ouuut
				n =  send(sockfd, buffer, 10, 0);

				if (n <= 0) 
				{
					printf("ERROR writing to socket\n");
					myexit();
				}
			}

			break;
		}
	}

	// Shut down the communication socket
	closesocket(sockfd);

	WSACleanup();

	return 0;
}

//-----------------------------------------------------------------
// WINSOCK FUNCTIONS
//-----------------------------------------------------------------

/* InitWinsock */
bool InitWinsock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0) 
	{
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);

		return false;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		printf("Could not find a usable version of Winsock.dll\n");

		WSACleanup();

		return false;
	}

	return true;
}

//-----------------------------------------------------------------
// PROCESS FUNCTIONS
//-----------------------------------------------------------------

/* CreateChildProcess */
static BOOL CreateChildProcess( 
							   LPSTR szCommandLine, 
							   LPCSTR szWorkingDir, 
							   HANDLE hStdoutWr, 
							   HANDLE hStdinRd, 
							   BOOL bShow, 
							   BOOL bBlock, 
							   INT nTimeout )
{
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFOA siStartInfo;
	BOOL bResult = FALSE;

	// Set up members of the PROCESS_INFORMATION structure.
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure.
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = hStdoutWr;
	siStartInfo.hStdOutput = hStdoutWr;
	siStartInfo.hStdInput = hStdinRd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	siStartInfo.wShowWindow = bShow ? SW_SHOW : SW_HIDE;

	// Create the child process.
	BOOL bFuncRetn = CreateProcessA( NULL,
		szCommandLine,    // command line
		NULL,             // process security attributes
		NULL,             // primary thread security attributes
		TRUE,             // handles are inherited
		CREATE_NO_WINDOW, // creation flags
		NULL,             // use parent's environment
		szWorkingDir,     // use client specified working dir
		&siStartInfo,     // STARTUPINFO pointer
		&piProcInfo );    // receives PROCESS_INFORMATION

	if( bFuncRetn == TRUE )
	{
		// Possibly wait until child process exits.
		if( bBlock == TRUE )
		{
			if( WAIT_TIMEOUT != WaitForSingleObject( piProcInfo.hProcess, nTimeout ) )
			{
				// Success
				bResult = TRUE;
			}

			// Kill the process we spawned
			(void)TerminateProcess( piProcInfo.hProcess, 0 );
		}
		else
		{
			// Success
			bResult = TRUE;
		}

		// Cleanup
		CloseHandle( piProcInfo.hProcess );
		CloseHandle( piProcInfo.hThread );
	}

	return bResult;
}

/* WriteToPipe */
static DWORD WriteToPipe( HANDLE hStdoutWr, LPSTR szToWrite, DWORD dwBytes )
{
	DWORD dwWritten;

	WriteFile( hStdoutWr, szToWrite, dwBytes, &dwWritten, NULL );

	return dwWritten;
}

/* ReadFromPipe */
static DWORD ReadFromPipe( HANDLE hStdinRd, LPSTR szToRead, DWORD dwBytes )
{
	DWORD dwRead;

	// Read output from the child process, and write to parent's STDOUT.
	for (;;)
	{
		if( !ReadFile( hStdinRd, szToRead, dwBytes, &dwRead, NULL ) || dwRead == 0 )
			break;
	}

	return dwRead;
}

/* Spawn a process */
BOOL SpawnProcess( LPSTR szCommandLine, BOOL bShow, BOOL bBlock, INT nTimeoutMS, LPSTR szOutput, DWORD dwOutputBytes, LPCSTR szWorkingDir )
{
	HANDLE hStdout, hChildStdinRd, hChildStdinWr, 
		hChildStdoutRdDup, hChildStdinWrDup, 
		hChildStdoutRd, hChildStdoutWr;
	SECURITY_ATTRIBUTES saAttr;
	BOOL bSuccess = FALSE;

	// Set the bInheritHandle flag so pipe handles are inherited.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Get the handle to the current STDOUT.
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	// Create a pipe for the child process's STDOUT.
	if( !CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0) )
		return FALSE;

	// Create noninheritable read handle and close the inheritable read
	// handle.
	bSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd, GetCurrentProcess(),
		&hChildStdoutRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);

	if( !bSuccess )
		return FALSE;

	CloseHandle(hChildStdoutRd);

	// Create a pipe for the child process's STDIN.
	if( !CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0) )
		return FALSE;

	// Duplicate the write handle to the pipe so it is not inherited.
	bSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdinWr, GetCurrentProcess(),
		&hChildStdinWrDup, 0, FALSE, DUPLICATE_SAME_ACCESS);

	if( !bSuccess )
		return FALSE;

	CloseHandle(hChildStdinWr);

	// Now create the child process.
	bSuccess = CreateChildProcess( szCommandLine, szWorkingDir, hChildStdoutWr, hChildStdinRd, bShow, bBlock, nTimeoutMS );
	if( !bSuccess )
		return FALSE;

	// Close the write end of the pipe before reading from the
	// read end of the pipe.
	if( !CloseHandle(hChildStdoutWr) )
		return FALSE;

	// Read from pipe that is the standard output for child process.
	ReadFromPipe( hChildStdoutRdDup, szOutput, dwOutputBytes );

	CloseHandle(hChildStdinRd);
	CloseHandle(hChildStdinWrDup);
	CloseHandle(hChildStdoutRdDup);

	return TRUE;
}

//-----------------------------------------------------------------
// ADB FUNCTIONS
//-----------------------------------------------------------------

/* Execute "adb.exe start-server" */
bool ADBStartServer()
{
	bool bSuccess = false;

	if ( SpawnProcess( "adb.exe start-server", FALSE ) == TRUE )
	{
		bSuccess = true;
	}

	return bSuccess;
}

/* Execute "adb.exe kill-server" possibly followed by "adb.exe start-server" */
bool ADBKillServer( bool bRestart )
{
	bool bSuccess = false;

	if( SpawnProcess( "adb.exe kill-server", FALSE ) == TRUE )
	{
		if( !bRestart || ADBStartServer() )
		{
			bSuccess = true;
		}
	}

	return bSuccess;
}

/* Execute "adb.exe forward <local> <device>" */
bool ADBForward( const char *szSerialNum, unsigned int iLocalPort, unsigned int iDevicePort )
{
	static char szCmdLine[128] = {0};

	if(szSerialNum != NULL)
		sprintf_s( szCmdLine, sizeof(szCmdLine), "%s -s %s forward tcp:%i tcp:%i", sDeviceMgr_ADB, szSerialNum, iLocalPort, iDevicePort );
	else
		sprintf_s( szCmdLine, sizeof(szCmdLine), "%s forward tcp:%i tcp:%i", sDeviceMgr_ADB, iLocalPort, iDevicePort );

	return SpawnProcess( szCmdLine, FALSE ) ? true : false;
}
