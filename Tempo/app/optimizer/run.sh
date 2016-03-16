#!/bin/bash

bs=$(sed -n 's/.*batch_size[ \t]*=[ \t]*\([^;]*\).*/\1/p' conf/opt.conf)
traj=$(sed -n 's/.*trajectory[ \t]*=[ \t"]*\([^";]*\).*/\1/p' conf/opt.conf)

rm -rf $traj;

./opt.app > log/`date +"%m%d-%H%M%S"` 2>info &

while [ ! -f $traj ]; do
    sleep 1;
done

tail -n 100000 -f $traj | awk -v BS=$bs -f res_view.awk
