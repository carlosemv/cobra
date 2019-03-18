# COBRA
COmmon Bidimentional RAsterizer (COBRA) - A simple 2D rasterizer implemented in C++

## Table of Contents
+ [1. Main features](#features)
+ [2. Build](#build)
+ [3. Run](#run)
+ [4. Scene description](#description)
+ [4.1. Collections](#collections)
+ [4.2. Objects](#objects)
+ [4.2.1. Object types](#obj-types)
+ [4.2.2. Filling](#filling)
+ [4.3. Description example](#desc-example)
+ [5. Examples](#examples)
+ [6. (Possible) future features](#future)
+ [7. Acknowledgements and licensing](#licensing)

## <a name="features"></a> Main features:
* [x] Lines, implemented with Bresenham algorithm
* [x] Circles, implemented with midpoint algorithm
* [x] Circle arcs
* [x] Fills
    * [x] Polygon scan-line
    * [x] Flood fill
* [x] Antialiasing (convolution-based)
* [x] Collections (named user-defined values)
    * [x] Colors
    * [x] Points
    * [x] Arcs
    * [x] Variables (integers, floats)
    
## <a name="build"></a> Build
After cloning/downloading the repository, and navigating to it, do:
```bash
mkdir bin
cd bin
cmake ..
make
```

## <a name="run"></a> Run

This command will read the scene description on the specified file, and produce a png image with the specified file name.

```bash
./bin/cobra <your scene description file> <your output file name>
```

## <a name="description"></a> Scene description

Scene descriptions are written in YAML. Below is a brief guide on how to do so.
But first, we define some type descriptors: (I) means integer, (N) non-negative integer, (S) string, and (R) a real number between 0 and 1 (inclusive). [x] stands for the same type as (x), but means the corresponding value is optional. <x, x, ...> stands for a list with elements of type x. An optional list would be represented as [<x, x, ...>].

The "root" of the description can have the following fields (note that the first two are necessary):
* height: (N), defines scene height (in pixels)
* width: (N), defines scene width (in pixels)
* background: [S] or <R, R, R>, defines background color (see *color* below in *collections*)

### <a name="collections"></a> Collections
After that, we will we go into the *collections*, where you can define and give name to certain values.
The collections are:
* colors
* points
* arcs
* variables

"colors", "points", and "arcs", are lists, where each element must be either:
(i) a list with two elements, the first being the name (S), and the second the corresponding value (<R, R, R>, <N, N>, and <R, R>, respectively); or
(ii) a map, with keys "name" and "value" mapped to the values described in (i).

A **color**'s value is defined as a list <R, R, R>, representing proportions for red, greed, and blue, respectively. Meaning, for an implementation with color depth of 256 (for example), a color <0, 0.5, 1.0> would be mapped to an RGB value of <0, 128, 255>. Some default colors may be already defined by the implementation.

A **point**'s value is defined as a list <N, N>. If we name these values respectively x and y, x gives the horizontal (rightwards) distance of that point from the origin, and y the vertical (upwards) distance. The origin is located in the lower left corner of the scene.

An **arc**'s value is defined as a list <R, R>, each value being a proportion of 2pi radians, and the arc being defined between the point in a given circle with the first angle, and the point with the second angle, relative to that circle's rightmost point, going anticlockwise.
This means that, if we have an arc with values <0.25, 0.75>, we'll have an arc between pi/2 and 3pi/4 radians, or 90 and 270 degrees, and it will go from the uppermost point in a given circle to its lowermost point, anticlockwise. It's also possible to have an arc where the first value is greater than the first: <0.75, 0.25>, together with the previous arc, for example, would give us a full circle.

Now, a **variable** is defined similarly, but with an additional field: "type", which can have a value of "integer", meaning that its value is an integer; or "float", meaning that its value is a floating-point (or "real") number (bounds are implementation-dependent). When defined as a list, the type must come before name and value. While variables can be used mostly any place a value of that type is expected, note that you can *not* operate on variables (such as a1+a2, or a3-10, or any such expression).

### <a name="objects"></a> Objects
Now comes the main part of the scene description, where we define all objects in the scene. The "objects" section is a list, where each element is a map, having the following base fields:
* name: [S], simply names an object (no use as of yet besides readability)
* type: (S), defines object type (see *types* below)
* color: [S] or [<R, R, R>], defines the object's (line) color (see *color* above, in *collections*)
More fields are defines depending on the object's type.

#### <a name="obj-types"></a> Object types
There are six basic objects, and these are their types and specific fields:

A **line** defines a line between the points "start" and "end" (<N, N> or (S) each, see *point* above, in *collections*)).

A **polyline** defines multiple connected line segments, usually for open shapes. For this it must have a "points" field, a list in which each element is a point (again, <N, N> or (S)). Each point on that list is connected to the next, up until the second-to-last one.

A **polygon** is similar to a polyline, and also has a "points" field, the difference being that the last point is also connected to the first, defining a closed shape. Therefore, it also can be *filled* (see *filling* below).

A **rect** is a rectangle, which is just a specific type of polygon, defined by its "corner" (point representing its upper left corner), "width" (N) and "height" (N).

A **circle** is exactly what it sounds like, and is defined by a "radius" (N), and by its "center" (a point, so <N, N> or (S)). It can also be filled, although not by all methods.

An **arc** is similar to a circle, in that it also has center and radius fields. But, it must also have an "arc" field (<R, R> or (S)), defining its arc, or range over the full circle (see *arc* above, in *collections*).


#### <a name="filling"></a> Filling
Polygons and circles can also be *filled*. There are two methods for filling, scanline and flood fill. Both share these fields:
* fill: [S], if it has a value of "scanline"
* fill_color: [S] or <R, R, R>, which defines the color used to fill the object (see *color* above, in *collections*)
Scanline works only on polygons, and will be done to an object if the value of "fill" is "scanline".

Now, the other method, flood fill, works on all closed shapes, and has a required field of "flood_points", which is a list of points. For each point in that list, the flood fill algorithm will color all pixels orthogonally reachable from that point that have the same color as the original point (think bucket paint tool).

Note that if "fill" is not defined, scanline will be assumed, unless "flood_points" is defined. And if "fill_color" is not defined, an implementation-depended default color will be used.

### <a name="desc-example"></a> Description example
Below is a simple but complete example of a scene description, which produces the image above it.

<img src="https://github.com/carlosemv/cobra/raw/master/examples/simple/simple.png" width="300">

```yaml
background: white
height: 100
width: 100
antialiasing: false
colors:
  - name: magenta
    value: [1.0, 0, 1.0]
  - name: cyan
    value: [0, 1., 1.]
points:
  - [center, [50, 50]]
  - [bar_bottom, [80, 40]]
arcs:
  - [lquarter, [0.375, 0.625]]
  - [rquarter, [0.875, 0.125]]
variables:
  - type: integer
    name: arcs_r
    value: 15
  - [integer, t_top, 25]
objects:
  - name: l1
    type: line
    color: magenta
    start: [0, 50]
    end: [99, 99]
  - name: l2
    type: line
    color: cyan
    start: [0, 50]
    end: [99, 0]
  - name: red_line
    type: line
    color: red
    start: [50, 50]
    end: [30, 99]
  - name: green_line
    type: line
    color: green
    start: [50, 50]
    end: [30, 0]
  - name: blue_line
    type: line
    color: blue
    start: [50, 50]
    end: [99, 50]
  - name: gray_bar
    type: line
    start: bar_bottom
    end: [80, 60]

  - type: polyline
    points: [[90, 15], [80, 20], [90, 25], 
      [80, 30], [90, 35], bar_bottom]

  - type: circle
    color: mid_gray
    center: center
    radius: 26

  - type: arc
    center: center
    radius: arcs_r
    arc: lquarter
  - type: arc
    center: center
    radius: arcs_r
    arc: rquarter

  - type: rect
    corner: [12, t_top]
    height: 20
    width: 7
    color: blue
    fill_color: black
  - type: polygon
    color: red
    fill_color: black
    points: [[25, 5], [25, t_top], [40, 15]]

  - type: circle
    center: [35, 90]
    radius: 8
    flood_points: [[30, 90], [40, 90]]
    fill_color: mid_gray
```

## <a name="examples"></a> Examples
[Aladdinsane](/examples/aladdinsane/):<br>
![bowie](/examples/aladdinsane/aladdinsane.png)

[Iristar](/examples/iristar/):<br>
![iris](/examples/iristar/iristar.png)

## <a name="future"></a> (Possible) future features:
Besides those unmarked on main features, we could have:
* Thickness parameter for lines
* Thickness parameter for other objects
* Ellipsis
* Alternative aliasing techniques
* Flood fill without user-defined flood points
* "Fill" collection
* String type for variables
* Expressions involving variables

## <a name="licensing"></a> Acknowledgements and licensing
Two libraries were used in this project and are included in this repository:
* [STB image writing library](https://github.com/nothings/stb "STB libraries"), a single-file public domain (or MIT licensed) library used here for writing png images.
* [OpenGL mathematics library](https://glm.g-truc.net "OpenGL Mathematics"), a header only mathematics library for graphics software. Only its vector classes and associated operations are used in this project, for (computational) efficiency. Its source code and documentation are licensed under the [Happy Bunny License (Modified MIT) or the MIT License](external/glm/manual.md#section0).
