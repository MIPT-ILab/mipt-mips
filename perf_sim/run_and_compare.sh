#!/bin/bash

current_dir=`pwd`

for sim in func_sim perf_sim;
do
    echo "Making $sim..."
    rm $sim_output -f;
    pushd ../$sim >> /dev/null;  
    make $sim;

    echo "Running $sim..."
    ./$sim $1 $2 | tee $current_dir/$sim.output;
    popd >> /dev/null;
done

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


