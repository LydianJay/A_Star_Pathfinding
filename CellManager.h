#pragma once
#include "Structs.h"
#include "s2d/GLSimp2DGraphics.h"
#include <vector>
#include <cmath>
#include <algorithm>

class CellManager {

public:

private:
	unsigned int m_cellCol, m_cellRow;
	Cell* m_cells;
	s2d::S2DGraphics* m_graphics;
	Cell* m_start, *m_end;
	std::vector<Cell*> m_open, m_close;
	Cell* m_currentClickedCell = nullptr;
public:
	void initGraphics(s2d::S2DGraphics* graphics);
	void initCells(int x, int y);
	void drawCells(s2d::S2DGraphics* g, int w, int h);
	void resetCell();
	void executeAStar();
	void clickCell(float x, float y, float w, float h);
	void moveStartLoc(float msX, float msY, float w, float h);
	void moveEndLoc(float msX, float msY, float w, float h);
private:
	void calculateNeigbor(Cell* current);
	Cell* getLowestFCost();
	Cell* getLowestFCostFromNeighbor(Cell* cell);

	float getHeuristic(int x, int y);
	float getGCost(Cell* from);



};

