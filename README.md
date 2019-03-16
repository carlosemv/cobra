# COBRA
COmmon Bidimentional RAsterizer (COBRA) - A simple 2D rasterizer implemented in C++

## Main features:
* [x] Lines, implemented with Bresenham algorithm
* [x] Circles, implemented with midpoint algorithm
* [ ] Circle arcs
* [x] Fills
    * [x] Polygon scan-line
    * [x] Flood fill
* [x] Antialiasing (convolution-based)
* [x] Collections (named user-defined values)
    * [x] Colors
    * [x] Points
    
## (Possible) future features:
Besides those unmarked on main features, we could have:
* Thickness parameter for lines
* Thickness parameter for other objects
* Ellipsis
* Alternative aliasing techniques
* Flood fill without user-defined flood points
* "Fill" collection

## Build

```bash
mkdir bin
cd bin
cmake ..
make
```

## Run

This command will read the scene description on the specified filed, and produce a png image accordingly.
The filename of the image produced is defined by the "out_file" field in your scene description.

```bash
./bin/cobra <your scene description file>
```

## Scene description

Scene descriptions are written in YAML. Below is a brief guide on how to do so.
But first, we define some type descriptors: (I) means integer, (N) non-negative integer, (S) string, and (R) a real number between 0 and 1 (inclusive). [x] means the same as (x), but is optional. <x, x, ...> means a list with elements of type x.


The "root" of the description must have the fields:
* out_file: (S) defines output file path/name
* height: (N) defines scene height
* width: (N) defines scene width
* background: [S] defines background color 

Beyond that, there are three important sections:

**colors** is a list of maps with two keys:
* name: (S) defines a color name
* rgb: <R, R, R> defines the color associated with the given name

**points** is a list of maps with two keys:
* name: (S) defines a point name
* rgb: <N, N> defines the point associated with the given name

Note that all points are defined by <x, y>, where x gives the horizontal (rightwards) distance of that point from the origin, and y the vertical (upwards) distance. The origin is located in the lower left corner of the scene.

```yaml
out_file: ./imgs/i0.png
background: white
height: 100
width: 100
colors:
  - name: magenta
    rgb: [1.0, 0, 1.0]
  - name: cyan
    rgb: [0, 1., 1.]
points:
  - ["mycenter", [50, 50]]
objects:
  - name: l1
    type: line
    color: magenta
    start: [0, 50]
    end: [99, 99]
  - name: red_line
    type: line
    color: red
    start: [50, 50]
    end: [30, 99]
  - type: polyline
    points: [[20, 40], [25, 60], [30, 40], [35, 60], [40, 40]]
  - type: polygon
    fill_color: magenta
    points: [[0, 0], [25, 10], [10, 40]]
  - type: polygon
    color: red
    fill_color: black
    points: [[80, 10], [88, 30], [94, 12]]
  - type: circle
    color: mid_gray
    center: mycenter
    radius: 20
  - type: rect
    corner: [50, 20]
    height: 15
    width: 5
    color: blue
    fill_color: black
```
