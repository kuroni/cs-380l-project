#!/bin/bash
rm -rf testingdir
rm -rf testingdircopy

mkdir testingdir
cd testingdir
echo "Creating 5000 files of size 128KB" 
for i in {1..5000}
do
   dd if=/dev/urandom bs=1024 count=128 of=test-${i}.bin
done

cd ..
echo "Testing with ./cpr"
./test.sh ./cpr testingdir testingdircopy

echo "Testing with ./cpr-no-falloc" 
rm -rf testingdircopy
./test.sh ./cpr-no-falloc testingdir testingdircopy

echo "Testing with ./cpr-single-file" 
rm -rf testingdircopy
./test.sh ./cpr-single-file testingdir testingdircopy

echo "Testing with ./cpr-new-buffer" 
rm -rf testingdircopy
./test.sh ./cpr-new-buffer testingdir testingdircopy

echo "Testing with cp -r" 
rm -rf testingdircopy
./test.sh "cp -r" testingdir testingdircopy


echo "Tests done"

rm -rf testingdir
rm -rf testingdircopy

echo "5000 files 128kB testing done"


mkdir testingdir
cd testingdir
echo "Creating 10000 files of size 128KB" 
for i in {1..10000}
do
   dd if=/dev/urandom bs=1024 count=128 of=test-${i}.bin
done

cd ..
echo "Testing with ./cpr"
./test.sh ./cpr testingdir testingdircopy

echo "Testing with ./cpr-no-falloc" 
rm -rf testingdircopy
./test.sh ./cpr-no-falloc testingdir testingdircopy

echo "Testing with ./cpr-single-file" 
rm -rf testingdircopy
./test.sh ./cpr-single-file testingdir testingdircopy

echo "Testing with ./cpr-new-buffer" 
rm -rf testingdircopy
./test.sh ./cpr-new-buffer testingdir testingdircopy

echo "Testing with cp -r" 
rm -rf testingdircopy
./test.sh "cp -r" testingdir testingdircopy
echo "Tests done"

rm -rf testingdir
rm -rf testingdircopy

echo "10000 files 128kB testing done"


mkdir testingdir
cd testingdir
echo "Creating 15000 files of size 128KB" 
for i in {1..15000}
do
   dd if=/dev/urandom bs=1024 count=128 of=test-${i}.bin
done

cd ..
echo "Testing with ./cpr"
./test.sh ./cpr testingdir testingdircopy

echo "Testing with ./cpr-no-falloc" 
rm -rf testingdircopy
./test.sh ./cpr-no-falloc testingdir testingdircopy

echo "Testing with ./cpr-single-file" 
rm -rf testingdircopy
./test.sh ./cpr-single-file testingdir testingdircopy

echo "Testing with ./cpr-new-buffer" 
rm -rf testingdircopy
./test.sh ./cpr-new-buffer testingdir testingdircopy


echo "Testing with cp -r" 
rm -rf testingdircopy
./test.sh "cp -r" testingdir testingdircopy
echo "Tests done"

rm -rf testingdir
rm -rf testingdircopy

echo "15000 files 128kB testing done"


mkdir testingdir
cd testingdir
echo "Creating 20000 files of size 128KB" 
for i in {1..20000}
do
   dd if=/dev/urandom bs=1024 count=128 of=test-${i}.bin
done

cd ..
echo "Testing with ./cpr"
./test.sh ./cpr testingdir testingdircopy


echo "Testing with ./cpr-no-falloc" 
rm -rf testingdircopy
./test.sh ./cpr-no-falloc testingdir testingdircopy

echo "Testing with ./cpr-single-file" 
rm -rf testingdircopy
./test.sh ./cpr-single-file testingdir testingdircopy

echo "Testing with ./cpr-new-buffer" 
rm -rf testingdircopy
./test.sh ./cpr-new-buffer testingdir testingdircopy

echo "Testing with cp -r" 
rm -rf testingdircopy
./test.sh "cp -r" testingdir testingdircopy
echo "Tests done"

rm -rf testingdir
rm -rf testingdircopy

echo "20000 files 128kB testing done"



