#!/bin/bash

function db_sv_adapter() {
  while(true) do nc -l -p 7778 <fifo2 | psql >fifo2; done
}

. ~/.pgdb-g1627123_u-g1627123_u
db_sv_adapter
