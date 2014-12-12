//-----------------------------------------------------------------------------
// 
//  FbxModelConverter
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "FrmApplication.h"

#include <iostream>
#include <stdio.h>
#include <string>
#include <tchar.h>
#include <vector>


#include "Base.h"
#include "Color.h"
#include "FilmboxManager.h"
#include "FilmboxNode.h"
#include "FbxTypes.h"
#include "FbxTypeConverters.h"
#include "Mesh.h"
#include "Scene.h"
#include "SceneSerializer.h"
#include "Vector3.h"

#include "tootlelib.h"

//-----------------------------------------------------------------------------
// Required framework class and function
//-----------------------------------------------------------------------------

class AdrenoApplication : public CFrmApplication
{
    public:
        AdrenoApplication( const CHAR* name ) : CFrmApplication( name ) { }

        virtual BOOL Initialize()   { return TRUE; }
        virtual BOOL Resize()       { return TRUE; }
        virtual VOID Update()       { }
        virtual VOID Render()       { }
        virtual VOID Destroy()      { }
};

//-----------------------------------------------------------------------------

CFrmApplication* FrmCreateApplicationInstance()
{
    return new AdrenoApplication( "Adreno Application" );
}

//-----------------------------------------------------------------------------

namespace
{
    struct FbxModelSettings
    {
        std::string InputFilename;

        bool        WriteModel;
        std::string ModelFilename;

        bool        WriteAnim;
        std::string AnimFilename;

        bool        Optimize;
        float       WeldThreshold;

        FbxModelSettings()
        : InputFilename()
        , WriteModel( false )
        , ModelFilename()
        , WriteAnim( false )
        , AnimFilename()
        , Optimize( true )
        , WeldThreshold( 0.001f )
        {

        }
    };
}

//-----------------------------------------------------------------------------

FbxModelSettings ParseCommandLineArgs( int argc, _TCHAR* argv[] )
{
    FbxModelSettings settings;

    if( argc > 1 )
    {
        settings.InputFilename = argv[ 1 ];
    }
    
    if( argc > 2 )
    {
        bool look_for_model_filename = false;
        bool look_for_anim_filename  = false;
        bool look_for_weld_threshold = false;

        for( int i = 2; i < argc; ++i )
        {
            const char* arg = argv[ i ];

            if( MCE::FBX::StringICompare( arg, "-m" ) == 0 )
            {
                settings.WriteModel = true;

                look_for_model_filename = true;
                look_for_anim_filename  = false;
                look_for_weld_threshold = false;
            }
            else if( MCE::FBX::StringICompare( arg, "-a" ) == 0 )
            {
                settings.WriteAnim = true;

                look_for_model_filename = false;
                look_for_anim_filename  = true;
                look_for_weld_threshold = false;
            }
            else if( MCE::FBX::StringICompare( arg, "-o" ) == 0 )
            {
                settings.Optimize = false;

                look_for_model_filename = false;
                look_for_anim_filename  = false;
                look_for_weld_threshold = false;
            }
            else if( MCE::FBX::StringICompare( arg, "-w" ) == 0 )
            {
                look_for_model_filename = false;
                look_for_anim_filename  = false;
                look_for_weld_threshold = true;
            }
            else if( look_for_model_filename )
            {
                settings.ModelFilename = arg;

                look_for_model_filename = false;
                look_for_anim_filename  = false;
                look_for_weld_threshold = false;
            }
            else if( look_for_anim_filename )
            {
                settings.AnimFilename = arg;

                look_for_model_filename = false;
                look_for_anim_filename  = false;
                look_for_weld_threshold = false;
            }
            else if( look_for_weld_threshold )
            {
                settings.WeldThreshold = (float)( atof( arg ) );

                look_for_model_filename = false;
                look_for_anim_filename  = false;
                look_for_weld_threshold = false;
            }
        }
    }

    return settings;
}

//-----------------------------------------------------------------------------

void InitImplicitFilenames( FbxModelSettings& settings )
{
    if( !settings.InputFilename.empty() )
    {
        if( settings.WriteModel && settings.ModelFilename.empty() )
        {
            char filename[ 512 ];
            MCE::FBX::StripFileExtension( filename, settings.InputFilename.c_str(), 512 );
            settings.ModelFilename = filename + std::string( ".model" );
        }

        if( settings.WriteAnim && settings.AnimFilename.empty() )
        {
            char filename[ 512 ];
            MCE::FBX::StripFileExtension( filename, settings.InputFilename.c_str(), 512 );
            settings.AnimFilename = filename + std::string( ".anim" );
        }
    }
}

//-----------------------------------------------------------------------------

void DisplayHelp()
{
    std::cout << "FbxModelConverter usage:" << std::endl;
    std::cout << std::endl;
    std::cout << "FbxModelConverter <source_filename> [-m] [model_filename] [-a] [anim_filename] [-o] [-w] [weld_threshold]" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "   source_filename  The name of the input .fbx file." << std::endl;
    std::cout << "   -m               If set, a model file will be output." << std::endl;
    std::cout << "   model_filename   If not set, the model will output to source_filename.model" << std::endl;
    std::cout << "   -a               If set, a animation file will be output." << std::endl;
    std::cout << "   anim_filename    If not set, the anim will output to source_filename.anim" << std::endl;
    std::cout << "   -o               If set, optimizations will be turned off.  By default they're on." << std::endl;
    std::cout << "   -w               If set, you can supply a value after this to define the vertex weld threshold." << std::endl;
    std::cout << "   weld_threshold   This value defines how precise the vertices need to match to be welded.  Default is 0.001" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "   FbxModelConverter c:\\MyCharacter.fbx -m" << std::endl;
    std::cout << "   FbxModelConverter c:\\MyCharacter.fbx -m -o" << std::endl;
    std::cout << "   FbxModelConverter c:\\MyCharacter.fbx -a" << std::endl;
    std::cout << "   FbxModelConverter c:\\MyCharacter.fbx -m -a" << std::endl;
    std::cout << "   FbxModelConverter c:\\SourceArt\\MyCharacter.fbx -m c:\\MyProject\\media\\MyCharacter.model" << std::endl;
    std::cout << "   FbxModelConverter c:\\SourceArt\\MyCharacter.fbx -a c:\\MyProject\\media\\MyCharacter.anim" << std::endl;
    std::cout << "   FbxModelConverter c:\\SourceArt\\MyCharacter.fbx -m c:\\MyProject\\media\\MyCharacter.model -a c:\\MyProject\\media\\MyCharacter.anim" << std::endl;
    std::cout << std::endl;
}

//-----------------------------------------------------------------------------

int _tmain( int argc, _TCHAR* argv[] )
{
	// Set the DLL path so that the necessary DLLs can be found
	CHAR* strSdkRootDir = getenv( "ADRENO_SDK_DIR" );
	if( strSdkRootDir )
	{
		CHAR strDllPath[512] = "";
		strcat( strDllPath, strSdkRootDir );
		strcat( strDllPath, "\\System" );
		//SetDllDirectory( strDllPath );
	}

    if( argc < 2 )
    {
        DisplayHelp();
        return 1;
    }

    FbxModelSettings settings = ParseCommandLineArgs( argc, argv );
    InitImplicitFilenames( settings );

    const char* import_filename    = settings.InputFilename.c_str();
    const char* model_filename     = settings.ModelFilename.c_str();
    const char* animation_filename = settings.AnimFilename.c_str();

    MCE::FBX::InitializeFilmboxManager();

    TootleResult tootle_result = TootleInit();
    FBX_ASSERT( tootle_result == TOOTLE_OK, "Failed to initialize the Tootle mesh optimization library" );

    MCE::FBX::FilmboxManager* fbx_manager = MCE::FBX::GetFilmboxManager();
    fbx_manager->SetOptimizeMeshes( settings.Optimize );
    fbx_manager->SetWeldThreshold( settings.WeldThreshold );

    KFbxScene* fbx_scene = fbx_manager->ImportScene( import_filename );

    if( fbx_scene )
    {
        MCE::FBX::Scene* mce_scene = fbx_manager->ConvertScene( fbx_scene );

        if( mce_scene )
        {
            MCE::FBX::SceneSerializer serializer;

            if( settings.WriteModel && model_filename != NULL )
            {
                serializer.WriteModelToFile( *mce_scene, model_filename );
            }

            if( settings.WriteAnim && animation_filename != NULL )
            {
                serializer.WriteAnimationToFile( *mce_scene, animation_filename );
            }

            delete mce_scene;
            mce_scene = NULL;
        }

        fbx_scene->Destroy();
    }

    TootleCleanup();

    MCE::FBX::ReleaseFilmboxManager();

    return 0;
}
