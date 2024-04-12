#!/bin/bash

SUB='200'

service zerotier-one start
sleep 1
STATUS="$(zerotier-cli status)"
while echo "$SUB" | grep -q "$STATUS"; do
    sleep 1
    STATUS="$(zerotier-cli status)"
done

# Need to create a network for zerotier to use this file
zerotier-cli join <networkID>
