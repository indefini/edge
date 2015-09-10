#include "e_mod_edge.h"
#define __UNUSED__

typedef struct _Edge Edge;

struct _Edge
{
	Evas_Object* r;
	E_Zone* zone;
	Eina_Bool down;
	Eina_Bool started;
	Eina_Bool ignore;
	int start;
};

static Edge* edge_new()
{
	Edge* edge = calloc(1, sizeof(*edge));
	return edge;
}

static void edge_free(Edge* edge)
{
	if (edge->r) {
		evas_object_del(edge->r);
	}
}


EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "Edge"
};

static Eina_List *_handlers = NULL;

static Eina_Bool
_e_winlist_cb_mouse_down(void *data, int type __UNUSED__, void *event)
{
	Edge* edge = data;
   Ecore_Event_Mouse_Button *ev;

   ev = event;
   if (ev->buttons == 1) {
   		printf("mouse down  %d, %d \n", ev->x, ev->y);
   		printf("zone stuff  %d \n", edge->zone->w);
		int margin = 5;
	if (ev->x >= edge->zone->w - margin) {
   		printf("!!!!edge mouse down  %d, %d \n", ev->x, ev->y);
		edge->down = true;
		edge->start = ev->x;
	}
   }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_e_winlist_cb_mouse_up(void *data __UNUSED__, int type __UNUSED__, void *event)
{
	Edge* edge = data;
   Ecore_Event_Mouse_Button *ev;

   ev = event;
   if (ev->buttons == 1) {
	   if (edge->down) {
   		printf("edge mouse up  %d, %d \n", ev->x, ev->y);
	   }
	edge->down = false;
	edge->started = false;
	edge->ignore = false;
	evas_object_move(edge->r, edge->zone->w, 0);
   }

   return ECORE_CALLBACK_PASS_ON;
}


static Eina_Bool
_e_winlist_cb_mouse_move(void *data __UNUSED__, int type __UNUSED__, void *event)
{
	Edge* edge = data;
	int x, y, w, h;
	evas_object_geometry_get(edge->r, &x, &y, &w, &h);


   Ecore_Event_Mouse_Move *ev = event;
   if (edge->down && !edge->ignore ) {
	   if (edge->started) {
		   int nx = ev->x;
		   if (edge->zone->w - nx > w) {
			   nx = edge->zone->w - w;
		   }
		   evas_object_move(edge->r, nx, 0);
	   }
	   else {
   			//printf("edge mouse event %d, %d \n", ev->x, ev->y);
			if (ev->x < edge->start ) {
				edge->started = true;
				int nx = ev->x;
		   		if (edge->zone->w - nx > w) {
			   		nx = edge->zone->w - w;
		   		}
				evas_object_move(edge->r, nx, 0);
			}
			else if (ev->x > edge->start ) {
				edge->ignore = true;
			}
	   	}

   }

   return ECORE_CALLBACK_PASS_ON;
}

Evas_Object* add_stuff()
{
	  E_Comp *comp;
      Eina_List *l;

	    E_Zone *zone;
	   Eina_List *zones = NULL;
   		Eina_List *l2;


      EINA_LIST_FOREACH((Eina_List*)e_comp_list(), l, comp)
      {
         EINA_LIST_FOREACH(comp->zones, l2, zone)
         {
            zones = eina_list_append(zones, zone);
            printf("PENGUINS:   Zone: %s - %s || %d,%d @ %dx%d\n",
                   zone->comp->name, zone->name, zone->x, zone->y, zone->w, zone->h);
			Edge* edge = edge_new();
		Evas_Object* r = evas_object_rectangle_add(zone->comp->evas);
        evas_object_layer_set(r, E_LAYER_DESKTOP_TOP + 10);
		evas_object_color_set(r, 55, 50, 50, 255);
		evas_object_move(r, zone->w, 0);
		evas_object_resize(r, 100, zone->h);
		evas_object_show(r);
		edge->r = r;
		edge->zone = zone;
		return edge;
         }
      }

	  return NULL;

}



EAPI void *
e_modapi_init(E_Module *m)
{
   E_Desk *desk;
   Eina_List *l;

   Edge* edge = add_stuff();


   E_LIST_HANDLER_APPEND(_handlers, ECORE_EVENT_MOUSE_BUTTON_DOWN, _e_winlist_cb_mouse_down, edge);
   E_LIST_HANDLER_APPEND(_handlers, ECORE_EVENT_MOUSE_BUTTON_UP, _e_winlist_cb_mouse_up, edge);
   E_LIST_HANDLER_APPEND(_handlers, ECORE_EVENT_MOUSE_MOVE, _e_winlist_cb_mouse_move, edge);


#define HANDLER(_h, _e, _f)                                \
  _h = ecore_event_handler_add(E_EVENT_##_e,               \
                               (Ecore_Event_Handler_Cb)_f, \
                               NULL);

   /*
   _G.handler_client_resize_begin =
      e_client_hook_add(E_CLIENT_HOOK_RESIZE_BEGIN, _resize_begin_hook, NULL);
   _G.handler_client_add =
      e_client_hook_add(E_CLIENT_HOOK_EVAL_PRE_FRAME_ASSIGN, _add_hook, NULL);
   HANDLER(_G.handler_client_resize, CLIENT_RESIZE, _resize_hook);
   HANDLER(_G.handler_client_move, CLIENT_MOVE, _move_hook);

   HANDLER(_G.handler_client_iconify, CLIENT_ICONIFY, _iconify_hook);
   HANDLER(_G.handler_client_uniconify, CLIENT_UNICONIFY, _iconify_hook);

   HANDLER(_G.handler_desk_set, CLIENT_DESK_SET, _desk_set_hook);
   HANDLER(_G.handler_compositor_resize, COMPOSITOR_RESIZE,
           _compositor_resize_hook);
		   */
#undef HANDLER

#define ACTION_ADD(_action, _cb, _title, _value, _params, _example, _editable) \
  {                                                                            \
     const char *_name = _value;                                               \
     if ((_action = e_action_add(_name))) {                                    \
          _action->func.go = _cb;                                              \
          e_action_predef_name_set(N_("Tiling"), _title, _name,                \
                                   _params, _example, _editable);              \
       }                                                                       \
  }

   /* Module's actions */
	/*
   ACTION_ADD(_G.act_togglefloat, _e_mod_action_toggle_floating_cb,
              N_("Toggle floating"), "toggle_floating", NULL, NULL, 0);

   ACTION_ADD(_G.act_move_up, _e_mod_action_move_up_cb,
              N_("Move the focused window up"), "move_up", NULL, NULL, 0);
   ACTION_ADD(_G.act_move_down, _e_mod_action_move_down_cb,
              N_("Move the focused window down"), "move_down", NULL, NULL, 0);
   ACTION_ADD(_G.act_move_left, _e_mod_action_move_left_cb,
              N_("Move the focused window left"), "move_left", NULL, NULL, 0);
   ACTION_ADD(_G.act_move_right, _e_mod_action_move_right_cb,
              N_("Move the focused window right"), "move_right", NULL, NULL, 0);

   ACTION_ADD(_G.act_toggle_split_mode, _e_mod_action_toggle_split_mode,
              N_("Toggle split mode"), "toggle_split_mode", NULL, NULL, 0);

   ACTION_ADD(_G.act_swap_window, NULL, N_("Swap window"), "swap_window", NULL,
              NULL, 0);
   _G.act_swap_window->func.go_mouse = _e_mod_action_swap_window_go_mouse;
   _G.act_swap_window->func.end_mouse = _e_mod_action_swap_window_end_mouse;
   */

#undef ACTION_ADD


   //return m;
   return edge;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
	Edge* edge = m->data;
	edge_free(edge);
	free(edge);
#define SAFE_FREE(x, freefunc) \
   if (x) \
     { \
        freefunc(x); \
        x = NULL; \
     }
#define FREE_HANDLER(x)            \
   SAFE_FREE(x, ecore_event_handler_del);

	/*
   FREE_HANDLER(_G.handler_client_resize);
   FREE_HANDLER(_G.handler_client_move);

   FREE_HANDLER(_G.handler_client_iconify);
   FREE_HANDLER(_G.handler_client_uniconify);

   FREE_HANDLER(_G.handler_desk_set);

   SAFE_FREE(_G.handler_client_resize_begin, e_client_hook_del);
   SAFE_FREE(_G.handler_client_add, e_client_hook_del);
   */
#undef FREE_HANDLER
#undef SAFE_FREE

#define ACTION_DEL(act, title, value)             \
  if (act) {                                      \
       e_action_predef_name_del("Tiling", title); \
       e_action_del(value);                       \
       act = NULL;                                \
    }
	/*
   ACTION_DEL(_G.act_togglefloat, "Toggle floating", "toggle_floating");
   ACTION_DEL(_G.act_move_up, "Move the focused window up", "move_up");
   ACTION_DEL(_G.act_move_down, "Move the focused window down", "move_down");
   ACTION_DEL(_G.act_move_left, "Move the focused window left", "move_left");
   ACTION_DEL(_G.act_move_right, "Move the focused window right", "move_right");

   ACTION_DEL(_G.act_toggle_split_mode, "Toggle split mode",
              "toggle_split_mode");
   ACTION_DEL(_G.act_swap_window, "Swap window", "swap_window");
   */
#undef ACTION_DEL

   return 1;
}

