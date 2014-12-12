//--------------------------------------------------------------------------------------
// File: PackedResource.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Crossplatform.h"
#if defined (_WIN32)
#include <windows.h>
#include <io.h>
#define access _access
#elif LINUX_OR_OSX
#include "Platform.h"
//#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#endif

#include "PackedResource.h"
#include "Image.h"


//--------------------------------------------------------------------------------------
// Name: class CDataBuffer
// Desc: Helper class for managing a buffer of binary data
//--------------------------------------------------------------------------------------
class CDataBuffer
{
    BYTE*     m_pData;
    UINT32    m_nSize;
    UINT32    m_nMaxSize;
public:
    bool   WriteData(const VOID* pData, UINT nSize)
    {
        // Align data to 4-byte boundaries
        UINT32 nAlignedSize = (nSize + 3) & ~3;

        // Allocate memory for the data
        if (m_nMaxSize == 0)
        {
            m_pData = (BYTE*)malloc(nAlignedSize);
            m_nMaxSize = nAlignedSize;
        }
        else if (m_nSize + nAlignedSize > m_nMaxSize)
        {
            while(m_nSize + nAlignedSize > m_nMaxSize)
                m_nMaxSize = m_nMaxSize * 2;

            m_pData = (BYTE*)realloc(m_pData, m_nMaxSize);
        }

        // Add the data to the buffer
        memcpy(&m_pData[m_nSize], pData, nSize);
        m_nSize += nAlignedSize;
        return true;
    }
    VOID*     GetData() { return m_pData; }
    UINT32    GetSize() { return m_nSize; }

    VOID      ClearData() { free(m_pData); m_pData = NULL; m_nMaxSize = 0; m_nSize = 0; }

    CDataBuffer () { m_pData = NULL; m_nSize = 0; m_nMaxSize = 0; }
};


//--------------------------------------------------------------------------------------
// Static data for the packed resource
//--------------------------------------------------------------------------------------
static RESOURCE      g_Resources[1000];
static UINT32        g_nNumResources;

static CDataBuffer   g_SystemData;
static CDataBuffer   g_BufferData;


//--------------------------------------------------------------------------------------
// Accessors for the packed resource
//--------------------------------------------------------------------------------------
RESOURCE* PackedResource::GetResources()        { return g_Resources; }
UINT32    PackedResource::GetNumResources()     { return g_nNumResources; }
VOID*     PackedResource::GetSystemData()       { return g_SystemData.GetData(); }
VOID*     PackedResource::GetBufferData()       { return g_BufferData.GetData(); }
UINT32    PackedResource::GetSystemDataOffset() { return g_SystemData.GetSize(); }
UINT32    PackedResource::GetBufferDataOffset() { return g_BufferData.GetSize(); }


//--------------------------------------------------------------------------------------
// Name: AddResource()
// Desc: 
//--------------------------------------------------------------------------------------
VOID PackedResource::AddResource(const CHAR* strName)
{
    RESOURCE* pResource = &g_Resources[g_nNumResources++];

    pResource->strName = new CHAR[128];
    strcpy(pResource->strName, strName);

    pResource->nSystemDataOffset = GetSystemDataOffset();
    pResource->nBufferDataOffset = GetBufferDataOffset();
}


//--------------------------------------------------------------------------------------
// Name: AddSystemData()
// Desc: 
//--------------------------------------------------------------------------------------
bool PackedResource::AddSystemData(const VOID* pData, UINT32 nSize)
{ 
    return g_SystemData.WriteData(pData, nSize); 
}


//--------------------------------------------------------------------------------------
// Name: AddBufferData()
// Desc: 
//--------------------------------------------------------------------------------------
bool PackedResource::AddBufferData(const VOID* pData, UINT32 nSize)
{ 
    return g_BufferData.WriteData(pData, nSize);
}


//--------------------------------------------------------------------------------------
// Name: WriteOutputFile()
// Desc: 
//--------------------------------------------------------------------------------------
bool PackedResource::WriteOutputFile(const CHAR* strOutputFilename)
{
    unsigned long dwNumBytesWritten;
    unsigned long     nNumResources   = PackedResource::GetNumResources();
    RESOURCE* pResources      = PackedResource::GetResources();
    unsigned long     nSystemDataSize = PackedResource::GetSystemDataOffset();
    unsigned long     nBufferDataSize = PackedResource::GetBufferDataOffset();
    VOID*     pSystemData     = PackedResource::GetSystemData();
    VOID*     pBufferData     = PackedResource::GetBufferData();

#if defined (_WIN32)
    // Make sure the output directory exists
    CHAR strDrive[_MAX_DRIVE], strDir[_MAX_DIR], strOutputDir[MAX_PATH];
    _splitpath(strOutputFilename, strDrive, strDir, NULL, NULL);
    _makepath(strOutputDir, strDrive, strDir, NULL, NULL);
    char strMasterPath[MAX_PATH] = {0};
#elif LINUX_OR_OSX
    CHAR strOutputDir[PATH_MAX];
    CHAR strOutputFullPath[PATH_MAX];
    strcpy(strOutputFullPath, strOutputFilename);
    char * parentDir = dirname(strOutputFullPath);		// dirname modifies the input char*
    strcpy(strOutputDir, parentDir);
    char strMasterPath[PATH_MAX] = {0};
#endif

    char *dir = strtok(strOutputDir, "\\/");
    while (dir != NULL)
    {
        strcat(strMasterPath, dir);


            // TODO: we could verify that this is actually a directory before creating it..
#if defined (_WIN32)
        // see if it exists, if it doesn't, create it
        if (access(strMasterPath, 0) != 0)
        {
            CreateDirectory(strMasterPath, NULL);
		}
#elif LINUX_OR_OSX
        // see if it exists, if it doesn't, create it
        if (access(strMasterPath, F_OK) != 0)
        {
			mkdir(strMasterPath, S_IRWXU | S_IRWXG | S_IRWXO);
		}
#endif
        
        
        // get the next folder in the path
        dir = strtok(NULL, "\\/");

        // append the separator
#if defined (_WIN32)
        strcat(strMasterPath, "\\");
#elif LINUX_OR_OSX
        strcat(strMasterPath, "/");
#endif
    }

    // Create the file
#if defined (_WIN32)
    HANDLE hFile = CreateFile(strOutputFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        return false;
    }
#elif LINUX_OR_OSX
    FILE *fptr = fopen(strOutputFilename, "wb");
    if (!fptr)
    {
        printf("Could not open file for writing: %s\n", strOutputFilename);
        return false;
    }
#endif

    // Compute the resource and string table sizes
    UINT32 nResourceTableSize = nNumResources * sizeof (RESOURCE);

    UINT32 nStringTableSize = 0;
    for (unsigned long i = 0; i < nNumResources; i++)
        nStringTableSize += strlen(pResources[i].strName)+1;

    // Write the file header
    PAK_FILE_HEADER header;
    header.nFileSignature  = PAK_FILE_SIGNATURE;
    header.nSystemDataSize = nResourceTableSize + nStringTableSize + nSystemDataSize;
    header.nBufferDataSize = nBufferDataSize;
    header.nNumResources   = nNumResources;

#if defined (_WIN32)
    if (!WriteFile(hFile, &header, sizeof (PAK_FILE_HEADER), &dwNumBytesWritten, NULL))
        return false;
#elif LINUX_OR_OSX
    if (!fwrite(&header, sizeof(PAK_FILE_HEADER), 1, fptr))
    {
        printf("Could not write PAK_FILE_HEADER.\n");
        return false;
    }
#endif

    // Write the resource table
    RESOURCE resource;
    resource.strName           = (CHAR*)nResourceTableSize;
    resource.nSystemDataOffset = 0;
    resource.nBufferDataOffset = 0;
    resource.pObject           = NULL;

    for (unsigned long i = 0; i < nNumResources; i++)
    {
        resource.nSystemDataOffset = nResourceTableSize + nStringTableSize + pResources[i].nSystemDataOffset;
        resource.nBufferDataOffset = pResources[i].nBufferDataOffset;

#if defined (_WIN32)
        if (!WriteFile(hFile, &resource, sizeof (RESOURCE), &dwNumBytesWritten, NULL))
            return false;
#elif LINUX_OR_OSX
        if (!fwrite(&resource, sizeof(RESOURCE), 1, fptr))
        {
            printf("Could not write RESOURCE struct.\n");
            return false;
        }
#endif
        resource.strName += strlen(pResources[i].strName)+1;
    }

    // Write the string table
    for (unsigned long i = 0; i < nNumResources; i++)
    {
#if defined (_WIN32)
        if (!WriteFile(hFile, pResources[i].strName, strlen(pResources[i].strName)+1, &dwNumBytesWritten, NULL))
            return false;
#elif LINUX_OR_OSX
        if (!fwrite(pResources[i].strName, strlen(pResources[i].strName)+1, 1, fptr))
        {
            printf("Could not write resource %s\n", pResources[i].strName);
            return false;
        }
#endif
    }

#if defined (_WIN32)
    // Write the system data
    if (!WriteFile(hFile, pSystemData, nSystemDataSize, &dwNumBytesWritten, NULL))
        return false;

    // Write the buffer data
    if (!WriteFile(hFile, pBufferData, nBufferDataSize, &dwNumBytesWritten, NULL))
        return false;

    // Cleanup and return
    CloseHandle(hFile);
#elif LINUX_OR_OSX
    if (!fwrite(pSystemData, nSystemDataSize, 1, fptr))
        return false;
    if (!fwrite(pBufferData, nBufferDataSize, 1, fptr))
        return false;

    fclose(fptr);
#endif
    return true;
}


//--------------------------------------------------------------------------------------
// Name: WriteImageData()
// Desc: Writes image data to a packed resource
//--------------------------------------------------------------------------------------
bool PackedResource::WriteImageData(CImage* pImage)
{
    if (pImage->m_nFlags & IMG_COMPRESSED)
    {
        if (!PackedResource::AddBufferData(pImage->m_pData, pImage->m_nMipChainSize))
            return false;
    }
    else
    {
        UINT32 nMipWidth  = pImage->m_nWidth;
        UINT32 nMipHeight = pImage->m_nHeight;

        for (UINT32 i=0; i<pImage->m_nNumLevels; i++)
        {
            pImage->Resize(nMipWidth, nMipHeight);

            if (!PackedResource::AddBufferData(pImage->m_pData, pImage->m_nBaseSize))
                return false;

            if (nMipWidth  > 1) nMipWidth  = nMipWidth  / 2;
            if (nMipHeight > 1) nMipHeight = nMipHeight / 2;
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////

VOID PackedResource::ClearResources()
{
    for (int i = 0; i < g_nNumResources; ++i)
    {
        delete [] g_Resources[i].strName;
        g_Resources[i].strName = NULL;
    }

    g_nNumResources = 0;
    g_BufferData.ClearData();
    g_SystemData.ClearData();
}
