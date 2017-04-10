#!/bin/bash

RED="\033[31m"
GREEN="\033[32m"
DCOLOR="\033[39m"
LINE="--------------------------------------------------------------"

echo "Making MIPS simulator..."
make perf_sim


echo "Removing old logs..."
rm -f func_sim.output perf_sim.output


echo
echo    "Running functional simulation..."
echo -e ${LINE}

./perf_sim $1 $2 | tee func_sim.output;

echo -e ${LINE}


echo
echo    "Running performance simulation..."
echo -e ${LINE}

./perf_sim --performance $1 $2 | tee perf_sim.output;

echo -e ${LINE}


if ! diff -q perf_sim.output func_sim.output >/dev/null; then
  >&2 echo -e   "${RED}********************** Test NOT passed! **********************${DCOLOR}"
else
  >&2 echo -e "${GREEN}************************ Test passed! ************************${DCOLOR}"
fi

