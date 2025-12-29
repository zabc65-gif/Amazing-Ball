# Amazing Ball - Distribution

Ce document explique comment utiliser les versions exécutables du jeu Amazing Ball.

## Version macOS (Amazing_Ball_macOS.zip)

### Installation
1. Décompressez le fichier `Amazing_Ball_macOS.zip`
2. Vous obtiendrez une application `Amazing Ball.app`
3. Double-cliquez sur `Amazing Ball.app` pour lancer le jeu

### Note importante pour macOS
Si macOS vous empêche d'ouvrir l'application (message "Apple ne peut pas vérifier..."):
1. Allez dans **Préférences Système** → **Sécurité et confidentialité**
2. Cliquez sur **Ouvrir quand même** dans la section "Général"
3. Ou faites un clic droit sur l'application → **Ouvrir** → **Ouvrir**

### Exigences
- macOS 10.13 (High Sierra) ou plus récent
- SDL2 et SDL2_mixer doivent être installés via Homebrew:
  ```bash
  brew install sdl2 sdl2_mixer
  ```

## Version Windows (Amazing_Ball_Windows.zip)

### Installation
1. Décompressez le fichier `Amazing_Ball_Windows.zip`
2. Vous obtiendrez un dossier `Amazing Ball Windows` contenant:
   - `AmazingBall.exe` (l'exécutable du jeu)
   - `SDL2.dll` et `SDL2_mixer.dll` (bibliothèques nécessaires)
   - Le dossier `assets` (musiques et sons)

### Lancement
Double-cliquez sur `AmazingBall.exe` pour lancer le jeu.

### Exigences
- Windows 10 ou plus récent
- Toutes les bibliothèques nécessaires (DLL) sont incluses

## Contrôles du jeu

### Menu
- **Flèches Haut/Bas** - Navigation dans les menus
- **Entrée/Espace** - Valider une sélection
- **Flèche Droite** - Accéder aux meilleurs scores (depuis le menu principal)
- **Flèche Gauche/ESC** - Retour au menu précédent

### En jeu
- **Flèches directionnelles** - Déplacement du joueur
- **W ou Espace** - Sauter
- **Shift** - Attaquer
- **ESC** - Retour au menu

## Modes de difficulté

- **Facile** - Étoile électrique centrale uniquement
- **Moyen** - Difficulté équilibrée (par défaut)
- **Difficile** - Étoile électrique centrale + étoile satellite en orbite

## Objectif

Traversez les salles en évitant les trous noirs et les étoiles électriques pour atteindre la zone de fin à droite. Récupérez le maximum de points en un minimum de temps!

## Nouveautés de la version finale

### Gameplay
- Collision améliorée avec les ennemis (plus réactive)
- Knockback lors des collisions avec ennemis
- Ennemis plus agressifs (approchent plus près du joueur)
- Système d'invincibilité après collision (1.5 secondes)

### Interface
- Nouvel écran "Meilleurs Scores" accessible depuis le menu principal
- Affichage des scores par difficulté avec couleurs distinctives
- Correction de l'affichage des scores (suppression des caractères parasites)
- Ombres ajoutées sous les flèches de départ pour plus de profondeur

### Audio
- Musique de menu améliorée
- Sons d'attaque et de saut

## Crédits

Jeu développé avec SDL2 en C++17
Version finale - Décembre 2024
