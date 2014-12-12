//=============================================================================
// FILE: log_dev.h
//
// Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
//=============================================================================
#ifdef BUILD_CONSOLE_APP

#include <stdio.h>
#define LOGI(format, ...) ((void)fprintf(stdout, format "\n", ## __VA_ARGS__))
#define LOGW(format, ...) ((void)fprintf(stdout, format "\n", ## __VA_ARGS__))
#define LOGE(format, ...) ((void)fprintf(stderr, format "\n", ## __VA_ARGS__))

#else
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, app_name, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, app_name, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, app_name, __VA_ARGS__))

#endif
