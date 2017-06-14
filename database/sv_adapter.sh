#!/bin/bash

function db_sv_adapter() {
  socat TCP4-LISTEN:7778,reuseaddr,fork EXEC:"psql -q"
}

. ~/.pgdb-g1627123_u-g1627123_u
db_sv_adapter
