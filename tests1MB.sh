#!/bin/bash
rm -rf testingdir
rm -rf testingdircopy

mkdir testingdir
cd testingdir
echo "Creating 500 files of size 1MB" 
for i in {1..500}
do
   dd if=/dev/urandom bs=1024 count=1024 of=test-${i}.bin status=none
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


echo "500 files 1 MB tests done"

rm -rf testingdir
rm -rf testingdircopy



mkdir testingdir
cd testingdir
echo "Creating 1000 files of size 1MB" 
for i in {1..1000}
do
   dd if=/dev/urandom bs=1024 count=1024 of=test-${i}.bin status=none
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


echo "1000 files 1 MB tests done"

rm -rf testingdir
rm -rf testingdircopy


mkdir testingdir
cd testingdir
echo "Creating 1500 files of size 1MB" 
for i in {1..1500}
do
   dd if=/dev/urandom bs=1024 count=1024 of=test-${i}.bin status=none
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


echo "1500 files 1 MB tests done"

rm -rf testingdir
rm -rf testingdircopy


mkdir testingdir
cd testingdir
echo "Creating 2000 files of size 1MB" 
for i in {1..2000}
do
   dd if=/dev/urandom bs=1024 count=1024 of=test-${i}.bin status=none
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


echo "2000 files 1 MB tests done"

rm -rf testingdir
rm -rf testingdircopy

