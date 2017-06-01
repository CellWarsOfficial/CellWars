#!/bin/bash

function db_sv_adapter() {
  while(true) do nc -l -p 7778 <fifo2 | awk '{print "COPY ("$0") TO STDOUT WITH CSV;";}' | psql >fifo2; done
}

. ~/.pgdb-g1627123_u-g1627123_u
db_sv_adapter
