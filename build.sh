RED='\033[0;31m' 
GREEN='\033[0;32m'
NC='\033[0m' # No Color

if [[ ! -d "build" ]]; then
  mkdir build
  echo -e "${GREEN}\033[0;32msince 'build' directory not exists, we created it${NC}"
fi

make -f makefile_client
make -f makefile_server
echo -e "${GREEN}Client and server successfully compiled and stored in the 'build' folder${NC}"