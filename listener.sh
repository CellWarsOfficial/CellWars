#!/bin/bash

socat -t 100 TCP4-LISTEN:$1,reuseaddr,fork EXEC:./run_up.sh
