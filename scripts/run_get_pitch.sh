#!/bin/bash

# Put here the program (maybe with path)
GETF0="get_pitch --vmaxnorm=0.42 --pot_min=-50 --R10_min=0.4" 

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done

exit 0

