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

#include <mutex>

#include "IFile.h"
#include "ILogger.h"
#include "media_library.hpp"
#include "media/media_item.h"

media_item* fileToMediaItem( FilePtr file );

class TizenLogger : public ILogger
{
    virtual void Error( const std::string& msg ) override
    {
        dlog_print( DLOG_ERROR, "medialibrary", msg.c_str() );
    }

    virtual void Warning( const std::string& msg ) override
    {
        dlog_print( DLOG_WARN, "medialibrary", msg.c_str() );
    }

    virtual void Info( const std::string& msg ) override
    {
        dlog_print( DLOG_INFO, "medialibrary", msg.c_str() );
    }
};

struct media_library : public IMediaLibraryCb
{
public:
    media_library();
    virtual ~media_library() = default;

    // IMediaLibraryCb
    virtual void onFileAdded( FilePtr file ) override;
    virtual void onFileUpdated( FilePtr file ) override;

    virtual void onDiscoveryStarted( const std::string& entryPoint ) override;
    virtual void onDiscoveryCompleted( const std::string& entryPoint ) override;

    void registerOnChange(media_library_file_list_changed_cb cb, void* cbUserData);
    void unregisterOnChange(media_library_file_list_changed_cb cb, void* cbUserData);

    void registerOnItemUpdated(media_library_item_updated_cb cb, void* userData);
    void unregisterOnItemUpdated(media_library_item_updated_cb cb, void* userData);

public:
    std::unique_ptr<IMediaLibrary> ml;
    std::unique_ptr<TizenLogger> logger;

private:
    void onChange();

private:
    // Holds the number of discoveries ongoing
    // This gets incremented by the caller thread (most likely the main loop)
    // and gets decremented by the discovery thread, hence the need for atomic
    int m_nbDiscovery;
    // Holds the number of changes since last call to fileListChangedCb.
    // This can be accessed from both the discovery & metadata threads
    int m_nbElemChanged;
    std::mutex m_mutex;
    std::vector<std::pair<media_library_file_list_changed_cb, void*>> m_onChangeCb;
    std::vector<std::pair<media_library_item_updated_cb, void*>> m_onItemUpdatedCb;
};
