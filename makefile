TARGET = dave
CC = clang++
LFLAGS =  -framework Foundation
CFLAGS = -arch x86_64 -ObjC++ -std=c++0x -stdlib=libc++ -mmacosx-version-min=10.7 -fobjc-arc -fobjc-nonfragile-abi -O3
SRCS = main.cpp DGTime.cpp DGTerminal.cpp IncString.cpp OCMD4.cpp OCHashing.cpp OCShadowHashData.m DGServer.cpp DGCloud.m DGCloudController.cpp DGAttack.m DGController.m

all: $(SRCS)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) $(SRCS)

debug: CFLAGS += -DDEBUG -g
debug: all

clean:
	rm -f $(TARGET)
