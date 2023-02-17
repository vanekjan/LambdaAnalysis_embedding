#!/bin/bash

starver DEV

rm ./log/local.log

root4star -b -q 'starsim.C(50,510,1234,"pp:minbiasLambdaLambdaBar")' > local.log
