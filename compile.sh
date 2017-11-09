#/bin/bash
#Cross compiling script for linux
i686-w64-mingw32-c++ -c Win32InputBox.cpp -lcomdlg32 -static-libstdc++ -static-libgcc -static -lpthread -lshlwapi -fpermissive -o inputbox.o
i686-w64-mingw32-c++ delftParallel.cpp inputbox.o -lcomdlg32 -static-libstdc++ -static-libgcc -static -lpthread -lshlwapi -o delftParallel.exe

