#!/usr/bin/env bash

function run_tests {
    sh "test/check_noobcsv"

    if test $? -eq 0
    then
        echo -e "\n  \e[32mALL TESTS SUCCEEDED\e[0m\n"
    else
        echo -e "\n  \e[91mSOME TESTS FAILED\e[0m\n"
    fi
}

make -s check > /dev/null 2>&1

if test $? -eq 0
then
    run_tests
else
    rm "test/check_noobcsv"

    make check > /dev/null 2>&1

    if test -e test/check_noobcsv
    then
        run_tests
    else
        echo -e "\n\e[91m  BUILD FAILED:\e[0m\n"
        make -s check
    fi
fi
