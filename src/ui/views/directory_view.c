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

#include <Elementary.h>

#include "directory_view.h"

typedef struct directory_data {
    Evas_Object *parent;
    char *file_path;
} directory_data_s;

void
list_selected_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info)
{
    directory_data_s *dd= data;
    struct stat sb;
    stat(dd->file_path, &sb);

    if (S_ISREG(sb.st_mode)){

        /* Launch the media player */
        LOGI("VLC Player launch");
        LOGI("Won't play the video at the time. Will be soon");

    }

    else if (S_ISDIR(sb.st_mode))
    {
        /* Continue to browse media folder */
        create_directory_view(dd->file_path, dd->parent);
    }

}

static void
free_list_item_data(void *data, Evas_Object *obj, void *event_info)
{
    directory_data_s *dd = data;
    /* Free the file path when the current list is deleted */
    /* For example when the player is launched or a new list is created */
    free(dd->file_path);
    LOGD("Path free");

}

Evas_Object*
create_directory_view(const char* path, Evas_Object *parent)
{
    char *buff;
    directory_data_s *dd;
    const char *str = NULL;
    Evas_Object *file_list;
    DIR* rep = NULL;
    struct dirent* current_folder = NULL;

    /* Make a realpath to use a clean path in the function */
    buff = realpath(path, NULL);

    if (buff == NULL)
    {
        LOGI("No path");
        return NULL ;
    }

    /* Open the path repository then put it as a dirent variable */
    rep = opendir(buff);

    if  (rep == NULL)
    {
        char *error;
        error = strerror(errno);
        LOGI("Empty repository or Error due to %s", error);
        free(buff);

        return NULL ;
    }

    Evas_Object *box = elm_box_add(parent);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(box);

    Evas_Object *directory =  elm_label_add(parent);
    elm_object_text_set(directory, buff);
    elm_box_pack_end(box, directory);
    evas_object_show(directory);

    /* Add a first list append with ".." to go back in directory */
    file_list = elm_list_add(parent);
    dd = malloc(sizeof(*dd));
    dd->parent = parent;
    asprintf(&dd->file_path, "%s/..", buff);
    Elm_Object_Item *item = elm_list_item_append(file_list, "..", NULL, NULL, list_selected_cb, dd);
    elm_object_item_del_cb_set(item, free_list_item_data);

    while ((current_folder = readdir(rep)) != NULL)
    {
        dd = malloc(sizeof(*dd));
        /* Put the list parent in the directory_data struct for callbacks */
        dd->parent = parent;

        /* Concatenate the file path and the selected folder or file name */
        asprintf(&dd->file_path, "%s/%s", buff, current_folder->d_name);
        /* Put the folder or file name as a usable string for list item label */
        str = current_folder->d_name;

        /* Avoid genlist item append for "." and ".." d_name */
        if (str && (strcmp(str, ".") == 0 || strcmp(str, "..") == 0))
        {
            continue;
        }

        /* Set and append new item in the list */
        Elm_Object_Item *item = elm_list_item_append(file_list, str, NULL, NULL, list_selected_cb, dd);
        /* */
        elm_object_item_del_cb_set(item, free_list_item_data);
    }

    elm_list_go(file_list);

    elm_box_pack_end(box, file_list);
    evas_object_size_hint_weight_set(file_list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    /* The next line is required or the list won't show up */
    evas_object_size_hint_align_set(file_list, EVAS_HINT_FILL, EVAS_HINT_FILL);

    evas_object_show(file_list);

    elm_object_content_set(parent, box);
    free(buff);

    return box;
}
