#!/bin/bash

echo -e "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n"
status=0
make uninstall || status=1
git pull || status=1
make install || status=1
printf "$status\r\n"
exit $status
