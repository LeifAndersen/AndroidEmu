gcc m68kmake.c -o m68kmake.exe
m68kmake
gcc -c m68kcpu.c  -o m68kcpu.o  -O6 -march=pentium -fomit-frame-pointer -DGENESIS_HACKS="1"
gcc -c m68kops.c  -o m68kops.o  -O6 -march=pentium -fomit-frame-pointer -DGENESIS_HACKS="1"
gcc -c m68kopac.c -o m68kopac.o -O6 -march=pentium -fomit-frame-pointer -DGENESIS_HACKS="1"
gcc -c m68kopdm.c -o m68kopdm.o -O6 -march=pentium -fomit-frame-pointer -DGENESIS_HACKS="1"
gcc -c m68kopnz.c -o m68kopnz.o -O6 -march=pentium -fomit-frame-pointer -DGENESIS_HACKS="1"
ar rs m68k.oa m68kcpu.o m68kops.o m68kopac.o m68kopdm.o m68kopnz.o
