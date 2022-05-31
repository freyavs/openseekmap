#!/usr/bin/env sh

DIR=$(dirname $0)

echo "Getting data for tests"

sh ${DIR}/generate_data.sh | tail -n +2 > planet.data

echo "Starting to run tests"

echo "Compiling..."

# NOTE: Please run this script in the tests directory
gcc -std=c18 -O3 -Wall -Werror -lm $(cat sources) -o runtests 

echo "Running tests"

${DIR}/runtests ${DIR}/planet.data

echo "Testing complete (failed tests will throw 'TEST FAILED')"

echo "Removing test data"
rm country*
rm planet*

exit 0
