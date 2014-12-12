//--------------------------------------------------------------------------------------
// main.cpp
//
// Converts a mesh into a binary format for easy runtime loading by SDK samples.
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#if defined (_WIN32)
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>

#if defined (__linux__)
#include <limits.h>
#include <strings.h>
#include <libgen.h>
#define MAX_PATH PATH_MAX
typedef float FLOAT;
typedef int INT;
#define stricmp strcasecmp
#endif

#include "FrmExtendedMesh.h"

void RemoveFileExt(char *name) 
{
    char *ext = strrchr(name, '.');
    *ext = '\0';
}

//--------------------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program
//--------------------------------------------------------------------------------------
int main( int argc, CHAR* argv[] )
{
    // Parse input arguments
    CHAR strInputFileName[MAX_PATH]  = "";
    CHAR strOutputFileName[MAX_PATH] = "";
    BOOL bPrintUsage = FALSE;
    BOOL bPrintLogo  = TRUE;

    FLOAT fScale = 0.0f;
    BOOL  bSetOnGroundPlane = FALSE;
    BOOL  bAddNormals       = FALSE;
    BOOL  bAddTangentSpace  = FALSE;
    BOOL  bRotate90AboutX   = FALSE;
    BOOL  bReverseWinding   = FALSE;
    BOOL  bStripify         = FALSE;

    for( INT i=1; i<argc; i++ )
    {
        CHAR* strArg = argv[i];

        if( strArg[0] == '-' || strArg[0] == '/' )
        {
            if( strArg[1] == 'o' )
            {
                if( strOutputFileName[0] )
                    bPrintUsage = TRUE;
                strcpy( strOutputFileName, argv[++i] );
            }
            else if( strArg[1] == 's' )
            {
                fScale = (FLOAT32)atof( argv[++i] );
            }
            else if( strArg[1] == 'g' )
            {
                bSetOnGroundPlane = TRUE;
            }
            else if( strArg[1] == 'n' )
            {
                bAddNormals = TRUE;
            }
            else if( strArg[1] == 't' )
            {
                bAddTangentSpace = TRUE;
            }
            else if( strArg[1] == 'r' )
            {
                bRotate90AboutX = TRUE;
            }
            else if( strArg[1] == 'w' )
            {
                bReverseWinding = TRUE;
            }
            else if( strArg[1] == 'x' )
            {
                bStripify = TRUE;
            }
            else if( !stricmp( strArg+1, "nologo" ) )
            {
                bPrintLogo = FALSE;
            }
            else
            {
                bPrintUsage = TRUE;
            }
        }
        else
        {
            if( strInputFileName[0] )
                bPrintUsage = TRUE;
            strcpy( strInputFileName, strArg );
        }
    }

    // Make sure we've got everything we need
    if( strInputFileName[0]==0 )
        bPrintUsage = TRUE;

    // Print usage
    if( bPrintUsage )
    {
        printf( "Usage:\n" );
        printf( "  MeshConverter [-nologo] InputFile [-o OutputFile] [-s Scale] [-g] [-n]\n" );
        printf( "     InputFile     : The name of the source mesh to convert\n" );
        printf( "     -o OutputFile : Output file name (defaults to 'InputFile.mesh')\n" );
        printf( "     -s Scale      : Forces mesh to the specified height\n" );
        printf( "     -r            : Rotate the mesh 90 degrees about the X-axis\n" );
        printf( "     -g            : Forces the mesh to lay on the ground place\n" );
        printf( "     -n            : Adds normals to the mesh\n" );
        printf( "     -t            : Adds tangent space vectors to the mesh\n" );
        printf( "     -w            : Reverse the winding order of the mesh vertices\n" );
        printf( "     -x            : Optimize meshes into tri-strips (not fully implemented)\n" );
        printf( "\n" );
        return 1;
    }

    // Print logo
    if( bPrintLogo )
        printf( "Snapdragon SDK Mesh Converter\n" );

    // Construct the output filenames
    if( 0 == strOutputFileName[0] )
    {
#if defined (_WIN32)
        CHAR strDrive[_MAX_DRIVE], strDir[_MAX_DIR], strName[_MAX_PATH];
        _splitpath( strInputFileName, strDrive, strDir, strName, NULL );
        _makepath( strOutputFileName, strDrive, strDir, strName, ".mesh" );
#elif defined (__linux__)
	char strDir[PATH_MAX] = {0};
	char strName[PATH_MAX] = {0};

	strcpy(strDir, dirname(strInputFileName));
	strcpy(strName, basename(strInputFileName));
	RemoveFileExt(strName);

	strcpy(strOutputFileName, strDir);
	strcat(strOutputFileName, "/");
	strcat(strOutputFileName, strName);
	strcat(strOutputFileName, ".mesh");
#endif

        if( !strcmp( strInputFileName, strOutputFileName ) )
            strcat( strOutputFileName, ".new" );
    }

    // Test if the source file exists
    FILE* file = fopen( strInputFileName, "rb" );
    if( NULL == file )
    {
        printf( "Error: Couldn't find file %s\n", strInputFileName );
        return 1;
    }
    fclose( file );

    // Load the mesh
    CFrmExtendedMesh mesh;
    if( FALSE == mesh.LoadInputMesh( strInputFileName ) )
    {
        printf( "Error: Couldn't load file %s as any support mesh file\n", strInputFileName );
        return 1;
    }

    if( bRotate90AboutX )
    {
        mesh.Rotate( FRM_PI/2, FRMVECTOR3( 1.0f, 0.0f, 0.0f ) );
    }

    // Optionally offset the mesh to lay flat on the X-Z plane
    if( bSetOnGroundPlane )
    {
        FRMVECTOR3 min, max;
        mesh.ComputeBounds( min, max );
        mesh.OffsetAndScale( FRMVECTOR3( 0.0f, -min.y, 0.0f ), 1.0f );
    }

    // Optionally rescale the mesh to be the specified height (in meters)
    if( fScale > 0.0f )
    {
        FRMVECTOR3 min, max;
        mesh.ComputeBounds( min, max );
        FLOAT32 h = max.y - min.y;
        mesh.OffsetAndScale( FRMVECTOR3( 0.0f, 0.0f, 0.0f ), fScale / h );
        mesh.ComputeBounds( min, max );
    }

    // Optionally add normals to the mesh
    if( bAddNormals )
    {
        mesh.AddNormals();
    }
    if( bAddTangentSpace )
    {
        mesh.AddTangentSpace();
    }

    // Optionally reverse the winding order of the mesh vertices
    if( bReverseWinding )
    {
        if( FALSE == mesh.ReverseWinding() )
        {
            printf( "Error: Cannot (yet) reverse the winding order for a mesh that contains triangle strips!\n" );
            return 1;
        }
    }

    // Optionally stripify the mesh
    if( bStripify )
    {
       mesh.Stripify();
    }

    // Save the new file
    if( FALSE == mesh.SaveOutputMesh( strOutputFileName ) )
    {
        printf( "Error: Couldn't save file %s\n", strOutputFileName );
        return 1;
    }

    // Return success
    return 0;
}

