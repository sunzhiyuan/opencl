ADRENO_STL_LIB := libstlport
ADRENO_STL_INCLUDE := external/stlport/$(ADRENO_STL_LIB).mk

# By default build both CPU and GPU, set these to 0 on command line to build only GPU or CPU
BUILD_CPU ?= 0
BUILD_GPU ?= 1

commonCflags   := -c -g -O0  -W -x c++ \
	-DANDROID \
	-DQUALCOMM_NO_EXCEPTIONS \
	-DSTDC_LIMIT_MACROS \
	-DPLATFORM_POSIX \
	-DSTDC_CONSTANT_MACROS

commonIncludes := $(QC_PROP_ROOT)/gles/adreno200/include/public \
        bionic/ \
        external/stlport/stlport


commonSharedLibs-arm := \
	libcutils \
	libdl \
	libOpenCL \
	$(ADRENO_STL_LIB)

commonSharedLibs-a3xx := \
	libcutils \
	libdl \
	libOpenCL \
	$(ADRENO_STL_LIB)
