//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <OpenGLES/FrmUtilsGLES.h>
#include "Scene.h"


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Skinning" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_nActiveMesh      = 0;
    m_pAnimationSet[0] = NULL;
    m_pAnimationSet[1] = NULL;
    m_hSkinningShader  = 0;
    m_pLogoTexture     = NULL;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the Skinning shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "In_Position",    FRM_VERTEX_POSITION },
            { "In_BoneIndices", FRM_VERTEX_BONEINDICES },
            { "In_BoneWeights", FRM_VERTEX_BONEWEIGHTS },
            { "In_Normal",      FRM_VERTEX_NORMAL },
            { "In_TexCoord",    FRM_VERTEX_TEXCOORD0 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Skinning.vs",
                                                      "Samples/Shaders/Skinning.fs",
                                                      &m_hSkinningShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_SkinningConstants.m_slotWorldMatrix      = glGetUniformLocation( m_hSkinningShader,  "g_matWorldArray" );
        m_SkinningConstants.m_slotViewMatrix       = glGetUniformLocation( m_hSkinningShader,  "g_matView" );
        m_SkinningConstants.m_slotProjMatrix       = glGetUniformLocation( m_hSkinningShader,  "g_matProj" );
        m_SkinningConstants.m_slotMaterialAmbient  = glGetUniformLocation( m_hSkinningShader,  "g_Material.vAmbient" );
        m_SkinningConstants.m_slotMaterialDiffuse  = glGetUniformLocation( m_hSkinningShader,  "g_Material.vDiffuse" );
        m_SkinningConstants.m_slotMaterialSpecular = glGetUniformLocation( m_hSkinningShader,  "g_Material.vSpecular" );

        // Assign texture locations
        glUseProgram( m_hSkinningShader );
        glUniform1i( glGetUniformLocation( m_hSkinningShader, "g_DiffuseSampler" ), 0 );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: BindAnimationDataToMesh()
// Desc: Uses ASCII names to bind animation data to corresponding mesh frames
//--------------------------------------------------------------------------------------
VOID BindAnimationDataToMesh( FRM_ANIMATION_SET* pAnimationSet, CFrmMesh* pMesh )
{
    // Bind the animation to the mesh frames
    for( UINT32 i=0; i<pAnimationSet->m_nNumAnimations; i++ )
    {
        for( UINT32 j=0; j<pMesh->m_nNumFrames; j++ )
        {
            if( !strcmp( pMesh->m_pFrames[j].m_strName,
                           pAnimationSet->m_pAnimations[i].m_strFrameName ) )
            {
                pAnimationSet->m_pAnimations[i].m_pMeshFrame = &pMesh->m_pFrames[j];
                break;
            }
        }
    }
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/Skinning.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Toggle Animated Character" );

    // Load the first skinned mesh. Skinned meshes contain per-vertex bone weights and
    // bone indices, which are used by the vertex shader to perform skinning.
    if( FALSE == m_Mesh[0].Load( "Samples/Meshes/Boy03.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh[0].MakeDrawable( &resource ) )
        return FALSE;

    // Load the second skinned mesh
    if( FALSE == m_Mesh[1].Load( "Samples/Meshes/Dman.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh[1].MakeDrawable( &resource ) )
        return FALSE;

    // Read the first animation from a file. The animation data is a set of key framed
    // values to scale, translate, and rotate various bone transforms belonging to a
    // skinned mesh. Animation data and mesh data are kept separate so that mulitple
    // animations (jump, walk, run, etc,) can play back on a single mesh, and multiple
    // meshes may share an animation. 
    if( FALSE == FrmReadAnimation( "Samples/Meshes/Boy03.anim", &m_pAnimationSet[0] ) )
        return FALSE;

    // Read the second animation from a file
    if( FALSE == FrmReadAnimation( "Samples/Meshes/Dman.anim", &m_pAnimationSet[1] ) )
        return FALSE;

    // Associate the animations with a mesh, linking ASCII-named animation frames to
    // corresponding mesh frames.
    BindAnimationDataToMesh( m_pAnimationSet[0], &m_Mesh[0] );
    BindAnimationDataToMesh( m_pAnimationSet[1], &m_Mesh[1] );

    // Initialize the shaders. The skinning vertex shader does all the work to transform
    // a mesh according to a palette of matrix transforms
    if( FALSE == InitShaders() )
        return FALSE;
    
    // Create the view matrix
    FRMVECTOR3 vPosition = FRMVECTOR3( 2.0f, 1.3f, 3.0f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matView = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 0.1f, 200.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroy objects created by the sample
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_hSkinningShader )
        glDeleteProgram( m_hSkinningShader );
    
    if( m_pLogoTexture )
        m_pLogoTexture->Release();

    delete m_pAnimationSet[0];
    delete m_pAnimationSet[1];
}


//--------------------------------------------------------------------------------------
// Name: SetAnimationTime()
// Desc: Sets the periodic time for an animation set, interpolating between the
//       appropriate key-framed values, and updating corresponding mesh frames
//--------------------------------------------------------------------------------------
VOID SetAnimationTime( FRM_ANIMATION_SET* pAnimationSet, UINT32 nPeriodicAnimTime )
{
    nPeriodicAnimTime = nPeriodicAnimTime % pAnimationSet->m_nPeriodInTicks;

    for( UINT32 i=0; i<pAnimationSet->m_nNumAnimations; i++ )
    {
        FRM_ANIMATION*  pAnimation = &pAnimationSet->m_pAnimations[i];
        FRM_MESH_FRAME* pMeshFrame = pAnimation->m_pMeshFrame;
        if( NULL == pMeshFrame )
            continue;

        UINT32 key1 = 0;
        while( pAnimation->m_pKeyTimes[key1] < nPeriodicAnimTime )
            key1++;

        UINT32 key0 = key1 - 1;
        if( key1 == 0 )
            key0 = pAnimationSet->m_nNumAnimations-1;

        UINT32 nTime0 = pAnimation->m_pKeyTimes[key0];
        UINT32 nTime1 = pAnimation->m_pKeyTimes[key1];
        FLOAT32  fLerp = (FLOAT32)( nPeriodicAnimTime - nTime0 ) / (FLOAT32)( nTime1 - nTime0 );

        FRM_ANIMATION_KEY* pKey0 = &pAnimation->m_pKeyValues[key0];
        FRM_ANIMATION_KEY* pKey1 = &pAnimation->m_pKeyValues[key1];

        // Lerp the values for pKey0 and pKey1
		FRMVECTOR3 v3Tmp = FrmVector3Lerp( pKey0->m_vScale, pKey1->m_vScale, fLerp );
        FRMMATRIX4X4 matScale     = FrmMatrixScale( v3Tmp );
		FRMVECTOR4 v4Tmp = FrmVector4SLerp( pKey0->m_vRotate, pKey1->m_vRotate, fLerp );
		FRMMATRIX4X4 matRotate    = FrmMatrixRotate( v4Tmp );
		v3Tmp = FrmVector3Lerp( pKey0->m_vTranslate, pKey1->m_vTranslate, fLerp );
		FRMMATRIX4X4 matTranslate = FrmMatrixTranslate( v3Tmp );

        // Build the matrix transform for the mesh frame
        pMeshFrame->m_matTransform = FrmMatrixMultiply( matScale, matRotate );
        pMeshFrame->m_matTransform = FrmMatrixMultiply( pMeshFrame->m_matTransform, matTranslate );
    }
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: Use the timer and input to update objects in the scene
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fTime        = m_Timer.GetFrameTime();
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    // Toggle the active animated character
    if( nPressedButtons & INPUT_KEY_1 )
        m_nActiveMesh = ( m_nActiveMesh + 1 ) % 2;

    // Advance the animation time, updating each frame's m_matTransform
    static UINT32 nPeriodicAnimTime = 0;
    nPeriodicAnimTime += (UINT32)( fElapsedTime * FRM_ANIMATION_TICKS_PER_SEC );
    SetAnimationTime( m_pAnimationSet[m_nActiveMesh], nPeriodicAnimTime );
}


//--------------------------------------------------------------------------------------
// Name: DrawSkinnedMesh()
// Desc: Draw the skinned mesh
//--------------------------------------------------------------------------------------
VOID DrawSkinnedMesh( FRM_MESH* pMesh, 
                      UINT32 hSkinningShader, SHADER_CONSTANTS* pShaderConstants,
                      FRMMATRIX4X4 matView, FRMMATRIX4X4 matProj )
{
    // Set the buffers
    FrmSetVertexBuffer( pMesh->m_hVertexBuffer );
    FrmSetVertexLayout( pMesh->m_VertexLayout, pMesh->m_nVertexSize );
    FrmSetIndexBuffer( pMesh->m_hIndexBuffer );

    // Set the skinning shader
    glUseProgram( hSkinningShader );
    glUniformMatrix4fv( pShaderConstants->m_slotViewMatrix, 1, FALSE, (FLOAT32*)&matView );
    glUniformMatrix4fv( pShaderConstants->m_slotProjMatrix, 1, FALSE, (FLOAT32*)&matProj );

    // Draw all subsets
    for( UINT32 nSubset = 0; nSubset < pMesh->m_nNumSubsets; nSubset++ )
    {
        FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[nSubset];

        // Set material color properties 
        FRMVECTOR3 vAmbient  = 0.2f * pSubset->m_vAmbientColor;
        FRMVECTOR4 vDiffuse  = pSubset->m_vDiffuseColor;
        FRMVECTOR4 vSpecular = pSubset->m_vSpecularColor;
        glUniform3fv( pShaderConstants->m_slotMaterialAmbient,  1, (FLOAT32*)&vAmbient );
        glUniform4fv( pShaderConstants->m_slotMaterialDiffuse,  1, (FLOAT32*)&vDiffuse );
        glUniform4fv( pShaderConstants->m_slotMaterialSpecular, 1, (FLOAT32*)&vSpecular );

        // Setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
        for( UINT32 iTexture = 0; iTexture < pSubset->m_nNumTextures; iTexture++ )
        {
            FrmSetTexture( iTexture, pSubset->m_pTextures[iTexture].m_pTexture ? 
                                     pSubset->m_pTextures[iTexture].m_pTexture->m_hTextureHandle : 
                                     NULL );
        }

        // Build a palette of 4x3 world matrices
        FRMMATRIX4X3 matWorldMatrixArray[MAX_BONES];
        for( UINT32 iBone = 0; iBone < pMesh->m_nNumBoneMatrices; ++iBone )
        {
            UINT32 nBoneId = pSubset->m_pBoneIds[iBone];
            if( nBoneId < pMesh->m_nNumBones )
            {
                FRMMATRIX4X4 matBoneMatrix = pMesh->m_pBones[nBoneId].m_pFrame->m_matTransform;

                // Isolate the 4x3 portion of the transposed bone matrix
                matBoneMatrix = FrmMatrixTranspose( matBoneMatrix );
                memcpy( &matWorldMatrixArray[iBone], &matBoneMatrix, sizeof(FRMMATRIX4X3) );
            }
        }

        // Set the bone matrix palette to the vertex shader
        glUniform4fv( pShaderConstants->m_slotWorldMatrix, pMesh->m_nNumBoneMatrices*3, 
                                                           (FLOAT32*)&matWorldMatrixArray[0] );

        // Draw the mesh
        FrmDrawIndexedVertices( pSubset->m_nPrimType, pSubset->m_nNumIndices, 
                                pMesh->m_nIndexSize, pSubset->m_nIndexOffset );
    }

    FrmSetVertexBuffer( NULL );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: Render the scene
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    // Set default states
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    // Clear the backbuffer and depth-buffer
    glClearColor( 0.2f, 0.2f, 0.6f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    // Draw the skinned mesh
    DrawSkinnedMesh( m_Mesh[m_nActiveMesh].m_pFrames[0].m_pMesh,
                     m_hSkinningShader, &m_SkinningConstants,
                     m_matView, m_matProj );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

