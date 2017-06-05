#!/bin/bash

socat TCP4-LISTEN:$1,fork SYSTEM:"./run_up.sh"
