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
#ifndef DELAUNAY_H
#define DELAUNAY_H
#include <iostream>
#include <ostream>
#include <vector>
#include <boost/shared_ptr.hpp>

class TestDelaunay;

namespace realtimeplot {
	namespace delaunay {
		/*
		 * This implementation follows the "corner method" as described in:
		 * Venkates and Lee, An improved incremental Delaunay Triangulation Algorithm
		 */
		class Vertex {
			public:
				float x, y;
				Vertex( float x, float y ) :
					x(x), y(y)
			{}
				bool operator==( Vertex &v ) {
					if (x == v.x && y == v.y)
						return true;
					else
						return false;
				}

				Vertex scalar( float s ) {
					Vertex v = Vertex( s*x, s*y );
					return v;
				}

				Vertex operator+( Vertex v ) {
					return Vertex( x+v.x, y+v.y );
				}
				Vertex operator-( Vertex &v ) {
					return Vertex( x-v.x, y-v.y );
				}
				float dot(Vertex &v) {
					return x*v.x+y*v.y;
				}
		};

		class Edge {
			public:
				boost::shared_ptr<Vertex> pV0;
				boost::shared_ptr<Vertex> pV1;
				Edge( boost::shared_ptr<Vertex> pv0, boost::shared_ptr<Vertex> pv1 ) {
					if (pv0->x <= pv1->x) {
						pV0 = pv0;
						pV1 = pv1;
					} else {
						pV0 = pv1;
						pV1 = pv0;
					}
				}
				bool intersect( Edge& e );
				bool include( Vertex &v );
				Vertex intersectionVertex( Edge& e );
		};

		class Corner;
		class Triangle {
			public:
				std::vector<boost::shared_ptr<Corner> > corners;

				/**
				 * \brief Check if the point is in this triangle
				 */
				bool inTriangle( boost::shared_ptr<Vertex> pV );

				bool inCircumCircle( boost::shared_ptr<Vertex> pV ); 

				/**
				 * \brief Returns a vertex corresponding to the center of the triangle
				 */
				Vertex center();
			protected:
				/**
				 * \brief Returns a vertex corresponding to the center of the triangle given two edges
				 *
				 * Intermediate method, introduced to minimize code duplication
				 */
				Vertex centerGivenTwoEdges( Vertex &v1, Vertex &v2 );
			};

		class Corner {
			public:
				boost::shared_ptr<Vertex> vertex;
				boost::shared_ptr<Corner> next;
				boost::shared_ptr<Corner> previous;
				boost::shared_ptr<Corner> opposite;
				boost::shared_ptr<Triangle> triangle;
				Corner() {};
		};

		class Delaunay {
			public:
				Delaunay() {};
				Delaunay( float xmin, float xmax, float ymin, float ymax );

				void add_data( boost::shared_ptr<Vertex> vertex );

				boost::shared_ptr<Triangle>
					findTriangle( boost::shared_ptr<Vertex> vertex,
						boost::shared_ptr<Triangle> tr );

				void createNewTriangles( boost::shared_ptr<Vertex> vertex,
						boost::shared_ptr<Triangle> triangle );

				/**
				 * \brief Method that checks if edges need to be flipped and then flips them
				 *
				 * Method is recursive, so that new created triangles cause the method
				 * to be called again
				 *
				 * pC is the new vertex, so we only check if the pC->opposite is inside
				 * the circumCircle. See "An improved incremental Delaunay Triangulation
				 * Algorithm for details".
				 *
				 * The count is there to help detecting infinite loops (caused by numerical
				 * inprecision
				 */
				void flipEdgesRecursively( boost::shared_ptr<Corner> pC, size_t count = 0 );
				std::vector<boost::shared_ptr<Vertex> > vertices;
				std::vector<boost::shared_ptr<Triangle> > triangles;
				std::vector<boost::shared_ptr<Corner> > corners;
			protected:
				/**
				 * \brief Set the super triangle that encompasses a rectangle
				 *
				 * This method is normally only used internally
				 */
				void setSuperTriangle( float min_x, float max_x, float min_y, float max_y ); 	
		};
	};
};
std::ostream & operator<<(std::ostream &out,
		const realtimeplot::delaunay::Vertex &v ); 
std::ostream & operator<<(std::ostream &out,
		const realtimeplot::delaunay::Edge &e ); 
std::ostream & operator<<(std::ostream &out,
		const realtimeplot::delaunay::Triangle &t );
#endif
