# Makefile pour le jeu Zelda-like

CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Détection automatique de l'emplacement de SDL2
SDL2_CFLAGS = $(shell sdl2-config --cflags)
SDL2_LDFLAGS = $(shell sdl2-config --libs)

CXXFLAGS += $(SDL2_CFLAGS)
LDFLAGS = $(SDL2_LDFLAGS) -lSDL2_mixer

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TARGET = amazing_ball

# Trouver tous les fichiers .cpp dans src/
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Règle par défaut
all: $(BUILD_DIR) $(TARGET)

# Créer le dossier build s'il n'existe pas
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compilation de l'exécutable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Compilation réussie! Lancez avec: ./$(TARGET)"

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "✓ Projet nettoyé"

# Compiler et lancer
run: all
	./$(TARGET)

# Recompiler complètement
rebuild: clean all

.PHONY: all clean run rebuild
