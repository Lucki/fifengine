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

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/layer.h"
#include "pathfinder/searchspace.h"
#include "util/fife_math.h"

#include "routepathersearch.h"

namespace FIFE {
	RoutePatherSearch::RoutePatherSearch(const int session_id, const Location& from, const Location& to, SearchSpace* searchSpace)
	: Search(session_id, from, to, searchSpace) {
		int coord = m_searchspace->convertCoordToInt(from.getLayerCoordinates());
		//int dest = m_searchspace->convertCoordToInt(to.getLayerCoordinates());
		int max_index = m_searchspace->getMaxIndex();
		m_sortedfrontier.pushElement(PriorityQueue<int, float>::value_type(coord, 0.0f));
		m_spt.resize(max_index + 1, -1);
		m_sf.resize(max_index + 1, -1);
		m_gCosts.resize(max_index + 1, 0.0f);
	}

	//TODO: Tidy up this function.
	std::vector<Location> RoutePatherSearch::updateSearch() {
		if(m_sortedfrontier.empty()) {
			setSearchStatus(search_status_failed);
			return std::vector<Location>();
		}
		PriorityQueue<int, float>::value_type topvalue = m_sortedfrontier.getPriorityElement();
		m_sortedfrontier.popElement();
		int next = topvalue.first;
		m_spt[next] = m_sf[next];
		ModelCoordinate destCoord = m_to.getLayerCoordinates();
		int destcoordInt = m_searchspace->convertCoordToInt(destCoord);
		if(destcoordInt == next) {
			m_status = search_status_complete;
			return calcPath();
		}
		//use destination layer for getting the cell coordinates for now, this should be moved
		//into search space.
		ModelCoordinate nextCoord = m_searchspace->convertIntToCoord(next);
		std::vector<ModelCoordinate> adjacents;
		m_searchspace->getLayer()->getCellGrid()->getAccessibleCoordinates(nextCoord, adjacents);
		for(std::vector<ModelCoordinate>::iterator i = adjacents.begin(); i != adjacents.end(); ++i) {
			//first determine if coordinate is in search space.
			Location loc = m_to;
			loc.setLayerCoordinates((*i));
			if(m_searchspace->isInSearchSpace(loc)) {
				float hCost = fabs((float)((destCoord.x - i->x) + (destCoord.y - i->y)) * 0.01f);
				float gCost = m_gCosts[next] + loc.getLayer()->getCellGrid()->getAdjacentCost(nextCoord, (*i));
				int adjacentInt = m_searchspace->convertCoordToInt((*i));
				if(m_sf[adjacentInt] == -1) {
					m_sortedfrontier.pushElement(PriorityQueue<int, float>::value_type(adjacentInt, gCost + hCost));
					m_gCosts[adjacentInt] = gCost;
					m_sf[adjacentInt] = next;
				}
				else if(gCost < m_gCosts[adjacentInt] && m_spt[adjacentInt] == -1) {
					m_sortedfrontier.changeElementPriority(adjacentInt, gCost + hCost);
					m_gCosts[adjacentInt] = gCost;
					m_sf[adjacentInt] = next;
				}
			} 
		}
		return std::vector<Location>();
	}

	//TODO: This function needs cleaning up too.
	std::vector<Location> RoutePatherSearch::calcPath() {
		int current = m_searchspace->convertCoordToInt(m_to.getLayerCoordinates());
		int end = m_searchspace->convertCoordToInt(m_from.getLayerCoordinates());
		std::vector<Location> path;
		path.push_back(m_to);
		while(current != end) {
			current = m_spt[current];
			Location newnode(m_to);
			ModelCoordinate currentCoord = m_searchspace->convertIntToCoord(current);
			newnode.setLayerCoordinates(currentCoord);
			newnode.setExactLayerCoordinates(intPt2doublePt(currentCoord));
			path.push_back(newnode);
		}
		std::reverse(path.begin(), path.end());
		return path;
	}
}
