//--------------------------------------------------------------------------------------
// File: BinaryDataResource.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon Toolkit
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Crossplatform.h"

#if defined (_WIN32)
#include <windows.h>
#elif LINUX_OR_OSX
#include <sys/stat.h>
#include "Platform.h"
#include <stdio.h>
#endif

#include "PackedResource.h"



//--------------------------------------------------------------------------------------
// Name: ProcessBinaryDataResource()
// Desc: 
//--------------------------------------------------------------------------------------
bool ProcessBinaryDataResource( const char* strResourceID, const char* strFileName )
{
  
    if( NULL == strResourceID )
    {
      return false;
    }
        
    if( NULL == strFileName )
    {
        return false;
    }
    
    // Read the BinaryData
    void* pData;
    unsigned long dwSize = 0;

#if defined (_WIN32)
    {
        HANDLE hFile = CreateFileA( strFileName, GENERIC_READ, FILE_SHARE_READ,
                                    NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
        if( INVALID_HANDLE_VALUE == hFile )
            return false;


        dwSize = GetFileSize( hFile, NULL );
        pData  = (VOID*)new BYTE[dwSize];

        DWORD dwNumBytesRead;
        ReadFile( hFile, pData, dwSize, &dwNumBytesRead, NULL );
        CloseHandle( hFile );
    }
#elif LINUX_OR_OSX
    {
        FILE *fptr = fopen(strFileName, "rb");
        //if (fseek(fptr, 0, SEEK_END) == 0)
        //    dwSize = ftell(fptr);
        

	
        struct stat fileStat;
        if (stat(strFileName, &fileStat) == -1)
	{
	  return false;
	}
          
        dwSize = (unsigned long)fileStat.st_size;
	
        pData = (void*)new BYTE[dwSize];
	
	fseek(fptr, 0, SEEK_SET);
	fread(pData,dwSize,1,fptr);
	fclose(fptr);
	
    }
#endif

    // Write the BinaryData to the packed resource file
    {
      
        PackedResource::AddResource(strResourceID);
	PackedResource::AddSystemData(pData, dwSize);
    }
    
    // Cleanup and return
    delete[] (BYTE *)pData;
    return true;
}

