#!/usr/bin/env bash

set -e
jang_data="4 Jang email@com"
zed_data="3 Zed test@com"
./ex17 test.db c 10 10
./ex17 test.db s $jang_data
./ex17 test.db s $zed_data
zed=$(./ex17 test.db g 3)
jang=$(./ex17 test.db g 4)

# Get test
if [ "$jang_data" != "$jang" ]
then
    echo "Test failed!"
    echo "$jang is not equal to $jang_data"
    exit 1
fi

if [ "$zed_data" != "$zed" ]
then
    echo "Test failed!"
    exit 1
fi

# Find test
absurd_name=Jed
find_absurd=$(./ex17 test.db f $absurd_name)
if [ ! -z "$find_absurd" ]
then
    echo "Test failed"
    echo "Find test found $absurd_name in test.db"
    exit 1
fi

existing_name=Zed
find_existing=$(./ex17 test.db f $existing_name)
if [ -z "$find_existing" ]
then
    echo "Test failed"
    echo "Find test couldn't find $existing_name in test.db"
    exit 1
fi

existing_name=Jang
find_existing=$(./ex17 test.db f $existing_name)
if [ -z "$find_existing" ]
then
    echo "Test failed"
    echo "Find test couldn't find $existing_name in test.db"
    exit 1
fi

rm -f test.db
