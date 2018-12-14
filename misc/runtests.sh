#ooo fancy colors
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

printf "\n${RED}(NOTE: PLEASE RUN ./client.sh and ./server.sh BEFORE RUNNING THESE TESTS)${NC}"
printf "\n${GREEN}Press any key to start tests...${NC}\n"
read -n 1 -s
./test_create.sh
printf "\n${GREEN}Press any key to run next test..${NC}\n"
read -n 1 -s
./cat_test.sh
printf "\n${GREEN}Press any key to run next test..${NC}\n"
read -n 1 -s
./trunc_test.sh
printf "\n${GREEN}Press any key to run next test..${NC}\n"
read -n 1 -s
./mkdir_test.sh
printf "\n${GREEN}Press any key to run next test..${NC}\n"
read -n 1 -s
./ls_test.sh
printf "\n${GREEN}Press any key to run next test..(opening the saved file in vim)${NC}\n"
read -n 1 -s
./vim_test.sh 
