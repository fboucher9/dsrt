# dsrt

Display a jpeg image on root window to serve as wallpaper for lnch window
manager.


## Install


Use the following commands to install the `dsrt` application.


        make
        sudo cp dsrt /usr/bin


## Uninstall


Use the following command to uninstall the `dsrt` application:


        sudo rm /usr/bin/dsrt


## Usage


Use the following command-line syntax to invoke the `dsrt` application:


        dsrt [options] <files>


## Examples


To display the file `wallpaper.jpg` on root window, use the following command:


        dsrt wallpaper.jpg


## Options


The following command-line options are supported by the `dsrt` application:


*-c*|*--center*  Center image in view

*-p*|*--preview*  Preview image in a child window

*-e*|*--embed*  Embed image into parent window

*-f*|*--fit*  Fit image in view while preserving aspect ratio

*-g*|*--geom*  Specify size of view

*-s*|*--shadow*  Blend image with black background

*--*  Mark the end of options, all following arguments are files


## Environment

**WINDOWID**  The window id of parent window for embed option.

## Credits

Based on Xlib_JPEG_Example-8 by George Peter Staplin.
See CREDITS for more information.

## TODO

- Support for bicubic filtering
- Support for other image file formats

