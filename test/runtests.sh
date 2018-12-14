printf "\n(NOTE: PLEASE RUN ./client.sh and ./server.sh BEFORE RUNNING THESE TESTS)"
printf "\nPress any key to start tests...\n"
read -n 1 -s
./test_create.sh
printf "\nPress any key to run next test..\n"
read -n 1 -s
./cat_test.sh
printf "\nPress any key to run next test..\n"
read -n 1 -s
./trunc_test.sh
printf "\nPress any key to run next test..\n"
read -n 1 -s
./mkdir_test.sh
printf "\nPress any key to run next test..\n"
read -n 1 -s
./ls_test.sh
printf "\nPress any key to run next test..(opening the saved file in vim)\n"
read -n 1 -s
./vim_test.sh 
