#!/bin/bash

while(true)
do
  socat TCP4-LISTEN:1631,fork SYSTEM:"./run_up.sh"
done
