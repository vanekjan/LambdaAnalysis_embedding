#!/bin/bash

starver SL21d

rm ./log/local_starsim.log

#root4star -b -q 'starsim.C(50,200,1234,"pp:minbiasLambdaLambdaBar")' > ./log/local_starsim.log
root4star -b -q 'starsim.C(50,200,1234,"pp:minbiasLambda")' > ./log/local_starsim.log
#root4star -b -q 'starsim.C(50,200,1234,"pp:minbiasProtonPionPair")' > ./log/local_starsim.log
