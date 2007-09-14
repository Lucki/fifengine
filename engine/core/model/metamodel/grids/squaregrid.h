/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_MODEL_GRIDS_SQUAREGRID_H
#define FIFE_MODEL_GRIDS_SQUAREGRID_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "cellgrid.h"

namespace FIFE {
	class SquareGrid: public CellGrid {
	public:
		SquareGrid(bool diagonals_accessible=false);
		virtual ~SquareGrid();

		const std::string& getName() const;
		bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target);
		float getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target);
		unsigned int getCellSideCount() const { return 4; }
		ExactModelCoordinate toElevationCoordinates(const ExactModelCoordinate& layer_coords);
		ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& elevation_coord);
		ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& elevation_coord);
		void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell);

	private:
		bool isAccessibleDiagonal(const ModelCoordinate& curpos, const ModelCoordinate& target);
		bool m_diagonals_accessible;
	};
}

#endif
