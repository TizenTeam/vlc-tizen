/*****************************************************************************
 * Copyright © 2015 VideoLAN, VideoLabs SAS
 *****************************************************************************
 *
 * Authors: Nicolas Rechatin [nicolas@videolabs.io]
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

#include "media_storage.h"

#include <app.h>
#include <efl_extension.h>
#include <storage.h>
#include <system_settings.h>

#include "interface.h"
#include "sidebar.h"
#include "audio_player.h"
#include "menu/main_popup_list.h"

#include "views/audio_view.h"
#include "views/video_view.h"
#include "views/directory_view.h"
#include "views/settings_view.h"
#include "views/about_view.h"

struct interface {
    Evas_Object *win;
    Evas_Object *conform;

    Evas_Object *nf_content;
    Evas_Object *mini_player_content_box;

    /* */
    Evas_Object *sidebar;
    int sidebar_idx;
    Evas_Object *sidebar_toggle_btn;

    /* */
    Evas_Object *current_view;

    /* Context popup-menu */
    Evas_Object *popup_toggle_btn;
    Evas_Object *popup;

    Evas_Object *nf_toolbar;

    mini_player *p_mini_player;
    application *p_app;
};

/* TODO : A lot of size hints are Hard Coded with pixel values (using a Samsung Z1 phone) */
/* TODO : the use of Dpi or Aspect Ratio will be mandatory in the futur */

/* TODO : Some UI widget design are not perfect. */
/* TODO : Using Edje will probably be mandatory to set some of the widget and get a clean UI */
/* TODO : For examples : Headers & Toolbar Y axis scales */

/* TODO : Use Edje ColorClass to set the general widget colors of the app */
/* TODO :(VLC : #ff8800 / RGBA : 255, 136, 0, 255) */

/* TODO : Remember that the currently used player is the Tizen native one */
/* TODO : VLC haven't be linked to the current UI app */

/* TODO : Managing Hardware Key is mandatory to publish a Tizen AppStore App */
/* TODO : The Menu & Back Key aren't currently set */
/* TODO : This should be solved using the #include <efl_extension.h> */
/* TODO : Then use the void eext_object_event_callback_add func */
/* TODO : See more on https://developer.tizen.org/development/guides/native-application/ui/efl-extension */

/* CALLBACKS */
static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
    ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
    interface *intf = data;
    /* Let window go to hide state. */
    if (!elm_object_disabled_get(intf->sidebar) && !elm_panel_hidden_get(intf->sidebar)) {
        elm_panel_hidden_set(intf->sidebar, EINA_TRUE);
    } else if (evas_object_visible_get(intf->popup)) {
        evas_object_del(intf->popup); //since elm_popup_dismiss doesn't work
    } else {
        elm_win_lower(intf->win);
    }
}

static void
left_panel_button_clicked_cb(void *data, Evas_Object * obj, void *event_info)
{
    interface *intf = data;
    /* Disable the panel when left button is pressed */
    if (!elm_object_disabled_get(intf->sidebar)) elm_panel_toggle(intf->sidebar);
}

static void
right_panel_button_clicked_cb(void *data, Evas_Object * obj, void *event_info)
{
    interface *intf = data;

    intf->popup = create_popup(intf->mini_player_content_box,intf);
    evas_object_size_hint_weight_set(intf->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

}

void
intf_show_previous_view(interface *intf)
{
    intf_create_view(intf, intf->sidebar_idx);
}

void
intf_update_mini_player(interface *intf)
{
    if((mini_player_play_state(intf->p_mini_player) == true) && (mini_player_visibility_state(intf->p_mini_player) == false))
    {
        mini_player_show(intf->p_mini_player);
    }

    if((mini_player_play_state(intf->p_mini_player) == false) && (mini_player_fs_state(intf->p_mini_player) == true))
    {
        mini_player_stop(intf->p_mini_player);
    }
}

/* GETTERS */
application *
intf_get_application(interface *p_intf)
{
    return p_intf->p_app;
}

mini_player *
intf_get_mini_player(interface *p_intf)
{
    return p_intf->p_mini_player;
}

Evas*
intf_get_window(interface *intf)
{
    return evas_object_evas_get(intf->win);
}

Evas_Object*
intf_get_sidebar(interface *intf)
{
    return intf->sidebar;
}

Evas_Object *
intf_get_miniplayer_content_box(interface *intf)
{
    return intf->mini_player_content_box;
}

Evas_Object *
intf_get_main_naviframe(interface *intf)
{
    return intf->nf_content;
}

Evas_Object *
intf_get_toolbar(interface *intf)
{
    return intf->nf_toolbar;
}

/* CREATION */
static Evas_Object*
create_base_layout(Evas_Object *parent)
{
    Evas_Object *layout;
    layout = elm_layout_add(parent);

    /* */
    elm_layout_theme_set(layout, "layout", "drawer", "panel");
    evas_object_show(layout);

    return layout;
}

static Evas_Object *
create_button(Evas_Object *parent, char *style, char *text)
{
    Evas_Object *button;

    button = elm_button_add(parent);
    elm_object_style_set(button, style);

    /* */
    evas_object_show(button);

    return button;
}

static const char*
get_type_tag(int panel){
    switch(panel)
        {
        case VIEW_AUDIO:
            return "Audio";
        case VIEW_FILES:
            return "Directory";
        case VIEW_SETTINGS:
            return "Settings";
        case VIEW_ABOUT:
            return "About";
        default:
            return "Video";
        }
}
void
intf_create_view(interface *intf, int sidebar_idx)
{
    Evas_Object *content = intf->nf_content;
    Evas_Object *view;
    intf->sidebar_idx = sidebar_idx;

    /* Create the view depending on with panel item list is selected */
    switch(sidebar_idx)
    {
    case VIEW_VIDEO:
    case VIEW_AUTO:
        view = create_video_view(fetch_media_path(MEDIA_DIRECTORY_VIDEOS), content);
        break;
    case VIEW_AUDIO:
        intf->nf_toolbar = view = create_audio_view(intf, content);
        break;
    case VIEW_FILES:
        view = create_directory_view(application_get_media_path(intf->p_app), content);
        break;
    case VIEW_SETTINGS:
        view = create_setting_view(content);
        break;
    case VIEW_ABOUT:
        view = create_about_view(content);
        break;

    }
    /* Push the view in the naviframe with the corresponding header */
    elm_naviframe_item_push(content, get_type_tag(sidebar_idx), NULL, NULL, view, "basic");

    /* Create then set the panel toggle btn and add his callbacks */
    intf->sidebar_toggle_btn = create_button(intf->nf_content, "naviframe/drawers", NULL);
    evas_object_smart_callback_add(intf->sidebar_toggle_btn, "clicked", left_panel_button_clicked_cb, intf);
    elm_object_part_content_set(intf->nf_content, "title_left_btn", intf->sidebar_toggle_btn);

    /* */
    intf->popup_toggle_btn = create_button(intf->nf_content, "naviframe/drawers", NULL);
    evas_object_smart_callback_add(intf->popup_toggle_btn, "clicked", right_panel_button_clicked_cb, intf);
    elm_object_part_content_set(intf->nf_content, "title_right_btn", intf->popup_toggle_btn);
}

static Evas_Object*
create_main_content(interface *intf, Evas_Object *parent)
{
    /* Create a content box to display the content and the mini player */
    intf->mini_player_content_box = elm_box_add(parent);
    elm_box_horizontal_set(intf->mini_player_content_box, EINA_FALSE);

    /* Create both of the content_box subObjects */
    intf->p_mini_player = mini_player_create(intf, intf->mini_player_content_box);
    intf->nf_content = elm_naviframe_add(intf->mini_player_content_box);

    /* Put the naviframe at the top of the content_box */
    evas_object_size_hint_align_set(intf->nf_content, EVAS_HINT_FILL, 0.0);
    evas_object_size_hint_weight_set(intf->nf_content, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    /* Add the content naviframe in the content_box */
    elm_box_pack_end(intf->mini_player_content_box, intf->nf_content);

    /* */
    evas_object_show(intf->nf_content);

    /* Ask the box to recalculate her current children dislay */
    elm_box_recalculate(intf->mini_player_content_box);


    return intf->mini_player_content_box;
}

static Evas_Object*
create_main_view(interface *intf)
{
    Evas_Object *layout;

    /* Add a layout to the conformant */
    layout = create_base_layout(intf->conform);

    /* Create the panel and put it in the layout */
    intf->sidebar = create_sidebar(intf, layout);
    elm_object_part_content_set(layout, "elm.swallow.left", intf->sidebar);

    /* Create the content box and put it in the layout */
    intf->mini_player_content_box = create_main_content(intf, layout);
    elm_object_part_content_set(layout, "elm.swallow.content", intf->mini_player_content_box);
    /* */
    evas_object_size_hint_weight_set(intf->nf_content, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(intf->nf_content, EVAS_HINT_FILL, EVAS_HINT_FILL);
    /* */
    evas_object_show(intf->mini_player_content_box);

    return layout;
}

interface *
intf_create(application *app)
{
    interface *intf = calloc(1, sizeof(*intf));
    intf->p_app = app;

    Evas_Object *bg, *base_layout;

    /* Add and set the main Window */
    intf->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
    elm_win_autodel_set(intf->win, EINA_TRUE);

    /* Handle rotations */
    if (elm_win_wm_rotation_supported_get(intf->win)) {
        int rots[4] = { 0, 90, 180, 270 };
        elm_win_wm_rotation_available_rotations_set(intf->win, (const int *)(&rots), 4);
    }

    /* Handle callbacks */
    evas_object_smart_callback_add(intf->win, "delete,request", win_delete_request_cb, NULL);
    eext_object_event_callback_add(intf->win, EEXT_CALLBACK_BACK, win_back_cb, intf);

    /* Add and set a conformant in the main Window */
    intf->conform = elm_conformant_add(intf->win);
    elm_win_conformant_set(intf->win, EINA_TRUE);
    evas_object_size_hint_weight_set(intf->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    /* */
    elm_win_indicator_mode_set(intf->win, ELM_WIN_INDICATOR_SHOW);
    elm_win_indicator_opacity_set(intf->win, ELM_WIN_INDICATOR_OPAQUE);
    elm_win_resize_object_add(intf->win, intf->conform);
    evas_object_show(intf->conform);

    /* Add and set a bg in the conformant */
    bg = elm_bg_add(intf->conform);
    elm_object_style_set(bg, "indicator/headerbg");
    /* Add the bg in the conformant */
    elm_object_part_content_set(intf->conform, "elm.swallow.indicator_bg", bg);
    evas_object_show(bg);

    /* Create the main view in the conformant */
    base_layout = create_main_view(intf);
    elm_object_content_set(intf->conform, base_layout);

    /* Create the default view in the content naviframe */
    intf_create_view(intf, VIEW_AUTO);

    /* Add both left and right content naviframe buttons */
    intf->sidebar_toggle_btn = create_button(intf->nf_content, "naviframe/drawers", NULL);
    evas_object_smart_callback_add(intf->sidebar_toggle_btn, "clicked", left_panel_button_clicked_cb, intf);
    elm_object_part_content_set(intf->nf_content, "title_left_btn", intf->sidebar_toggle_btn);

    intf->popup_toggle_btn = create_button(intf->nf_content, "naviframe/drawers", NULL);
    evas_object_smart_callback_add(intf->popup_toggle_btn, "clicked", right_panel_button_clicked_cb, intf);
    elm_object_part_content_set(intf->nf_content, "title_right_btn", intf->popup_toggle_btn);

    /* */
    evas_object_show(intf->win);

    return intf;
}

/* DESTRUCTION */
void
intf_destroy(interface *intf)
{
    free(intf);
}