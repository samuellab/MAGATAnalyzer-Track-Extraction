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
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-static-libgcc -static-libstdc++
CXXFLAGS=-static-libgcc -static-libstdc++

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../Necessary\ Libraries\ and\ Includes/CV/lib -L../../yaml-cpp ../../Track\ Extractor\ Netbeans\ Project/trackExtractor.lib ../../Image-Stack-Compressor/image_stack_compressor.lib -lcv -lcxcore -lhighgui ../../yaml-cpp/libyaml-cpp.lib

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk extract-stack.exe

extract-stack.exe: ../../Track\ Extractor\ Netbeans\ Project/trackExtractor.lib

extract-stack.exe: ../../Image-Stack-Compressor/image_stack_compressor.lib

extract-stack.exe: ../../yaml-cpp/libyaml-cpp.lib

extract-stack.exe: ${OBJECTFILES}
	${LINK.cc} -o extract-stack.exe ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Necessary\ Libraries\ and\ Includes/CV/headers -I../../source\ code -I../../yaml-cpp -I../../yaml-cpp/include -I../../yaml-cpp/include/yaml-cpp -I../../yaml-cpp/src -I../../Track\ Extractor\ Netbeans\ Project -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} extract-stack.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
