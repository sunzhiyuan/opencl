#!/bin/sh
curDir=${PWD}
cd $1
./$2 $3
cd $curDir
