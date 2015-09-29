/*****************************************************************************
 * Copyright © 2015 VideoLAN, VideoLabs SAS
 *****************************************************************************
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


#ifndef MEDIA_ITEM_H_
#define MEDIA_ITEM_H_

enum MEDIA_ITEM_TYPE {
    MEDIA_ITEM_TYPE_UNKNOWN,
    MEDIA_ITEM_TYPE_VIDEO,
    MEDIA_ITEM_TYPE_AUDIO,
    MEDIA_ITEM_TYPE_SUBTITLE,
    MEDIA_ITEM_TYPE_DIRECTORY,
    MEDIA_ITEM_TYPE_ARCHIVE
};

typedef struct media_item media_item;
struct media_item {
    char *psz_path;
    enum MEDIA_ITEM_TYPE i_type;
};

media_item *
media_item_create(const char *psz_path, enum MEDIA_ITEM_TYPE i_type);

void
media_item_destroy(media_item *p_mi);

#endif /* MEDIA_ITEM_H_ */
