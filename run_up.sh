#!/bin/bash

rm -rf .tmpoutput
touch .tmpoutput
echo -e "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n" >> .tmpoutput
status=0
make uninstall >> .tmpoutput 2>&1 || status=1
git pull >> .tmpoutput 2>&1 || status=1
make install >> .tmpoutput 2>&1 || status=1
printf "$status\n"
cat .tmpoutput
exit $status
