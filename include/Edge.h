#ifndef __EDGE_H__
#define __EDGE_H__

#include "Point.h"

struct Edge
{
	int y_min, y_max;
	int x_min;
	ipair_t inv_m;
	int x_remain;

	Edge(Point p1, Point p2);

	static bool et_compare(Edge e1, Edge e2);
	inline friend std::ostream& operator<<(std::ostream& out, const Edge& e)
	{
		out << "Edge [y_min=" << e.y_min;
		out << ", y_max=" << e.y_max;
		out << ", x_min=" << e.x_min;
		out << ", 1/m=" << e.inv_m.first << "/" << e.inv_m.second;
		out << ", x_rem=" << e.x_remain;
		out << "]";
		return out;
	}
};

#endif
