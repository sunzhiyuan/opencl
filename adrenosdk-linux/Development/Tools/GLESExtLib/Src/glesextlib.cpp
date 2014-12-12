//=============================================================================
// Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
//=============================================================================
// FILE: glesextlib.cpp
//
// OpenGL ES Extension Library.
//=============================================================================


#include "glesextlib.h"

#if defined(ANDROID)
#include <android/log.h>
#endif

// For vprintf()
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* OpenGL ES 3.0 */

PFNGLREADBUFFERPROC                     glReadBuffer                        = NULL;
PFNGLDRAWRANGEELEMENTSPROC              glDrawRangeElements                 = NULL;
PFNGLTEXIMAGE3DPROC                     glTexImage3D                        = NULL;
PFNGLTEXSUBIMAGE3DPROC                  glTexSubImage3D                     = NULL;
PFNGLCOPYTEXSUBIMAGE3DPROC              glCopyTexSubImage3D                 = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DPROC           glCompressedTexImage3D              = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC        glCompressedTexSubImage3D           = NULL;
PFNGLGENQUERIESPROC                     glGenQueries                        = NULL;
PFNGLDELETEQUERIESPROC                  glDeleteQueries                     = NULL;
PFNGLISQUERYPROC                        glIsQuery                           = NULL;
PFNGLBEGINQUERYPROC                     glBeginQuery                        = NULL;
PFNGLENDQUERYPROC                       glEndQuery                          = NULL;
PFNGLGETQUERYIVPROC                     glGetQueryiv                        = NULL;
PFNGLGETQUERYOBJECTUIVPROC              glGetQueryObjectuiv                 = NULL;
PFNGLUNMAPBUFFERPROC                    glUnmapBuffer                       = NULL;
PFNGLGETBUFFERPOINTERVPROC              glGetBufferPointerv                 = NULL;
PFNGLDRAWBUFFERSPROC                    glDrawBuffers                       = NULL;
PFNGLUNIFORMMATRIX2X3FVPROC             glUniformMatrix2x3fv                = NULL;
PFNGLUNIFORMMATRIX3X2FVPROC             glUniformMatrix3x2fv                = NULL;
PFNGLUNIFORMMATRIX2X4FVPROC             glUniformMatrix2x4fv                = NULL;
PFNGLUNIFORMMATRIX4X2FVPROC             glUniformMatrix4x2fv                = NULL;
PFNGLUNIFORMMATRIX3X4FVPROC             glUniformMatrix3x4fv                = NULL;
PFNGLUNIFORMMATRIX4X3FVPROC             glUniformMatrix4x3fv                = NULL;
PFNGLBLITFRAMEBUFFERPROC                glBlitFramebuffer                   = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample    = NULL;
PFNGLFRAMEBUFFERTEXTURELAYERPROC        glFramebufferTextureLayer           = NULL;
PFNGLMAPBUFFERRANGEPROC                 glMapBufferRange                    = NULL;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC         glFlushMappedBufferRange            = NULL;
PFNGLBINDVERTEXARRAYPROC                glBindVertexArray                   = NULL;
PFNGLDELETEVERTEXARRAYSPROC             glDeleteVertexArrays                = NULL;
PFNGLGENVERTEXARRAYSPROC                glGenVertexArrays                   = NULL;
PFNGLISVERTEXARRAYPROC                  glIsVertexArray                     = NULL;
PFNGLGETINTEGERI_VPROC                  glGetIntegeri_v                     = NULL;
PFNGLBEGINTRANSFORMFEEDBACKPROC         glBeginTransformFeedback            = NULL;
PFNGLENDTRANSFORMFEEDBACKPROC           glEndTransformFeedback              = NULL;
PFNGLBINDBUFFERRANGEPROC                glBindBufferRange                   = NULL;
PFNGLBINDBUFFERBASEPROC                 glBindBufferBase                    = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC      glTransformFeedbackVaryings         = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC    glGetTransformFeedbackVarying       = NULL;
PFNGLVERTEXATTRIBIPOINTERPROC           glVertexAttribIPointer              = NULL;
PFNGLGETVERTEXATTRIBIIVPROC             glGetVertexAttribIiv                = NULL;
PFNGLGETVERTEXATTRIBIUIVPROC            glGetVertexAttribIuiv               = NULL;
PFNGLVERTEXATTRIBI4IPROC                glVertexAttribI4i                   = NULL;
PFNGLVERTEXATTRIBI4UIPROC               glVertexAttribI4ui                  = NULL;
PFNGLVERTEXATTRIBI4IVPROC               glVertexAttribI4iv                  = NULL;
PFNGLVERTEXATTRIBI4UIVPROC              glVertexAttribI4uiv                 = NULL;
PFNGLGETUNIFORMUIVPROC                  glGetUniformuiv                     = NULL;
PFNGLGETFRAGDATALOCATIONPROC            glGetFragDataLocation               = NULL;
PFNGLUNIFORM1UIPROC                     glUniform1ui                        = NULL;
PFNGLUNIFORM2UIPROC                     glUniform2ui                        = NULL;
PFNGLUNIFORM3UIPROC                     glUniform3ui                        = NULL;
PFNGLUNIFORM4UIPROC                     glUniform4ui                        = NULL;
PFNGLUNIFORM1UIVPROC                    glUniform1uiv                       = NULL;
PFNGLUNIFORM2UIVPROC                    glUniform2uiv                       = NULL;
PFNGLUNIFORM3UIVPROC                    glUniform3uiv                       = NULL;
PFNGLUNIFORM4UIVPROC                    glUniform4uiv                       = NULL;
PFNGLCLEARBUFFERIVPROC                  glClearBufferiv                     = NULL;
PFNGLCLEARBUFFERUIVPROC                 glClearBufferuiv                    = NULL;
PFNGLCLEARBUFFERFVPROC                  glClearBufferfv                     = NULL;
PFNGLCLEARBUFFERFIPROC                  glClearBufferfi                     = NULL;
PFNGLGETSTRINGIPROC                     glGetStringi                        = NULL;
PFNGLCOPYBUFFERSUBDATAPROC              glCopyBufferSubData                 = NULL;
PFNGLGETUNIFORMINDICESPROC              glGetUniformIndices                 = NULL;
PFNGLGETACTIVEUNIFORMSIVPROC            glGetActiveUniformsiv               = NULL;
PFNGLGETUNIFORMBLOCKINDEXPROC           glGetUniformBlockIndex              = NULL;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC        glGetActiveUniformBlockiv           = NULL;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC      glGetActiveUniformBlockName         = NULL;
PFNGLUNIFORMBLOCKBINDINGPROC            glUniformBlockBinding               = NULL;
PFNGLDRAWARRAYSINSTANCEDPROC            glDrawArraysInstanced               = NULL;
PFNGLDRAWELEMENTSINSTANCEDPROC          glDrawElementsInstanced             = NULL;
PFNGLFENCESYNCPROC                      glFenceSync                         = NULL;
PFNGLISSYNCPROC                         glIsSync                            = NULL;
PFNGLDELETESYNCPROC                     glDeleteSync                        = NULL;
PFNGLCLIENTWAITSYNCPROC                 glClientWaitSync                    = NULL;
PFNGLWAITSYNCPROC                       glWaitSync                          = NULL;
PFNGLGETINTEGER64VPROC                  glGetInteger64v                     = NULL;
PFNGLGETSYNCIVPROC                      glGetSynciv                         = NULL;
PFNGLGETINTEGER64I_VPROC                glGetInteger64i_v                   = NULL;
PFNGLGETBUFFERPARAMETERI64VPROC         glGetBufferParameteri64v            = NULL;
PFNGLGENSAMPLERSPROC                    glGenSamplers                       = NULL;
PFNGLDELETESAMPLERSPROC                 glDeleteSamplers                    = NULL;
PFNGLISSAMPLERPROC                      glIsSampler                         = NULL;
PFNGLBINDSAMPLERPROC                    glBindSampler                       = NULL;
PFNGLSAMPLERPARAMETERIPROC              glSamplerParameteri                 = NULL;
PFNGLSAMPLERPARAMETERIVPROC             glSamplerParameteriv                = NULL;
PFNGLSAMPLERPARAMETERFPROC              glSamplerParameterf                 = NULL;
PFNGLSAMPLERPARAMETERFVPROC             glSamplerParameterfv                = NULL;
PFNGLGETSAMPLERPARAMETERIVPROC          glGetSamplerParameteriv             = NULL;
PFNGLGETSAMPLERPARAMETERFVPROC          glGetSamplerParameterfv             = NULL;
PFNGLVERTEXATTRIBDIVISORPROC            glVertexAttribDivisor               = NULL;
PFNGLBINDTRANSFORMFEEDBACKPROC          glBindTransformFeedback             = NULL;
PFNGLDELETETRANSFORMFEEDBACKSPROC       glDeleteTransformFeedbacks          = NULL;
PFNGLGENTRANSFORMFEEDBACKSPROC          glGenTransformFeedbacks             = NULL;
PFNGLISTRANSFORMFEEDBACKPROC            glIsTransformFeedback               = NULL;
PFNGLPAUSETRANSFORMFEEDBACKPROC         glPauseTransformFeedback            = NULL;
PFNGLRESUMETRANSFORMFEEDBACKPROC        glResumeTransformFeedback           = NULL;
PFNGLGETPROGRAMBINARYPROC               glGetProgramBinary                  = NULL;
PFNGLPROGRAMBINARYPROC                  glProgramBinary                     = NULL;
PFNGLPROGRAMPARAMETERIPROC              glProgramParameteri                 = NULL;
PFNGLINVALIDATEFRAMEBUFFERPROC          glInvalidateFramebuffer             = NULL;
PFNGLINVALIDATESUBFRAMEBUFFERPROC       glInvalidateSubFramebuffer          = NULL;
PFNGLTEXSTORAGE2DPROC                   glTexStorage2D                      = NULL;
PFNGLTEXSTORAGE3DPROC                   glTexStorage3D                      = NULL;
PFNGLGETINTERNALFORMATIVPROC            glGetInternalformativ               = NULL;


/* OpenGL ES 2.0 Extensions */

PFNGLEGLIMAGETARGETTEXTURE2DOESPROC				glEGLImageTargetTexture2DOES			= NULL;
PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC	glEGLImageTargetRenderbufferStorageOES	= NULL;
PFNGLGETPROGRAMBINARYOESPROC					glGetProgramBinaryOES					= NULL;
PFNGLPROGRAMBINARYOESPROC						glProgramBinaryOES						= NULL;
PFNGLMAPBUFFEROESPROC							glMapBufferOES							= NULL;
PFNGLUNMAPBUFFEROESPROC							glUnmapBufferOES						= NULL;
PFNGLGETBUFFERPOINTERVOESPROC					glGetBufferPointervOES					= NULL;
PFNGLTEXIMAGE3DOESPROC							glTexImage3DOES							= NULL;
PFNGLTEXSUBIMAGE3DOESPROC						glTexSubImage3DOES						= NULL;
PFNGLCOPYTEXSUBIMAGE3DOESPROC					glCopyTexSubImage3DOES					= NULL;
PFNGLCOMPRESSEDTEXIMAGE3DOESPROC				glCompressedTexImage3DOES				= NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC				glCompressedTexSubImage3DOES			= NULL;
PFNGLFRAMEBUFFERTEXTURE3DOES					glFramebufferTexture3DOES				= NULL;
PFNGLBINDVERTEXARRAYOESPROC						glBindVertexArrayOES					= NULL;
PFNGLDELETEVERTEXARRAYSOESPROC					glDeleteVertexArraysOES					= NULL;
PFNGLGENVERTEXARRAYSOESPROC						glGenVertexArraysOES					= NULL;
PFNGLISVERTEXARRAYOESPROC						glIsVertexArrayOES						= NULL;
PFNGLDEBUGMESSAGECONTROLPROC					glDebugMessageControl					= NULL;
PFNGLDEBUGMESSAGEINSERTPROC						glDebugMessageInsert					= NULL;
PFNGLDEBUGMESSAGECALLBACKPROC					glDebugMessageCallback					= NULL;
PFNGLGETDEBUGMESSAGELOGPROC						glGetDebugMessageLog					= NULL;
PFNGLPUSHDEBUGGROUPPROC							glPushDebugGroup						= NULL;
PFNGLPOPDEBUGGROUPPROC							glPopDebugGroup							= NULL;
PFNGLOBJECTLABELPROC							glObjectLabel							= NULL;
PFNGLGETOBJECTLABELPROC							glGetObjectLabel						= NULL;
PFNGLOBJECTPTRLABELPROC							glObjectPtrLabel						= NULL;
PFNGLGETOBJECTPTRLABELPROC						glGetObjectPtrLabel						= NULL;
PFNGLGETPOINTERVPROC							glGetPointerv							= NULL;
PFNGLGETPERFMONITORGROUPSAMDPROC				glGetPerfMonitorGroupsAMD				= NULL;
PFNGLGETPERFMONITORCOUNTERSAMDPROC				glGetPerfMonitorCountersAMD				= NULL;
PFNGLGETPERFMONITORGROUPSTRINGAMDPROC			glGetPerfMonitorGroupStringAMD			= NULL;
PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC			glGetPerfMonitorCounterStringAMD		= NULL;
PFNGLGETPERFMONITORCOUNTERINFOAMDPROC			glGetPerfMonitorCounterInfoAMD			= NULL;
PFNGLGENPERFMONITORSAMDPROC						glGenPerfMonitorsAMD					= NULL;
PFNGLDELETEPERFMONITORSAMDPROC					glDeletePerfMonitorsAMD					= NULL;
PFNGLSELECTPERFMONITORCOUNTERSAMDPROC			glSelectPerfMonitorCountersAMD			= NULL;
PFNGLBEGINPERFMONITORAMDPROC					glBeginPerfMonitorAMD					= NULL;
PFNGLENDPERFMONITORAMDPROC						glEndPerfMonitorAMD						= NULL;
PFNGLGETPERFMONITORCOUNTERDATAAMDPROC			glGetPerfMonitorCounterDataAMD			= NULL;
PFNGLBLITFRAMEBUFFERANGLEPROC					glBlitFramebufferANGLE					= NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC	glRenderbufferStorageMultisampleANGLE	= NULL;
PFLGLDRAWARRAYSINSTANCEDANGLEPROC				glDrawArraysInstancedANGLE				= NULL;
PFLGLDRAWELEMENTSINSTANCEDANGLEPROC				glDrawElementsInstancedANGLE			= NULL;
PFLGLVERTEXATTRIBDIVISORANGLEPROC				glVertexAttribDivisorANGLE				= NULL;
PFLGLGETTRANSLATEDSHADERSOURCEANGLEPROC			glGetTranslatedShaderSourceANGLE		= NULL;
PFNGLCOPYTEXTURELEVELSAPPLEPROC					glCopyTextureLevelsAPPLE				= NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC	glRenderbufferStorageMultisampleAPPLE	= NULL;
PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC		glResolveMultisampleFramebufferAPPLE	= NULL;
PFNGLFENCESYNCAPPLEPROC							glFenceSyncAPPLE						= NULL;
PFNGLISSYNCAPPLEPROC							glIsSyncAPPLE							= NULL;
PFNGLDELETESYNCAPPLEPROC						glDeleteSyncAPPLE						= NULL;
PFNGLCLIENTWAITSYNCAPPLEPROC					glClientWaitSyncAPPLE					= NULL;
PFNGLWAITSYNCAPPLEPROC							glWaitSyncAPPLE							= NULL;
PFNGLGETINTEGER64VAPPLEPROC						glGetInteger64vAPPLE					= NULL;
PFNGLGETSYNCIVAPPLEPROC							glGetSyncivAPPLE						= NULL;
PFNGLLABELOBJECTEXTPROC							glLabelObjectEXT						= NULL;
PFNGLGETOBJECTLABELEXTPROC						glGetObjectLabelEXT						= NULL;
PFNGLINSERTEVENTMARKEREXTPROC					glInsertEventMarkerEXT					= NULL;
PFNGLPUSHGROUPMARKEREXTPROC						glPushGroupMarkerEXT					= NULL;
PFNGLPOPGROUPMARKEREXTPROC						glPopGroupMarkerEXT						= NULL;
PFNGLDISCARDFRAMEBUFFEREXTPROC					glDiscardFramebufferEXT					= NULL;
PFNGLMAPBUFFERRANGEEXTPROC						glMapBufferRangeEXT						= NULL;
PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC				glFlushMappedBufferRangeEXT				= NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC		glRenderbufferStorageMultisampleEXT		= NULL;
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC		glFramebufferTexture2DMultisampleEXT	= NULL;
PFNGLREADBUFFERINDEXEDEXTPROC					glReadBufferIndexedEXT					= NULL;
PFNGLDRAWBUFFERSINDEXEDEXTPROC					glDrawBuffersIndexedEXT					= NULL;
PFNGLGETINTEGERI_VEXTPROC						glGetIntegeri_vEXT						= NULL;
PFNGLMULTIDRAWARRAYSEXTPROC						glMultiDrawArraysEXT					= NULL;
PFNGLMULTIDRAWELEMENTSEXTPROC					glMultiDrawElementsEXT					= NULL;
PFNGLGENQUERIESEXTPROC							glGenQueriesEXT							= NULL;
PFNGLDELETEQUERIESEXTPROC						glDeleteQueriesEXT						= NULL;
PFNGLISQUERYEXTPROC								glIsQueryEXT							= NULL;
PFNGLBEGINQUERYEXTPROC							glBeginQueryEXT							= NULL;
PFNGLENDQUERYEXTPROC							glEndQueryEXT							= NULL;
PFNGLGETQUERYIVEXTPROC							glGetQueryivEXT							= NULL;
PFNGLGETQUERYOBJECTUIVEXTPROC					glGetQueryObjectuivEXT					= NULL;
PFNGLGETGRAPHICSRESETSTATUSEXTPROC				glGetGraphicsResetStatusEXT				= NULL;
PFNGLREADNPIXELSEXTPROC							glReadnPixelsEXT						= NULL;
PFNGLGETNUNIFORMFVEXTPROC						glGetnUniformfvEXT						= NULL;
PFNGLGETNUNIFORMIVEXTPROC						glGetnUniformivEXT						= NULL;
PFNGLUSEPROGRAMSTAGESEXTPROC					glUseProgramStagesEXT					= NULL;
PFNGLACTIVESHADERPROGRAMEXTPROC					glActiveShaderProgramEXT				= NULL;
PFNGLCREATESHADERPROGRAMVEXTPROC				glCreateShaderProgramvEXT				= NULL;
PFNGLBINDPROGRAMPIPELINEEXTPROC					glBindProgramPipelineEXT				= NULL;
PFNGLDELETEPROGRAMPIPELINESEXTPROC				glDeleteProgramPipelinesEXT				= NULL;
PFNGLGENPROGRAMPIPELINESEXTPROC					glGenProgramPipelinesEXT				= NULL;
PFNGLISPROGRAMPIPELINEEXTPROC					glIsProgramPipelineEXT					= NULL;
PFNGLPROGRAMPARAMETERIEXTPROC					glProgramParameteriEXT					= NULL;
PFNGLGETPROGRAMPIPELINEIVEXTPROC				glGetProgramPipelineivEXT				= NULL;
PFNGLPROGRAMUNIFORM1IEXTPROC					glProgramUniform1iEXT					= NULL;
PFNGLPROGRAMUNIFORM2IEXTPROC					glProgramUniform2iEXT					= NULL;
PFNGLPROGRAMUNIFORM3IEXTPROC					glProgramUniform3iEXT					= NULL;
PFNGLPROGRAMUNIFORM4IEXTPROC					glProgramUniform4iEXT					= NULL;
PFNGLPROGRAMUNIFORM1FEXTPROC					glProgramUniform1fEXT					= NULL;
PFNGLPROGRAMUNIFORM2FEXTPROC					glProgramUniform2fEXT					= NULL;
PFNGLPROGRAMUNIFORM3FEXTPROC					glProgramUniform3fEXT					= NULL;
PFNGLPROGRAMUNIFORM4FEXTPROC					glProgramUniform4fEXT					= NULL;
PFNGLPROGRAMUNIFORM1IVEXTPROC					glProgramUniform1ivEXT					= NULL;
PFNGLPROGRAMUNIFORM2IVEXTPROC					glProgramUniform2ivEXT					= NULL;
PFNGLPROGRAMUNIFORM3IVEXTPROC					glProgramUniform3ivEXT					= NULL;
PFNGLPROGRAMUNIFORM4IVEXTPROC					glProgramUniform4ivEXT					= NULL;
PFNGLPROGRAMUNIFORM1FVEXTPROC					glProgramUniform1fvEXT					= NULL;
PFNGLPROGRAMUNIFORM2FVEXTPROC					glProgramUniform2fvEXT					= NULL;
PFNGLPROGRAMUNIFORM3FVEXTPROC					glProgramUniform3fvEXT					= NULL;
PFNGLPROGRAMUNIFORM4FVEXTPROC					glProgramUniform4fvEXT					= NULL;
PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC				glProgramUniformMatrix2fvEXT			= NULL;
PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC				glProgramUniformMatrix3fvEXT			= NULL;
PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC				glProgramUniformMatrix4fvEXT			= NULL;
PFNGLVALIDATEPROGRAMPIPELINEEXTPROC				glValidateProgramPipelineEXT			= NULL;
PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC			glGetProgramPipelineInfoLogEXT			= NULL;
PFNGLTEXSTORAGE1DEXTPROC						glTexStorage1DEXT						= NULL;
PFNGLTEXSTORAGE2DEXTPROC						glTexStorage2DEXT						= NULL;
PFNGLTEXSTORAGE3DEXTPROC						glTexStorage3DEXT						= NULL;
PFNGLTEXTURESTORAGE1DEXTPROC					glTextureStorage1DEXT					= NULL;
PFNGLTEXTURESTORAGE2DEXTPROC					glTextureStorage2DEXT					= NULL;
PFNGLTEXTURESTORAGE3DEXTPROC					glTextureStorage3DEXT					= NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC		glRenderbufferStorageMultisampleIMG		= NULL;
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC		glFramebufferTexture2DMultisampleIMG	= NULL;
PFNGLCOVERAGEMASKNVPROC							glCoverageMaskNV						= NULL;
PFNGLCOVERAGEOPERATIONNVPROC					glCoverageOperationNV					= NULL;
PFNGLDRAWBUFFERSNVPROC							glDrawBuffersNV							= NULL;
PFNDRAWARRAYSINSTANCEDNVPROC					glDrawArraysInstancedNV					= NULL;
PFNDRAWELEMENTSINSTANCEDNVPROC					glDrawElementsInstancedNV				= NULL;
PFNGLDELETEFENCESNVPROC							glDeleteFencesNV						= NULL;
PFNGLGENFENCESNVPROC							glGenFencesNV							= NULL;
PFNGLISFENCENVPROC								glIsFenceNV								= NULL;
PFNGLTESTFENCENVPROC							glTestFenceNV							= NULL;
PFNGLGETFENCEIVNVPROC							glGetFenceivNV							= NULL;
PFNGLFINISHFENCENVPROC							glFinishFenceNV							= NULL;
PFNGLSETFENCENVPROC								glSetFenceNV							= NULL;
PFNBLITFRAMEBUFFERNVPROC						glBlitFramebufferNV						= NULL;
PFNRENDERBUFFERSTORAGEMULTISAMPLENVPROC			glRenderbufferStorageMultisampleNV		= NULL;
PFNVERTEXATTRIBDIVISORNVPROC					glVertexAttribDivisorNV					= NULL;
PFNGLREADBUFFERNVPROC							glReadBufferNV							= NULL;
PFNGLALPHAFUNCQCOMPROC							glAlphaFuncQCOM							= NULL;
PFNGLGETDRIVERCONTROLSQCOMPROC					glGetDriverControlsQCOM					= NULL;
PFNGLGETDRIVERCONTROLSTRINGQCOMPROC				glGetDriverControlStringQCOM			= NULL;
PFNGLENABLEDRIVERCONTROLQCOMPROC				glEnableDriverControlQCOM				= NULL;
PFNGLDISABLEDRIVERCONTROLQCOMPROC				glDisableDriverControlQCOM				= NULL;
PFNGLEXTGETTEXTURESQCOMPROC						glExtGetTexturesQCOM					= NULL;
PFNGLEXTGETBUFFERSQCOMPROC						glExtGetBuffersQCOM						= NULL;
PFNGLEXTGETRENDERBUFFERSQCOMPROC				glExtGetRenderbuffersQCOM				= NULL;
PFNGLEXTGETFRAMEBUFFERSQCOMPROC					glExtGetFramebuffersQCOM				= NULL;
PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC			glExtGetTexLevelParameterivQCOM			= NULL;
PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC			glExtTexObjectStateOverrideiQCOM		= NULL;
PFNGLEXTGETTEXSUBIMAGEQCOMPROC					glExtGetTexSubImageQCOM					= NULL;
PFNGLEXTGETBUFFERPOINTERVQCOMPROC				glExtGetBufferPointervQCOM				= NULL;
PFNGLEXTGETSHADERSQCOMPROC						glExtGetShadersQCOM						= NULL;
PFNGLEXTGETPROGRAMSQCOMPROC						glExtGetProgramsQCOM					= NULL;
PFNGLEXTISPROGRAMBINARYQCOMPROC					glExtIsProgramBinaryQCOM				= NULL;
PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC			glExtGetProgramBinarySourceQCOM			= NULL;
PFNGLSTARTTILINGQCOMPROC						glStartTilingQCOM						= NULL;
PFNGLENDTILINGQCOMPROC							glEndTilingQCOM							= NULL;

//-----------------------------------------------------------------------------
CGLESExtLib::CGLESExtLib()
//-----------------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------------
CGLESExtLib::~CGLESExtLib()
//-----------------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------------
fpnGenericFunctionPointer CGLESExtLib::GetExtension(const char* pszExtensionName)
//-----------------------------------------------------------------------------
{
    fpnGenericFunctionPointer fpnExtension = eglGetProcAddress(pszExtensionName);

    if (fpnExtension == NULL)
    {
		OutputLog("The function '%s' is not supported by the OpenGL driver on this device\n", pszExtensionName);
    }

    return fpnExtension;
}

//-----------------------------------------------------------------------------
void CGLESExtLib::OutputLog(const char* pszMessage, ...)
//-----------------------------------------------------------------------------
{
	va_list args;
	va_start(args, pszMessage);

#if defined(ANDROID)
	__android_log_vprint(ANDROID_LOG_INFO, "GLESExtLib", pszMessage, args);
#else
	vprintf(pszMessage, args);
#endif

	va_end(args);
}

//-----------------------------------------------------------------------------
int CGLESExtLib::Create()
//-----------------------------------------------------------------------------
{
    /* Check OpenGL ES version */
    char* strVendor = (char*)glGetString(GL_VENDOR);
    char *strRenderer = (char*)glGetString(GL_RENDERER);
    char *strVersion = (char*)glGetString(GL_VERSION);

	OutputLog("Vendor: %s\n", strVendor);
    OutputLog("Renderer: %s\n", strRenderer);
    OutputLog("Version: %s\n", strVersion);

	/* OpenGL ES 3.0 */

    glReadBuffer                        = (PFNGLREADBUFFERPROC)GetExtension("glReadBuffer");
    glDrawRangeElements                 = (PFNGLDRAWRANGEELEMENTSPROC)GetExtension("glDrawRangeElements");
    glTexImage3D                        = (PFNGLTEXIMAGE3DPROC)GetExtension("glTexImage3D");
    glTexSubImage3D                     = (PFNGLTEXSUBIMAGE3DPROC)GetExtension("glTexSubImage3D");
    glCopyTexSubImage3D                 = (PFNGLCOPYTEXSUBIMAGE3DPROC)GetExtension("glCopyTexSubImage3D");
    glCompressedTexImage3D              = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)GetExtension("glCompressedTexImage3D");
    glCompressedTexSubImage3D           = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)GetExtension("glCompressedTexSubImage3D");
    glGenQueries                        = (PFNGLGENQUERIESPROC)GetExtension("glGenQueries");
    glDeleteQueries                     = (PFNGLDELETEQUERIESPROC)GetExtension("glDeleteQueries");
    glIsQuery                           = (PFNGLISQUERYPROC)GetExtension("glIsQuery");
    glBeginQuery                        = (PFNGLBEGINQUERYPROC)GetExtension("glBeginQuery");
    glEndQuery                          = (PFNGLENDQUERYPROC)GetExtension("glEndQuery");
    glGetQueryiv                        = (PFNGLGETQUERYIVPROC)GetExtension("glGetQueryiv");
    glGetQueryObjectuiv                 = (PFNGLGETQUERYOBJECTUIVPROC)GetExtension("glGetQueryObjectuiv");
    glUnmapBuffer                       = (PFNGLUNMAPBUFFERPROC)GetExtension("glUnmapBuffer");
    glGetBufferPointerv                 = (PFNGLGETBUFFERPOINTERVPROC)GetExtension("glGetBufferPointerv");
    glDrawBuffers                       = (PFNGLDRAWBUFFERSPROC)GetExtension("glDrawBuffers");
    glUniformMatrix2x3fv                = (PFNGLUNIFORMMATRIX2X3FVPROC)GetExtension("glUniformMatrix2x3fv");
    glUniformMatrix3x2fv                = (PFNGLUNIFORMMATRIX3X2FVPROC)GetExtension("glUniformMatrix3x2fv");
    glUniformMatrix2x4fv                = (PFNGLUNIFORMMATRIX2X4FVPROC)GetExtension("glUniformMatrix2x4fv");
    glUniformMatrix4x2fv                = (PFNGLUNIFORMMATRIX4X2FVPROC)GetExtension("glUniformMatrix4x2fv");
    glUniformMatrix3x4fv                = (PFNGLUNIFORMMATRIX3X4FVPROC)GetExtension("glUniformMatrix3x4fv");
    glUniformMatrix4x3fv                = (PFNGLUNIFORMMATRIX4X3FVPROC)GetExtension("glUniformMatrix4x3fv");
    glBlitFramebuffer                   = (PFNGLBLITFRAMEBUFFERPROC)GetExtension("glBlitFramebuffer");
    glRenderbufferStorageMultisample    = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)GetExtension("glRenderbufferStorageMultisample");
    glFramebufferTextureLayer           = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)GetExtension("glFramebufferTextureLayer");
    glMapBufferRange                    = (PFNGLMAPBUFFERRANGEPROC)GetExtension("glMapBufferRange");
    glFlushMappedBufferRange            = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)GetExtension("glFlushMappedBufferRange");
    glBindVertexArray                   = (PFNGLBINDVERTEXARRAYPROC)GetExtension("glBindVertexArray");
    glDeleteVertexArrays                = (PFNGLDELETEVERTEXARRAYSPROC)GetExtension("glDeleteVertexArrays");
    glGenVertexArrays                   = (PFNGLGENVERTEXARRAYSPROC)GetExtension("glGenVertexArrays");
    glIsVertexArray                     = (PFNGLISVERTEXARRAYPROC)GetExtension("glIsVertexArray");
    glGetIntegeri_v                     = (PFNGLGETINTEGERI_VPROC)GetExtension("glGetIntegeri_v");
    glBeginTransformFeedback            = (PFNGLBEGINTRANSFORMFEEDBACKPROC)GetExtension("glBeginTransformFeedback");
    glEndTransformFeedback              = (PFNGLENDTRANSFORMFEEDBACKPROC)GetExtension("glEndTransformFeedback");
    glBindBufferRange                   = (PFNGLBINDBUFFERRANGEPROC)GetExtension("glBindBufferRange");
    glBindBufferBase                    = (PFNGLBINDBUFFERBASEPROC)GetExtension("glBindBufferBase");
    glTransformFeedbackVaryings         = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)GetExtension("glTransformFeedbackVaryings");
    glGetTransformFeedbackVarying       = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)GetExtension("glGetTransformFeedbackVarying");
    glVertexAttribIPointer              = (PFNGLVERTEXATTRIBIPOINTERPROC)GetExtension("glVertexAttribIPointer");
    glGetVertexAttribIiv                = (PFNGLGETVERTEXATTRIBIIVPROC)GetExtension("glGetVertexAttribIiv");
    glGetVertexAttribIuiv               = (PFNGLGETVERTEXATTRIBIUIVPROC)GetExtension("glGetVertexAttribIuiv");
    glVertexAttribI4i                   = (PFNGLVERTEXATTRIBI4IPROC)GetExtension("glVertexAttribI4i");
    glVertexAttribI4ui                  = (PFNGLVERTEXATTRIBI4UIPROC)GetExtension("glVertexAttribI4ui");
    glVertexAttribI4iv                  = (PFNGLVERTEXATTRIBI4IVPROC)GetExtension("glVertexAttribI4iv");
    glVertexAttribI4uiv                 = (PFNGLVERTEXATTRIBI4UIVPROC)GetExtension("glVertexAttribI4uiv");
    glGetUniformuiv                     = (PFNGLGETUNIFORMUIVPROC)GetExtension("glGetUniformuiv");
    glGetFragDataLocation               = (PFNGLGETFRAGDATALOCATIONPROC)GetExtension("glGetFragDataLocation");
    glUniform1ui                        = (PFNGLUNIFORM1UIPROC)GetExtension("glUniform1ui");
    glUniform2ui                        = (PFNGLUNIFORM2UIPROC)GetExtension("glUniform2ui");
    glUniform3ui                        = (PFNGLUNIFORM3UIPROC)GetExtension("glUniform3ui");
    glUniform4ui                        = (PFNGLUNIFORM4UIPROC)GetExtension("glUniform4ui");
    glUniform1uiv                       = (PFNGLUNIFORM1UIVPROC)GetExtension("glUniform1uiv");
    glUniform2uiv                       = (PFNGLUNIFORM2UIVPROC)GetExtension("glUniform2uiv");
    glUniform3uiv                       = (PFNGLUNIFORM3UIVPROC)GetExtension("glUniform3uiv");
    glUniform4uiv                       = (PFNGLUNIFORM4UIVPROC)GetExtension("glUniform4uiv");
    glClearBufferiv                     = (PFNGLCLEARBUFFERIVPROC)GetExtension("glClearBufferiv");
    glClearBufferuiv                    = (PFNGLCLEARBUFFERUIVPROC)GetExtension("glClearBufferuiv");
    glClearBufferfv                     = (PFNGLCLEARBUFFERFVPROC)GetExtension("glClearBufferfv");
    glClearBufferfi                     = (PFNGLCLEARBUFFERFIPROC)GetExtension("glClearBufferfi");
    glGetStringi                        = (PFNGLGETSTRINGIPROC)GetExtension("glGetStringi");
    glCopyBufferSubData                 = (PFNGLCOPYBUFFERSUBDATAPROC)GetExtension("glCopyBufferSubData");
    glGetUniformIndices                 = (PFNGLGETUNIFORMINDICESPROC)GetExtension("glGetUniformIndices");
    glGetActiveUniformsiv               = (PFNGLGETACTIVEUNIFORMSIVPROC)GetExtension("glGetActiveUniformsiv");
    glGetUniformBlockIndex              = (PFNGLGETUNIFORMBLOCKINDEXPROC)GetExtension("glGetUniformBlockIndex");
    glGetActiveUniformBlockiv           = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)GetExtension("glGetActiveUniformBlockiv");
    glGetActiveUniformBlockName         = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)GetExtension("glGetActiveUniformBlockName");
    glUniformBlockBinding               = (PFNGLUNIFORMBLOCKBINDINGPROC)GetExtension("glUniformBlockBinding");
    glDrawArraysInstanced               = (PFNGLDRAWARRAYSINSTANCEDPROC)GetExtension("glDrawArraysInstanced");
    glDrawElementsInstanced             = (PFNGLDRAWELEMENTSINSTANCEDPROC)GetExtension("glDrawElementsInstanced");
    glFenceSync                         = (PFNGLFENCESYNCPROC)GetExtension("glFenceSync");
    glIsSync                            = (PFNGLISSYNCPROC)GetExtension("glIsSync");
    glDeleteSync                        = (PFNGLDELETESYNCPROC)GetExtension("glDeleteSync");
    glClientWaitSync                    = (PFNGLCLIENTWAITSYNCPROC)GetExtension("glClientWaitSync");
    glWaitSync                          = (PFNGLWAITSYNCPROC)GetExtension("glWaitSync");
    glGetInteger64v                     = (PFNGLGETINTEGER64VPROC)GetExtension("glGetInteger64v");
    glGetSynciv                         = (PFNGLGETSYNCIVPROC)GetExtension("glGetSynciv");
    glGetInteger64i_v                   = (PFNGLGETINTEGER64I_VPROC)GetExtension("glGetInteger64i_v");
    glGetBufferParameteri64v            = (PFNGLGETBUFFERPARAMETERI64VPROC)GetExtension("glGetBufferParameteri64v");
    glGenSamplers                       = (PFNGLGENSAMPLERSPROC)GetExtension("glGenSamplers");
    glDeleteSamplers                    = (PFNGLDELETESAMPLERSPROC)GetExtension("glDeleteSamplers");
    glIsSampler                         = (PFNGLISSAMPLERPROC)GetExtension("glIsSampler");
    glBindSampler                       = (PFNGLBINDSAMPLERPROC)GetExtension("glBindSampler");
    glSamplerParameteri                 = (PFNGLSAMPLERPARAMETERIPROC)GetExtension("glSamplerParameteri");
    glSamplerParameteriv                = (PFNGLSAMPLERPARAMETERIVPROC)GetExtension("glSamplerParameteriv");
    glSamplerParameterf                 = (PFNGLSAMPLERPARAMETERFPROC)GetExtension("glSamplerParameterf");
    glSamplerParameterfv                = (PFNGLSAMPLERPARAMETERFVPROC)GetExtension("glSamplerParameterfv");
    glGetSamplerParameteriv             = (PFNGLGETSAMPLERPARAMETERIVPROC)GetExtension("glGetSamplerParameteriv");
    glGetSamplerParameterfv             = (PFNGLGETSAMPLERPARAMETERFVPROC)GetExtension("glGetSamplerParameterfv");
    glVertexAttribDivisor               = (PFNGLVERTEXATTRIBDIVISORPROC)GetExtension("glVertexAttribDivisor");
    glBindTransformFeedback             = (PFNGLBINDTRANSFORMFEEDBACKPROC)GetExtension("glBindTransformFeedback");
    glDeleteTransformFeedbacks          = (PFNGLDELETETRANSFORMFEEDBACKSPROC)GetExtension("glDeleteTransformFeedbacks");
    glGenTransformFeedbacks             = (PFNGLGENTRANSFORMFEEDBACKSPROC)GetExtension("glGenTransformFeedbacks");
    glIsTransformFeedback               = (PFNGLISTRANSFORMFEEDBACKPROC)GetExtension("glIsTransformFeedback");
    glPauseTransformFeedback            = (PFNGLPAUSETRANSFORMFEEDBACKPROC)GetExtension("glPauseTransformFeedback");
    glResumeTransformFeedback           = (PFNGLRESUMETRANSFORMFEEDBACKPROC)GetExtension("glResumeTransformFeedback");
    glGetProgramBinary                  = (PFNGLGETPROGRAMBINARYPROC)GetExtension("glGetProgramBinary");
    glProgramBinary                     = (PFNGLPROGRAMBINARYPROC)GetExtension("glProgramBinary");
    glProgramParameteri                 = (PFNGLPROGRAMPARAMETERIPROC)GetExtension("glProgramParameteri");
    glInvalidateFramebuffer             = (PFNGLINVALIDATEFRAMEBUFFERPROC)GetExtension("glInvalidateFramebuffer");
    glInvalidateSubFramebuffer          = (PFNGLINVALIDATESUBFRAMEBUFFERPROC)GetExtension("glInvalidateSubFramebuffer");
    glTexStorage2D                      = (PFNGLTEXSTORAGE2DPROC)GetExtension("glTexStorage2D");
    glTexStorage3D                      = (PFNGLTEXSTORAGE3DPROC)GetExtension("glTexStorage3D");
    glGetInternalformativ               = (PFNGLGETINTERNALFORMATIVPROC)GetExtension("glGetInternalformativ");

	/* OpenGL ES 2.0 Extensions */

	glEGLImageTargetTexture2DOES			= (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)GetExtension("glEGLImageTargetTexture2DOES");
	glEGLImageTargetRenderbufferStorageOES	= (PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC)GetExtension("glEGLImageTargetRenderbufferStorageOES");
	glGetProgramBinaryOES					= (PFNGLGETPROGRAMBINARYOESPROC)GetExtension("glGetProgramBinaryOES");
	glProgramBinaryOES						= (PFNGLPROGRAMBINARYOESPROC)GetExtension("glProgramBinaryOES");
	glMapBufferOES							= (PFNGLMAPBUFFEROESPROC)GetExtension("glMapBufferOES");
	glUnmapBufferOES						= (PFNGLUNMAPBUFFEROESPROC)GetExtension("glUnmapBufferOES");
	glGetBufferPointervOES					= (PFNGLGETBUFFERPOINTERVOESPROC)GetExtension("glGetBufferPointervOES");
	glTexImage3DOES							= (PFNGLTEXIMAGE3DOESPROC)GetExtension("glTexImage3DOES");
	glTexSubImage3DOES						= (PFNGLTEXSUBIMAGE3DOESPROC)GetExtension("glTexSubImage3DOES");
	glCopyTexSubImage3DOES					= (PFNGLCOPYTEXSUBIMAGE3DOESPROC)GetExtension("glCopyTexSubImage3DOES");
	glCompressedTexImage3DOES				= (PFNGLCOMPRESSEDTEXIMAGE3DOESPROC)GetExtension("glCompressedTexImage3DOES");
	glCompressedTexSubImage3DOES			= (PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC)GetExtension("glCompressedTexSubImage3DOES");
	glFramebufferTexture3DOES				= (PFNGLFRAMEBUFFERTEXTURE3DOES)GetExtension("glFramebufferTexture3DOES");
	glBindVertexArrayOES					= (PFNGLBINDVERTEXARRAYOESPROC)GetExtension("glBindVertexArrayOES");
	glDeleteVertexArraysOES					= (PFNGLDELETEVERTEXARRAYSOESPROC)GetExtension("glDeleteVertexArraysOES");
	glGenVertexArraysOES					= (PFNGLGENVERTEXARRAYSOESPROC)GetExtension("glGenVertexArraysOES");
	glIsVertexArrayOES						= (PFNGLISVERTEXARRAYOESPROC)GetExtension("glIsVertexArrayOES");
	glDebugMessageControl					= (PFNGLDEBUGMESSAGECONTROLPROC)GetExtension("glDebugMessageControl");
	glDebugMessageInsert					= (PFNGLDEBUGMESSAGEINSERTPROC)GetExtension("glDebugMessageInsert");
	glDebugMessageCallback					= (PFNGLDEBUGMESSAGECALLBACKPROC)GetExtension("glDebugMessageCallback");
	glGetDebugMessageLog					= (PFNGLGETDEBUGMESSAGELOGPROC)GetExtension("glGetDebugMessageLog");
	glPushDebugGroup						= (PFNGLPUSHDEBUGGROUPPROC)GetExtension("glPushDebugGroup");
	glPopDebugGroup							= (PFNGLPOPDEBUGGROUPPROC)GetExtension("glPopDebugGroup");
	glObjectLabel							= (PFNGLOBJECTLABELPROC)GetExtension("glObjectLabel");
	glGetObjectLabel						= (PFNGLGETOBJECTLABELPROC)GetExtension("glGetObjectLabel");
	glObjectPtrLabel						= (PFNGLOBJECTPTRLABELPROC)GetExtension("glObjectPtrLabel");
	glGetObjectPtrLabel						= (PFNGLGETOBJECTPTRLABELPROC)GetExtension("glGetObjectPtrLabel");
	glGetPointerv							= (PFNGLGETPOINTERVPROC)GetExtension("glGetPointerv");
	glGetPerfMonitorGroupsAMD				= (PFNGLGETPERFMONITORGROUPSAMDPROC)GetExtension("glGetPerfMonitorGroupsAMD");
	glGetPerfMonitorCountersAMD				= (PFNGLGETPERFMONITORCOUNTERSAMDPROC)GetExtension("glGetPerfMonitorCountersAMD");
	glGetPerfMonitorGroupStringAMD			= (PFNGLGETPERFMONITORGROUPSTRINGAMDPROC)GetExtension("glGetPerfMonitorGroupStringAMD");
	glGetPerfMonitorCounterStringAMD		= (PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC)GetExtension("glGetPerfMonitorCounterStringAMD");
	glGetPerfMonitorCounterInfoAMD			= (PFNGLGETPERFMONITORCOUNTERINFOAMDPROC)GetExtension("glGetPerfMonitorCounterInfoAMD");
	glGenPerfMonitorsAMD					= (PFNGLGENPERFMONITORSAMDPROC)GetExtension("glGenPerfMonitorsAMD");
	glDeletePerfMonitorsAMD					= (PFNGLDELETEPERFMONITORSAMDPROC)GetExtension("glDeletePerfMonitorsAMD");
	glSelectPerfMonitorCountersAMD			= (PFNGLSELECTPERFMONITORCOUNTERSAMDPROC)GetExtension("glSelectPerfMonitorCountersAMD");
	glBeginPerfMonitorAMD					= (PFNGLBEGINPERFMONITORAMDPROC)GetExtension("glBeginPerfMonitorAMD");
	glEndPerfMonitorAMD						= (PFNGLENDPERFMONITORAMDPROC)GetExtension("glEndPerfMonitorAMD");
	glGetPerfMonitorCounterDataAMD			= (PFNGLGETPERFMONITORCOUNTERDATAAMDPROC)GetExtension("glGetPerfMonitorCounterDataAMD");
	glBlitFramebufferANGLE					= (PFNGLBLITFRAMEBUFFERANGLEPROC)GetExtension("glBlitFramebufferANGLE");
	glRenderbufferStorageMultisampleANGLE	= (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC)GetExtension("glRenderbufferStorageMultisampleANGLE");
	glDrawArraysInstancedANGLE				= (PFLGLDRAWARRAYSINSTANCEDANGLEPROC)GetExtension("glDrawArraysInstancedANGLE");
	glDrawElementsInstancedANGLE			= (PFLGLDRAWELEMENTSINSTANCEDANGLEPROC)GetExtension("glDrawElementsInstancedANGLE");
	glVertexAttribDivisorANGLE				= (PFLGLVERTEXATTRIBDIVISORANGLEPROC)GetExtension("glVertexAttribDivisorANGLE");
	glGetTranslatedShaderSourceANGLE		= (PFLGLGETTRANSLATEDSHADERSOURCEANGLEPROC)GetExtension("glGetTranslatedShaderSourceANGLE");
	glCopyTextureLevelsAPPLE				= (PFNGLCOPYTEXTURELEVELSAPPLEPROC)GetExtension("glCopyTextureLevelsAPPLE");
	glRenderbufferStorageMultisampleAPPLE	= (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC)GetExtension("glRenderbufferStorageMultisampleAPPLE");
	glResolveMultisampleFramebufferAPPLE	= (PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC)GetExtension("glResolveMultisampleFramebufferAPPLE");
	glFenceSyncAPPLE						= (PFNGLFENCESYNCAPPLEPROC)GetExtension("glFenceSyncAPPLE");
	glIsSyncAPPLE							= (PFNGLISSYNCAPPLEPROC)GetExtension("glIsSyncAPPLE");
	glDeleteSyncAPPLE						= (PFNGLDELETESYNCAPPLEPROC)GetExtension("glDeleteSyncAPPLE");
	glClientWaitSyncAPPLE					= (PFNGLCLIENTWAITSYNCAPPLEPROC)GetExtension("glClientWaitSyncAPPLE");
	glWaitSyncAPPLE							= (PFNGLWAITSYNCAPPLEPROC)GetExtension("glWaitSyncAPPLE");
	glGetInteger64vAPPLE					= (PFNGLGETINTEGER64VAPPLEPROC)GetExtension("glGetInteger64vAPPLE");
	glGetSyncivAPPLE						= (PFNGLGETSYNCIVAPPLEPROC)GetExtension("glGetSyncivAPPLE");
	glLabelObjectEXT						= (PFNGLLABELOBJECTEXTPROC)GetExtension("glLabelObjectEXT");
	glGetObjectLabelEXT						= (PFNGLGETOBJECTLABELEXTPROC)GetExtension("glGetObjectLabelEXT");
	glInsertEventMarkerEXT					= (PFNGLINSERTEVENTMARKEREXTPROC)GetExtension("glInsertEventMarkerEXT");
	glPushGroupMarkerEXT					= (PFNGLPUSHGROUPMARKEREXTPROC)GetExtension("glPushGroupMarkerEXT");
	glPopGroupMarkerEXT						= (PFNGLPOPGROUPMARKEREXTPROC)GetExtension("glPopGroupMarkerEXT");
	glDiscardFramebufferEXT					= (PFNGLDISCARDFRAMEBUFFEREXTPROC)GetExtension("glDiscardFramebufferEXT");
	glMapBufferRangeEXT						= (PFNGLMAPBUFFERRANGEEXTPROC)GetExtension("glMapBufferRangeEXT");
	glFlushMappedBufferRangeEXT				= (PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC)GetExtension("glFlushMappedBufferRangeEXT");
	glRenderbufferStorageMultisampleEXT		= (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)GetExtension("glRenderbufferStorageMultisampleEXT");
	glFramebufferTexture2DMultisampleEXT	= (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)GetExtension("glFramebufferTexture2DMultisampleEXT");
	glReadBufferIndexedEXT					= (PFNGLREADBUFFERINDEXEDEXTPROC)GetExtension("glReadBufferIndexedEXT");
	glDrawBuffersIndexedEXT					= (PFNGLDRAWBUFFERSINDEXEDEXTPROC)GetExtension("glDrawBuffersIndexedEXT");
	glGetIntegeri_vEXT						= (PFNGLGETINTEGERI_VEXTPROC)GetExtension("glGetIntegeri_vEXT");
	glMultiDrawArraysEXT					= (PFNGLMULTIDRAWARRAYSEXTPROC)GetExtension("glMultiDrawArraysEXT");
	glMultiDrawElementsEXT					= (PFNGLMULTIDRAWELEMENTSEXTPROC)GetExtension("glMultiDrawElementsEXT");
	glGenQueriesEXT							= (PFNGLGENQUERIESEXTPROC)GetExtension("glGenQueriesEXT");
	glDeleteQueriesEXT						= (PFNGLDELETEQUERIESEXTPROC)GetExtension("glDeleteQueriesEXT");
	glIsQueryEXT							= (PFNGLISQUERYEXTPROC)GetExtension("glIsQueryEXT");
	glBeginQueryEXT							= (PFNGLBEGINQUERYEXTPROC)GetExtension("glBeginQueryEXT");
	glEndQueryEXT							= (PFNGLENDQUERYEXTPROC)GetExtension("glEndQueryEXT");
	glGetQueryivEXT							= (PFNGLGETQUERYIVEXTPROC)GetExtension("glGetQueryivEXT");
	glGetQueryObjectuivEXT					= (PFNGLGETQUERYOBJECTUIVEXTPROC)GetExtension("glGetQueryObjectuivEXT");
	glGetGraphicsResetStatusEXT				= (PFNGLGETGRAPHICSRESETSTATUSEXTPROC)GetExtension("glGetGraphicsResetStatusEXT");
	glReadnPixelsEXT						= (PFNGLREADNPIXELSEXTPROC)GetExtension("glReadnPixelsEXT");
	glGetnUniformfvEXT						= (PFNGLGETNUNIFORMFVEXTPROC)GetExtension("glGetnUniformfvEXT");
	glGetnUniformivEXT						= (PFNGLGETNUNIFORMIVEXTPROC)GetExtension("glGetnUniformivEXT");
	glUseProgramStagesEXT					= (PFNGLUSEPROGRAMSTAGESEXTPROC)GetExtension("glUseProgramStagesEXT");
	glActiveShaderProgramEXT				= (PFNGLACTIVESHADERPROGRAMEXTPROC)GetExtension("glActiveShaderProgramEXT");
	glCreateShaderProgramvEXT				= (PFNGLCREATESHADERPROGRAMVEXTPROC)GetExtension("glCreateShaderProgramvEXT");
	glBindProgramPipelineEXT				= (PFNGLBINDPROGRAMPIPELINEEXTPROC)GetExtension("glBindProgramPipelineEXT");
	glDeleteProgramPipelinesEXT				= (PFNGLDELETEPROGRAMPIPELINESEXTPROC)GetExtension("glDeleteProgramPipelinesEXT");
	glGenProgramPipelinesEXT				= (PFNGLGENPROGRAMPIPELINESEXTPROC)GetExtension("glGenProgramPipelinesEXT");
	glIsProgramPipelineEXT					= (PFNGLISPROGRAMPIPELINEEXTPROC)GetExtension("glIsProgramPipelineEXT");
	glProgramParameteriEXT					= (PFNGLPROGRAMPARAMETERIEXTPROC)GetExtension("glProgramParameteriEXT");
	glGetProgramPipelineivEXT				= (PFNGLGETPROGRAMPIPELINEIVEXTPROC)GetExtension("glGetProgramPipelineivEXT");
	glProgramUniform1iEXT					= (PFNGLPROGRAMUNIFORM1IEXTPROC)GetExtension("glProgramUniform1iEXT");
	glProgramUniform2iEXT					= (PFNGLPROGRAMUNIFORM2IEXTPROC)GetExtension("glProgramUniform2iEXT");
	glProgramUniform3iEXT					= (PFNGLPROGRAMUNIFORM3IEXTPROC)GetExtension("glProgramUniform3iEXT");
	glProgramUniform4iEXT					= (PFNGLPROGRAMUNIFORM4IEXTPROC)GetExtension("glProgramUniform4iEXT");
	glProgramUniform1fEXT					= (PFNGLPROGRAMUNIFORM1FEXTPROC)GetExtension("glProgramUniform1fEXT");
	glProgramUniform2fEXT					= (PFNGLPROGRAMUNIFORM2FEXTPROC)GetExtension("glProgramUniform2fEXT");
	glProgramUniform3fEXT					= (PFNGLPROGRAMUNIFORM3FEXTPROC)GetExtension("glProgramUniform3fEXT");
	glProgramUniform4fEXT					= (PFNGLPROGRAMUNIFORM4FEXTPROC)GetExtension("glProgramUniform4fEXT");
	glProgramUniform1ivEXT					= (PFNGLPROGRAMUNIFORM1IVEXTPROC)GetExtension("glProgramUniform1ivEXT");
	glProgramUniform2ivEXT					= (PFNGLPROGRAMUNIFORM2IVEXTPROC)GetExtension("glProgramUniform2ivEXT");
	glProgramUniform3ivEXT					= (PFNGLPROGRAMUNIFORM3IVEXTPROC)GetExtension("glProgramUniform3ivEXT");
	glProgramUniform4ivEXT					= (PFNGLPROGRAMUNIFORM4IVEXTPROC)GetExtension("glProgramUniform4ivEXT");
	glProgramUniform1fvEXT					= (PFNGLPROGRAMUNIFORM1FVEXTPROC)GetExtension("glProgramUniform1fvEXT");
	glProgramUniform2fvEXT					= (PFNGLPROGRAMUNIFORM2FVEXTPROC)GetExtension("glProgramUniform2fvEXT");
	glProgramUniform3fvEXT					= (PFNGLPROGRAMUNIFORM3FVEXTPROC)GetExtension("glProgramUniform3fvEXT");
	glProgramUniform4fvEXT					= (PFNGLPROGRAMUNIFORM4FVEXTPROC)GetExtension("glProgramUniform4fvEXT");
	glProgramUniformMatrix2fvEXT			= (PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC)GetExtension("glProgramUniformMatrix2fvEXT");
	glProgramUniformMatrix3fvEXT			= (PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC)GetExtension("glProgramUniformMatrix3fvEXT");
	glProgramUniformMatrix4fvEXT			= (PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC)GetExtension("glProgramUniformMatrix4fvEXT");
	glValidateProgramPipelineEXT			= (PFNGLVALIDATEPROGRAMPIPELINEEXTPROC)GetExtension("glValidateProgramPipelineEXT");
	glGetProgramPipelineInfoLogEXT			= (PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC)GetExtension("glGetProgramPipelineInfoLogEXT");
	glTexStorage1DEXT						= (PFNGLTEXSTORAGE1DEXTPROC)GetExtension("glTexStorage1DEXT");
	glTexStorage2DEXT						= (PFNGLTEXSTORAGE2DEXTPROC)GetExtension("glTexStorage2DEXT");
	glTexStorage3DEXT						= (PFNGLTEXSTORAGE3DEXTPROC)GetExtension("glTexStorage3DEXT");
	glTextureStorage1DEXT					= (PFNGLTEXTURESTORAGE1DEXTPROC)GetExtension("glTextureStorage1DEXT");
	glTextureStorage2DEXT					= (PFNGLTEXTURESTORAGE2DEXTPROC)GetExtension("glTextureStorage2DEXT");
	glTextureStorage3DEXT					= (PFNGLTEXTURESTORAGE3DEXTPROC)GetExtension("glTextureStorage3DEXT");
	glRenderbufferStorageMultisampleIMG		= (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC)GetExtension("glRenderbufferStorageMultisampleIMG");
	glFramebufferTexture2DMultisampleIMG	= (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC)GetExtension("glFramebufferTexture2DMultisampleIMG");
	glCoverageMaskNV						= (PFNGLCOVERAGEMASKNVPROC)GetExtension("glCoverageMaskNV");
	glCoverageOperationNV					= (PFNGLCOVERAGEOPERATIONNVPROC)GetExtension("glCoverageOperationNV");
	glDrawBuffersNV							= (PFNGLDRAWBUFFERSNVPROC)GetExtension("glDrawBuffersNV");
	glDrawArraysInstancedNV					= (PFNDRAWARRAYSINSTANCEDNVPROC)GetExtension("glDrawArraysInstancedNV");
	glDrawElementsInstancedNV				= (PFNDRAWELEMENTSINSTANCEDNVPROC)GetExtension("glDrawElementsInstancedNV");
	glDeleteFencesNV						= (PFNGLDELETEFENCESNVPROC)GetExtension("glDeleteFencesNV");
	glGenFencesNV							= (PFNGLGENFENCESNVPROC)GetExtension("glGenFencesNV");
	glIsFenceNV								= (PFNGLISFENCENVPROC)GetExtension("glIsFenceNV");
	glTestFenceNV							= (PFNGLTESTFENCENVPROC)GetExtension("glTestFenceNV");
	glGetFenceivNV							= (PFNGLGETFENCEIVNVPROC)GetExtension("glGetFenceivNV");
	glFinishFenceNV							= (PFNGLFINISHFENCENVPROC)GetExtension("glFinishFenceNV");
	glSetFenceNV							= (PFNGLSETFENCENVPROC)GetExtension("glSetFenceNV");
	glBlitFramebufferNV						= (PFNBLITFRAMEBUFFERNVPROC)GetExtension("glBlitFramebufferNV");
	glRenderbufferStorageMultisampleNV		= (PFNRENDERBUFFERSTORAGEMULTISAMPLENVPROC)GetExtension("glRenderbufferStorageMultisampleNV");
	glVertexAttribDivisorNV					= (PFNVERTEXATTRIBDIVISORNVPROC)GetExtension("glVertexAttribDivisorNV");
	glReadBufferNV							= (PFNGLREADBUFFERNVPROC)GetExtension("glReadBufferNV");
	glAlphaFuncQCOM							= (PFNGLALPHAFUNCQCOMPROC)GetExtension("glAlphaFuncQCOM");
	glGetDriverControlsQCOM					= (PFNGLGETDRIVERCONTROLSQCOMPROC)GetExtension("glGetDriverControlsQCOM");
	glGetDriverControlStringQCOM			= (PFNGLGETDRIVERCONTROLSTRINGQCOMPROC)GetExtension("glGetDriverControlStringQCOM");
	glEnableDriverControlQCOM				= (PFNGLENABLEDRIVERCONTROLQCOMPROC)GetExtension("glEnableDriverControlQCOM");
	glDisableDriverControlQCOM				= (PFNGLDISABLEDRIVERCONTROLQCOMPROC)GetExtension("glDisableDriverControlQCOM");
	glExtGetTexturesQCOM					= (PFNGLEXTGETTEXTURESQCOMPROC)GetExtension("glExtGetTexturesQCOM");
	glExtGetBuffersQCOM						= (PFNGLEXTGETBUFFERSQCOMPROC)GetExtension("glExtGetBuffersQCOM");
	glExtGetRenderbuffersQCOM				= (PFNGLEXTGETRENDERBUFFERSQCOMPROC)GetExtension("glExtGetRenderbuffersQCOM");
	glExtGetFramebuffersQCOM				= (PFNGLEXTGETFRAMEBUFFERSQCOMPROC)GetExtension("glExtGetFramebuffersQCOM");
	glExtGetTexLevelParameterivQCOM			= (PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC)GetExtension("glExtGetTexLevelParameterivQCOM");
	glExtTexObjectStateOverrideiQCOM		= (PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC)GetExtension("glExtTexObjectStateOverrideiQCOM");
	glExtGetTexSubImageQCOM					= (PFNGLEXTGETTEXSUBIMAGEQCOMPROC)GetExtension("glExtGetTexSubImageQCOM");
	glExtGetBufferPointervQCOM				= (PFNGLEXTGETBUFFERPOINTERVQCOMPROC)GetExtension("glExtGetBufferPointervQCOM");
	glExtGetShadersQCOM						= (PFNGLEXTGETSHADERSQCOMPROC)GetExtension("glExtGetShadersQCOM");
	glExtGetProgramsQCOM					= (PFNGLEXTGETPROGRAMSQCOMPROC)GetExtension("glExtGetProgramsQCOM");
	glExtIsProgramBinaryQCOM				= (PFNGLEXTISPROGRAMBINARYQCOMPROC)GetExtension("glExtIsProgramBinaryQCOM");
	glExtGetProgramBinarySourceQCOM			= (PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC)GetExtension("glExtGetProgramBinarySourceQCOM");
	glStartTilingQCOM						= (PFNGLSTARTTILINGQCOMPROC)GetExtension("glStartTilingQCOM");
	glEndTilingQCOM							= (PFNGLENDTILINGQCOMPROC)GetExtension("glEndTilingQCOM");


    return 1;
}

