//=============================================================================
// Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
//=============================================================================
// FILE: glesextlib.h
//
// OpenGL ES Extension Library.
//=============================================================================

#ifndef __GLES_EXT_LIB_H__
#define __GLES_EXT_LIB_H__

#define GLESEXTLIB 1
#define USE_GLES3_WRAPPER 1

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>

/* OpenGL ES 3.0 */

typedef void           (GL_APIENTRYP PFNGLREADBUFFERPROC) (GLenum mode);
typedef void           (GL_APIENTRYP PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices);
typedef void           (GL_APIENTRYP PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
typedef void           (GL_APIENTRYP PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels);
typedef void           (GL_APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void           (GL_APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data);
typedef void           (GL_APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data);
typedef void           (GL_APIENTRYP PFNGLGENQUERIESPROC) (GLsizei n, GLuint* ids);
typedef void           (GL_APIENTRYP PFNGLDELETEQUERIESPROC) (GLsizei n, const GLuint* ids);
typedef GLboolean      (GL_APIENTRYP PFNGLISQUERYPROC) (GLuint id);
typedef void           (GL_APIENTRYP PFNGLBEGINQUERYPROC) (GLenum target, GLuint id);
typedef void           (GL_APIENTRYP PFNGLENDQUERYPROC) (GLenum target);
typedef void           (GL_APIENTRYP PFNGLGETQUERYIVPROC) (GLenum target, GLenum pname, GLint* params);
typedef void           (GL_APIENTRYP PFNGLGETQUERYOBJECTUIVPROC) (GLuint id, GLenum pname, GLuint* params);
typedef GLboolean      (GL_APIENTRYP PFNGLUNMAPBUFFERPROC) (GLenum target);
typedef void           (GL_APIENTRYP PFNGLGETBUFFERPOINTERVPROC) (GLenum target, GLenum pname, GLvoid** params);
typedef void           (GL_APIENTRYP PFNGLDRAWBUFFERSPROC) (GLsizei n, const GLenum* bufs);
typedef void           (GL_APIENTRYP PFNGLUNIFORMMATRIX2X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void           (GL_APIENTRYP PFNGLUNIFORMMATRIX3X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void           (GL_APIENTRYP PFNGLUNIFORMMATRIX2X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void           (GL_APIENTRYP PFNGLUNIFORMMATRIX4X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void           (GL_APIENTRYP PFNGLUNIFORMMATRIX3X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void           (GL_APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void           (GL_APIENTRYP PFNGLBLITFRAMEBUFFERPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void           (GL_APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void           (GL_APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef GLvoid*        (GL_APIENTRYP PFNGLMAPBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef void           (GL_APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEPROC) (GLenum target, GLintptr offset, GLsizeiptr length);
typedef void           (GL_APIENTRYP PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void           (GL_APIENTRYP PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint* arrays);
typedef void           (GL_APIENTRYP PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);
typedef GLboolean      (GL_APIENTRYP PFNGLISVERTEXARRAYPROC) (GLuint array);
typedef void           (GL_APIENTRYP PFNGLGETINTEGERI_VPROC) (GLenum target, GLuint index, GLint* data);
typedef void           (GL_APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKPROC) (GLenum primitiveMode);
typedef void           (GL_APIENTRYP PFNGLENDTRANSFORMFEEDBACKPROC) (void);
typedef void           (GL_APIENTRYP PFNGLBINDBUFFERRANGEPROC) (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void           (GL_APIENTRYP PFNGLBINDBUFFERBASEPROC) (GLenum target, GLuint index, GLuint buffer);
typedef void           (GL_APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSPROC) (GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode);
typedef void           (GL_APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name);
typedef void           (GL_APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
typedef void           (GL_APIENTRYP PFNGLGETVERTEXATTRIBIIVPROC) (GLuint index, GLenum pname, GLint* params);
typedef void           (GL_APIENTRYP PFNGLGETVERTEXATTRIBIUIVPROC) (GLuint index, GLenum pname, GLuint* params);
typedef void           (GL_APIENTRYP PFNGLVERTEXATTRIBI4IPROC) (GLuint index, GLint x, GLint y, GLint z, GLint w);
typedef void           (GL_APIENTRYP PFNGLVERTEXATTRIBI4UIPROC) (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
typedef void           (GL_APIENTRYP PFNGLVERTEXATTRIBI4IVPROC) (GLuint index, const GLint* v);
typedef void           (GL_APIENTRYP PFNGLVERTEXATTRIBI4UIVPROC) (GLuint index, const GLuint* v);
typedef void           (GL_APIENTRYP PFNGLGETUNIFORMUIVPROC) (GLuint program, GLint location, GLuint* params);
typedef GLint          (GL_APIENTRYP PFNGLGETFRAGDATALOCATIONPROC) (GLuint program, const GLchar *name);
typedef void           (GL_APIENTRYP PFNGLUNIFORM1UIPROC) (GLint location, GLuint v0);
typedef void           (GL_APIENTRYP PFNGLUNIFORM2UIPROC) (GLint location, GLuint v0, GLuint v1);
typedef void           (GL_APIENTRYP PFNGLUNIFORM3UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void           (GL_APIENTRYP PFNGLUNIFORM4UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void           (GL_APIENTRYP PFNGLUNIFORM1UIVPROC) (GLint location, GLsizei count, const GLuint* value);
typedef void           (GL_APIENTRYP PFNGLUNIFORM2UIVPROC) (GLint location, GLsizei count, const GLuint* value);
typedef void           (GL_APIENTRYP PFNGLUNIFORM3UIVPROC) (GLint location, GLsizei count, const GLuint* value);
typedef void           (GL_APIENTRYP PFNGLUNIFORM4UIVPROC) (GLint location, GLsizei count, const GLuint* value);
typedef void           (GL_APIENTRYP PFNGLCLEARBUFFERIVPROC) (GLenum buffer, GLint drawbuffer, const GLint* value);
typedef void           (GL_APIENTRYP PFNGLCLEARBUFFERUIVPROC) (GLenum buffer, GLint drawbuffer, const GLuint* value);
typedef void           (GL_APIENTRYP PFNGLCLEARBUFFERFVPROC) (GLenum buffer, GLint drawbuffer, const GLfloat* value);
typedef void           (GL_APIENTRYP PFNGLCLEARBUFFERFIPROC) (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
typedef const GLubyte* (GL_APIENTRYP PFNGLGETSTRINGIPROC) (GLenum name, GLuint index);
typedef void           (GL_APIENTRYP PFNGLCOPYBUFFERSUBDATAPROC) (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
typedef void           (GL_APIENTRYP PFNGLGETUNIFORMINDICESPROC) (GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices);
typedef void           (GL_APIENTRYP PFNGLGETACTIVEUNIFORMSIVPROC) (GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);
typedef GLuint         (GL_APIENTRYP PFNGLGETUNIFORMBLOCKINDEXPROC) (GLuint program, const GLchar* uniformBlockName);
typedef void           (GL_APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKIVPROC) (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
typedef void           (GL_APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName);
typedef void           (GL_APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC) (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
typedef void           (GL_APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC) (GLenum mode, GLint first, GLsizei count, GLsizei instanceCount);
typedef void           (GL_APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC) (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instanceCount);
typedef GLsync         (GL_APIENTRYP PFNGLFENCESYNCPROC) (GLenum condition, GLbitfield flags);
typedef GLboolean      (GL_APIENTRYP PFNGLISSYNCPROC) (GLsync sync);
typedef void           (GL_APIENTRYP PFNGLDELETESYNCPROC) (GLsync sync);
typedef GLenum         (GL_APIENTRYP PFNGLCLIENTWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void           (GL_APIENTRYP PFNGLWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void           (GL_APIENTRYP PFNGLGETINTEGER64VPROC) (GLenum pname, GLint64* params);
typedef void           (GL_APIENTRYP PFNGLGETSYNCIVPROC) (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values);
typedef void           (GL_APIENTRYP PFNGLGETINTEGER64I_VPROC) (GLenum target, GLuint index, GLint64* data);
typedef void           (GL_APIENTRYP PFNGLGETBUFFERPARAMETERI64VPROC) (GLenum target, GLenum pname, GLint64* params);
typedef void           (GL_APIENTRYP PFNGLGENSAMPLERSPROC) (GLsizei count, GLuint* samplers);
typedef void           (GL_APIENTRYP PFNGLDELETESAMPLERSPROC) (GLsizei count, const GLuint* samplers);
typedef GLboolean      (GL_APIENTRYP PFNGLISSAMPLERPROC) (GLuint sampler);
typedef void           (GL_APIENTRYP PFNGLBINDSAMPLERPROC) (GLuint unit, GLuint sampler);
typedef void           (GL_APIENTRYP PFNGLSAMPLERPARAMETERIPROC) (GLuint sampler, GLenum pname, GLint param);
typedef void           (GL_APIENTRYP PFNGLSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, const GLint* param);
typedef void           (GL_APIENTRYP PFNGLSAMPLERPARAMETERFPROC) (GLuint sampler, GLenum pname, GLfloat param);
typedef void           (GL_APIENTRYP PFNGLSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, const GLfloat* param);
typedef void           (GL_APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, GLint* params);
typedef void           (GL_APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, GLfloat* params);
typedef void           (GL_APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC) (GLuint index, GLuint divisor);
typedef void           (GL_APIENTRYP PFNGLBINDTRANSFORMFEEDBACKPROC) (GLenum target, GLuint id);
typedef void           (GL_APIENTRYP PFNGLDELETETRANSFORMFEEDBACKSPROC) (GLsizei n, const GLuint* ids);
typedef void           (GL_APIENTRYP PFNGLGENTRANSFORMFEEDBACKSPROC) (GLsizei n, GLuint* ids);
typedef GLboolean      (GL_APIENTRYP PFNGLISTRANSFORMFEEDBACKPROC) (GLuint id);
typedef void           (GL_APIENTRYP PFNGLPAUSETRANSFORMFEEDBACKPROC) (void);
typedef void           (GL_APIENTRYP PFNGLRESUMETRANSFORMFEEDBACKPROC) (void);
typedef void           (GL_APIENTRYP PFNGLGETPROGRAMBINARYPROC) (GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary);
typedef void           (GL_APIENTRYP PFNGLPROGRAMBINARYPROC) (GLuint program, GLenum binaryFormat, const GLvoid* binary, GLsizei length);
typedef void           (GL_APIENTRYP PFNGLPROGRAMPARAMETERIPROC) (GLuint program, GLenum pname, GLint value);
typedef void           (GL_APIENTRYP PFNGLINVALIDATEFRAMEBUFFERPROC) (GLenum target, GLsizei numAttachments, const GLenum* attachments);
typedef void           (GL_APIENTRYP PFNGLINVALIDATESUBFRAMEBUFFERPROC) (GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void           (GL_APIENTRYP PFNGLTEXSTORAGE2DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void           (GL_APIENTRYP PFNGLTEXSTORAGE3DPROC) (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef void           (GL_APIENTRYP PFNGLGETINTERNALFORMATIVPROC) (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params);

extern PFNGLREADBUFFERPROC                      glReadBuffer;
extern PFNGLDRAWRANGEELEMENTSPROC               glDrawRangeElements;
extern PFNGLTEXIMAGE3DPROC                      glTexImage3D;
extern PFNGLTEXSUBIMAGE3DPROC                   glTexSubImage3D;
extern PFNGLCOPYTEXSUBIMAGE3DPROC               glCopyTexSubImage3D;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC            glCompressedTexImage3D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC         glCompressedTexSubImage3D;
extern PFNGLGENQUERIESPROC                      glGenQueries;
extern PFNGLDELETEQUERIESPROC                   glDeleteQueries;
extern PFNGLISQUERYPROC                         glIsQuery;
extern PFNGLBEGINQUERYPROC                      glBeginQuery;
extern PFNGLENDQUERYPROC                        glEndQuery;
extern PFNGLGETQUERYIVPROC                      glGetQueryiv;
extern PFNGLGETQUERYOBJECTUIVPROC               glGetQueryObjectuiv;
extern PFNGLUNMAPBUFFERPROC                     glUnmapBuffer;
extern PFNGLGETBUFFERPOINTERVPROC               glGetBufferPointerv;
extern PFNGLDRAWBUFFERSPROC                     glDrawBuffers;
extern PFNGLUNIFORMMATRIX2X3FVPROC              glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC              glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC              glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC              glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC              glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC              glUniformMatrix4x3fv;
extern PFNGLBLITFRAMEBUFFERPROC                 glBlitFramebuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC  glRenderbufferStorageMultisample;
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC         glFramebufferTextureLayer;
extern PFNGLMAPBUFFERRANGEPROC                  glMapBufferRange;
extern PFNGLFLUSHMAPPEDBUFFERRANGEPROC          glFlushMappedBufferRange;
extern PFNGLBINDVERTEXARRAYPROC                 glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC              glDeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC                 glGenVertexArrays;
extern PFNGLISVERTEXARRAYPROC                   glIsVertexArray;
extern PFNGLGETINTEGERI_VPROC                   glGetIntegeri_v;
extern PFNGLBEGINTRANSFORMFEEDBACKPROC          glBeginTransformFeedback;
extern PFNGLENDTRANSFORMFEEDBACKPROC            glEndTransformFeedback;
extern PFNGLBINDBUFFERRANGEPROC                 glBindBufferRange;
extern PFNGLBINDBUFFERBASEPROC                  glBindBufferBase;
extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC       glTransformFeedbackVaryings;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGPROC     glGetTransformFeedbackVarying;
extern PFNGLVERTEXATTRIBIPOINTERPROC            glVertexAttribIPointer;
extern PFNGLGETVERTEXATTRIBIIVPROC              glGetVertexAttribIiv;
extern PFNGLGETVERTEXATTRIBIUIVPROC             glGetVertexAttribIuiv;
extern PFNGLVERTEXATTRIBI4IPROC                 glVertexAttribI4i;
extern PFNGLVERTEXATTRIBI4UIPROC                glVertexAttribI4ui;
extern PFNGLVERTEXATTRIBI4IVPROC                glVertexAttribI4iv;
extern PFNGLVERTEXATTRIBI4UIVPROC               glVertexAttribI4uiv;
extern PFNGLGETUNIFORMUIVPROC                   glGetUniformuiv;
extern PFNGLGETFRAGDATALOCATIONPROC             glGetFragDataLocation;
extern PFNGLUNIFORM1UIPROC                      glUniform1ui;
extern PFNGLUNIFORM2UIPROC                      glUniform2ui;
extern PFNGLUNIFORM3UIPROC                      glUniform3ui;
extern PFNGLUNIFORM4UIPROC                      glUniform4ui;
extern PFNGLUNIFORM1UIVPROC                     glUniform1uiv;
extern PFNGLUNIFORM2UIVPROC                     glUniform2uiv;
extern PFNGLUNIFORM3UIVPROC                     glUniform3uiv;
extern PFNGLUNIFORM4UIVPROC                     glUniform4uiv;
extern PFNGLCLEARBUFFERIVPROC                   glClearBufferiv;
extern PFNGLCLEARBUFFERUIVPROC                  glClearBufferuiv;
extern PFNGLCLEARBUFFERFVPROC                   glClearBufferfv;
extern PFNGLCLEARBUFFERFIPROC                   glClearBufferfi;
extern PFNGLGETSTRINGIPROC                      glGetStringi;
extern PFNGLCOPYBUFFERSUBDATAPROC               glCopyBufferSubData;
extern PFNGLGETUNIFORMINDICESPROC               glGetUniformIndices;
extern PFNGLGETACTIVEUNIFORMSIVPROC             glGetActiveUniformsiv;
extern PFNGLGETUNIFORMBLOCKINDEXPROC            glGetUniformBlockIndex;
extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC         glGetActiveUniformBlockiv;
extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC       glGetActiveUniformBlockName;
extern PFNGLUNIFORMBLOCKBINDINGPROC             glUniformBlockBinding;
extern PFNGLDRAWARRAYSINSTANCEDPROC             glDrawArraysInstanced;
extern PFNGLDRAWELEMENTSINSTANCEDPROC           glDrawElementsInstanced;
extern PFNGLFENCESYNCPROC                       glFenceSync;
extern PFNGLISSYNCPROC                          glIsSync;
extern PFNGLDELETESYNCPROC                      glDeleteSync;
extern PFNGLCLIENTWAITSYNCPROC                  glClientWaitSync;
extern PFNGLWAITSYNCPROC                        glWaitSync;
extern PFNGLGETINTEGER64VPROC                   glGetInteger64v;
extern PFNGLGETSYNCIVPROC                       glGetSynciv;
extern PFNGLGETINTEGER64I_VPROC                 glGetInteger64i_v;
extern PFNGLGETBUFFERPARAMETERI64VPROC          glGetBufferParameteri64v;
extern PFNGLGENSAMPLERSPROC                     glGenSamplers;
extern PFNGLDELETESAMPLERSPROC                  glDeleteSamplers;
extern PFNGLISSAMPLERPROC                       glIsSampler;
extern PFNGLBINDSAMPLERPROC                     glBindSampler;
extern PFNGLSAMPLERPARAMETERIPROC               glSamplerParameteri;
extern PFNGLSAMPLERPARAMETERIVPROC              glSamplerParameteriv;
extern PFNGLSAMPLERPARAMETERFPROC               glSamplerParameterf;
extern PFNGLSAMPLERPARAMETERFVPROC              glSamplerParameterfv;
extern PFNGLGETSAMPLERPARAMETERIVPROC           glGetSamplerParameteriv;
extern PFNGLGETSAMPLERPARAMETERFVPROC           glGetSamplerParameterfv;
extern PFNGLVERTEXATTRIBDIVISORPROC             glVertexAttribDivisor;
extern PFNGLBINDTRANSFORMFEEDBACKPROC           glBindTransformFeedback;
extern PFNGLDELETETRANSFORMFEEDBACKSPROC        glDeleteTransformFeedbacks;
extern PFNGLGENTRANSFORMFEEDBACKSPROC           glGenTransformFeedbacks;
extern PFNGLISTRANSFORMFEEDBACKPROC             glIsTransformFeedback;
extern PFNGLPAUSETRANSFORMFEEDBACKPROC          glPauseTransformFeedback;
extern PFNGLRESUMETRANSFORMFEEDBACKPROC         glResumeTransformFeedback;
extern PFNGLGETPROGRAMBINARYPROC                glGetProgramBinary;
extern PFNGLPROGRAMBINARYPROC                   glProgramBinary;
extern PFNGLPROGRAMPARAMETERIPROC               glProgramParameteri;
extern PFNGLINVALIDATEFRAMEBUFFERPROC           glInvalidateFramebuffer;
extern PFNGLINVALIDATESUBFRAMEBUFFERPROC        glInvalidateSubFramebuffer;
extern PFNGLTEXSTORAGE2DPROC                    glTexStorage2D;
extern PFNGLTEXSTORAGE3DPROC                    glTexStorage3D;
extern PFNGLGETINTERNALFORMATIVPROC             glGetInternalformativ;


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
extern PFNGLFRAMEBUFFERTEXTURE3DOES						glFramebufferTexture3DOES;
extern PFNGLBINDVERTEXARRAYOESPROC						glBindVertexArrayOES;
extern PFNGLDELETEVERTEXARRAYSOESPROC					glDeleteVertexArraysOES;
extern PFNGLGENVERTEXARRAYSOESPROC						glGenVertexArraysOES;
extern PFNGLISVERTEXARRAYOESPROC						glIsVertexArrayOES;
extern PFNGLDEBUGMESSAGECONTROLPROC						glDebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTPROC						glDebugMessageInsert;
extern PFNGLDEBUGMESSAGECALLBACKPROC					glDebugMessageCallback;
extern PFNGLGETDEBUGMESSAGELOGPROC						glGetDebugMessageLog;
extern PFNGLPUSHDEBUGGROUPPROC							glPushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC							glPopDebugGroup;
extern PFNGLOBJECTLABELPROC								glObjectLabel;
extern PFNGLGETOBJECTLABELPROC							glGetObjectLabel;
extern PFNGLOBJECTPTRLABELPROC							glObjectPtrLabel;
extern PFNGLGETOBJECTPTRLABELPROC						glGetObjectPtrLabel;
extern PFNGLGETPOINTERVPROC								glGetPointerv;
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
extern PFLGLDRAWARRAYSINSTANCEDANGLEPROC				glDrawArraysInstancedANGLE;
extern PFLGLDRAWELEMENTSINSTANCEDANGLEPROC				glDrawElementsInstancedANGLE;
extern PFLGLVERTEXATTRIBDIVISORANGLEPROC				glVertexAttribDivisorANGLE;
extern PFLGLGETTRANSLATEDSHADERSOURCEANGLEPROC			glGetTranslatedShaderSourceANGLE;
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
extern PFNDRAWARRAYSINSTANCEDNVPROC						glDrawArraysInstancedNV;
extern PFNDRAWELEMENTSINSTANCEDNVPROC					glDrawElementsInstancedNV;
extern PFNGLDELETEFENCESNVPROC							glDeleteFencesNV;
extern PFNGLGENFENCESNVPROC								glGenFencesNV;
extern PFNGLISFENCENVPROC								glIsFenceNV;
extern PFNGLTESTFENCENVPROC								glTestFenceNV;
extern PFNGLGETFENCEIVNVPROC							glGetFenceivNV;
extern PFNGLFINISHFENCENVPROC							glFinishFenceNV;
extern PFNGLSETFENCENVPROC								glSetFenceNV;
extern PFNBLITFRAMEBUFFERNVPROC							glBlitFramebufferNV;
extern PFNRENDERBUFFERSTORAGEMULTISAMPLENVPROC			glRenderbufferStorageMultisampleNV;
extern PFNVERTEXATTRIBDIVISORNVPROC						glVertexAttribDivisorNV;
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

/* CGLESExtLib class */

typedef void (*fpnGenericFunctionPointer)(void);

class CGLESExtLib
{
public:

    CGLESExtLib();
    ~CGLESExtLib();

public:

    int Create();

protected:

    fpnGenericFunctionPointer GetExtension(const char* pszExtensionName);
	void OutputLog(const char* pszMessage, ...);
};


#endif // __GLES_EXT_LIB_H__
