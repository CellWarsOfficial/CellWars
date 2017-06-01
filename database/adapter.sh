#!/bin/bash

function db_adapter() {

while(true)
do
  array=(`nc -l 7777 < fifo1 | head -n 1 | sed 's/[^0-9_ ]//g' | awk '{print $1;}' | tr '_' ' ' | awk '{print "COPY (SELECT * FROM agents.grid WHERE x>="$1" AND x<="$3" AND y>="$2" AND y<="$4") TO STDOUT;";}' | psql`)

  cp index.html to_send.html

  for (( i=0; i<=$(( ${#array[@]} -1 )); i+=3 ))
  do
    pos=#$(( ${array[$i]} * 60 + ${array[$((i + 1))]} ))
    typ=$(( array[$((i + 2))] % 3 ))
    color=redcell
    if [[ $typ -eq 1 ]]
    then
      color=bluecell
    fi
    if [[ $typ -eq 2 ]]
    then
      color=redcell
    fi
    eval "sed -i -e 's/\"$pos\"/\"$color\"/g' to_send.html"
  done

  cat to_send | sed -e '1i HTTP/1.1 200 OK\n' > fifo1
  rm to_send.html
done
}

. ~/.pgdb-g1627123_u-g1627123_u
db_adapter
