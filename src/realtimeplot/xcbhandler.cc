#include "realtimeplot/xcbhandler.h"

#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_event.h>
#include <X11/keysym.h>

#include "realtimeplot/events.h"

namespace realtimeplot {
	DisplayHandler* XcbHandler::pInstance = 0;
	DisplayHandler* XcbHandler::Instance() {
		boost::mutex::scoped_lock(i_mutex);
		if (pInstance == 0) {
			pInstance = new XcbHandler();
		}
		return pInstance;
	}

	void DisplayHandler::send_event( void* window_id, boost::shared_ptr<Event> pEvent ) {
		map_mutex.lock();
		mapWindow[window_id]->add_event( pEvent, true ); 
		map_mutex.unlock();
	}


	void* XcbHandler::open_window(size_t width, size_t height,
			boost::shared_ptr<EventHandler> pEventHandler ) {
		xcb_drawable_t win;

		win = xcb_generate_id(connection);
		xcb_create_window(connection,XCB_COPY_FROM_PARENT,win,
				screen->root,0,0,width,height,0,
				XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);

		map_mutex.lock();
		mapWindow[(void*) win] = pEventHandler;
		map_mutex.unlock();

		xcb_change_property(connection, XCB_PROP_MODE_REPLACE, win, reply->atom, 4, 32, 1,
				&reply2->atom);

		xcb_map_window(connection,win);

		//xcb_flush(connection);

		return (void *)win;
	}

	XcbHandler::XcbHandler() : DisplayHandler() 
	{
		connection = xcb_connect(NULL,NULL);
		screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

		visual_type = get_root_visual_type( screen );

		mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		values[0] = screen->white_pixel;
		values[1] = //XCB_EVENT_MASK_NO_EVENT |
			XCB_EVENT_MASK_KEY_PRESS |
			//XCB_EVENT_MASK_KEY_RELEASE |
			//XCB_EVENT_MASK_BUTTON_PRESS |
			//XCB_EVENT_MASK_BUTTON_RELEASE |
			//XCB_EVENT_MASK_ENTER_WINDOW |
			//XCB_EVENT_MASK_LEAVE_WINDOW |
			//XCB_EVENT_MASK_POINTER_MOTION |
			//XCB_EVENT_MASK_POINTER_MOTION_HINT |
			//XCB_EVENT_MASK_BUTTON_1_MOTION |
			//XCB_EVENT_MASK_BUTTON_2_MOTION |
			//XCB_EVENT_MASK_BUTTON_3_MOTION |
			//XCB_EVENT_MASK_BUTTON_4_MOTION |
			//XCB_EVENT_MASK_BUTTON_5_MOTION |
			//XCB_EVENT_MASK_BUTTON_MOTION |
			//XCB_EVENT_MASK_KEYMAP_STATE |
			XCB_EVENT_MASK_EXPOSURE |
			//XCB_EVENT_MASK_VISIBILITY_CHANGE |
			XCB_EVENT_MASK_STRUCTURE_NOTIFY |
			//XCB_EVENT_MASK_RESIZE_REDIRECT |
			//XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
			//XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
			XCB_EVENT_MASK_FOCUS_CHANGE;
			//| XCB_EVENT_MASK_PROPERTY_CHANGE |
			//XCB_EVENT_MASK_COLOR_MAP_CHANGE |
			//XCB_EVENT_MASK_OWNER_GRAB_BUTTON;
		

		xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12,
				"WM_PROTOCOLS");
		reply = xcb_intern_atom_reply(connection, cookie, 0);
		xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16,
				"WM_DELETE_WINDOW");
		reply2 = xcb_intern_atom_reply(connection, cookie2, 0);

		pXEventProcessingThrd = boost::shared_ptr<boost::thread>( 
				new boost::thread( boost::bind( 
						&realtimeplot::XcbHandler::process_xevents, this ) ) );

	}

	void XcbHandler::process_xevents() {
		xcb_generic_event_t *event;

		while (event = xcb_wait_for_event (connection)) {
			switch(XCB_EVENT_RESPONSE_TYPE(event)) {
				case XCB_CLIENT_MESSAGE:
					xcb_client_message_event_t* msg;
					msg = (xcb_client_message_event_t *)event;
					if(msg->data.data32[0] ==
							reply2->atom)
					{
						send_event( (void*) msg->window, boost::shared_ptr<Event>( 
									new CloseWindowEvent() ) ); 
					}
					break;
				case XCB_UNMAP_WINDOW:
					break;
				case XCB_CONFIGURE_NOTIFY:
					xcb_configure_notify_event_t *conf;
					conf = (xcb_configure_notify_event_t *)event;
					send_event( (void*) conf->window, boost::shared_ptr<Event>( 
								new ScaleXSurfaceEvent( conf->width, conf->height ) ) ); 
					break;
				case XCB_EXPOSE:
					//display();
					break;
				case XCB_KEY_PRESS:
					/* Handle the Key Press event type */
					xcb_key_press_event_t *ev;
					ev = (xcb_key_press_event_t *)event;
					xcb_keysym_t key;
					key = xcb_key_symbols_get_keysym(xcb_key_symbols_alloc(connection),ev->detail,0);
					if (key == XK_space)  {
						send_event( (void*) conf->window, boost::shared_ptr<Event>( 
								new PauseEvent() ) ); 
					}
					else if (key == XK_w)  {
						send_event( (void*) conf->window, boost::shared_ptr<Event>( 
									new SaveEvent( "realtimeplot.png" ) ) );
					}
					else if (key == XK_Left) {
						send_event( (void*) conf->window, boost::shared_ptr<Event>( 
									new MoveEvent( -1, 0 ) ) );
					} else if (key == XK_Right) {
						send_event( (void*) conf->window, boost::shared_ptr<Event>( 
									new MoveEvent( 1, 0 ) ) );
					} else if (key == XK_Up) {
						send_event( (void*) conf->window, boost::shared_ptr<Event>( 
									new MoveEvent( 0, 1 ) ) );
					} else if (key == XK_Down) {
						send_event( (void*) conf->window, boost::shared_ptr<Event>( 
									new MoveEvent( 0, -1 ) ) );
					} else if (key == XK_KP_Add) { 
						send_event( (void*) conf->window, boost::shared_ptr<Event>( 
									new ZoomEvent( 0.95 ) ) );
					} else if (key == XK_KP_Subtract) { 
						send_event( (void*) conf->window, boost::shared_ptr<Event>( 
									new ZoomEvent( 1.05 ) ) );
					}
					break;	
				default:
					break;
			}
		}
		free(event);
	}
	
	Cairo::RefPtr<Cairo::Surface> XcbHandler::get_cairo_surface( void* window_id, 
			size_t width, size_t height ) {
		Cairo::XcbSurface::create( connection, *(xcb_drawable_t*) window_id, 
				visual_type, width, height );
	}


	void XcbHandler::set_title( void* win, std::string title ) {
		xcb_change_property_checked (connection, XCB_PROP_MODE_REPLACE, 
				*(xcb_drawable_t*) win,
				XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
				title.length(), title.c_str());
	}

	void XcbHandler::close_window( void* win ) {
			xcb_unmap_window( connection, *(xcb_drawable_t*) win );
			xcb_destroy_window( connection, *(xcb_drawable_t*) win );
			xcb_flush(connection);
	}

	xcb_visualtype_t *XcbHandler::get_root_visual_type(xcb_screen_t *s)
	{
		xcb_visualid_t root_visual;
		xcb_visualtype_t  *visual_type = NULL;
		xcb_depth_iterator_t depth_iter;

		depth_iter = xcb_screen_allowed_depths_iterator(s);

		for(;depth_iter.rem;xcb_depth_next(&depth_iter)) {
			xcb_visualtype_iterator_t visual_iter;

			visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
			for(;visual_iter.rem;xcb_visualtype_next(&visual_iter)) {
				if(s->root_visual == visual_iter.data->visual_id) {
					visual_type = visual_iter.data;
					break;
				}
			}
		}

		return visual_type;
	}

};
