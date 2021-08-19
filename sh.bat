@ECHO OFF
cd build
make clean
make
cd ..
.\build\typeconv.exe "result.py" "example.h" "kserial_ack_t"