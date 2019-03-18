import math
from colorsys import hsv_to_rgb

def circle_offsets(radius):
    x = 0
    y = radius

    d = 1 - radius
    dl = 3
    dse = -2*y + 5

    yield (x, y)
    while y > x:
        if (d < 0):
            d += dl
            dse += 2
        else:
            d += dse
            dse += 4
            y -= 1
        dl += 2
        x += 1
        yield (x, y)

def circle_points(center, radius, step=1):
    cx, cy = center
    sections = [[] for _ in range(8)]
    for x, y in circle_offsets(radius):
        sections[0].append((cx + x, cy + y))
        sections[1].append((cx + y, cy + x))
        sections[2].append((cx + y, cy - x))
        sections[3].append((cx + x, cy - y))
        
        sections[4].append((cx - x, cy - y))
        sections[5].append((cx - y, cy - x))
        sections[6].append((cx - y, cy + x))
        sections[7].append((cx - x, cy + y))
    return [p for sec in sections for p in sec][::step]

def relative_angle(center, point):
    x, y = point
    cx, cy = center
    rad = math.atan2(y - cy, x - cx) + math.pi
    return rad / math.tau

def get_color(hue):
    return hsv_to_rgb(hue, 1, 1)

def make_line(start, end, color="white", **kwargs):
    line_str = "  - {type: line"
    for p_name, p in (("start", start), ("end", end)):
        if isinstance(p, tuple):
            line_str += ", {}: [{}, {}]".format(p_name, *p)
        else:
            line_str += ", {}: {}".format(p_name, p)

    if isinstance(color, tuple):
        line_str += ", color: [{}, {}, {}]".format(
            *tuple(round(c, 4) for c in color))
    else:
        line_str += ", color: {}".format(color)

    for k, v in kwargs.items():
        line_str += ", {}: {}".format(k, v)
    line_str += "}"
    return line_str


height, width = 750, 750

header = """\
background: black
height: {}
width: {}
antialiasing: false
""".format(height, width)
print(header)

center = (height//2, width//2)
def_points = """\
points:
  - ["center", [{}, {}]]
""".format(*center)
print(def_points)

print("objects:")
skip = 4
r = min(center) - 10

for point in circle_points(center, r, skip):
    angle = relative_angle(center, point)
    color = get_color(angle)
    print(make_line("center", point, color))

for point in circle_points(center, r//3, skip):
    print(make_line("center", point, "white"))

