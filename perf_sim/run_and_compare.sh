#!/bin/bash

RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[93m"
DCOLOR="\033[39m"
LINE="--------------------------------------------------------------"


wrapper()
{
    echo
    echo -e "${YELLOW}$1"
    echo -e "${LINE}${DCOLOR}"
    echo "$2"
    echo -e "${YELLOW}${LINE}${DCOLOR}"
}

echo "Making MIPS simulator..."
make perf_sim
echo "Removing old logs..."
rm -f func_sim.output perf_sim.output

wrapper "Running functional simulation..." \
"$(./perf_sim --functional-only $1 $2 | tee func_sim.output)"


wrapper "Running performance simulation..." \
"$(./perf_sim $1 $2 | tee perf_sim.output)"


DIFF=$(diff perf_sim.output func_sim.output)

if [ "${DIFF}" ]
then
  wrapper "\`diff\` output:" "${DIFF}"

  >&2 echo -e   "${RED}\n********************** Test NOT passed! **********************${DCOLOR}"
else
  >&2 echo -e "${GREEN}\n************************ Test passed! ************************${DCOLOR}"
fi
