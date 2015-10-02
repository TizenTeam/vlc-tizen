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

#ifndef DIRECTORY_VIEW_H_
#define DIRECTORY_VIEW_H_

#include "ui/interface.h"

typedef struct directory_view {
    Evas_Object *p_parent;
    interface *p_intf;
} directory_view;

typedef struct directory_data {
    directory_view *dv;
    char *file_path;
    bool is_file;
} directory_data;

interface_view*
create_directory_view(interface *intf, Evas_Object *parent);

Evas_Object*
browse(directory_view *dv, const char* path);

#endif /* DIRECTORY_VIEW_H_ */
