#!/bin/bash


if [ "$1" = "" ]
then
  echo
  echo "== Variables must be set!"
  echo "== ./create_project [1] [2]"
  echo "== [1]: name of the project"
  echo "== [2]: name of the proejct in the file"
else
  if [ "$2" = "" ]
  then
    echo
    echo "== Variables must be set!"
    echo "== ./create_project [1] [2]"
    echo "== [1]: name of the project"
    echo "== [2]: name of the proejct in the file"
  else
    mkdir ${1}
    cp ${KEBIPATH}/dummy_project/CMakeLists.txt ${1}/CMakeLists.txt
    cp ${KEBIPATH}/dummy_project/gitignore      ${1}/.gitignore


    mkdir ${1}/geant4
    cp ${KEBIPATH}/dummy_project/geant4/DUMMYDetectorConstruction.cc ${1}/geant4/${2}DetectorConstruction.cc
    cp ${KEBIPATH}/dummy_project/geant4/DUMMYDetectorConstruction.hh ${1}/geant4/${2}DetectorConstruction.hh


    mkdir ${1}/detector
    cp ${KEBIPATH}/dummy_project/detector/LinkDefDetector.h     ${1}/detector/LinkDef.h
    cp ${KEBIPATH}/dummy_project/detector/DUMMYDetector.cc      ${1}/detector/${2}Detector.cc
    cp ${KEBIPATH}/dummy_project/detector/DUMMYDetector.hh      ${1}/detector/${2}Detector.hh
    cp ${KEBIPATH}/dummy_project/detector/DUMMYDetectorPlane.cc ${1}/detector/${2}DetectorPlane.cc
    cp ${KEBIPATH}/dummy_project/detector/DUMMYDetectorPlane.hh ${1}/detector/${2}DetectorPlane.hh


    mkdir ${1}/task
    cp ${KEBIPATH}/dummy_project/task/LinkDef.h    ${1}/task/LinkDef.h
    cp ${KEBIPATH}/dummy_project/task/DUMMYTask.cc ${1}/task/${2}DoSomethingTask.cc
    cp ${KEBIPATH}/dummy_project/task/DUMMYTask.hh ${1}/task/${2}DoSomethingTask.hh


    mkdir ${1}/macros
    cp ${KEBIPATH}/dummy_project/macros/eve.C          ${1}/macros/
    cp ${KEBIPATH}/dummy_project/macros/dummy.mc.cc    ${1}/macros/${1}.mc.cc
    cp ${KEBIPATH}/dummy_project/macros/dummy.par      ${1}/macros/${1}.par
    cp ${KEBIPATH}/dummy_project/macros/dummy.gen      ${1}/macros/${1}.gen
    cp ${KEBIPATH}/dummy_project/macros/run_geant4.mac ${1}/macros/
    cp ${KEBIPATH}/dummy_project/macros/vis.mac        ${1}/macros/
    cp ${KEBIPATH}/dummy_project/macros/doSomething.C  ${1}/macros/


    sed -i '' s/DUMMY/${2}/g ${1}/CMakeLists.txt
    sed -i '' s/DUMMY/${2}/g ${1}/geant4/*
    sed -i '' s/DUMMY/${2}/g ${1}/detector/*
    sed -i '' s/DUMMY/${2}/g ${1}/task/*
    sed -i '' s/DUMMY/${2}/g ${1}/macros/*

    sed -i '' s/DUMMYPROJECT/${1}/g ${1}/CMakeLists.txt
    sed -i '' s/DUMMYPROJECT/${1}/g ${1}/geant4/*
    sed -i '' s/DUMMYPROJECT/${1}/g ${1}/detector/*
    sed -i '' s/DUMMYPROJECT/${1}/g ${1}/task/*
    sed -i '' s/DUMMYPROJECT/${1}/g ${1}/macros/*

    sed -i '' s/dummy/${1}/g ${1}/macros/*
  fi
fi
