echo "Test other schemes"

cd Kyber
make clean
make 
./test_speed768
cd ..

cd Saber
make clean
make 
./test_speed768
cd ..

cd NTTRU
./test_speed.sh
cd ..

cd NTRU-HRSS
make clean
make 
./test_speed
cd ..

cd SNTRU-Prime
make clean
make 
./test_speed
cd ..