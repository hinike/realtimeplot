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
#ifndef REALTIMEPLOT_EVENTS_H
#define REALTIMEPLOT_EVENTS_H
#include "realtimeplot/eventhandler.h"
#include "realtimeplot/backend.h"


namespace realtimeplot {

    class PlotConfig;
    /**
     * \brief Event that is send when the config gets updated
     */
    class ConfigEvent : public Event {
        public:
            ConfigEvent( PlotConfig new_config );
            virtual void execute( boost::shared_ptr<BackendPlot> bPl );
        private:
            PlotConfig config;
     };

    /**
     * \brief Event that can contain multiple events
     *
     * Should be used when one wants to be certain that the events
     * added are processed directly after each other. For example if
     * one wants to set a color, plot a point and unset the color, 
     * without the danger that another process suddenly plots something
     * else in the mean time.
		 */
    class MultipleEvents : public Event {
        public:
            MultipleEvents( std::vector<boost::shared_ptr<Event> > events );
            virtual void execute( boost::shared_ptr<BackendPlot> bPl );
        private:
            std::vector<boost::shared_ptr<Event> > events;
    };

    /**
     * \brief Sets the color used to draw on the plot
     *
     * Can be followed by a restore event to restore the color to its previous value
     */
    class SetColorEvent : public Event {
        public:
            SetColorEvent( Color color );
            virtual void execute( boost::shared_ptr<BackendPlot> bPl );
        private:
            Color color;
    };

    /**
     * \brief Restores the plot to former (saved) settings
     *
     * Can (for example) be used after SetColorEvent to restore former Color
     */
    class RestoreEvent : public Event {
        public:
            RestoreEvent() {}
            virtual void execute( boost::shared_ptr<BackendPlot> bPl ) {bPl->restore();}
    };

    /**
      \brief Event that draws a point at x, y with Color color
      */
    class PointEvent : public Event {
        public:
            PointEvent( float x, float y );
            virtual void execute( boost::shared_ptr<BackendPlot> bPl );
        private:
            float x_crd, y_crd;
    };

 		/**
		 \brief Event that adds a point to an existing line

		 If no line exists yet a new one will be started with starting point x, y
         @param id can be any int and identifies to which line a point belongs

		 */
     class LineAddEvent : public Event {
        public:
            LineAddEvent( float x, float y, int id, Color color );
            virtual void execute( boost::shared_ptr<BackendPlot> bPl );
        private:
            float x_crd, y_crd;
            int id;
						Color color;
    };

		/**
		 \brief Event that plots a number (float) at the specified x,y coordinate
		 */
    class NumberEvent : public Event {
        public:
            NumberEvent( float x, float y, float i );
            virtual void execute( boost::shared_ptr<BackendPlot> bPl );
        private:
            float x_crd, y_crd, nr;
    };

    /**
      \brief Event to save the current plot to the specified file
      */
    class SaveEvent : public Event {
        public:
            SaveEvent( std::string filename );
            virtual void execute( boost::shared_ptr<BackendPlot> bPl );
        private:
            std::string filename;
    };

		/**
		 \brief Event to clear the current plot
		 */
    class ClearEvent : public Event {
        public:
            ClearEvent();
            virtual void execute( boost::shared_ptr<BackendPlot> bPl );
    };
		/**
		 \brief Event to close the current plot (window)
		 */
    class CloseEvent : public Event {
        public:
            CloseEvent();
            virtual void execute( boost::shared_ptr<BackendPlot> bPl );
    };


 }

#endif

