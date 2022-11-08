#include "CellManager.h"
#include <ctime>
#include <iostream>
void CellManager::initGraphics(s2d::S2DGraphics* graphics) {
	m_graphics = graphics;
}

void CellManager::initCells(int x, int y) {

	m_cellCol = x; 
	m_cellRow = y;
	srand(time(0));
	m_cells = new Cell[x * y];

	for (int row = 0; row < y; row++) {
		for (int col = 0; col < x; col++) {
			
			m_cells[col + row * m_cellCol] = { CELL_TYPE::OPEN, 99999.0f,0,0, col, row, nullptr };
		}
	}

	int xC = rand() % m_cellCol, yC = rand() % m_cellRow;
	

	m_cells[xC + yC * m_cellCol].type = CELL_TYPE::START;
	m_start = &m_cells[xC + yC * m_cellCol];
	xC = rand() % m_cellCol, yC = rand() % m_cellRow;
	m_end = &m_cells[xC + yC * m_cellCol];
	m_cells[xC + yC * m_cellCol].type = CELL_TYPE::END;
}

void CellManager::drawCells(s2d::S2DGraphics* g, int w, int h) {
	
	float cellW = w / m_cellCol, cellH = h / m_cellRow;

	for (int y = 0; y < m_cellRow; y++) {
		for (int x = 0; x < m_cellCol; x++) {

			Cell tempCell = m_cells[x + y * m_cellCol];
			s2d::S2D_COLOR color = s2d::PXLCLR_WHITE;

			switch (tempCell.type) {

			case CELL_TYPE::OPEN:
				color = {165,165,165,255};
				break;
			case CELL_TYPE::CLOSE:
				color = s2d::PXLCLR_BLACK;
				break;
			case CELL_TYPE::START:

				if (m_currentClickedCell != nullptr) {
					if (m_currentClickedCell->type == CELL_TYPE::START) {
						color = s2d::PXLCLR_BLUE;
					}
					else {
						color = { 0, 0, 128, 255 };
					}
				}
				else {
					color = { 0, 0, 128, 255 };
				}
				
				break;
			case CELL_TYPE::END:
				if (m_currentClickedCell != nullptr) {
					if (m_currentClickedCell->type == CELL_TYPE::END) {
						color = s2d::PXLCLR_RED;
					}
					else {
						color = { 128, 0, 0, 255 };
					}
				}
				else {
					color = { 128, 0, 0, 255 };
				}
				break;
			case CELL_TYPE::PATH:
				color = { 125, 125, 125, 255 };
				break;

			case CELL_TYPE::VISITED:
				color = { 125, 125, 160, 255 };
				break;
			
			default:
				color = s2d::PXLCLR_BLACK;
				break;
			}
			g->drawRect(x * cellW, y * cellH, cellW, cellH, color);

		}
	}


}

void CellManager::resetCell() {

	m_open.clear();
	m_close.clear();

	for (int row = 0; row < m_cellRow; row++) {
		for (int col = 0; col < m_cellCol; col++) {
			
			Cell c = m_cells[col + row * m_cellCol];
			if (c.type == CELL_TYPE::START || c.type == CELL_TYPE::END) {
				continue;
			}

			m_cells[col + row * m_cellCol] = { CELL_TYPE::OPEN, 99999.0f,0,0, col, row, nullptr };
		}
	}


}

bool isAlreadyInCloseList(std::vector<Cell*>& closeList, Cell* cell) {

	for (auto ptr : closeList) {
		if (ptr == cell)return true;
	}
	return false;
}

bool isAlreadyInOpenList(std::vector<Cell*>& openList, Cell* cell) {

	for (auto ptr : openList) {
		if (ptr == cell)return true;
	}
	return false;
}

void CellManager::calculateNeigbor(Cell* tile) {

	int x = tile->x, y = tile->y;

	std::vector<Cell*> neighbor;

	if (y < m_cellRow - 1) {
		neighbor.push_back(&m_cells[x + (y + 1) * m_cellCol]);
	}

	if (y > 0) {
		neighbor.push_back(&m_cells[x + (y - 1) * m_cellCol]);
	}
	if (x < m_cellCol - 1) {
		neighbor.push_back(&m_cells[(x + 1) + y * m_cellCol]);
	}
	if (x > 0) {
		neighbor.push_back(&m_cells[(x - 1) + y * m_cellCol]);
	}


	if (y < m_cellRow - 1 && x < m_cellCol - 1) {
		neighbor.push_back(&m_cells[(x + 1) + (y + 1) * m_cellCol]);
	}

	if (y < m_cellRow - 1 && x > 0) {
		neighbor.push_back(&m_cells[(x - 1) + (y + 1) * m_cellCol]);
	}

	if (y > 0 && x > 0) {
		neighbor.push_back(&m_cells[(x - 1) + (y - 1) * m_cellCol]);
	}

	if (y > 0 && x < m_cellCol - 1) {
		neighbor.push_back(&m_cells[(x + 1) + (y - 1) * m_cellCol]);
	}


	for (int i = 0; i < neighbor.size(); i++) {
		
		
		if (neighbor.at(i)->type == CELL_TYPE::CLOSE)continue;
		
		
		
		if (isAlreadyInCloseList(m_close,neighbor[i])) { continue; }

		
		neighbor.at(i)->parent = tile;
		neighbor.at(i)->gCost = getGCost(neighbor[i]);
		neighbor.at(i)->hCost = getHeuristic(neighbor[i]->x, neighbor[i]->y);
		neighbor.at(i)->fCost = neighbor.at(i)->gCost + neighbor.at(i)->hCost;
		

		if (!isAlreadyInOpenList(m_open,neighbor[i])) {
			
			

			m_open.push_back(neighbor[i]);
		}
	}


}

Cell* CellManager::getLowestFCost() {

	if (m_open.empty())return nullptr;
	std::sort(m_open.begin(), m_open.end(), [](Cell* f, Cell* t) {
		return f->fCost < t->fCost;
		}
	);

	return m_open[0];
}



void CellManager::executeAStar() {



	Cell* currentCell = m_start;
	
	m_open.push_back(m_start);
	calculateNeigbor(currentCell);
	int counter = 0;

	while (!m_open.empty() && currentCell != m_end) {

		currentCell = getLowestFCost();
		m_open.erase(std::remove(m_open.begin(), m_open.end(), currentCell));

		if (!isAlreadyInCloseList(m_close, currentCell)) {
			m_close.push_back(currentCell);
		}

		if (currentCell == m_end) {

			break;
		}
		calculateNeigbor(currentCell);

		
	}

	Cell* temp = m_end->parent;

	while (temp != m_start && temp != nullptr) {
		
		temp->type = CELL_TYPE::PATH;
		temp = temp->parent;
	}



}

void CellManager::clickCell(float x, float y, float w, float h) {


	float fOffSetX = w / m_cellCol, fOffSetY = h / m_cellRow;
	int xPos = x / fOffSetX, yPos = y / fOffSetY;
	Cell* ptr = &m_cells[xPos + yPos * m_cellCol];




	if (m_currentClickedCell != nullptr) {

		if (ptr->type != CELL_TYPE::OPEN) {
			m_currentClickedCell = nullptr;
			return;
		}
		

		ptr->type = m_currentClickedCell->type;
		m_currentClickedCell->type = CELL_TYPE::OPEN;
		
		switch (ptr->type)
		{
		case CELL_TYPE::START:
			m_start = ptr;
			break;
		case CELL_TYPE::END:
			m_end = ptr;
			break;
		
		default:

			break;
		}
		m_currentClickedCell = nullptr;
		return;
	}





	if (ptr->type == CELL_TYPE::OPEN) {
		ptr->type = CELL_TYPE::CLOSE;
	}
	else if (ptr->type == CELL_TYPE::CLOSE) {
		ptr->type = CELL_TYPE::OPEN;
	}
	else if (ptr->type == CELL_TYPE::START || ptr->type == CELL_TYPE::END) {
		m_currentClickedCell = ptr;
	}
	 





}

void CellManager::moveStartLoc(float msX, float msY, float w, float h) {

	float fOffSetX = w / m_cellCol, fOffSetY = h / m_cellRow;
	int xPos = msX / fOffSetX, yPos = msY / fOffSetY;
	Cell* ptr = &m_cells[xPos + yPos * m_cellCol];

	if (ptr->type == CELL_TYPE::OPEN) {
		m_start->type = CELL_TYPE::OPEN;
		ptr->type = CELL_TYPE::START;
		m_start = ptr;
	}

}

void CellManager::moveEndLoc(float msX, float msY, float w, float h) {

	float fOffSetX = w / m_cellCol, fOffSetY = h / m_cellRow;
	int xPos = msX / fOffSetX, yPos = msY / fOffSetY;
	Cell* ptr = &m_cells[xPos + yPos * m_cellCol];

	if (ptr->type == CELL_TYPE::OPEN) {
		m_end->type = CELL_TYPE::OPEN;
		ptr->type = CELL_TYPE::END;
		m_end = ptr;
	}

}

Cell* CellManager::getLowestFCostFromNeighbor(Cell* tile) {
	int x = tile->x, y = tile->y;

	std::vector<Cell*> neighbor;

	if (y < 19 - 1) {
		neighbor.push_back(&m_cells[x + (y + 1) * m_cellCol]);
		
	}

	if (y > 0) {
		neighbor.push_back(&m_cells[x + (y - 1) * m_cellCol]);
	}
	if (x < 19 - 1) {
		neighbor.push_back(&m_cells[(x + 1) + y * m_cellCol]);
	}
	if (x > 0) {
		neighbor.push_back(&m_cells[(x - 1) + y * m_cellCol]);
	}


	if (y < 19 - 1 && x < 19 - 1) {
		neighbor.push_back(&m_cells[(x + 1) + (y + 1) * m_cellCol]);
	}

	if (y < 19 - 1 && x > 0) {
		neighbor.push_back(&m_cells[(x - 1) + (y + 1) * m_cellCol]);
	}

	if (y > 0 && x > 0) {
		neighbor.push_back(&m_cells[(x - 1) + (y - 1) * m_cellCol]);
	}

	if (y > 0 && x < 19 - 1) {
		neighbor.push_back(&m_cells[(x + 1) + (y - 1) * m_cellCol]);
	}


	if (neighbor.empty())return nullptr;
	std::sort(neighbor.begin(), neighbor.end(), [](Cell* f, Cell* t) {
		return f->fCost < t->fCost;
		}
	);

	return neighbor[0];
}



float CellManager::getHeuristic(int x, int y) {

	int x2 = m_end->x, y2 = m_end->y;
	return sqrtf( pow((float)(x2 - x), 2) + pow((float)(y2 - y), 2)); // euclidian distance
}

float CellManager::getGCost(Cell* from) {


	int x = from->x, y = from->y;
	float parentGcost = 0;
	if (from->parent != nullptr) parentGcost = from->parent->gCost;


	int x2 = m_end->x, y2 = m_end->y;
	return sqrtf(pow((float)(x2 - x), 2) + pow((float)(y2 - y), 2)) + parentGcost; // euclidian distance
}
