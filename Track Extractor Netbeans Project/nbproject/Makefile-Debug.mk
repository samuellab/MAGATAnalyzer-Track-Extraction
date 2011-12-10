#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/763899645/MidlineMaggotTrackPoint.o \
	${OBJECTDIR}/_ext/763899645/CvUtilsPlusPlus.o \
	${OBJECTDIR}/_ext/763899645/PointExtractor.o \
	${OBJECTDIR}/_ext/763899645/AutoThresholder.o \
	${OBJECTDIR}/_ext/763899645/StackLoader.o \
	${OBJECTDIR}/_ext/763899645/TrackExtractor.o \
	${OBJECTDIR}/_ext/763899645/trackbuilder.o \
	${OBJECTDIR}/_ext/763899645/MaggotTrackPoint.o \
	${OBJECTDIR}/_ext/1610408862/tictoc.o \
	${OBJECTDIR}/_ext/763899645/newDLLWrapper.o \
	${OBJECTDIR}/_ext/763899645/MaggotReprocessor.o \
	${OBJECTDIR}/_ext/763899645/BatchExtractor.o \
	${OBJECTDIR}/_ext/763899645/FrameLoader.o \
	${OBJECTDIR}/_ext/1610408862/Timer.o \
	${OBJECTDIR}/_ext/763899645/WormPointExtractor.o \
	${OBJECTDIR}/_ext/763899645/MaggotTrackBuilder.o \
	${OBJECTDIR}/_ext/763899645/MaggotTrackExtractor.o \
	${OBJECTDIR}/_ext/763899645/newmain.o \
	${OBJECTDIR}/_ext/763899645/communicator.o \
	${OBJECTDIR}/_ext/763899645/ImLoader.o \
	${OBJECTDIR}/_ext/763899645/ImTrackPoint.o \
	${OBJECTDIR}/_ext/763899645/ContourPoint.o \
	${OBJECTDIR}/_ext/763899645/trackpoint.o \
	${OBJECTDIR}/_ext/763899645/track.o \
	${OBJECTDIR}/_ext/763899645/trackmatch.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-DBUILD_DLL -static-libgcc -static-libstdc++
CXXFLAGS=-DBUILD_DLL -static-libgcc -static-libstdc++

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../Necessary\ Libraries\ and\ Includes/CV/lib -L../yaml-cpp ../Image-Stack-Compressor/image_stack_compressor.lib -lcv -lcxcore -lhighgui ../yaml-cpp/libyaml-cpp.lib

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk ../libRevisedTrackExtractor.dll

../libRevisedTrackExtractor.dll: ../Image-Stack-Compressor/image_stack_compressor.lib

../libRevisedTrackExtractor.dll: ../yaml-cpp/libyaml-cpp.lib

../libRevisedTrackExtractor.dll: ${OBJECTFILES}
	${MKDIR} -p ..
	${LINK.cc} -shared -o ../libRevisedTrackExtractor.dll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/763899645/MidlineMaggotTrackPoint.o: ../source\ code/MidlineMaggotTrackPoint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/MidlineMaggotTrackPoint.o ../source\ code/MidlineMaggotTrackPoint.cpp

${OBJECTDIR}/_ext/763899645/CvUtilsPlusPlus.o: ../source\ code/CvUtilsPlusPlus.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/CvUtilsPlusPlus.o ../source\ code/CvUtilsPlusPlus.cpp

${OBJECTDIR}/_ext/763899645/PointExtractor.o: ../source\ code/PointExtractor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/PointExtractor.o ../source\ code/PointExtractor.cpp

${OBJECTDIR}/_ext/763899645/AutoThresholder.o: ../source\ code/AutoThresholder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/AutoThresholder.o ../source\ code/AutoThresholder.cpp

${OBJECTDIR}/_ext/763899645/StackLoader.o: ../source\ code/StackLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/StackLoader.o ../source\ code/StackLoader.cpp

${OBJECTDIR}/_ext/763899645/TrackExtractor.o: ../source\ code/TrackExtractor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/TrackExtractor.o ../source\ code/TrackExtractor.cpp

${OBJECTDIR}/_ext/763899645/trackbuilder.o: ../source\ code/trackbuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/trackbuilder.o ../source\ code/trackbuilder.cpp

${OBJECTDIR}/_ext/763899645/MaggotTrackPoint.o: ../source\ code/MaggotTrackPoint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/MaggotTrackPoint.o ../source\ code/MaggotTrackPoint.cpp

${OBJECTDIR}/_ext/1610408862/tictoc.o: ../Image-Stack-Compressor/tictoc/tictoc.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1610408862
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1610408862/tictoc.o ../Image-Stack-Compressor/tictoc/tictoc.cpp

${OBJECTDIR}/_ext/763899645/newDLLWrapper.o: ../source\ code/newDLLWrapper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/newDLLWrapper.o ../source\ code/newDLLWrapper.cpp

${OBJECTDIR}/_ext/763899645/MaggotReprocessor.o: ../source\ code/MaggotReprocessor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/MaggotReprocessor.o ../source\ code/MaggotReprocessor.cpp

${OBJECTDIR}/_ext/763899645/BatchExtractor.o: ../source\ code/BatchExtractor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/BatchExtractor.o ../source\ code/BatchExtractor.cpp

${OBJECTDIR}/_ext/763899645/FrameLoader.o: ../source\ code/FrameLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/FrameLoader.o ../source\ code/FrameLoader.cpp

${OBJECTDIR}/_ext/1610408862/Timer.o: ../Image-Stack-Compressor/tictoc/Timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1610408862
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1610408862/Timer.o ../Image-Stack-Compressor/tictoc/Timer.cpp

${OBJECTDIR}/_ext/763899645/WormPointExtractor.o: ../source\ code/WormPointExtractor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/WormPointExtractor.o ../source\ code/WormPointExtractor.cpp

${OBJECTDIR}/_ext/763899645/MaggotTrackBuilder.o: ../source\ code/MaggotTrackBuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/MaggotTrackBuilder.o ../source\ code/MaggotTrackBuilder.cpp

${OBJECTDIR}/_ext/763899645/MaggotTrackExtractor.o: ../source\ code/MaggotTrackExtractor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/MaggotTrackExtractor.o ../source\ code/MaggotTrackExtractor.cpp

${OBJECTDIR}/_ext/763899645/newmain.o: ../source\ code/newmain.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/newmain.o ../source\ code/newmain.cpp

${OBJECTDIR}/_ext/763899645/communicator.o: ../source\ code/communicator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/communicator.o ../source\ code/communicator.cpp

${OBJECTDIR}/_ext/763899645/ImLoader.o: ../source\ code/ImLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/ImLoader.o ../source\ code/ImLoader.cpp

${OBJECTDIR}/_ext/763899645/ImTrackPoint.o: ../source\ code/ImTrackPoint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/ImTrackPoint.o ../source\ code/ImTrackPoint.cpp

${OBJECTDIR}/_ext/763899645/ContourPoint.o: ../source\ code/ContourPoint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/ContourPoint.o ../source\ code/ContourPoint.cpp

${OBJECTDIR}/_ext/763899645/trackpoint.o: ../source\ code/trackpoint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/trackpoint.o ../source\ code/trackpoint.cpp

${OBJECTDIR}/_ext/763899645/track.o: ../source\ code/track.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/track.o ../source\ code/track.cpp

${OBJECTDIR}/_ext/763899645/trackmatch.o: ../source\ code/trackmatch.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/763899645
	${RM} $@.d
	$(COMPILE.cc) -g -I. -I../source\ code -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../Image-Stack-Compressor -I../yaml-cpp/include -I../yaml-cpp/include/yaml-cpp -I../Image-Stack-Compressor/tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/763899645/trackmatch.o ../source\ code/trackmatch.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ../libRevisedTrackExtractor.dll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
