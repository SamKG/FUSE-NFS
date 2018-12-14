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
vim /tmp/client16555/testfile.txt
