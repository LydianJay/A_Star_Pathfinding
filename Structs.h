#pragma once


enum class CELL_TYPE{
	OPEN, CLOSE, START, END, PATH, VISITED
};

struct Cell {
	
	CELL_TYPE type;
	float fCost;
	float gCost;
	float hCost;
	int x, y;

	Cell* parent;
};