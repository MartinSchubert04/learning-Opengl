CXX = g++
CXXFLAGS = -g -std=c++17 -I../include
LDFLAGS = -L../lib -lglfw3dll
SRC_DIR = ../src
BUILD_DIR = ../build

# Encuentra todos los .cpp (recursivamente) excepto glad.c
ALL_SOURCES = $(shell find $(SRC_DIR) -name '*.cpp' ! -name 'glad.c')

# Solo compilará archivos que contengan int main(
MAIN_SOURCES = $(shell grep -l 'int main' $(ALL_SOURCES))

# Nombre base sin ruta ni extensión
TARGETS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.exe,$(MAIN_SOURCES))

# Regla por defecto
all: $(TARGETS)

# Cómo compilar cada archivo individualmente
$(BUILD_DIR)/%.exe: $(SRC_DIR)/%.cpp $(SRC_DIR)/glad.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< $(SRC_DIR)/glad.c $(LDFLAGS) -o $@

clean:
	rm -f $(BUILD_DIR)/*.exe