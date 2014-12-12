# CMake file for building a QCLBenchmark application

INCLUDE_DIRECTORIES(../../common/inc)

LINK_DIRECTORIES(../../common/lib)
LINK_DIRECTORIES($ENV{3DIP_ROOT}/build/vs2008/Debug)

#--------- Local flags
ADD_DEFINITIONS(-D_CONSOLE)
  
if(WIN32)
  ADD_DEFINITIONS(-WX)
endif(WIN32)

# Build target
ADD_EXECUTABLE(WaveletTransform
  transformMain.cpp
  clfunctions.cpp 
  refer_transform.cpp
  common.h
  cltransform.h
  dwt_forward_x.cl
  dwt_forward_y.cl
  dwt_inverse_x.cl
  dwt_inverse_y.cl 
  haar_forward.cl
  haar_inverse.cl 
)

TARGET_LINK_LIBRARIES(WaveletTransform OpenCL)

SET_TARGET_PROPERTIES(WaveletTransform
                     PROPERTIES FOLDER "sample")