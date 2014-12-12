//--------------------------------------------------------------------------------------
// ShaderPreprocessor.cpp
//
// Preprocesses a shader text file into a final format.
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#if defined(_WIN32)
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>

#include "Crossplatform.h"
#if defined(_WIN32)
#include <direct.h>
#include <io.h>

#define chdir _chdir
#elif LINUX_OR_OSX
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define stricmp strcasecmp		// address POSIX issue for stricmp
#endif

// The global output string
std::string g_strOutput;
std::string g_strVertexShaderOutput;
std::string g_strFragmentShaderOutput;
std::string g_strVertexShaderFilename;
std::string g_strFragmentShaderFilename;

// The global input path
#if defined (_WIN32)
	CHAR g_strDrive[_MAX_DRIVE];
	CHAR g_strDir[_MAX_DIR];
	CHAR g_strName[_MAX_PATH];
#elif LINUX_OR_OSX
	typedef unsigned char BYTE;
	char g_strDrive[255];
	char g_strDir[255];
	char g_strName[PATH_MAX];
#endif

char* g_strDefines[100] = {0};
int g_nNumDefines     = 0;

//--------------------------------------------------------------------------------------
// Name: stristr()
// Desc: Case-insensitive string searching helper function
//--------------------------------------------------------------------------------------
char* stristr(const char* str1, const char* str2)
{
    if (!*str2)
        return((char*)str1);

    for(char* cp = (char*)str1; *cp; cp++)
    {
        char* s1 = cp;
        char* s2 = (char*)str2;

        while(*s1 && *s2 && !(tolower(*s1)-tolower(*s2)))
        {
            s1++, s2++;
        }

        if (!*s2)
            return cp;
    }

    return NULL;
}


//--------------------------------------------------------------------------------------
// Name: LoadFile()
// Desc: 
//--------------------------------------------------------------------------------------
BYTE* LoadFile(const char* strFileName)
{
    FILE* file = fopen(strFileName, "rb");
    if (NULL == file)
    {
#if LINUX_OR_OSX
	char strPathName[PATH_MAX];
	strcat(strPathName, g_strDir);
	strcat(strPathName, "/");
	strcat(strPathName, strFileName);
#elif defined (_WIN32)
        CHAR strPathName[MAX_PATH];
        _makepath(strPathName, g_strDrive, g_strDir, strFileName, "");
#endif


        file = fopen(strPathName, "rb");
        if (NULL == file)
        {
            printf ("Error: Could not find the file %s\n", strFileName);
            return NULL;
        }
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    BYTE* pData = new BYTE[length+1];
    fread(pData, length, 1, file);
    fclose(file);
    pData[length] = 0;

    return pData;
}

bool ParseAttributeStatements(char* strCurrent) 
{
    if (strCurrent == stristr(strCurrent, "attribute"))
    {
        char strCopy[256];
        strcpy(strCopy, strCurrent);
        strtok(strCopy, " :;");
        char* strType    = strtok(NULL, " :;");
        char* strName    = strtok(NULL, " :;");
        char* strColon   = strtok(NULL, " ;");
        char* strUsage   = strtok(NULL, " :;");
        char* strComment = strtok(NULL, "");
        if (strComment == NULL)
            strComment = (char *)"";

        if (strColon && strColon[0] == ':')
        {
            g_strOutput += "attribute ";
            g_strOutput += strType;
            g_strOutput += " ";
            g_strOutput += strUsage;
            g_strOutput += ";\n";

            g_strOutput += "#define ";
            g_strOutput += strName;
            g_strOutput += " ";
            g_strOutput += strUsage;
            g_strOutput += " ";
            g_strOutput += strComment;
            g_strOutput += "\n";
            return true;
        }
    }

    return false;
}



//--------------------------------------------------------------------------------------
// Name: ParseShaderFromFile()
// Desc: Parses a shader text file to extract a specific portion
//--------------------------------------------------------------------------------------
bool ParseShaderFromFile(const char* strFileName, const char* strTag = NULL)
{
    // Read in the file
    char* pShaderData = (char*)LoadFile(strFileName);
    if (NULL == pShaderData)
        return false;

    // Process the ASCII shader data
    char* str = pShaderData;
    bool bInsideAnyTag     = false;
    bool bInsideCorrectTag = false;
    bool bCorrectTagWasFound = false;

    // strTag will be NULL, if we are processing #include files
    if (strTag == NULL)
    {
        bInsideAnyTag = true;
        bInsideCorrectTag = true;
        bCorrectTagWasFound = true;
        strTag = "";
    }

    while(1)
    {
        // Isolate the current line
        char* pEOL = str;
        while(*pEOL && *pEOL != '\n')
            pEOL++;
        if (*pEOL == '\0')
            break;
        *pEOL = '\0';
        char* strCurrent = str;
        str += strlen(str) + 1;

        // Check for tags
        if (*strCurrent == '[')
        {
            bInsideAnyTag     = true;
            bInsideCorrectTag = stristr(strCurrent, strTag) ? true : false;

            if (bInsideCorrectTag)
                bCorrectTagWasFound = true;

            continue;
        }

        if (bInsideCorrectTag)
        {
            // Handle include statements
            if (*strCurrent == '#' && stristr(strCurrent, "#include"))
            {
                char strCopy[256];
                strcpy(strCopy, strCurrent);
                strtok(strCopy, " ");
                char* strFile  = strtok(NULL, "\"");

                if (false == ParseShaderFromFile(strFile, NULL))
                    return false;

                continue;
            }

            // Check for 'attribute' statements
            if (ParseAttributeStatements(strCurrent))
                continue;
        }
        else
        {
            // Skip this line, since it's not part of our tagged body of text
            continue;
        }

        // If we get here, keep the string
        g_strOutput += strCurrent;
        g_strOutput += "\n";
    }

    delete pShaderData;

    if (false == bCorrectTagWasFound)
    {
        printf ("Error: Couldn't find the tag: %s\n", strTag);
        return false;
    }

    return true;
}

bool ExtractShaders(const char* strFileName, const char* strTag = NULL)
{
    // Read in the file
    char* pShaderData = (char*)LoadFile(strFileName);
    if (NULL == pShaderData)
        return false;

    // Process the ASCII shader data
    char* str = pShaderData;
    bool bInsideAnyTag     = false;
    bool bInsideVertexShaderTag = false;
    bool bInsideFragmentShaderTag = false;
    bool bNoTags = true;

    // strTag will be NULL, if we are processing #include files
    if (strTag == NULL)
    {
        bInsideAnyTag = true;
        bInsideVertexShaderTag = true;
        bInsideFragmentShaderTag = true;
        strTag = "";
    }

    while (1)
    {
        // Isolate the current line
        char* pEOL = str;
        while(*pEOL && *pEOL != '\n')
            pEOL++;
        if (*pEOL == '\0')
            break;
        *pEOL = '\0';
        char* strCurrent = str;
        str += strlen(str) + 1;

        // Check for tags
        if (*strCurrent == '[')
        {
            bInsideAnyTag     = true;
            bNoTags = false;
         
            // grab last 3 characters of tag
            // All shaders in toolkit currently using this convention (April 2013)
            // "VS]" for vertex shader
            // "FS]" for fragment shader
            int len = strlen(strCurrent);
            if (strstr(strCurrent, "VS]"))
            {
                // we have a vertex shader
                bInsideVertexShaderTag = true;
                bInsideFragmentShaderTag = false;

                // get the vertex shader filename, chopping off first [ and last VS]
                std::string cstr = strCurrent;
                g_strVertexShaderFilename = cstr.substr(1, len-strlen(strstr(strCurrent, "VS]"))-1);
            }
            else if (strstr(strCurrent, "FS]"))
            {
                // we have a fragment shader
                bInsideFragmentShaderTag = true;
                bInsideVertexShaderTag = false;

                // get the fragment shader filename, chopping off first [ and last FS]
                std::string cstr = strCurrent;
                g_strFragmentShaderFilename = cstr.substr(1, len-strlen(strstr(strCurrent, "FS]"))-1);
            }

            continue;
        }

        if (bInsideAnyTag)
        {
            // Handle include statements
            if (*strCurrent == '#' && stristr(strCurrent, "#include"))
            {
                char strCopy[256];
                strcpy(strCopy, strCurrent);
                strtok(strCopy, " ");
                char* strFile  = strtok(NULL, "\"");

                if (false == ParseShaderFromFile(strFile, NULL))
                    return false;

                // grab string for shader, depending on which shader we're in
                if (bInsideVertexShaderTag)
                {
                    g_strVertexShaderOutput += g_strOutput;
                    g_strVertexShaderOutput += "\n";
                    g_strOutput.clear();
                }
                else if (bInsideFragmentShaderTag)
                {
                    g_strFragmentShaderOutput += g_strOutput;
                    g_strFragmentShaderOutput += "\n";
                    g_strOutput.clear();
                }
                continue;
            }

            // Check for 'attribute' statements
            if (ParseAttributeStatements(strCurrent))
            {
                // grab string for shader, depending on which shader we're in
                if (bInsideVertexShaderTag)
                {
                    g_strVertexShaderOutput += g_strOutput;
                    g_strVertexShaderOutput += "\n";
                    g_strOutput.clear();
                }
                else if (bInsideFragmentShaderTag)
                {
                    g_strFragmentShaderOutput += g_strOutput;
                    g_strFragmentShaderOutput += "\n";
                    g_strOutput.clear();
                }
                continue;
            }
        }
        else
        {
            // Skip this line, since it's not part of our tagged body of text
            continue;
        }

        // grab string for shader, depending on which shader we're in
        if (bInsideVertexShaderTag)
        {
            g_strVertexShaderOutput += strCurrent;
            g_strVertexShaderOutput += "\n";
        }

        if (bInsideFragmentShaderTag)
        {
            g_strFragmentShaderOutput += strCurrent;
            g_strFragmentShaderOutput += "\n";
        }
    }

    delete pShaderData;

    return !bNoTags;
}

void WriteShaderFile(std::string filename, std::string strShaderCode)
{
    FILE *fptr = fopen(filename.c_str(), "wb");
    for (int idx = 0; idx < g_nNumDefines; idx++)
    {
        fputs("#define ", fptr);
        fputs(g_strDefines[idx], fptr);
        fputs("\n", fptr);
    }

    fputs(strShaderCode.c_str(), fptr);
    fclose(fptr);
}

void VerifyAndCreateDirectory(const char *strInput) 
{
#if LINUX_OR_OSX
    char strMasterPath[PATH_MAX] = {0};
    char strInputPath[PATH_MAX] = {0};
#else
    char strMasterPath[MAX_PATH] = {0};
    char strInputPath[MAX_PATH] = {0};
#endif
    strcpy(strInputPath, strInput);
    char *dir = strtok(strInputPath, "\\/");
    while (dir != NULL)
    {
        strcat(strMasterPath, dir);

        // see if it exists, if it doesn't, create it
#if defined (_WIN32)
        if (_access(strMasterPath, 0) != 0)
#elif LINUX_OR_OSX
	if (access(strMasterPath, 0) != 0)
#endif
        {
#if defined (_WIN32)
            // TODO: we could verify that this is actually a directory before creating it..
            _mkdir(strMasterPath);
#elif LINUX_OR_OSX
	    mkdir(strMasterPath, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
        }

        // get the next folder in the path
        dir = strtok(NULL, "\\/");

        // append the separator
#if defined (_WIN32)
        strcat(strMasterPath, "\\");
#elif LINUX_OR_OSX
	strcat(strMasterPath, "/");
#endif
    }
}



//--------------------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program
//--------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Parse input arguments
#if LINUX_OR_OSX
    char strInputFileName[PATH_MAX]  = "";
    char strOutputFileName[PATH_MAX] = "";
#elif defined (_WIN32)
    char strInputFileName[MAX_PATH]  = "";
    char strOutputFileName[MAX_PATH] = "";
#endif

    std::string strOutputFolder;//[MAX_PATH] = "";
    std::string strInputFolder;
    bool bPrintUsage = false;
    bool bPrintLogo  = true;
    bool bParseFolder = false;
    bool bAutoParse = false;

    char *strTag = NULL;

    for (int i = 1; i < argc; i++)
    {
        char* strArg = argv[i];

        if (strArg[0] == '-' || strArg[0] == '/')
        {
            if (!stricmp(strArg+1, "out"))
            {
                strOutputFolder = argv[++i];
            }
            else if (strArg[1] == 'o')
            {
                if (strOutputFileName[0])
                    bPrintUsage = true;

                strcpy(strOutputFileName, argv[++i]);
            }
            else if (!stricmp(strArg+1, "in"))
            {
                strInputFolder = argv[++i];
                bParseFolder = true;
            }
            else if (strArg[1] == 't')
            {
                if (strTag)
                    bPrintUsage = true;

                strTag = argv[++i];
            }
            else if (strArg[1] == 'd')
            {
                g_strDefines[g_nNumDefines++] = argv[++i];
            }
            else if (strArg[1] == 'a')
            {
                bAutoParse = true;
            }
            else if (!stricmp(strArg+1, "nologo"))
            {
                bPrintLogo = false;
            }
            else
            {
                bPrintUsage = true;
            }
        }
        else
        {
            if (strInputFileName[0])
                bPrintUsage = true;
            strcpy(strInputFileName, strArg);
        }
    }

    // Make sure we've got everything we need
    if (!bParseFolder && ((strTag == NULL) || (strInputFileName[0] == 0) || (strOutputFileName == 0)))
        bPrintUsage = true;

    // Print usage
    if (bPrintUsage)
    {
        printf("Usage:\n");
        printf("  File: ShaderPreprocessor.exe Input [-nologo] [-a] [-r] [-f tag] [-v tag] [-d define] [-o Output]\n");
        printf("     -nologo              Suppress printing of the logo.\n");
        printf("     Input                The input GLSL file or folder that contains GLSL files.\n");
        printf("     -a                   Auto-parse the GLSL shaders without defining tags or whether it is a vertex or fragment shader (experimental).\n");
        printf("     -t tag               Process shader code with this tag.\n");
        printf("     -d define            Zero or more optional values to #define.\n");
        printf("     -o OutputFile        The output file containing the processed shader(s).\n");
        printf("     -in InputFolder      The input folder containing that contains the GLSL files.\n");
        printf("     -out OutputFolder    The output folder for the processed shader(s).\n");
        return 1;
    }

    // Print logo
    if (bPrintLogo)
    {
        printf("Snapdragon SDK Shader Preprocessor\n");
        printf("Copyright (c) 2013 Qualcomm Technologies, Inc.\n");
    }

    if (!bParseFolder)
    {
        // only parsing a file
        // get the path to the input file
#if defined (_WIN32)
        _splitpath(strInputFileName, g_strDrive, g_strDir, g_strName, NULL);

        // if no output file is specified then we'll create one in the current path
        if (0 == strOutputFileName[0])
            _makepath(strOutputFileName, g_strDrive, g_strDir, g_strName, ".bin");

#elif LINUX_OR_OSX
	if (0 == strOutputFileName[0])
	{
	    // we just won't support no output file being specified in linux (for now)
	    printf("ERROR: No output file defined on command line. Please define an output file.\n");
	    return 1;
	}
#endif


        // autoparsing - the new way
        if ((bAutoParse) && ExtractShaders(strInputFileName))
        {
            // successful extract
            // write out the shader files
            g_strVertexShaderFilename = strOutputFolder + g_strVertexShaderFilename;
            g_strVertexShaderFilename += ".vs";
            g_strFragmentShaderFilename = strOutputFolder + g_strFragmentShaderFilename;
            g_strFragmentShaderFilename += ".fs";
            WriteShaderFile(g_strVertexShaderFilename, g_strVertexShaderOutput);
            WriteShaderFile(g_strFragmentShaderFilename, g_strFragmentShaderOutput);

            // Output
            printf("%s -> %s and %s\n", strInputFileName, g_strVertexShaderFilename.c_str(), g_strFragmentShaderFilename.c_str());
        }
        else
        {
            // the old way
            // Parse the file using the specified tag
            if (false == ParseShaderFromFile(strInputFileName, strTag))
                return 1;
            
            // write it out
            WriteShaderFile(strOutputFileName, g_strOutput);

            // Output statistics
            printf ("%s -> %s\n", strTag, strOutputFileName);
        }
    }
    else
    {
        
#if defined (_WIN32)
	// get and store the current folder
        char strCurrentFolder[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH, strCurrentFolder);

        CHAR strDrive[_MAX_DRIVE], strDir[_MAX_DIR], strName[_MAX_PATH], strExt[_MAX_EXT];
        CHAR strWorkingDirectory[MAX_PATH];
        _splitpath(strInputFolder.c_str(), strDrive, strDir, strName, strExt);
        _makepath(strWorkingDirectory, strDrive, strDir, NULL, NULL);

        // change path to input directory
        chdir(strInputFolder.c_str());

        // parse all the shader files in the defined folder
        // auto parsing turned on automatically
        // Windows approach (will need xplatform)
        WIN32_FIND_DATA findData;
        HANDLE hFind;

        std::string strSearch = "*.*"; 

        hFind = FindFirstFile(strSearch.c_str(), &findData);
        std::string filename = findData.cFileName;

        while (FindNextFile(hFind, &findData))
        {
            filename = findData.cFileName;

            // just verify..
            if (filename.find(".glsl") != std::string::npos)
            {
                // found a glsl - get the shaders
                if (ExtractShaders(filename.c_str()))
                { 
                    // successful extract
                    chdir(strCurrentFolder);
                    VerifyAndCreateDirectory(strOutputFolder.c_str());
                    
                    // write out the shader files
                    if (!g_strVertexShaderFilename.empty())
                    {
			
                        g_strVertexShaderFilename += ".vs";
                        printf("Vertex: %s -> %s\n", filename.c_str(), g_strVertexShaderFilename.c_str());

                        g_strVertexShaderFilename = strOutputFolder + g_strVertexShaderFilename;
                        WriteShaderFile(g_strVertexShaderFilename, g_strVertexShaderOutput);
                    }

                    if (!g_strFragmentShaderFilename.empty())
                    {
                        g_strFragmentShaderFilename += ".fs";
                        printf("Fragment: %s -> %s\n", filename.c_str(), g_strFragmentShaderFilename.c_str());

                        g_strFragmentShaderFilename = strOutputFolder + g_strFragmentShaderFilename;
                        WriteShaderFile(g_strFragmentShaderFilename, g_strFragmentShaderOutput);
                    }
                
                    // change path to input directory
                    chdir(strInputFolder.c_str());
                }

                g_strVertexShaderFilename.clear();
                g_strFragmentShaderFilename.clear();
                g_strVertexShaderOutput.clear();
                g_strFragmentShaderOutput.clear();
                g_strOutput.clear();
            }
        }

        // done, reset directory
        chdir(strCurrentFolder);
#elif LINUX_OR_OSX
	// LINUX implementation
	// get and store the current folder
        char strCurrentFolder[PATH_MAX] = {0};
	getcwd(strCurrentFolder, PATH_MAX);

	chdir(strInputFolder.c_str());

	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			std::string filename = dir->d_name;
			printf("Looking at %s\n", filename.c_str());

			// just verify..
            		if (filename.find(".glsl") != std::string::npos)
            		{
				// found a glsl - get the shaders
				if (ExtractShaders(filename.c_str()))
				{ 
				    // successful extract
				    chdir(strCurrentFolder);
				    VerifyAndCreateDirectory(strOutputFolder.c_str());
				    
				    // write out the shader files
				    if (!g_strVertexShaderFilename.empty())
				    {
				        g_strVertexShaderFilename += ".vs";
				        printf("Vertex: %s -> %s\n", filename.c_str(), g_strVertexShaderFilename.c_str());

				        g_strVertexShaderFilename = strOutputFolder + g_strVertexShaderFilename;
				        WriteShaderFile(g_strVertexShaderFilename, g_strVertexShaderOutput);
				    }

				    if (!g_strFragmentShaderFilename.empty())
				    {
				        g_strFragmentShaderFilename += ".fs";
				        printf("Fragment: %s -> %s\n", filename.c_str(), g_strFragmentShaderFilename.c_str());

				        g_strFragmentShaderFilename = strOutputFolder + g_strFragmentShaderFilename;
				        WriteShaderFile(g_strFragmentShaderFilename, g_strFragmentShaderOutput);
				    }
				
				    // change path to input directory
				    chdir(strInputFolder.c_str());
				}

				g_strVertexShaderFilename.clear();
				g_strFragmentShaderFilename.clear();
				g_strVertexShaderOutput.clear();
				g_strFragmentShaderOutput.clear();
				g_strOutput.clear();
			}
		}

		closedir(d);
	}

	chdir(strCurrentFolder);
#endif
    }

    return 0;
}

