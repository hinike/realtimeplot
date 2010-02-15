/*
  -------------------------------------------------------------------
  
  Copyright (C) 2010, Edwin van Leeuwen
  
  This file is part of CairoPlot.
  
  CairoPlot is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  CairoPlot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with CairoPlot. If not, see <http://www.gnu.org/licenses/>.

  -------------------------------------------------------------------
*/

#include "cairo_plot/plot.h"

using namespace cairo_plot;

int main() {
	std::vector<double> data;
	for (int i=0; i<10; ++i) {
		data.push_back( 1 );
		data.push_back( 2 );
		data.push_back( 2.1 );
	}

	Histogram hist = Histogram();
	hist.set_data( data );

	sleep( 1 );

	std::vector<double> values;
	std::vector<int> counts;
	values.push_back( 1 );
	values.push_back( 2 );
	values.push_back( 2.1 );
	counts.push_back( 15 );
	counts.push_back( 10 );
	counts.push_back( 10 );

	hist.set_counts_data( values, counts );
	sleep( 2 );
}
