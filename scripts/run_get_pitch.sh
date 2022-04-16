#!/bin/bash

# Put here the program (maybe with path)
<<<<<<< HEAD
GETF0="get_pitch --umaxnorm=0.4"
=======
GETF0="get_pitch --vmaxnorm=0.42 --pot_min=-50 --R10_min=0.4" 
>>>>>>> 5eba51df9528b8af5f9a836ce901fbb2b79e3b57

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done

exit 0
