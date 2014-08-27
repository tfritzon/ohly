ELEMENTS=10000
OPERATIONS=1000000
DRATIO=10
make clean
make config=release
./bin/release/baseline $((ELEMENTS)) $((OPERATIONS)) $((DRATIO)) > baseline.output.txt
./bin/release/linear1  $((ELEMENTS)) $((OPERATIONS)) $((DRATIO)) > linear1.output.txt
#./bin/release/linear2  $((ELEMENTS)) $((OPERATIONS)) $((DRATIO)) > linear2.output.txt

