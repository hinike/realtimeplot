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

#include "realtimeplot/eventhandler.h"
#include "realtimeplot/plot.h"
#include "realtimeplot/backend.h"

namespace realtimeplot {

	EventHandler::EventHandler()
		: processing_events( true ),
		window_closed( false ),
		event_queue( 1000 )
	{
		//start processing thread
		pEventProcessingThrd = boost::shared_ptr<boost::thread>( 
				new boost::thread( boost::bind( 
						&realtimeplot::EventHandler::process_events, this ) ) );
	}

	EventHandler::~EventHandler() {
		if (pEventProcessingThrd->joinable())
			pEventProcessingThrd->join();
	}

	void EventHandler::add_event( boost::shared_ptr<Event> pEvent, 
			bool high_priority ) {
		event_queue.push( pEvent, high_priority );
	}

	int EventHandler::get_queue_size() {
		return event_queue.size();
	}

	void EventHandler::process_events() {
		//Ideally event queue would have a blocking get function
		while ( processing_events || !window_closed ) {
			boost::shared_ptr<Event> pEvent = event_queue.pop();
			pEvent->execute( pBPlot );
			if (get_queue_size() == 0) {
				if (pBPlot != nullptr) {
					pBPlot->display();
				}
			}

			// After the window has been closed we want to stop
			// as soon as processing_events = false (FinalEvent has
			// been sent)
			/*if (!window_closed && pBPlot != NULL) {
				window_closed = true;
			}*/
		}
		// Make sure we let pBPlot go/freed.
		if (pBPlot != nullptr)
			pBPlot.reset();
	}
}

