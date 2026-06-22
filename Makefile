# Usando g++ para C++
CXX = g++

# Flags de compilação - Apontando corretamente para as pastas libs externas
CXXFLAGS = -I"libs/SDL2-2.30.0/x86_64-w64-mingw32/include"

# Flags de linkagem
LDFLAGS = -L"libs/SDL2-2.30.0/x86_64-w64-mingw32/lib" -lmingw32 -lSDL2main -lSDL2 -mwindows

# Nome do executável final (ele será gerado na raiz)
TARGET = vm_app.exe

# Onde está o seu arquivo principal (na pasta vm/)
SRC = vm/main.cpp vm/cpu/cpu.cpp vm/memory/memory.cpp

# Regra padrão
all: $(TARGET)

# Regra para compilar
$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

# Limpar o executável antigo
clean:
	rm -f $(TARGET)