cp *.c ./bin
cp *.h ./bin
cp *.ini ./bin
cp -r ./parameters ./bin
cp -r ./algorithms ./bin
cp -r ./headers ./bin
cp -r ./utils ./bin
cp Makefile ./bin
cd ./bin
make
rm *.c
rm *.h
rm -r ./headers
rm -r ./utils
rm -r ./algorithms
rm Makefile
