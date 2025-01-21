#!/bin/bash
#submitdir=`/bin/pwd -P`
submitdir=$PWD
echo $submitdir

prodId=`date +%F_%H-%M`

#low number of events for testing
#nevents=100
nevents=1000

energy=200 #collisions energy
#energy=510 #collisions energy

random=$RANDOM

config="pp:minbiasLambda"
#config="pp:minbiasDiLambda"
#config="pp:minbiasDiLambdaBar"
#config="pp:minbiasLambdaLambdaBar"
#config="pp:minbiasProtonPionPair"
#config="pp:minbias"

#for output file name in xml file
tune="minbiasLambda"
#tune="minbiasDiLambda"
#tune="minbiasDiLambdaBar"
#tune="minbiasLambdaLambdaBar"
#tune="minbiasProtonPionPair"
#tune="minbias"

mkdir -p ./SubmitInfo/


echo "output directories"
echo ./production/starsim/${prodId}
echo ./jobs/starsim/${prodId}

mkdir -p ./production/starsim/${prodId}
mkdir -p ./jobs/starsim/${prodId}

star-submit-template -template submit_starsim.xml -entities submitdir=$submitdir,nevents=$nevents,random=$random,productionId=${prodId},config=$config,tune=$tune,energy=$energy

