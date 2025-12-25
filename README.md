# Amazing Ball

Un jeu d'aventure de type Zelda en C++ avec SDL2.

## Compilation

Pour compiler le projet:

```bash
make
```

Pour nettoyer les fichiers compilés:

```bash
make clean
```

Pour compiler et lancer directement:

```bash
make run
```

## Contrôles

- **Flèches directionnelles** - Déplacement du personnage
- **Espace** - Attaquer avec l'épée
- **ESC** - Quitter le jeu

## Structure du projet

```
Amazing Ball/
├── src/           # Code source (.cpp)
├── include/       # Headers (.hpp)
├── assets/        # Ressources (sprites, sons, etc.)
├── build/         # Fichiers objets compilés
├── Makefile       # Système de compilation
└── amazing_ball   # Exécutable du jeu
```

## Fonctionnalités actuelles

- ✅ Boucle de jeu principale à 60 FPS
- ✅ Système de rendu avec SDL2
- ✅ Personnage jouable avec déplacement 4 directions
- ✅ Système d'attaque avec épée
- ✅ Carte/donjons avec murs et sols
- ✅ Système de vie (3 coeurs)

## Prochaines étapes

- Système d'ennemis avec IA basique
- Système d'inventaire et objets
- Plusieurs salles/donjons
- Puzzles et boss
