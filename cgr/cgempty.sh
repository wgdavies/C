#!/bin/bash
echo "This one is empty --> $1" >> /home/calin/work/cgr/cg_empty_events.log
rmdir /cgroup/memory/$1
