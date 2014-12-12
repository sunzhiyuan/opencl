//=============================================================================
// FILE: hello_cl_android_native.cpp
//
// Android native specific implementation
//
// OS specific implementation need to perform the following tasks:
// 1) Take care of the application entry point
// 2) Create the application container
// 3) Handle system message (keypresses, program interuption)
// 4) Run the application message loop
//
// AUTHOR:          QUALCOMM
//
//                  Copyright (c) 2012 QUALCOMM Incorporated.
//                              All Rights Reserved.
//                    QUALCOMM Proprietary and Confidential.
//=============================================================================

#ifndef OS_ANDROID_NATIVE
#error "This file is only for Android native build"
#endif //OS_ANDROID_NATIVE

//=============================================================================
// INCLUDES
//=============================================================================

#include <jni.h>
#include <errno.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <android/log.h>
#include <android/window.h>
#include <android_native_app_glue.h>
#include <stdlib.h>
#include <stdio.h>
#include "hello_cl.h"
#include <android/log.h>

//=============================================================================
// defines
//=============================================================================
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "native-activity", __VA_ARGS__))




#define APP_NAME "hello_cl"
#define VERSION_MAJOR "1"
#define VERSION_MINOR "0"

//=============================================================================
// GLOBALS
//=============================================================================

//=============================================================================
// IMPLEMENTATION
//=============================================================================

//-----------------------------------------------------------------------------
void FatalError(const char *pszFormat, ...)
//-----------------------------------------------------------------------------
{
    va_list args;
    char szBuffer[256];

    va_start(args, pszFormat);
    vsnprintf(szBuffer, 256, pszFormat, args);
    va_end(args);

    exit(0);
}


//=============================================================================

/**
 * Shared state for our pAndroidApp.
 */
struct TEngine
{
    struct android_app *pAndroidApp;
};

//-----------------------------------------------------------------------------
void Terminate(struct android_app *pAndroidApp)
//-----------------------------------------------------------------------------
{
}

// Process the next input event.
//-----------------------------------------------------------------------------
static int32_t engine_handle_input(struct android_app* pAndroidApp, AInputEvent* event)
//-----------------------------------------------------------------------------
{
    return 0;
}

// Process the next main command.
//-----------------------------------------------------------------------------
static void engine_handle_cmd(struct android_app *pAndroidApp, int32_t cmd)
//-----------------------------------------------------------------------------
{
    struct TEngine *pEngine = (struct TEngine *)pAndroidApp->userData;

    switch (cmd)
    {
    default:
        LOGD("APP_CMD_0x%X (%d)", cmd, cmd); //note: at this point Log class might not have been initialized yet
        break;

    case APP_CMD_GAINED_FOCUS:
        LOGD("APP_CMD_GAINED_FOCUS");
        // App gains focus
        hello_cl();
        break;
    case APP_CMD_LOST_FOCUS:
        LOGD("APP_CMD_LOST_FOCUS");
        // App loses focus. Stop rendering.
        // This is to avoid consuming battery while not being used.
        break;
    }
}

// This is the main entry point of a native application that is using
// android_native_app_glue.  It runs in its own thread, with its own
// event loop for receiving input events and doing other things.
//-----------------------------------------------------------------------------
void android_main(struct android_app *state)
//-----------------------------------------------------------------------------
{
    struct TEngine engine;

    app_dummy();

    memset(&engine, 0, sizeof(engine));

    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.pAndroidApp = state;

    LOGD("Starting hello cl");
    while (1)
    {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source *source=NULL;


        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(-1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL)
            {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0)
            {
                return;
            }
        }
    }
}
