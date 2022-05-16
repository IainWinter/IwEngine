<div align="center">
  <img src="https://raw.githubusercontent.com/Tastyep/HitboxBuilder-2D/master/assets/images/title.png"  alt="logo">
</div>


## Introduction
Hitbox Builder is a lightweight C++ library aiming to automatizing the process of generating hitboxes composed by only convex polygons.

It was originally designed for 2D games, but it could also serve different purposes.

## License
Hitbox Builder is distributed under the [MIT License](https://raw.githubusercontent.com/Tastyep/SFML-HitboxBuilder-2D/develop/LICENSE)

## Requirements
- [SFML](https://www.sfml-dev.org/)
- [CMake](https://github.com/Kitware/CMake)
- [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/)

## Features
- Contour detection ([Marching squares](https://en.wikipedia.org/wiki/Marching_squares))
- Contour simplification ([Douglas–Peucker](https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm))
- Polygon triangulation ([Ear clipping](https://en.wikipedia.org/wiki/Two_ears_theorem))
- Polygon partitioning (Hertel-Mehlhorn)

Below is a representation of each stage of transformation of the data.

<div align="center">
  <img src="https://raw.githubusercontent.com/Tastyep/HitboxBuilder-2D/master/assets/images/banner.png"  alt="logo">
</div>

## Usage

```C++
// Initialize the builder.
HitboxBuilder::init();

// Create a hitbox for the given sprite with an accuracy of 70%.
const auto hitbox = HitboxBuilder::make(sprite, 70, false);

const auto& convexPolygons = hitbox.body();
const auto& boundingBox = hitbox.bound();

// Use these data as you like.
```

## Demos

To run the demos, execute the configure script with the option '-t'.

Execute the target HitboxBuilder-tester located in the build directory.

Some test bed commands are:
- <kbd>←</kbd> <kbd>→</kbd> keys to slide between the images.
- <kbd>↑</kbd> <kbd>↓</kbd> keys to increase/decrease the accuracy of the
algorithm.
- <kbd>Space</kbd> key to toggle the display of the sprite.

You can load your own images by adding them to the TestBed/assets/ directory. You will just need to restart the program as they are loaded at the beginning.

##### Output:

 Input                   | Output
:-----------------------:|:-------------------------:
![silhouette](https://raw.githubusercontent.com/Tastyep/HitboxBuilder-2D/master/assets/images/input.png) | ![Accuracy gif](https://raw.githubusercontent.com/Tastyep/HitboxBuilder-2D/master/assets/images/accuracy.gif)
 **Accuracy = 100**           |  **Accuracy = 66**
![Italy 100](https://raw.githubusercontent.com/Tastyep/HitboxBuilder-2D/master/assets/images/italy-100.png) | ![Italy 66](https://raw.githubusercontent.com/Tastyep/HitboxBuilder-2D/master/assets/images/italy-66.png)
