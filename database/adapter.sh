#!/bin/bash

function gen_page() {
  cp index.html .tmp_to_send.html
  array=(`echo "COPY (SELECT * FROM agents.grid) TO STDOUT;" | psql`)
  for (( i=0; i<=$(( ${#array[@]} -1 )); i+=3 ))
  do
    pos=#$(( ${array[$i]} * 60 + ${array[$((i + 1))]} ))
    typ=$(( array[$((i + 2))] % 3 ))
    color=greencell
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
}

function db_adapter() {

while(true)
do
  gen_page
  cat .tmp_to_send.html | sed -e '1i HTTP/1.1 200 OK\n' | nc -l -p 7777
done
}

. ~/.pgdb-g1627123_u-g1627123_u
db_adapter
