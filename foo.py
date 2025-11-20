import polygon_intersections

def _load_polygon(filename):
    vertices = []
    with open(filename, "r") as f:
        for line in f:
            if line.strip():
                x, y = line.split(",")
                vertices.append((float(x), float(y)))
    return vertices

def foo(filename):
    vertices = _load_polygon(filename)
    return polygon_intersections.count_intersections_with_diag(vertices)

