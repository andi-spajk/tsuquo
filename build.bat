mkdir obj
mkdir release
mkdir obj\windows
mkdir release\windows
mkdir dots
mkdir saves

set CFLAGS=-Wall -Werror -Wextra -std=c11
set REL_FLAGS=-O3
set DBG_FLAGS=-g3
:: no address/UB sanitizer on windows?

set REL=release\windows
set OBJ=obj\windows
set SRC=src

set REL_DEP=%REL%\main.o %REL%\control.o %REL%\dfa.o %REL%\lexer.o %REL%\minimize.o %REL%\nfa.o %REL%\parser.o %REL%\set.o
set DBG_DEP=%OBJ%\debug.o %OBJ%\control.o %OBJ%\dfa.o %OBJ%\lexer.o %OBJ%\minimize.o %OBJ%\nfa.o %OBJ%\parser.o %OBJ%\set.o

:: release build
gcc %CFLAGS% %REL_FLAGS% %SRC%\main.c -c -o %REL%\main.o
gcc %CFLAGS% %REL_FLAGS% %SRC%\control.c -c -o %REL%\control.o
gcc %CFLAGS% %REL_FLAGS% %SRC%\dfa.c -c -o %REL%\dfa.o
gcc %CFLAGS% %REL_FLAGS% %SRC%\lexer.c -c -o %REL%\lexer.o
gcc %CFLAGS% %REL_FLAGS% %SRC%\minimize.c -c -o %REL%\minimize.o
gcc %CFLAGS% %REL_FLAGS% %SRC%\nfa.c -c -o %REL%\nfa.o
gcc %CFLAGS% %REL_FLAGS% %SRC%\parser.c -c -o %REL%\parser.o
gcc %CFLAGS% %REL_FLAGS% %SRC%\set.c -c -o %REL%\set.o

gcc %CFLAGS% %REL_FLAGS% %REL_DEP% -o main.exe

:: debug build
gcc %CFLAGS% %DBG_FLAGS% %SRC%\main.c -c -o %OBJ%\debug.o
gcc %CFLAGS% %DBG_FLAGS% %SRC%\control.c -c -o %OBJ%\control.o
gcc %CFLAGS% %DBG_FLAGS% %SRC%\dfa.c -c -o %OBJ%\dfa.o
gcc %CFLAGS% %DBG_FLAGS% %SRC%\lexer.c -c -o %OBJ%\lexer.o
gcc %CFLAGS% %DBG_FLAGS% %SRC%\minimize.c -c -o %OBJ%\minimize.o
gcc %CFLAGS% %DBG_FLAGS% %SRC%\nfa.c -c -o %OBJ%\nfa.o
gcc %CFLAGS% %DBG_FLAGS% %SRC%\parser.c -c -o %OBJ%\parser.o
gcc %CFLAGS% %DBG_FLAGS% %SRC%\set.c -c -o %OBJ%\set.o

gcc %CFLAGS% %DBG_FLAGS% %DBG_DEP% -o debug.exe