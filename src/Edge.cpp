#include "Edge.h"

Edge::Edge(Point p1, Point p2)
{
	if (p1.y < p2.y or (p1.y == p2.y and p1.x < p2.x)) {
		y_min = p1.y;
		x_min = p1.x;
		y_max = p2.y;
	} else {
		y_min = p2.y;
		x_min = p2.x;
		y_max = p1.y;
	}

	inv_m.first = p1.x - p2.x;
	inv_m.second = p1.y - p2.y;

	if (inv_m.second < 0) {
		inv_m.second *= -1;
		inv_m.first *= -1;
	}

	x_remain = 0;
}

bool Edge::et_compare(Edge e1, Edge e2)
{
	if (e1.y_min == e2.y_min)
		return (e1.x_min < e2.x_min);
	return (e1.y_min < e2.y_min);
}
