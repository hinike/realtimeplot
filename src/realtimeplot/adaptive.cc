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

#include "realtimeplot/adaptive.h"

namespace realtimeplot {
	/**
	 * Adaptive Plot
	 */
	BackendAdaptivePlot::BackendAdaptivePlot( PlotConfig conf,
			boost::shared_ptr<EventHandler> pEventHandler )
		: BackendPlot( conf, pEventHandler ), max_data_x( -1 ), max_data_y( -1 ),
		min_data_x( 0 ), min_data_y( 0 ), adapting( true )
	{
		if( config.fixed_plot_area ) {
			adapting = false;
			if (pEventHandler) {
				convert_to_adaptive( pEventHandler )->adaptive = adapting;
			}
		}
	}

	bool BackendAdaptivePlot::within_plot_bounds( float x, float y ) {
		if (adapting && pEventHandler) {
			adapting = convert_to_adaptive( pEventHandler )->adaptive;
		}
		if (adapting) {
			// No previous points have been plotted yet
			if (max_data_x<min_data_x) {
				max_data_x = x;
				min_data_x = x;
				max_data_y = y;
				min_data_y = y;
			}
			if (x > max_data_x)
				max_data_x = x;
			else if (x<min_data_x)
				min_data_x = x;
			if (y > max_data_y)
				max_data_y = y;
			else if (y<min_data_y)
				min_data_y = y;
			adapt();
		}
		return BackendPlot::within_plot_bounds( x, y );
	}

	void BackendAdaptivePlot::reset( PlotConfig conf ) {
		// Check if adaptive was turned off
		if (conf.fixed_plot_area) { 
			adapting = false;
			if (pEventHandler) {
				convert_to_adaptive( pEventHandler )->processed_events.clear();
				convert_to_adaptive( pEventHandler )->adaptive = false;
			}
		}
		// Check if no_adaptive_events was changed
		if (config.no_adaptive_events > conf.no_adaptive_events) {
			if (pEventHandler 
					&& convert_to_adaptive( pEventHandler )->processed_events.size() > 
					conf.no_adaptive_events ) {
				convert_to_adaptive( pEventHandler )->processed_events.clear();
				convert_to_adaptive( pEventHandler )->adaptive = false;
				convert_to_adaptive( pEventHandler )->max_no_events = conf.no_adaptive_events;
			}
		} 
		if (adapting) {
			max_data_x = -1;
			max_data_y = -1;
			min_data_x = 0;
			min_data_y = 0;
			if (pEventHandler) {
				convert_to_adaptive( pEventHandler )->max_no_events = conf.no_adaptive_events;
				convert_to_adaptive( pEventHandler )->processed_events.clear();
			}
		}
		BackendPlot::reset( conf );
	}

	void BackendAdaptivePlot::clear() {
		if (adapting) {
			max_data_x = -1;
			max_data_y = -1;
			min_data_x = 0;
			min_data_y = 0;
			if (pEventHandler) {
				convert_to_adaptive( pEventHandler )->processed_events.clear();
			}
		}
		BackendPlot::clear();
	}


	void BackendAdaptivePlot::adapt() {
		// Only one data point
		bool oldpause = pause_display;
		pause_display = true;
		if (min_data_x == max_data_x) {
			config.max_x = max_data_x + 0.5;
			config.min_x = min_data_x - 0.5;
			config.max_y = max_data_y + 0.5;
			config.min_y = min_data_y - 0.5;
		} else {
			double xrange = max_data_x-min_data_x;
			config.max_x = max_data_x + 0.2*xrange;
			config.min_x = min_data_x - 0.2*xrange;
			double yrange = max_data_y-min_data_y;
			config.max_y = max_data_y + 0.2*yrange;
			config.min_y = min_data_y - 0.2*yrange;
		}
		BackendPlot::reset( config ); // Don't need to reset max_data etc, 
																	// so call parent reset
		if (pEventHandler) {
			adapting = false;
			convert_to_adaptive( pEventHandler )->reprocess();
			adapting = true;
		}
		pause_display = oldpause;
	}

	boost::shared_ptr<AdaptiveEventHandler> BackendAdaptivePlot::convert_to_adaptive( 
			boost::shared_ptr<EventHandler> pEventHandler ) {
		return boost::static_pointer_cast<AdaptiveEventHandler, 
					 EventHandler>( pEventHandler );
	}


	/**
	 * Adaptive EventHandler
	 */
	AdaptiveEventHandler::AdaptiveEventHandler( size_t no_events ) : 
		EventHandler(), adaptive( true ), max_no_events( no_events ) {}

	void AdaptiveEventHandler::reprocess() {
		boost::mutex::scoped_lock lock( proc_mutex );
		std::list<boost::shared_ptr<Event> >::iterator it = processed_events.begin();
		++it; // Skip openplotevent
		if ( processing_events || !window_closed ) {
			for (; it != processed_events.end(); ++it) {
				(*it)->execute( pBPlot );
			}
		}
	}

	void AdaptiveEventHandler::process_events() {
		while ( processing_events || !window_closed ) {
			boost::shared_ptr<Event> pEvent = event_queue.pop();
			if (adaptive ) {
				boost::mutex::scoped_lock lock( proc_mutex );
				if (processed_events.size() < max_no_events)
					processed_events.push_back( pEvent );
				else {
					adaptive = false;
					processed_events.clear();
				}
			}
			pEvent->execute( pBPlot );
			if (get_queue_size() == 0) {
				if (pBPlot != nullptr) {
					pBPlot->display();
				}
			}	
		}
		// Make sure we let pBPlot go/freed.
		if (pBPlot != nullptr)
			pBPlot.reset();
	}

};
