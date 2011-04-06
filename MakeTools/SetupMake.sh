#!/bin/bash

LOADED_SOURCE_DIR="$PWD/.."
LOADED_BUILD_DIR="$PWD/../Build"

echo -----------------------------------------------------------------------
echo LOADED_SOURCE_DIR = ${LOADED_SOURCE_DIR}
echo LOADED_BUILD_DIR  = ${LOADED_BUILD_DIR}
echo -----------------------------------------------------------------------

CXX_FLAGS="-Wall -Wno-sign-compare -pthread -msse -D__SSE__"
CXX_FLAGS_DEBUG="-g -D_DEBUG"
CXX_FLAGS_RELEASE="-O3 -Winline --param large-function-growth=400"

DEBUG_COMPILER_DIR="GCC_Debug"
RELEASE_COMPILER_DIR="GCC_Release"
GENERATOR="Unix Makefiles"

errorFound=0

if [ "$LOADED_SOURCE_DIR" = "" ]
then
  echo ERROR : the LOADED_SOURCE_DIR env variable is not set
  errorFound=1
fi

if [ "$LOADED_BUILD_DIR" = "" ]
then
  echo ERROR : the LOADED_BUILD_DIR env variable is not set
  errorFound=1
fi

#Les variables d'environement sont présentes
if [ $errorFound = 0 ]
then
  
  if (test ! -e $LOADED_SOURCE_DIR)
  then
    echo ERROR : the $LOADED_SOURCE_DIR dir does not exist
    errorFound=2
  fi

  if (test ! -e $LOADED_BUILD_DIR)
  then
    echo ERROR : the $LOADED_BUILD_DIR dir does not exist
    errorFound=2
  fi
  
  if [ $errorFound = 0 ]
  then
    if (test ! -e $LOADED_BUILD_DIR/$DEBUG_COMPILER_DIR ) 
    then 
      mkdir $LOADED_BUILD_DIR/$DEBUG_COMPILER_DIR 
    fi
    if (test ! -e $LOADED_BUILD_DIR/$DEBUG_COMPILER_DIR/Makefiles ) 
    then 
      mkdir $LOADED_BUILD_DIR/$DEBUG_COMPILER_DIR/Makefiles 
    fi
    if (test ! -e $LOADED_BUILD_DIR/$RELEASE_COMPILER_DIR ) 
    then 
      mkdir $LOADED_BUILD_DIR/$RELEASE_COMPILER_DIR 
    fi
    if (test ! -e $LOADED_BUILD_DIR/$RELEASE_COMPILER_DIR/Makefiles ) 
    then 
      mkdir $LOADED_BUILD_DIR/$RELEASE_COMPILER_DIR/Makefiles 
    fi
    
    comeBack=$PWD
    
    cd $LOADED_BUILD_DIR/$DEBUG_COMPILER_DIR/Makefiles
    cmake -G"$GENERATOR" -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_CXX_FLAGS="$CXX_FLAGS" -DCMAKE_CXX_FLAGS_DEBUG="$CXX_FLAGS_DEBUG" $LOADED_SOURCE_DIR
    
    
    cd $LOADED_BUILD_DIR/$RELEASE_COMPILER_DIR/Makefiles
    cmake -G"$GENERATOR" -DCMAKE_BUILD_TYPE=RELEASE $LOADED_SOURCE_DIR -DCMAKE_CXX_FLAGS="$CXX_FLAGS" -DCMAKE_CXX_FLAGS_DEBUG="$CXX_FLAGS_DEBUG"
    
    cd $comeBack
  fi
fi
