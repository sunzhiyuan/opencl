//--------------------------------------------------------------------------------------
// File: main.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Crossplatform.h"

#if defined (_WIN32)            // WIN32
#include <windows.h>
#elif LINUX_OR_OSX
#include "Platform.h"
#include <libgen.h>
#include <unistd.h>
#include <dirent.h>
#define _stricmp strcasecmp
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "PackedResource.h"
#include "tinyxml.h"


//--------------------------------------------------------------------------------------
// External funtions to process specific types of resources
//--------------------------------------------------------------------------------------
extern bool ProcessTextureResource(const CHAR* strResourceID, const CHAR* strFileName,
                                       const CHAR* strFormat, UINT32 nWidth, UINT32 nHeight,
                                       BOOL bGenMipMaps, BOOL bFlipImage, UINT32 nRotate,
                                       const CHAR* strMagFilter, const CHAR* strMinFilter,
                                       const CHAR* strMipFilter, const CHAR* strWrapMode);

extern bool ProcessTexture3DResource(const CHAR* strResourceID, const CHAR** strFileNames,
                                         const CHAR* strFormat,
                                         UINT32 nWidth, UINT32 nHeight, UINT32 nDepth,
                                         BOOL bGenMipMaps, BOOL bFlipImages, UINT32 nRotate,
                                         const CHAR* strMagFilter, const CHAR* strMinFilter,
                                         const CHAR* strMipFilter, const CHAR* strWrapMode);

extern bool ProcessCubeMapResource(const CHAR* strResourceID, const CHAR* strFileName[6],
                                       const CHAR* strFormat, UINT32 nSize,
                                       BOOL bGenMipMaps, BOOL bFlipImages, UINT32 nRotate,
                                       const CHAR* strMagFilter, const CHAR* strMinFilter,
                                       const CHAR* strMipFilter, const CHAR* strWrapMode);

extern bool ProcessBinaryDataResource(const CHAR* strResourceID, const CHAR* strFileName);


//--------------------------------------------------------------------------------------
// XML file reading support
//--------------------------------------------------------------------------------------
typedef TiXmlElement  XML_ELEMENT;
typedef TiXmlDocument XML_DOCUMENT;

bool LoadXmlDocument(const CHAR* strFileName, XML_DOCUMENT* pDocument)
{
    return pDocument->LoadFile(strFileName);
}

XML_ELEMENT* GetRootElement(XML_DOCUMENT* pDocument)
{
    return pDocument->RootElement();
}

const CHAR* GetElementName(XML_ELEMENT* pElement)
{ 
    return pElement->Value(); 
}

XML_ELEMENT* GetNextElement(XML_ELEMENT* pElement)
{ 
    return pElement->NextSiblingElement(); 
}

VOID GetStringAttribute(XML_ELEMENT* pElement, const CHAR* strAttribute, const CHAR** pstrResult)
{
    (*pstrResult) = pElement->Attribute(strAttribute);
}

VOID GetIntegerAttribute(XML_ELEMENT* pElement, const CHAR* strAttribute, INT32* pnResult)
{
    pElement->QueryIntAttribute(strAttribute, pnResult);
}

VOID GetUnsignedIntAttribute(XML_ELEMENT* pElement, const CHAR* strAttribute, UINT32* pnResult)
{
    pElement->QueryIntAttribute(strAttribute, (INT32*)pnResult);
}

VOID GetFloatAttribute(XML_ELEMENT* pElement, const CHAR* strAttribute, FLOAT* pfResult)
{
    const CHAR* strFloat = pElement->Attribute(strAttribute);
    if (strFloat)
    {
        (*pfResult) = (FLOAT)atof (strFloat);
    }
}

VOID GetBooleanAttribute(XML_ELEMENT* pElement, const CHAR* strAttribute, BOOL* pbResult)
{
    (*pbResult) = FALSE;

    const CHAR* strBoolean = pElement->Attribute(strAttribute);
    if (strBoolean)
    {
        if (!_stricmp(strBoolean, "TRUE"))
            (*pbResult) = TRUE;
        else if (!_stricmp(strBoolean, "1"))
            (*pbResult) = TRUE;
    }
}



//--------------------------------------------------------------------------------------
// Name: StringsAreEqual()
// Desc: Helper function for string comparisons
//--------------------------------------------------------------------------------------
inline bool StringsAreEqual(const CHAR* str1, const CHAR* str2)
{
    return !_stricmp(str1, str2);
}


void ReplaceSubstrInstances(std::string &original, const std::string& toReplace, const std::string& replaceWith)
{
   size_t pos= 0;
   
   pos=original.find(toReplace,pos);
   while(pos != std::string::npos)
   {
      original.replace(pos,toReplace.length(),replaceWith);
      pos+= replaceWith.length();      
      pos=original.find(toReplace,pos);
   }
}

//--------------------------------------------------------------------------------------
// Name: ParseTextureXMLElement()
// Desc: 
//--------------------------------------------------------------------------------------
bool ParseTextureXMLElement(XML_ELEMENT* pElement)
{
    const CHAR* strResourceID = NULL;
    const CHAR* strFileName   = NULL;
    const CHAR* strFormat     = NULL;
    const CHAR* strMagFilter  = NULL;
    const CHAR* strMinFilter  = NULL;
    const CHAR* strMipFilter  = NULL;
    const CHAR* strWrapMode   = NULL;
    UINT32 nWidth      = 0;
    UINT32 nHeight     = 0;
    BOOL   bGenMipMaps = FALSE;
    BOOL   bFlipImage  = FALSE;
    UINT32 nRotate     = 0;

    GetStringAttribute( pElement, "ResourceID",  &strResourceID);
    GetStringAttribute( pElement, "SrcFileName", &strFileName);
    GetStringAttribute( pElement, "DstFormat",   &strFormat );
    GetIntegerAttribute(pElement, "DstWidth",    (INT32*)&nWidth);
    GetIntegerAttribute(pElement, "DstHeight",   (INT32*)&nHeight);
    GetBooleanAttribute(pElement, "GenMipMaps",  &bGenMipMaps);
    GetBooleanAttribute(pElement, "FlipImage",   &bFlipImage);
    GetIntegerAttribute(pElement, "Rotate",      (INT32*)&nRotate);
    GetStringAttribute( pElement, "MagFilter",   &strMagFilter);
    GetStringAttribute( pElement, "MinFilter",   &strMinFilter);
    GetStringAttribute( pElement, "MipFilter",   &strMipFilter);
    GetStringAttribute( pElement, "WrapMode",    &strWrapMode);

    std::string original= strFileName, replacingStr="\/", toReplaceStr= "\\";
    ReplaceSubstrInstances(original,toReplaceStr, replacingStr);
    strFileName=original.c_str();
    
    
    
    printf ("Texture: \"%s\" -> \"%s\"", strFileName, strResourceID);
    fflush(stdout);

    return ProcessTextureResource(strResourceID, strFileName, strFormat,
                                         nWidth, nHeight, bGenMipMaps,
                                         bFlipImage, nRotate,
                                         strMagFilter, strMinFilter, strMipFilter,
                                         strWrapMode);

}


//--------------------------------------------------------------------------------------
// Name: ParseTexture3DXMLElement()
// Desc: 
//--------------------------------------------------------------------------------------
bool ParseTexture3DXMLElement(XML_ELEMENT* pElement)
{
    int i;
    const CHAR* strResourceID     = NULL;
    const CHAR* strFileNames[256];
    for(i=0;i<256;i++)
        strFileNames[i] = NULL;

    const CHAR* strFormat         = NULL;
    const CHAR* strMagFilter      = NULL;
    const CHAR* strMinFilter      = NULL;
    const CHAR* strMipFilter      = NULL;
    const CHAR* strWrapMode       = NULL;
    UINT32 MAX_FILENAME_SIZE = 128;
    UINT32 nWidth      = 0;
    UINT32 nHeight     = 0;
    UINT32 nDepth      = 0;
    BOOL   bGenMipMaps = FALSE;
    BOOL   bFlipImages = FALSE;
    UINT32 nRotate     = 0;

    GetStringAttribute( pElement, "ResourceID",  &strResourceID);
    GetStringAttribute( pElement, "DstFormat",   &strFormat );
    GetIntegerAttribute(pElement, "DstWidth",    (INT32*)&nWidth);
    GetIntegerAttribute(pElement, "DstHeight",   (INT32*)&nHeight);
    GetBooleanAttribute(pElement, "GenMipMaps",  &bGenMipMaps);
    GetBooleanAttribute(pElement, "FlipImages",  &bFlipImages);
    GetIntegerAttribute(pElement, "Rotate",      (INT32*)&nRotate);
    GetStringAttribute( pElement, "MagFilter",   &strMagFilter);
    GetStringAttribute( pElement, "MinFilter",   &strMinFilter);
    GetStringAttribute( pElement, "MipFilter",   &strMipFilter);
    GetStringAttribute( pElement, "WrapMode",    &strWrapMode);

    std::string replacingStr = "\/";
    std::string toReplaceStr = "\\";
    
    // Search all the attributes for source files
    const TiXmlAttribute* pAttribute     = pElement->FirstAttribute();
    const TiXmlAttribute* pLastAttribute = pElement->LastAttribute();
    while(pAttribute)
    {
        // Note: In order for this to work, TiXmlElement::Parse() was modified to
        // allow multiple instances of similarly-named attributes.
        if (0 == strcmp(pAttribute->Name(), "SrcFileName"))
	    {
	        //MAX_FILENAME_SIZE is 128 bytes. It's unlikely we will use more than this. 
            char *original = (char *)malloc(MAX_FILENAME_SIZE * sizeof(char));
            
            //Copy the string over from attribute, first MAX_FILENAME_SIZE only.
            strncpy(original, pAttribute->Value(), MAX_FILENAME_SIZE);

            //Replace all instances of "\" with "/" in the file path. 
            for(i = 0; i < strlen(original); i++)
            {
                if(original[i] == '\\')
                    original[i] = '/';
            }
           
            //Make our pointer point to the allocated memory.
            strFileNames[nDepth] = original;
	        
            ++nDepth;
	    }
	
        pAttribute = (pAttribute == pLastAttribute) ? NULL : pAttribute->Next();
    }

    printf ("Texture3D: \"%s\" -> \"%s\"", strFileNames[0], strResourceID);
    fflush(stdout);

    bool returnValue =  ProcessTexture3DResource(strResourceID, strFileNames, strFormat,
                                           nWidth, nHeight, nDepth,
                                           bGenMipMaps, bFlipImages, nRotate,
                                           strMagFilter, strMinFilter, strMipFilter,
                                           strWrapMode);

    //Free the memory that we allocated earlier to store the Texture3D names. 
    i = 0;
    while(strFileNames[i] != NULL){
        free((void *)strFileNames[i]);
        i++;
    }

    return returnValue;
}


//--------------------------------------------------------------------------------------
// Name: ParseCubeMapXMLElement()
// Desc: 
//--------------------------------------------------------------------------------------
bool ParseCubeMapXMLElement(XML_ELEMENT* pElement)
{
    const CHAR* strResourceID  = NULL;
    const CHAR* strFileName[6] = { NULL };
    const CHAR* strFormat      = NULL;
    const CHAR* strMagFilter   = NULL;
    const CHAR* strMinFilter   = NULL;
    const CHAR* strMipFilter   = NULL;
    const CHAR* strWrapMode    = NULL;
    UINT32 nSize       = 0;
    BOOL   bGenMipMaps = FALSE;
    BOOL   bFlipImages = TRUE;
    UINT32 nRotate     = 0;

    GetStringAttribute( pElement, "ResourceID",    &strResourceID);
    GetStringAttribute( pElement, "SrcFileNameXP", &strFileName[0]);
    GetStringAttribute( pElement, "SrcFileNameXN", &strFileName[1]);
    GetStringAttribute( pElement, "SrcFileNameYP", &strFileName[2]);
    GetStringAttribute( pElement, "SrcFileNameYN", &strFileName[3]);
    GetStringAttribute( pElement, "SrcFileNameZP", &strFileName[4]);
    GetStringAttribute( pElement, "SrcFileNameZN", &strFileName[5]);
    GetStringAttribute( pElement, "DstFormat",     &strFormat );
    GetIntegerAttribute(pElement, "DstSize",       (INT32*)&nSize);
    GetBooleanAttribute(pElement, "GenMipMaps",    &bGenMipMaps);
    GetBooleanAttribute(pElement, "FlipImages",    &bFlipImages);
    GetIntegerAttribute(pElement, "Rotate",        (INT32*)&nRotate);
    GetStringAttribute( pElement, "MagFilter",     &strMagFilter);
    GetStringAttribute( pElement, "MinFilter",     &strMinFilter);
    GetStringAttribute( pElement, "MipFilter",     &strMipFilter);
    GetStringAttribute( pElement, "WrapMode",      &strWrapMode);

    unsigned int curFilePathIndex= 0;
    std::string original= strFileName[curFilePathIndex], replacingStr="\/", toReplaceStr= "\\";
    ReplaceSubstrInstances(original,toReplaceStr, replacingStr);
    strFileName[curFilePathIndex]=original.c_str();
    ++curFilePathIndex;
    
    original= strFileName[curFilePathIndex];
    ReplaceSubstrInstances(original,toReplaceStr, replacingStr);
    strFileName[curFilePathIndex]=original.c_str();
    ++curFilePathIndex;
    
    original= strFileName[curFilePathIndex];
    ReplaceSubstrInstances(original,toReplaceStr, replacingStr);
    strFileName[curFilePathIndex]=original.c_str();
    ++curFilePathIndex;
    
    original= strFileName[curFilePathIndex];
    ReplaceSubstrInstances(original,toReplaceStr, replacingStr);
    strFileName[curFilePathIndex]=original.c_str();
    ++curFilePathIndex;
    
    original= strFileName[curFilePathIndex];
    ReplaceSubstrInstances(original,toReplaceStr, replacingStr);
    strFileName[curFilePathIndex]=original.c_str();
    ++curFilePathIndex;
    
    original= strFileName[curFilePathIndex];
    ReplaceSubstrInstances(original,toReplaceStr, replacingStr);
    strFileName[curFilePathIndex]=original.c_str();
    ++curFilePathIndex;
    
    
    printf ("CubeMap: \"%s\" -> \"%s\"", strFileName[0], strResourceID);
    fflush(stdout);

    return ProcessCubeMapResource(strResourceID, strFileName, strFormat,
                                         nSize, bGenMipMaps, bFlipImages, nRotate,
                                         strMagFilter, strMinFilter, strMipFilter,
                                         strWrapMode);
}


//--------------------------------------------------------------------------------------
// Name: ParseBinaryDataXMLElement()
// Desc: 
//--------------------------------------------------------------------------------------
bool ParseBinaryDataXMLElement(XML_ELEMENT* pElement)
{
    const CHAR* strResourceID = NULL;
    const CHAR* strFileName   = NULL;

    GetStringAttribute( pElement, "ResourceID",  &strResourceID);
    GetStringAttribute( pElement, "SrcFileName", &strFileName);

    std::string original= strFileName, replacingStr="\/", toReplaceStr= "\\";
    ReplaceSubstrInstances(original,toReplaceStr, replacingStr);
    strFileName=original.c_str();
    
    printf ("BinaryData: \"%s\" -> \"%s\"", strFileName, strResourceID);
    fflush(stdout);

    return ProcessBinaryDataResource(strResourceID, strFileName);
}


//--------------------------------------------------------------------------------------
// Name: ParseInputXMLFile()
// Desc: 
//--------------------------------------------------------------------------------------
bool ParseInputXMLFile(const CHAR* strFileName)
{
    // Load the XML file
    XML_DOCUMENT xmlFile;
    if (!LoadXmlDocument(strFileName, &xmlFile))
    {
        printf ("Error: file %s not found.\n", strFileName);
        return false;
    }

    // Walk through and parse all the elements in the file
    XML_ELEMENT* pElement = GetRootElement(&xmlFile);
    while(pElement)
    {
        bool result = false;

        UINT32 nBeforeSystemSize = PackedResource::GetSystemDataOffset();
        UINT32 nBeforeBufferSize = PackedResource::GetBufferDataOffset();

        const char* strType = GetElementName(pElement);

        if (StringsAreEqual(strType, "Texture"))
            result = ParseTextureXMLElement(pElement);
        else if (StringsAreEqual(strType, "Texture3D"))
            result = ParseTexture3DXMLElement(pElement);
        else if (StringsAreEqual(strType, "CubeMap"))
            result = ParseCubeMapXMLElement(pElement);
        else if (StringsAreEqual(strType, "BinaryData"))
            result = ParseBinaryDataXMLElement(pElement);
	else
	    printf("[LOGIC ERROR] Unrecognized XML type attribute..\n");

        // TODO: Add checks for other element types here (cubemaps, shaders, etc.)

        if (!result)
        {
            printf("...XML FAILED!\n");
            return result;
        }

        UINT32 nAfterSystemSize = PackedResource::GetSystemDataOffset();
        UINT32 nAfterBufferSize = PackedResource::GetBufferDataOffset();
        if (nAfterBufferSize - nBeforeBufferSize > 0)
            printf ("...(%ld bytes)\n", nAfterBufferSize - nBeforeBufferSize);
        else
            printf ("...(%ld bytes)\n", nAfterSystemSize - nBeforeSystemSize);

        pElement = GetNextElement(pElement);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////

BOOL IsDirectory(CHAR *strInput)
{
#if defined (_WIN32)
    struct _stat buff;
    if (_stat(strInput, &buff) == 0)
#elif LINUX_OR_OSX
    struct stat buff;
    if (stat(strInput, &buff) == 0)
#endif
    {
        if (buff.st_mode & S_IFDIR)
        {
            return TRUE;
        }
        else if (buff.st_mode & S_IFREG)
        {
            return FALSE;
        }
    }

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOL ProcessXMLInput(const CHAR *strInputFilePath, const CHAR *strOutputFilePath, const CHAR *strWorkingPath) 
{
    // Read the input file
    if (!ParseInputXMLFile(strInputFilePath))
    {
        return FALSE;
    }
#if defined (_WIN32)
    SetCurrentDirectory(strWorkingPath);
#elif LINUX_OR_OSX
    chdir(strWorkingPath);
#endif

    // Write the output file
    if (!PackedResource::WriteOutputFile(strOutputFilePath))
    {
        printf("Error: Could not create file %s.\n", strOutputFilePath);
        return FALSE;
    }

    PackedResource::ClearResources();

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void ProcessXMLInputDirectory(const CHAR *strInputFilePath, const CHAR *strOutputFilePath, const CHAR *strPreviousDirectory) 
{
#if defined (_WIN32)
    SetCurrentDirectory(strInputFilePath);

    // process a full directory of XML's
    // Windows approach (will need xplatform)
    WIN32_FIND_DATA findData;
    HANDLE hFind;

    std::string strSearch = "*.*"; 

    hFind = FindFirstFile(strSearch.c_str(), &findData);
    std::string filename;

    // find the directories
    do
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)   // if we found a folder, dive into it
        {
            filename = findData.cFileName;
            if (filename[0] == '.') continue;       // ignore current and prev directory identifiers
            std::string strCurrentPath(strInputFilePath);
            strCurrentPath += "\\" + filename;
            ProcessXMLInputDirectory(strCurrentPath.c_str(), strOutputFilePath, strPreviousDirectory);
        }
    } while (FindNextFile(hFind, &findData));
    FindClose(hFind);

    SetCurrentDirectory(strInputFilePath);      // reset current directory in case any recursive current directory stuff was done

    // done with directories
    hFind = FindFirstFile(strSearch.c_str(), &findData);

    do
    {
        //now going to parse through the files
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // found one, process it
            filename = findData.cFileName;

            // just to verify..
            if (filename.find(".xml") != std::string::npos)
            {
                // build file path
                std::string fullInputPath = strInputFilePath;
                if (*fullInputPath.rbegin() != '\\')
                    fullInputPath += "\\";

                fullInputPath += filename;

                std::string fullOutputPath = strOutputFilePath;
                if (*fullOutputPath.rbegin() != '\\')
                    fullOutputPath += "\\";
                fullOutputPath += filename.substr(0, filename.length()-4) + std::string(".pak");

                // found xml - process it
                if (ProcessXMLInput(fullInputPath.c_str(), fullOutputPath.c_str(), strPreviousDirectory))
                {
                    // Output
                    printf("%s -> %s\n", filename.c_str(), fullOutputPath.c_str());
                }

                SetCurrentDirectory(strInputFilePath);
            }
        }
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);

#elif LINUX_OR_OSX

    chdir(strInputFilePath);

    std::string filename;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {           
            if (dir->d_type == DT_DIR)	// if we found a directory, dive into it
            {
                filename = dir->d_name;
                if (filename[0] == '.') continue;	// ignore current and prev directories
                std::string strCurrentPath(strInputFilePath);
                strCurrentPath += "/" + filename;
                ProcessXMLInputDirectory(strCurrentPath.c_str(), strOutputFilePath, strPreviousDirectory);
            }
        }
    }
    closedir(d);

    chdir(strInputFilePath);	// reset current directory in case any recursive current dir stuff done

    // done with directories
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG)	// found a file
            {
                filename = dir->d_name;

                // just to verify..
                if (filename.find(".xml") != std::string::npos)
                {
                    // build file path
                    std::string fullInputPath = strInputFilePath;
                    if (*fullInputPath.rbegin() != '/')
                        fullInputPath += "/";

                    fullInputPath += filename;

                    std::string fullOutputPath = strOutputFilePath;
                    if (*fullOutputPath.rbegin() != '/')
                        fullOutputPath += "/";
                    fullOutputPath += filename.substr(0, filename.length()-4) + std::string(".pak");

                    // found xml - process it
                    if (ProcessXMLInput(fullInputPath.c_str(), fullOutputPath.c_str(), strPreviousDirectory))
                    {
                        // Output
                        printf("%s -> %s\n", filename.c_str(), fullOutputPath.c_str());
                    }

                    chdir(strInputFilePath);
                }               
            }       // if file found
        }   // while
    }

    closedir(d);
#endif
}



//////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------------------------
// Name: main()
// Desc: 
//--------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Print the logo
    printf ("Snapdragon Toolkit Resource Packer\n");

    // Print the usage
    if (argc < 2 || argc > 3)
    {
        printf ("Packs multiple resources (textures, etc.) into a single binary file.\n");
        printf ("\n");
        printf ("ResourcePacker source [destination]\n");
        printf ("  source        The source XML file describing the resources.\n");
        printf ("  destination   The name of the resulting binary (.pak) file.\n");
        printf ("\n");
        printf ("The source XML file should contain one or more elements describing\n");
        printf ("each resource. An example XML file might look like:\n");
        printf ("   <?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"); 
        printf ("\n");
        printf ("   <Texture\n");
        printf ("       ResourceID = \"TEST IMAGE\"\n");
        printf ("       SrcFileName   = \"..\\Textures\\Test.bmp\"\n");
        printf ("       DstFormat     = \"ATC\"\n");
        printf ("       DstWidth      = \"64\" \n");
        printf ("       DstHeight     = \"64\" \n");
        printf ("       GenMipMaps    = \"TRUE\"\n");
        printf ("   />\n");
        return 1;
    }

#if defined (_WIN32)
    CHAR strPreviousDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, strPreviousDirectory);

    // Construct the input and output filenames
    CHAR strInputFilePath[MAX_PATH] = {0};
    CHAR strOutputFilePath[MAX_PATH] = {0};
    {
        // Set the current working directory to match the input file
        _fullpath(strInputFilePath, argv[1], MAX_PATH);

        CHAR strDrive[_MAX_DRIVE], strDir[_MAX_DIR], strName[_MAX_PATH], strExt[_MAX_EXT];
        CHAR strWorkingDirectory[MAX_PATH];
        _splitpath(strInputFilePath, strDrive, strDir, strName, strExt);
        _makepath(strWorkingDirectory, strDrive, strDir, NULL, NULL);
        
        // the _stat function used in IsDirectory has trouble with a path ending in '\' so chopping it off it's there
        if (strInputFilePath[strlen(strInputFilePath)-1] == '\\')
            strInputFilePath[strlen(strInputFilePath)-1] = '\0';

        BOOL bProcessDirectory = IsDirectory(strInputFilePath);

        if (argc == 3)  // an output was defined
        {
            strcpy(strOutputFilePath, argv[2]);
        }
        else            // no output defined
        {
            if (!bProcessDirectory)
                _makepath(strOutputFilePath, strDrive, strDir, strName, ".pak");
            else
                _makepath(strOutputFilePath, strDrive, strDir, NULL, NULL); // set to directory, same as input
        }

        if (!bProcessDirectory)
        {
            SetCurrentDirectory(strWorkingDirectory);

            // process one XML file
            ProcessXMLInput(strInputFilePath, strOutputFilePath, strPreviousDirectory);
        }
        else
        {
            ProcessXMLInputDirectory(strInputFilePath, strOutputFilePath, strPreviousDirectory);
        }
    }

    // restore original path
    SetCurrentDirectory(strPreviousDirectory);

#elif LINUX_OR_OSX

    // store current directory
    CHAR strPreviousDirectory[PATH_MAX];
    getcwd(strPreviousDirectory, PATH_MAX);

    // Construct the input and output filenames
    CHAR strInputFilePath[PATH_MAX] = {0};
    CHAR strOutputFilePath[PATH_MAX] = {0};
    {
        // Set the current working directory to match the input file
        if (realpath(argv[1], strInputFilePath) == 0)
        {
            printf("Error: cannot resolve input path %s. Please ensure the input path is valid.\n", argv[1]);
            return 1;
        }

        CHAR strWorkingDirectory[PATH_MAX] = {0};
        strcpy(strWorkingDirectory, strInputFilePath);
        dirname(strWorkingDirectory);		// dirname modifies the input char*

        
        // the stat function used in IsDirectory has trouble with a path ending in '\' so chopping it off
        if (strInputFilePath[strlen(strInputFilePath)-1] == '/')
            strInputFilePath[strlen(strInputFilePath)-1] = '\0';

        BOOL bProcessDirectory = IsDirectory(strInputFilePath);

        if (argc == 3)  // an output was defined
        {
            strcpy(strOutputFilePath, argv[2]);
        }
        else            // no output defined
        {
            if (!bProcessDirectory)
            {
                //_makepath(strOutputFilePath, strDrive, strDir, strName, ".pak");
                // define fullpath for output
                strcpy(strOutputFilePath, strWorkingDirectory);
                strcat(strOutputFilePath, "/");

                // get rid of the filename extension so we can replace it - probably easier way to do this
                CHAR filename[PATH_MAX];
                strcpy(filename, basename(strInputFilePath));
                CHAR *dot = strrchr(filename, '.');
                if (dot) dot[1] = 0;

                // replace extension with .pak
                strcat(strOutputFilePath, ".pak");
            }
            else
            {
                // set to the input directory
                strcpy(strOutputFilePath, strWorkingDirectory);
            }
        }

        if (!bProcessDirectory)
        {
            chdir(strWorkingDirectory);

            // process one XML file
            ProcessXMLInput(strInputFilePath, strOutputFilePath, strPreviousDirectory);
        }
        else
        {
            printf("Processing directory input %s to output %s\n", strInputFilePath, strOutputFilePath);
            ProcessXMLInputDirectory(strInputFilePath, strOutputFilePath, strPreviousDirectory);
        }
    }

    // restore original path
    chdir(strPreviousDirectory);

#endif

    // Return success
    return 0;
}

