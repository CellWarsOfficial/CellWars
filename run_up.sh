#!/bin/bash

rm -rf .tmpoutput
status=0
git pull >> .tmpoutput 2>&1 || status=1
make install >> .tmpoutput 2>&1 || status=1
printf "$status\n"
cat .tmpoutput
exit $status
