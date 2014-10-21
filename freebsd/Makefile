GCC=g++
CFLAGS=-g
OBJECTS=common.o main.o connection.o user.o game.o instruction.o
SRC=../kailpy

kailpy:$(OBJECTS)
	$(GCC) $(CFLAGS) -o kailpy $(OBJECTS)
    
main.o:$(SRC)/main.cpp
	$(GCC) $(CFLAGS) -c $(SRC)/main.cpp 

connection.o:$(SRC)/connection.h \
				$(SRC)/connection.h \
				$(SRC)/instruction.h
	$(GCC) $(CFLAGS) -c $(SRC)/connection.cpp

common.o:$(SRC)/common.h \
				$(SRC)/common.cpp
	$(GCC) $(CFLAGS) -c $(SRC)/common.cpp
user.o:$(SRC)/user.h \
				$(SRC)/user.cpp
	$(GCC) $(CFLAGS) -c $(SRC)/user.cpp

game.o:$(SRC)/game.h \
				$(SRC)/game.cpp
	$(GCC) $(CFLAGS) -c $(SRC)/game.cpp

instruction.o:$(SRC)/instruction.h \
				$(SRC)/instruction.cpp
	$(GCC) $(CFLAGS) -c $(SRC)/instruction.cpp

clean:
	rm *.o
	rm kailpy
