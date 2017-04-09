#!/bin/bash

current_dir=`pwd`

echo "Making func_sim..."
rm func_sim.output -f;
pushd ../func_sim >> /dev/null;  
make func_sim;

echo "Running func_sim..."
./func_sim $1 $2 | tee $current_dir/func_sim.output;
popd >> /dev/null;

echo "Making perf_sim..."
rm perf_sim.output -f;
pushd ../perf_sim >> /dev/null;  
make perf_sim;

echo "Running perf_sim..."
./perf_sim -b $1 -n $2 | tee $current_dir/perf_sim.output;
popd >> /dev/null;

funcsim_md5=$(md5sum func_sim.output | cut -d ' ' -f 1)
perfsim_md5=$(md5sum perf_sim.output | cut -d ' ' -f 1)

if [ "$funcsim_md5" = "$perfsim_md5" ];
then 
    echo "********** Tests passed! **************"
else
    echo "********** Tests not passed! **************"
    vim -d func_sim.output perf_sim.output
    exit
fi


