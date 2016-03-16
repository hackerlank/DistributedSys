#!/bin/bash
echo "Extracting file into `pwd`"
# searches for the line number where finish the script and start the tar.gz
SKIP=`awk '/^__TARFILE_FOLLOWS__/ { print NR + 1; exit 0; }' $0`
# take the tarfile and pipe it into tar
tail -n +$SKIP $0 | tar -xz
# Run run.sh
cd exp_bundle/
sh -x run.sh 2>&1 > ~/run.log
# Any script here will happen after the tar file extract.
echo "Finished"
exit 0
# NOTE: Don't place any newline characters after the last line below.
__TARFILE_FOLLOWS__
