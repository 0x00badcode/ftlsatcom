# Compiler
CXX = g++

# OpenSSL and Libsodium path
OPENSSL_PATH = /opt/homebrew/opt/openssl@3
LIBSODIUM_PATH = /opt/homebrew/opt/libsodium

# Compiler Flags
CXXFLAGS = -std=c++11 -Wall -I$(OPENSSL_PATH)/include -I$(LIBSODIUM_PATH)/include -I./includes

# Linker Flags
LDFLAGS = -L$(OPENSSL_PATH)/lib -L$(LIBSODIUM_PATH)/lib -lcrypto -lssl -lsodium

# Target executable name
TARGET = ftlsat

# Source directory and all source files
SRCDIR = src
SRC = $(wildcard $(SRCDIR)/*.cpp) main.cpp

# Object files
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
