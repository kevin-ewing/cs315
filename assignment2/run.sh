make
./test_myio test_files/test_from.txt test_files/test_to.txt 


DIFF=$(diff test_files/test_from.txt test_files/test_to.txt)

if [[ $DIFF == "" ]]; 
then
    printf "Tests Passed\n"
else
    printf "ERROR: diff found. $HOSTNAME\n"
fi

make clean_out