//=============================================================================
// Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
//=============================================================================
// Provides access to OpenGL ES 3.0 functions through eglGetProcAddress
// (until Android provides them).
//=============================================================================

#ifndef __FRM_GLES3_H__
#define __FRM_GLES3_H__

#ifdef _OGLES3
#undef GL_GLEXT_PROTOTYPES  // we're going to define extension function prototypes here
#endif

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES2/gl2ext.h>

/* OpenGL ES 2.0 Extensions */

extern PFNGLEGLIMAGETARGETTEXTURE2DOESPROC				glEGLImageTargetTexture2DOES;
extern PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC	glEGLImageTargetRenderbufferStorageOES;
extern PFNGLGETPROGRAMBINARYOESPROC						glGetProgramBinaryOES;
extern PFNGLPROGRAMBINARYOESPROC						glProgramBinaryOES;
extern PFNGLMAPBUFFEROESPROC							glMapBufferOES;
extern PFNGLUNMAPBUFFEROESPROC							glUnmapBufferOES;
extern PFNGLGETBUFFERPOINTERVOESPROC					glGetBufferPointervOES;
extern PFNGLTEXIMAGE3DOESPROC							glTexImage3DOES;
extern PFNGLTEXSUBIMAGE3DOESPROC						glTexSubImage3DOES;
extern PFNGLCOPYTEXSUBIMAGE3DOESPROC					glCopyTexSubImage3DOES;
extern PFNGLCOMPRESSEDTEXIMAGE3DOESPROC					glCompressedTexImage3DOES;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC				glCompressedTexSubImage3DOES;
extern PFNGLFRAMEBUFFERTEXTURE3DOESPROC					glFramebufferTexture3DOES;
extern PFNGLBINDVERTEXARRAYOESPROC						glBindVertexArrayOES;
extern PFNGLDELETEVERTEXARRAYSOESPROC					glDeleteVertexArraysOES;
extern PFNGLGENVERTEXARRAYSOESPROC						glGenVertexArraysOES;
extern PFNGLISVERTEXARRAYOESPROC						glIsVertexArrayOES;
extern PFNGLDEBUGMESSAGECONTROLKHRPROC					glDebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTKHRPROC					glDebugMessageInsert;
extern PFNGLDEBUGMESSAGECALLBACKKHRPROC					glDebugMessageCallback;
extern PFNGLGETDEBUGMESSAGELOGKHRPROC					glGetDebugMessageLog;
extern PFNGLPUSHDEBUGGROUPKHRPROC						glPushDebugGroup;
extern PFNGLPOPDEBUGGROUPKHRPROC						glPopDebugGroup;
extern PFNGLOBJECTLABELKHRPROC							glObjectLabel;
extern PFNGLGETOBJECTLABELKHRPROC						glGetObjectLabel;
extern PFNGLOBJECTPTRLABELKHRPROC						glObjectPtrLabel;
extern PFNGLGETOBJECTPTRLABELKHRPROC					glGetObjectPtrLabel;
extern PFNGLGETPOINTERVKHRPROC							glGetPointerv;
extern PFNGLGETPERFMONITORGROUPSAMDPROC					glGetPerfMonitorGroupsAMD;
extern PFNGLGETPERFMONITORCOUNTERSAMDPROC				glGetPerfMonitorCountersAMD;
extern PFNGLGETPERFMONITORGROUPSTRINGAMDPROC			glGetPerfMonitorGroupStringAMD;
extern PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC			glGetPerfMonitorCounterStringAMD;
extern PFNGLGETPERFMONITORCOUNTERINFOAMDPROC			glGetPerfMonitorCounterInfoAMD;
extern PFNGLGENPERFMONITORSAMDPROC						glGenPerfMonitorsAMD;
extern PFNGLDELETEPERFMONITORSAMDPROC					glDeletePerfMonitorsAMD;
extern PFNGLSELECTPERFMONITORCOUNTERSAMDPROC			glSelectPerfMonitorCountersAMD;
extern PFNGLBEGINPERFMONITORAMDPROC						glBeginPerfMonitorAMD;
extern PFNGLENDPERFMONITORAMDPROC						glEndPerfMonitorAMD;
extern PFNGLGETPERFMONITORCOUNTERDATAAMDPROC			glGetPerfMonitorCounterDataAMD;
extern PFNGLBLITFRAMEBUFFERANGLEPROC					glBlitFramebufferANGLE;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC		glRenderbufferStorageMultisampleANGLE;
extern PFNGLDRAWARRAYSINSTANCEDANGLEPROC				glDrawArraysInstancedANGLE;
extern PFNGLDRAWELEMENTSINSTANCEDANGLEPROC				glDrawElementsInstancedANGLE;
extern PFNGLVERTEXATTRIBDIVISORANGLEPROC				glVertexAttribDivisorANGLE;
extern PFNGLGETTRANSLATEDSHADERSOURCEANGLEPROC			glGetTranslatedShaderSourceANGLE;
extern PFNGLCOPYTEXTURELEVELSAPPLEPROC					glCopyTextureLevelsAPPLE;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC		glRenderbufferStorageMultisampleAPPLE;
extern PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC		glResolveMultisampleFramebufferAPPLE;
extern PFNGLFENCESYNCAPPLEPROC							glFenceSyncAPPLE;
extern PFNGLISSYNCAPPLEPROC								glIsSyncAPPLE;
extern PFNGLDELETESYNCAPPLEPROC							glDeleteSyncAPPLE;
extern PFNGLCLIENTWAITSYNCAPPLEPROC						glClientWaitSyncAPPLE;
extern PFNGLWAITSYNCAPPLEPROC							glWaitSyncAPPLE;
extern PFNGLGETINTEGER64VAPPLEPROC						glGetInteger64vAPPLE;
extern PFNGLGETSYNCIVAPPLEPROC							glGetSyncivAPPLE;
extern PFNGLLABELOBJECTEXTPROC							glLabelObjectEXT;
extern PFNGLGETOBJECTLABELEXTPROC						glGetObjectLabelEXT;
extern PFNGLINSERTEVENTMARKEREXTPROC					glInsertEventMarkerEXT;
extern PFNGLPUSHGROUPMARKEREXTPROC						glPushGroupMarkerEXT;
extern PFNGLPOPGROUPMARKEREXTPROC						glPopGroupMarkerEXT;
extern PFNGLDISCARDFRAMEBUFFEREXTPROC					glDiscardFramebufferEXT;
extern PFNGLMAPBUFFERRANGEEXTPROC						glMapBufferRangeEXT;
extern PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC				glFlushMappedBufferRangeEXT;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC		glRenderbufferStorageMultisampleEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC		glFramebufferTexture2DMultisampleEXT;
extern PFNGLREADBUFFERINDEXEDEXTPROC					glReadBufferIndexedEXT;
extern PFNGLDRAWBUFFERSINDEXEDEXTPROC					glDrawBuffersIndexedEXT;
extern PFNGLGETINTEGERI_VEXTPROC						glGetIntegeri_vEXT;
extern PFNGLMULTIDRAWARRAYSEXTPROC						glMultiDrawArraysEXT;
extern PFNGLMULTIDRAWELEMENTSEXTPROC					glMultiDrawElementsEXT;
extern PFNGLGENQUERIESEXTPROC							glGenQueriesEXT;
extern PFNGLDELETEQUERIESEXTPROC						glDeleteQueriesEXT;
extern PFNGLISQUERYEXTPROC								glIsQueryEXT;
extern PFNGLBEGINQUERYEXTPROC							glBeginQueryEXT;
extern PFNGLENDQUERYEXTPROC								glEndQueryEXT;
extern PFNGLGETQUERYIVEXTPROC							glGetQueryivEXT;
extern PFNGLGETQUERYOBJECTUIVEXTPROC					glGetQueryObjectuivEXT;
extern PFNGLGETGRAPHICSRESETSTATUSEXTPROC				glGetGraphicsResetStatusEXT;
extern PFNGLREADNPIXELSEXTPROC							glReadnPixelsEXT;
extern PFNGLGETNUNIFORMFVEXTPROC						glGetnUniformfvEXT;
extern PFNGLGETNUNIFORMIVEXTPROC						glGetnUniformivEXT;
extern PFNGLUSEPROGRAMSTAGESEXTPROC						glUseProgramStagesEXT;
extern PFNGLACTIVESHADERPROGRAMEXTPROC					glActiveShaderProgramEXT;
extern PFNGLCREATESHADERPROGRAMVEXTPROC					glCreateShaderProgramvEXT;
extern PFNGLBINDPROGRAMPIPELINEEXTPROC					glBindProgramPipelineEXT;
extern PFNGLDELETEPROGRAMPIPELINESEXTPROC				glDeleteProgramPipelinesEXT;
extern PFNGLGENPROGRAMPIPELINESEXTPROC					glGenProgramPipelinesEXT;
extern PFNGLISPROGRAMPIPELINEEXTPROC					glIsProgramPipelineEXT;
extern PFNGLPROGRAMPARAMETERIEXTPROC					glProgramParameteriEXT;
extern PFNGLGETPROGRAMPIPELINEIVEXTPROC					glGetProgramPipelineivEXT;
extern PFNGLPROGRAMUNIFORM1IEXTPROC						glProgramUniform1iEXT;
extern PFNGLPROGRAMUNIFORM2IEXTPROC						glProgramUniform2iEXT;
extern PFNGLPROGRAMUNIFORM3IEXTPROC						glProgramUniform3iEXT;
extern PFNGLPROGRAMUNIFORM4IEXTPROC						glProgramUniform4iEXT;
extern PFNGLPROGRAMUNIFORM1FEXTPROC						glProgramUniform1fEXT;
extern PFNGLPROGRAMUNIFORM2FEXTPROC						glProgramUniform2fEXT;
extern PFNGLPROGRAMUNIFORM3FEXTPROC						glProgramUniform3fEXT;
extern PFNGLPROGRAMUNIFORM4FEXTPROC						glProgramUniform4fEXT;
extern PFNGLPROGRAMUNIFORM1IVEXTPROC					glProgramUniform1ivEXT;
extern PFNGLPROGRAMUNIFORM2IVEXTPROC					glProgramUniform2ivEXT;
extern PFNGLPROGRAMUNIFORM3IVEXTPROC					glProgramUniform3ivEXT;
extern PFNGLPROGRAMUNIFORM4IVEXTPROC					glProgramUniform4ivEXT;
extern PFNGLPROGRAMUNIFORM1FVEXTPROC					glProgramUniform1fvEXT;
extern PFNGLPROGRAMUNIFORM2FVEXTPROC					glProgramUniform2fvEXT;
extern PFNGLPROGRAMUNIFORM3FVEXTPROC					glProgramUniform3fvEXT;
extern PFNGLPROGRAMUNIFORM4FVEXTPROC					glProgramUniform4fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC				glProgramUniformMatrix2fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC				glProgramUniformMatrix3fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC				glProgramUniformMatrix4fvEXT;
extern PFNGLVALIDATEPROGRAMPIPELINEEXTPROC				glValidateProgramPipelineEXT;
extern PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC			glGetProgramPipelineInfoLogEXT;
extern PFNGLTEXSTORAGE1DEXTPROC							glTexStorage1DEXT;
extern PFNGLTEXSTORAGE2DEXTPROC							glTexStorage2DEXT;
extern PFNGLTEXSTORAGE3DEXTPROC							glTexStorage3DEXT;
extern PFNGLTEXTURESTORAGE1DEXTPROC						glTextureStorage1DEXT;
extern PFNGLTEXTURESTORAGE2DEXTPROC						glTextureStorage2DEXT;
extern PFNGLTEXTURESTORAGE3DEXTPROC						glTextureStorage3DEXT;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC		glRenderbufferStorageMultisampleIMG;
extern PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC		glFramebufferTexture2DMultisampleIMG;
extern PFNGLCOVERAGEMASKNVPROC							glCoverageMaskNV;
extern PFNGLCOVERAGEOPERATIONNVPROC						glCoverageOperationNV;
extern PFNGLDRAWBUFFERSNVPROC							glDrawBuffersNV;
extern PFNGLDRAWARRAYSINSTANCEDNVPROC					glDrawArraysInstancedNV;
extern PFNGLDRAWELEMENTSINSTANCEDNVPROC					glDrawElementsInstancedNV;
extern PFNGLDELETEFENCESNVPROC							glDeleteFencesNV;
extern PFNGLGENFENCESNVPROC								glGenFencesNV;
extern PFNGLISFENCENVPROC								glIsFenceNV;
extern PFNGLTESTFENCENVPROC								glTestFenceNV;
extern PFNGLGETFENCEIVNVPROC							glGetFenceivNV;
extern PFNGLFINISHFENCENVPROC							glFinishFenceNV;
extern PFNGLSETFENCENVPROC								glSetFenceNV;
extern PFNGLBLITFRAMEBUFFERNVPROC						glBlitFramebufferNV;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLENVPROC		glRenderbufferStorageMultisampleNV;
extern PFNGLVERTEXATTRIBDIVISORNVPROC					glVertexAttribDivisorNV;
extern PFNGLREADBUFFERNVPROC							glReadBufferNV;
extern PFNGLALPHAFUNCQCOMPROC							glAlphaFuncQCOM;
extern PFNGLGETDRIVERCONTROLSQCOMPROC					glGetDriverControlsQCOM;
extern PFNGLGETDRIVERCONTROLSTRINGQCOMPROC				glGetDriverControlStringQCOM;
extern PFNGLENABLEDRIVERCONTROLQCOMPROC					glEnableDriverControlQCOM;
extern PFNGLDISABLEDRIVERCONTROLQCOMPROC				glDisableDriverControlQCOM;
extern PFNGLEXTGETTEXTURESQCOMPROC						glExtGetTexturesQCOM;
extern PFNGLEXTGETBUFFERSQCOMPROC						glExtGetBuffersQCOM;
extern PFNGLEXTGETRENDERBUFFERSQCOMPROC					glExtGetRenderbuffersQCOM;
extern PFNGLEXTGETFRAMEBUFFERSQCOMPROC					glExtGetFramebuffersQCOM;
extern PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC			glExtGetTexLevelParameterivQCOM;
extern PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC			glExtTexObjectStateOverrideiQCOM;
extern PFNGLEXTGETTEXSUBIMAGEQCOMPROC					glExtGetTexSubImageQCOM;
extern PFNGLEXTGETBUFFERPOINTERVQCOMPROC				glExtGetBufferPointervQCOM;
extern PFNGLEXTGETSHADERSQCOMPROC						glExtGetShadersQCOM;
extern PFNGLEXTGETPROGRAMSQCOMPROC						glExtGetProgramsQCOM;
extern PFNGLEXTISPROGRAMBINARYQCOMPROC					glExtIsProgramBinaryQCOM;
extern PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC			glExtGetProgramBinarySourceQCOM;
extern PFNGLSTARTTILINGQCOMPROC							glStartTilingQCOM;
extern PFNGLENDTILINGQCOMPROC							glEndTilingQCOM;

typedef void (*fpnGenericFunctionPointer)(void);

int RetrieveES2ExtES3Funcs();


#endif 
