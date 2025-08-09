# KillerGrass DOOM

![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![G++](https://img.shields.io/badge/G++-00599C?style=flat&logo=c%2B%2B&logoColor=white)
<img src="https://upload.wikimedia.org/wikipedia/commons/5/5f/Windows_logo_-_2012.svg" width="20" alt="Windows"/>

This is the English README

[![Русский](https://img.shields.io/badge/README-Русский-blue)](README_ru.md)

<div align="center">
<img src="img/icon.png" width="400" alt="Windows"/>
</div>

# Educational minimal parody DOOM

This is a minimal educational parody of DOOM with Ray Casting and BSP functions. This game is written for educational purposes in pure C++ only under Windows. 

# Installing

To use the game you need to install `g++` and have Windows7/8/10 (or any where you can install g++). The authors use `g++` version `14.2.0`.

Clone repository:

```bash
git clone https://github.com/UbicaTravy/KillerGrassDOOM
```

For build executable file run the build.bat file or use `Makefile`:

```bash
make
```

Done! The file `killer_grass_doom.exe` will appear in your folder!

## Building release:

```bash
windres resources.rc -o resources.o
g++ -O3 -Wall -Wextra -static -mwindows -o killer_grass_doom.exe main.o game.o render.o trig_tables.o resources.o -lgdi32 -lwinmm
```

In `file resourses.rc` path to icon

```
MAINICON ICON "img/icon.ico"
```

# Game process

`WASD` - move

Arrows - camera

# Useful links

[Ray Casting](https://lodev.org/cgtutor/raycasting.html)

[BSP](https://en.wikipedia.org/wiki/Binary_space_partitioning)

# Current bugs

Floor rendering is a bit lame, we are working on this bug at the moment. If you want to help and know how - write to me!

# Screenshots from game

<div align="center">

<img src="img/screenshot1.png" width="400" alt="Windows"/>
<img src="img/screenshot2.png" width="400" alt="Windows"/>
<img src="img/screenshot3.png" width="400" alt="Windows"/>

</div>

# License

[![License](https://img.shields.io/badge/License-MIT-green)](LICENSE)

<div align="center">

### Created by KillerGrass

<img src="img/killergrass_logo.jpg" width="400" alt="KillerGrass_logo"/> 

[![GitHub](https://img.shields.io/badge/-GitHub-181717?style=for-the-badge&logo=github&logoColor=white)](https://github.com/UbicaTravy)

[![Telegram](https://img.shields.io/badge/-Telegram-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white)](https://t.me/killergrass_programms)

[![LIVE Channel](https://img.shields.io/badge/-LIVE&nbsp;Kanal-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white)](https://t.me/kanal_kashkamalhika)

</div>

<div align="center">

## Contacts

[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=flat&logo=telegram&logoColor=white)](https://t.me/KillerGrassContactBot)
[![Gmail](https://img.shields.io/badge/Gmail-D14836?style=flat&logo=gmail&logoColor=white)](mailto:killergrasscontact@gmail.com)
</div>