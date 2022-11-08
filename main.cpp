/*
	Author: Lloyd Jay Edradan
	Date: 10/18/2022
*/


#include "CellManager.h"
#include <iostream>


class AStarAlgorithm {

private:

	s2d::S2DGraphics* m_graphics;
	s2d::S2DWindow* m_window;
	unsigned int m_width, m_height; // window width and height

	CellManager m_cellManager;

public:
	AStarAlgorithm() {
		initGraphics();
	}

	void processInput() {

		s2d::MouseState ms = m_window->GetMouseState();
		s2d::KeyState ks = m_window->GetKeyState();

		

		if ( ms.state == S2D_MOUSE_CLICK || ms.state == S2D_MOUSE_HELD && ms.mouse == S2D_MOUSE_LEFT) {

			m_cellManager.clickCell(ms.pos.x, ms.pos.y, m_width, m_height);
			

			
		}

		


		if (ks.state == S2D_KEY_PRESSED) {

			switch (ks.key) {

			case VK_SPACE:
				m_cellManager.executeAStar();
				break;
			case VK_ESCAPE:
				m_cellManager.resetCell();
				break;
			default:
				break;
			}

		}
		





	}


	void initGraphics() {
		m_width = 800, m_height = 800;
		m_window = new s2d::S2DWindow(m_width, m_height, "A Star Algorithm - LydianJay", S2D_WINDOW_NO_RESIZE);
		m_graphics = new s2d::S2DGraphics(*m_window);
		
		m_cellManager.initGraphics(m_graphics);
		m_cellManager.initCells(20, 20);
		//m_cellManager.executeAStar();
	}


	void update() {


		while (!m_window->WindowShouldClose()) {
			processInput();
			m_cellManager.drawCells(m_graphics, m_width, m_height);

			m_graphics->flushBuffer();
			m_window->ProcessMessageB();
			m_window->SwapWindowBuffers();
		}


	}


};






int main() {


	AStarAlgorithm program;
	program.update();
	







	return 0;
}