#!/bin/bash

#starver DEV
#starver SL21d
starver SL12d_embed
#starver SL12d

rm ./log/bfc.log

root4star -b -q 'bfcMixer.C(50)' > ./log/bfc.log

mv *.root output_bfc/.
