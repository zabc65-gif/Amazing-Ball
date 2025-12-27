# Assets Audio

## Structure des dossiers

- `music/` : Fichiers musicaux (musique de fond)
- `sounds/` : Effets sonores

## Formats supportés

SDL2_mixer supporte les formats suivants:
- **WAV** : Format non compressé (recommandé pour les effets sonores courts)
- **OGG** : Format compressé (recommandé pour la musique de fond)
- **MP3** : Format compressé (supporté)
- **FLAC** : Format sans perte (supporté)

## Musiques nécessaires

Placer les fichiers suivants dans `music/`:
- `menu.ogg` ou `menu.mp3` : Musique du menu principal
- `gameplay.ogg` ou `gameplay.mp3` : Musique pendant le jeu
- `gameover.ogg` ou `gameover.mp3` : Musique de fin de partie

## Effets sonores nécessaires

Placer les fichiers suivants dans `sounds/`:
- `hole_fall.wav` : Son quand le joueur tombe dans un trou
- `level_complete.wav` : Son quand le joueur termine un niveau
- `menu_select.wav` : Son de sélection dans le menu
- `jump.wav` : Son du saut
- `land.wav` : Son de l'atterrissage

## Sources pour trouver de la musique et des sons libres de droits

- **OpenGameArt.org** : https://opengameart.org/
- **Freesound.org** : https://freesound.org/
- **Incompetech** : https://incompetech.com/music/royalty-free/
- **ccMixter** : http://ccmixter.org/

## Notes

- Les fichiers audio doivent avoir des noms exactement comme spécifié ci-dessus
- Pour les musiques, privilégier le format OGG (meilleure compression)
- Pour les effets sonores, privilégier le format WAV (chargement plus rapide)
- Les fichiers seront chargés au démarrage du jeu
