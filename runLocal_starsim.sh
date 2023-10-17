#!/bin/bash

starver SL21d

rm ./log/local.log

root4star -b -q 'starsim.C(50,200,1234,"pp:minbiasLambdaLambdaBar")' > local.log
