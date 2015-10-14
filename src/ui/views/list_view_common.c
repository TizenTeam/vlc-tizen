/*****************************************************************************
 * Copyright © 2015 VideoLAN, VideoLabs SAS
 *****************************************************************************
 *
 * Authors: Hugo Beauzée-Luyssen <hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
/*
 * By committing to this project, you allow VideoLAN and VideoLabs to relicense
 * the code to a different OSI approved license, in case it is required for
 * compatibility with the Store
 *****************************************************************************/

#include "common.h"

#include "list_view_private.h"
#include "controller/media_library_controller.h"

struct list_sys
{
    LIST_VIEW_COMMON
};

static void
list_view_clear(list_sys* p_list)
{
    elm_genlist_clear(p_list->p_list);
    list_view_toggle_empty(p_list, true);
}

static void
list_view_destroy(list_sys* p_list)
{
    media_library_controller_destroy(p_list->p_ctrl);
    elm_genlist_item_class_free(p_list->p_default_item_class);
    free(p_list);
}

static Evas_Object*
list_view_get_widget(list_sys* p_list)
{
    return p_list->p_container;
}

void
list_view_toggle_empty(list_sys* p_list_sys, bool b_empty)
{
    if (p_list_sys->b_empty == b_empty)
        return;
    p_list_sys->b_empty = b_empty;
    Evas_Object* p_list = p_list_sys->p_list;
    if (b_empty)
    {
        elm_box_unpack_all(p_list_sys->p_container);
        elm_box_pack_end(p_list_sys->p_container, p_list_sys->p_empty_label);
        evas_object_show(p_list_sys->p_empty_label);
        evas_object_hide(p_list);
    }
    else
    {
        elm_box_unpack_all(p_list_sys->p_container);
        elm_box_pack_end(p_list_sys->p_container, p_list);
        evas_object_show(p_list);
        evas_object_hide(p_list_sys->p_empty_label);
    }
}

void
list_view_common_setup(list_view* p_view, list_sys* p_list, interface* p_intf, Evas_Object* p_parent)
{
    p_list->p_intf = p_intf;

    /* Container box */
    p_list->p_container = elm_box_add(p_parent);

    /* Empty list label */
    p_list->p_empty_label = elm_label_add(p_list->p_container);
    elm_object_text_set(p_list->p_empty_label, "No content to display");

    /* Create genlist */
    p_list->p_list = elm_genlist_add(p_list->p_container);
    elm_scroller_single_direction_set(p_list->p_list, ELM_SCROLLER_SINGLE_DIRECTION_HARD);
    elm_genlist_homogeneous_set(p_list->p_list, EINA_TRUE);
    elm_genlist_mode_set(p_list->p_list, ELM_LIST_COMPRESS);

    /* Item Class */
    p_list->p_default_item_class = elm_genlist_item_class_new();
    p_list->p_default_item_class->item_style = "2line.top.3";

    /* Setup common callbacks */
    p_view->pf_del = &list_view_destroy;
    p_view->pf_clear = &list_view_clear;
    p_view->pf_get_widget = &list_view_get_widget;

    /* Ensure the initial update takes place (keep in mind that b_empty is 0 initialized) */
    list_view_toggle_empty(p_list, true);
}