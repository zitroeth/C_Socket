@echo off
echo Compiling...
gcc client.c -o client.exe -lws2_32
gcc server.c -o server.exe -lws2_32
echo Done!
pause