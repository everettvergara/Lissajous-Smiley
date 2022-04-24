# Lissajous-Smiley
A smiley face created using Lissajous Curves and SDL.

Can run at 26 FPS for 10,000,000 lissajous flies on a Macbook Pro 2021 M1 - Single thread.

Lissajous Curves
---

In animating a point moving along the perimiter of the circle, we do:

```c++
    int x = cx + radius * cos(angle_in_radians);
    int y = cy + radius * sin(angle_in_radians);
```

In Lissajous curves, we simply parameterized everything. We also introduced the element of randomness as initial values to give that "fly" effect. The overall output is amazingly beautiful.

```c++
    int x = cx + radius_x * cos(angle_in_radians_x);
    int y = cy + radius_y * sin(angle_in_radians_y);
```

![alt text](https://github.com/everettvergara/Lissajous-Smiley/blob/main/assets/lissajous.png)
