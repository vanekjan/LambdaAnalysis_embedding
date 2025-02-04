#!/bin/bash
#submitdir=`/bin/pwd -P`
submitdir=$PWD
echo $submitdir

prodId=`date +%F_%H-%M`

#nevents=100
nevents=1000

mkdir -p ./SubmitInfo/

#config="P13ibpp500RFF" #Run12 pp500
config="P12idpp200" #Run12 pp200
#config="P18ibpp510"    #Run17 pp510

#old file from Zilong
#daqfile="/star/u/vanekjan/pwg/vanekjan/ppEmbedding/Lambda_PYTHIA_pp/bfc_input/st_zerobias_adc_13078014_raw_1600001.daq"
daqfile="/star/u/vanekjan/pwg/vanekjan/ppEmbedding/Lambda_PYTHIA_pp/bfc_input/Run12_200/st_zerobias_adc_13057005_raw_1600001.daq"


echo "output directories"
echo ./production/bfc/${prodId}
echo ./jobs/bfc/${prodId}

mkdir -p ./production/bfc/${prodId}
mkdir -p ./jobs/bfc/${prodId}

star-submit-template -template submit_bfc.xml -entities submitdir=$submitdir,nevents=$nevents,productionId=${prodId},config=$config,daqfile=$daqfile,mFileList=${1}

