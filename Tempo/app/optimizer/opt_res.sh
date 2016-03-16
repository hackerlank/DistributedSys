#!/bin/bash

bs=$(sed -n 's/.*batch_size[ \t]*=[ \t]*\([^;]*\).*/\1/p' conf/opt.conf)
traj=$(sed -n 's/.*trajectory[ \t]*=[ \t"]*\([^";]*\).*/\1/p' conf/opt.conf)

if [ ! -f "$traj" ]; then
    echo "Can't find trajectory file: $traj";
    echo "You will need to run the optimizer first!";
    exit -1;
fi

awk -v bs=$bs '{ if (FNR % (2*bs+1) == 1) print $0 }' $traj | less
