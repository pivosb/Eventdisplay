#!/bin/bash
# combine tables

# set observatory environmental variables
source "$EVNDISPSYS"/setObservatory.sh VTS

# parameters replaced by parent script using sed
FLIST=TABLELIST
OFILE=OUTPUTFILE
ODIR=OUTPUTDIR

# combine the tables
"$EVNDISPSYS"/bin/combineLookupTables "$ODIR/$FLIST" "$ODIR/$OFILE.root" &> "$ODIR/$OFILE.log"

# smooth lookup tables
"$EVNDISPSYS"/bin/smoothLookupTables "$ODIR/$OFILE.root" "$ODIR/$OFILE-smoothed.root" &> "$ODIR/$OFILE-smoothed.log"

exit
