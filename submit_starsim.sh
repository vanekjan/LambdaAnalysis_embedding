#!/bin/bash
#submitdir=`/bin/pwd -P`
submitdir=$PWD
echo $submitdir

prodId=`date +%F_%H-%M`

#low number of events for testing
nevents=100

energy=510 #collisions energy

random=$RANDOM

config="pp:minbiasLambda"
#config="pp:dmeson"
#config="pp:dmesontune"
#config="pp:dmesontune_new"

#for output file name in xml file
tune="minbiasLambda"
#tune="dmeson"
#tune="dmesontune"
#tune="dmesontune_new"

mkdir -p ./SubmitInfo/


echo "output directories"
echo ./production/starsim/${prodId}
echo ./jobs/starsim/${prodId}

mkdir -p ./production/starsim/${prodId}
mkdir -p ./jobs/starsim/${prodId}

star-submit-template -template submit_starsim.xml -entities submitdir=$submitdir,nevents=$nevents,random=$random,productionId=${prodId},config=$config,tune=$tune,energy=$energy

