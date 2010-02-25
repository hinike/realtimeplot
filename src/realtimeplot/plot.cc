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
#include "realtimeplot/events.h"

namespace realtimeplot {

    Color::Color() {
        r = 1;
        g = 1;
        b = 1;
        a = 0;
    }

    Color::Color( double red, double green, double blue, double alpha ) {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    Color Color::black() {
        return Color( 0, 0, 0, 1 );
    }

    Color Color::white() {
        return Color( 1, 1, 1, 1 );
    }

    Color Color::red() {
        return Color( 1, 0, 0, 1 );
    }

    Color Color::green() {
        return Color( 0, 1, 0, 1 );
    }

    Color Color::blue() {
        return Color( 0, 0, 1, 1 );
    }

	Plot::Plot() {
		config = PlotConfig();
		pEventHandler = new EventHandler( config );
	}

	Plot::Plot( PlotConfig conf ) {
        config = conf;
		pEventHandler = new EventHandler( config );
	}

	Plot::~Plot() {
		//To get non persistent plot uncomment the following:
		//pEventHandler->plot_closed();
		delete pEventHandler;
	}

    
    void Plot::point( float x, float y ) {
        pEventHandler->add_event( new PointEvent(x, y) ); 
    }

    /**
     * \brief Function to plot a point of a specific color
     *
     * Default color of the point is black.
     * We send a vector of the events needed (set the color, plot the point
     * and restore color to its former value). A vector is used to make sure
     * that these events are processed directly following each other (thread
     * safety).
     */
    void Plot::point( float x, float y, Color color ) {
        std::vector<Event*> events(3);
        events[0] = new SetColorEvent( color );
        events[1] = new PointEvent( x, y );
        events[2] = new RestoreEvent();
        pEventHandler->add_event( new MultipleEvents( events ));
    }

	void Plot::line_add( float x, float y, int id ) {
		Event *pEvent = new LineAddEvent( x, y, id );
		pEventHandler->add_event( pEvent );
	}

	void Plot::number( float x, float y, float i ) {
		Event *pEvent = new NumberEvent( x, y, i );
		pEventHandler->add_event( pEvent );
	}

	void Plot::save( std::string filename ) {
		Event *pEvent = new SaveEvent( filename );
		pEventHandler->add_event( pEvent );
	}

	void Plot::clear() {
		Event *pEvent = new ClearEvent();
		pEventHandler->add_event( pEvent );
	}

    void Plot::update_config() {
		Event *pEvent = new ConfigEvent( config );
		pEventHandler->add_event( pEvent );
    }

	/*
	 * Histogram
	 */

	Histogram::Histogram() {
		no_bins = 4;
		max_y = 0;
	}

	Histogram::~Histogram() {
		delete pHistogram;
	}

	void Histogram::set_data( std::vector<double> the_data ) {
		data = the_data;
		fill_bins();
		plot();
	}

	void Histogram::fill_bins() {
		sort( data.begin(), data.end() );
	
		bins_x.clear();
		bins_y.clear();
		max_y = 0;

		bin_width = (data.back()-data.front())/(no_bins-1);
		for (int i=0; i<no_bins; ++i) {
			bins_x.push_back( data.front()+i*bin_width );
			bins_y.push_back( 0 );
		}

		int current_bin = 0;
		//should use iterator
		for (unsigned int i=0; i<data.size(); ++i) {
			while (data[i] > bins_x[current_bin]+0.5*bin_width) {
				++current_bin;
			}
			++bins_y[current_bin];
			if (bins_y[current_bin]>max_y)
				max_y = bins_y[current_bin];
		}
	}

	void Histogram::set_counts_data( std::vector<double> values,
			std::vector<int> counts ) {
		data.clear();
		for (unsigned int i=0;i<values.size();++i) {
			for (int j=0;j<counts[i];++j) {
				data.push_back( values[i] );
			}
		}
		fill_bins();
		plot();
	}

	void Histogram::plot() {
		config = PlotConfig();
		config.min_x = bins_x.front()-bin_width;
		config.max_x = bins_x.back()+bin_width;
		config.max_y = 1.1*max_y;
		pHistogram = new Plot( config );
		for (unsigned int i=0; i<bins_x.size(); ++i) {
			pHistogram->line_add( bins_x[i]-0.5*bin_width, 0, -1 );
			pHistogram->line_add( bins_x[i]-0.5*bin_width, bins_y[i], -1 );
			pHistogram->line_add( bins_x[i]+0.5*bin_width, bins_y[i], -1 );
			pHistogram->line_add( bins_x[i]+0.5*bin_width, 0, -1 );
		}
	}

}
