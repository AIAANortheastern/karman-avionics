#!/usr/bin/env bash

echo "########################################"
echo "Project Karman Avionics Build ..."
echo "########################################"
if [[ $# -eq 0 ]] ; then
    echo "No arguments supplied, defaulting to 'all'"
    BUILD="all"
else
    BUILD=$1
fi

VALID=0

if [ $BUILD == "clean" ] || [ $BUILD == "remake" ] ; then
    VALID=1
    echo "Cleaning ..."
    make clean
fi

if [ $BUILD == "all" ] || [ $BUILD == "remake" ]; then
    VALID=1
    echo "Building all ..."
    mkdir -p Debug
    touch Debug/build.out
    make 2>&1 | tee Debug/build.out
    echo -e "########################################\n"
    echo "Checking for errors ..."
    if grep -q error: Debug/build.out ; then
        grep error: Debug/build.out
    else
        echo "No errors"
    fi
    echo -e "\nChecking for warnings ..."
    if grep -q warning: Debug/build.out ; then
        grep warning: Debug/build.out
    else
        echo "No warnings"
    fi
fi

if [[ $VALID -eq 0 ]] ; then
    echo "Invalid option. Candidates are: all clean remake"
fi

echo ""

