g++ benchmark/test.cpp dependencies/xxhash.c -o benchmark/test -g -L./dependencies -lblake -flto -march=native -Ofast
g++ benchmark/test2.cpp dependencies/xxhash.c -o benchmark/test2 -march=native -Ofast -g -L./dependencies -lblake -flto
g++ benchmark/test3.cpp dependencies/xxhash.c -o benchmark/test3 -march=native -Ofast -g -L./dependencies -lblake -flto
g++ benchmark/test4.cpp dependencies/xxhash.c -o benchmark/test4 -march=native -Ofast -g -L./dependencies -lblake -flto
g++ benchmark/test5.cpp dependencies/xxhash.c -o benchmark/test5 -march=native -Ofast -g -L./dependencies -lblake
g++ benchmark/test6.cpp dependencies/xxhash.c -o benchmark/test6 -O0 -g -L./dependencies -lblake
g++ benchmark/test7.cpp dependencies/xxhash.c -o benchmark/test7 -march=native -Ofast -g -L./dependencies -lblake -flto
g++ benchmark/test8.cpp dependencies/xxhash.c -o benchmark/test8 -march=native -Ofast -g -L./dependencies -lblake -flto
g++ benchmark/test9.cpp dependencies/xxhash.c -o benchmark/test9 -march=native -Ofast -g -L./dependencies -lblake -flto
g++ benchmark/test10.cpp dependencies/xxhash.c -o benchmark/test10 -march=native -Ofast -g -L./dependencies -lblake -flto
g++ benchmark/test11.cpp dependencies/xxhash.c -o benchmark/test11 -march=native -Ofast -g -L./dependencies -lblake -flto
./benchmark/test
./benchmark/test2
./benchmark/test3
./benchmark/test4
./benchmark/test5
./benchmark/test6
./benchmark/test7
./benchmark/test8
./benchmark/test9
./benchmark/test10
./benchmark/test11