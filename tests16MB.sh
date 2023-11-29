#!/bin/bash
rm -rf testingdir
rm -rf testingdircopy

mkdir testingdir
cd testingdir
echo "Creating 50 files of size 16MB" 
for i in {1..50}
do
   dd if=/dev/urandom bs=1024 count=16384 of=test-${i}.bin status=none
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


echo "50 files 16 MB tests done"

rm -rf testingdir
rm -rf testingdircopy



mkdir testingdir
cd testingdir
echo "Creating 100 files of size 16MB" 
for i in {1..100}
do
   dd if=/dev/urandom bs=1024 count=16384 of=test-${i}.bin status=none
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


echo "100 files 16 MB tests done"

rm -rf testingdir
rm -rf testingdircopy


mkdir testingdir
cd testingdir
echo "Creating 125 files of size 16MB" 
for i in {1..125}
do
   dd if=/dev/urandom bs=1024 count=16384 of=test-${i}.bin status=none
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


echo "125 files 16 MB tests done"

rm -rf testingdir
rm -rf testingdircopy


mkdir testingdir
cd testingdir
echo "Creating 200 files of size 16MB" 
for i in {1..200}
do
   dd if=/dev/urandom bs=1024 count=16384 of=test-${i}.bin status=none
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


echo "200 files 16 MB tests done"

rm -rf testingdir
rm -rf testingdircopy

