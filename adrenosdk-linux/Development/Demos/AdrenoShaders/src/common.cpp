//=================================================================================================
// FILE:        Common.cpp
//	
// DESCRIPTION: OGLES 2.0 Sample Project common functions
//
// Author:        QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//                  Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                            All Rights Reserved.
//                         QUALCOMM Proprietary/GTDR
//=================================================================================================
// Common Sample Includes
#include "common.h"

#if defined(OS_ANDROID)
    #ifndef MAX_PATH
        #define MAX_PATH 256
    #endif
#endif // defined(OS_ANDROID)


//------------------------------------------------------------------------
void CovertLocalFileName(const char* pszFileName, char* pszFullFileName, UINT32 nFullFileNameBufSize)
//------------------------------------------------------------------------
{
#if defined(OS_WIN32)

    strncpy_s(pszFullFileName, nFullFileNameBufSize, pszFileName, _TRUNCATE);

#elif defined(OS_ANDROID)

    strncpy(pszFullFileName, pszFileName, nFullFileNameBufSize);

#elif defined(OS_WM)

    WCHAR wszFullModuleFileName[MAX_PATH];
    char  szRootDirName[MAX_PATH];

    if (strchr(pszFileName, ':') != NULL || pszFileName[0]=='\\')
    {
        // filename contains driver letter or starts at root, consider it full
        strncpy(pszFullFileName, pszFileName, nFullFileNameBufSize);
        return;
    }

    //get the root directory for this module
    GetModuleFileName((HMODULE)GetCurrentProcessId(), (LPWSTR)wszFullModuleFileName, MAX_PATH);
    //convert wide character sting to multibyte character string
    wcstombs(szRootDirName, wszFullModuleFileName, MAX_PATH);
    //remove the leading filename to obtain the root dir name
    *(strrchr(szRootDirName, '\\')) = '\0';
    //concatenate the rootdirname with the relative filename 
    strncpy(pszFullFileName, szRootDirName, nFullFileNameBufSize);
    strncat(pszFullFileName, "\\", nFullFileNameBufSize - strlen(pszFullFileName));
    strncat(pszFullFileName, pszFileName, nFullFileNameBufSize - strlen(pszFullFileName));

#endif
}

//------------------------------------------------------------------------
BOOL LoadFileIntoBuffer(const char* pszFileName, void** ppBuffer, UINT32* pBufferSize)
//------------------------------------------------------------------------
{
    FILE *  pFile = NULL;

    void *  pBuffer = NULL;
    UINT32  nBufferSize = 0;
    UINT32  nBytesRead = 0;
    char    szFullFileName[MAX_PATH] = {0};

    *pBufferSize = 0;
    *ppBuffer = NULL;

 
    CovertLocalFileName(pszFileName, szFullFileName, MAX_PATH);

#ifdef OS_WIN32
    fopen_s(&pFile, szFullFileName, "rb");
#else
    pFile = fopen(szFullFileName, "rb");
#endif
    if(!pFile)
    {
        return false;
    }

    // grab size of file
    fseek(pFile, 0, SEEK_END);
    nBufferSize = (UINT32)ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    // Always allocate an extra byte for NULL termination since may be loading text files
    pBuffer = malloc(nBufferSize + 1);
    if (pBuffer == NULL)
    {
        fclose(pFile);
        return false;
    }

#if defined(OS_WIN32) || defined(OS_WM)
    ZeroMemory(pBuffer, nBufferSize + 1);
#else
    memset(pBuffer, 0, nBufferSize + 1);
#endif // defined(OS_WIN32) || defined(OS_WM)


    nBytesRead = (UINT32)fread(pBuffer, 1, nBufferSize, pFile);
    if(nBytesRead != nBufferSize)
    {
        fclose(pFile);
        free(pBuffer);
        return false;
    }

    // No longer need the file
    fclose(pFile);

    // Assign return values
    *pBufferSize = nBufferSize;
    *ppBuffer = pBuffer;

    return true;
}
