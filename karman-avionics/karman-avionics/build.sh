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
    make 2>&1 | tee build.out
    echo -e "########################################\n"
    echo "Checking for errors ..."
    if grep -q error build.out ; then
        grep error build.out
    else
        echo "No errors"
    fi
    echo -e "\nChecking for warnings ..."
    if grep -q warning build.out ; then
        grep warning build.out
    else
        echo "No warnings"
    fi
fi

if [[ $VALID -eq 0 ]] ; then
    echo "Invalid option. Candidates are: all clean remake"
fi

echo ""

