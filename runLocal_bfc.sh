#!/bin/bash

starver DEV

rm ./log/bfc.log

root4star -b -q 'bfcMixer.C(50)' > ./log/bfc.log
