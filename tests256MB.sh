#!/bin/bash
rm -rf testingdir
rm -rf testingdircopy

mkdir testingdir
cd testingdir
echo "Creating 2 files of size 256MB" 
for i in {1..2}
do
   dd if=/dev/urandom bs=1024 count=262144 of=test-${i}.bin status=none
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


echo "2 files 256 MB tests done"

rm -rf testingdir
rm -rf testingdircopy



mkdir testingdir
cd testingdir
echo "Creating 4 files of size 256MB" 
for i in {1..4}
do
   dd if=/dev/urandom bs=1024 count=262144 of=test-${i}.bin status=none
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


echo "4 files 256 MB tests done"

rm -rf testingdir
rm -rf testingdircopy


mkdir testingdir
cd testingdir
echo "Creating 8 files of size 256MB" 
for i in {1..8}
do
   dd if=/dev/urandom bs=1024 count=262144 of=test-${i}.bin status=none
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


echo "8 files 256 MB tests done"

rm -rf testingdir
rm -rf testingdircopy


mkdir testingdir
cd testingdir
echo "Creating 16 files of size 256MB" 
for i in {1..16}
do
   dd if=/dev/urandom bs=1024 count=262144 of=test-${i}.bin status=none
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


echo "16 files 256 MB tests done"

rm -rf testingdir
rm -rf testingdircopy

