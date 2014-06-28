
Build the programs
------------------

    cmake .
    make


Generate the input
------------------

    ./mkrandom 100 > input_1e2.txt
    ./mkrandom 1000 > input_1e3.txt
    ./mkrandom 10000 > input_1e4.txt
    ./mkrandom 100000 > input_1e5.txt
    ./mkrandom 1000000 > input_1e6.txt
    ./mkrandom 10000000 > input_1e7.txt
    ./mkrandom 100000000 > input_1e8.txt
    ./mkrandom 1000000000 > input_1e9.txt


Measure the runtimes
--------------------
    
    time ./count input_1e2.txt
    time ./count input_1e3.txt
    time ./count input_1e4.txt
    time ./count input_1e5.txt
    time ./count input_1e6.txt
    time ./count input_1e7.txt
    time ./count input_1e8.txt
    time ./count input_1e9.txt





