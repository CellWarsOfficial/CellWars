#!/bin/bash

function db_adapter() {
  while(true) do nc -l -p 7777 <fifo | head -n 1 | sed 's/[^0-9_ ]//g' | awk '{print $1;}' | tr '_' ' ' | awk '{print "COPY (SELECT * FROM agents.grid WHERE x>="$1" AND x<="$3" AND y>="$2" AND y<="$4") TO STDOUT WITH CSV;";}' | psql | sed -e '1i HTTP/1.1 200 OK\n' >fifo; done
}

. ~/.pgdb-g1627123_u-g1627123_u
db_adapter
