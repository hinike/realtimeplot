/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, Edwin van Leeuwen

	 This file is part of RealTimePlot.

	 RealTimePlot is free software; you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation; either version 3 of the License, or
	 (at your option) any later version.

	 RealTimePlot is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License
	 along with RealTimePlot. If not, see <http://www.gnu.org/licenses/>.

	 -------------------------------------------------------------------
	 */

#include "realtimeplot/plot.h"

using namespace realtimeplot;

int main() {
	HeightMap hm = HeightMap();

	for (size_t i=0; i<20; ++i) {
		hm.add_data( 8*float(std::rand())/RAND_MAX, 50*float(std::rand())/RAND_MAX, 1, false );
	}
	hm.add_data( 8*float(std::rand())/RAND_MAX, 50*float(std::rand())/RAND_MAX, 1 );

	/*hm.add_data( 1.0, 1.2, 1 );
	hm.add_data( 5.1, 20, 1 );
	hm.add_data( 9.1, 2.2, 1 );
	hm.add_data( 5, 15, 1 );*/

	return 0;
}