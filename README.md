    __   ___    ___    __           _____ _
    \ \ / / |  / __|  / _|___ _ _  |_   _(_)______ _ _
     \ V /| |_| (__  |  _/ _ \ '_|   | | | |_ / -_) ' \
      \_/ |____\___| |_| \___/_|     |_| |_/__\___|_||_|


# Description

This project is the mobile port of VLC for the Tizen Operating System.
It is fully open source and licensed under the GPLv2 (or later) license.

It uses the libVLC engine through the EFL Emotion API.
The interface is completely done using EFL/Elm libraries, in pure C.

This project was done by VLC developers and VideoLAN members, sponsored by
VideoLabs (videolabs.io)


# Building

## Requirements

* Tizen SDK available on www.tizen.org .
* Tizen Extension SDK for Certificate available on [the Samsung developer
  website](http://developer.samsung.com/samsung-z/)

## Build libvlc

Define the TIZEN_SDK path:

    export TIZEN_SDK=/path/to/tizen-sdk

Define the target TIZEN_ABI:

    export TIZEN_ABI=armv7l (or x86)

Launch the `./buildall.sh` and please be patient, grab a beer or a coffee, since
it's going to take a while.

## Setting up the IDE

Open the Tizen IDE and import the project directory (the directory containing
this file). In order to have your mike git imported in the Tizen IDE workspace:
- clone the mike git into "$HOME/workspace/mike",
- in the IDE, import the project via File/Import/Tizen/Tizen Project,
- select "$HOME/workspace/mike", select a platform, a SDK version, enter a
  name, and click next,
- Answer "No to all" when the IDE ask you to overwrite the mike/src folder.

If you're using a Samsung device (like the Z1) you must request certificates
(even for development purpose!). Use the "Register Certificate" icon in the IDE
toolbar and request a developer certificate as well as a device certificate.

## Build the VLC app

Connect your device, you should be able to see it in the Connection Explorer,
right click on it and select "Permit to install applications".

Press build and if everything's fine you should end up with the working app on
your device.

# Profit!

:D
