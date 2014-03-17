mjwm
====

[![Build Status](https://travis-ci.org/chiku/mjwm.png?branch=master)](https://travis-ci.org/chiku/mjwm)

Overview
--------

mjwm creates JWM's menu from (freedesktop) desktop files. Include the generated file in the rootmenu section of your ~/.jwmrc

Read more about JWM (Joe's Window Manager) at [http://joewing.net/projects/jwm](http://joewing.net/projects/jwm)

It is based on mjm by insmyic <gminsm@gmail.com> [http://sourceforge.net/projects/mjm](http://sourceforge.net/projects/mjm)

Dependencies
------------

The application runs on a POSIX system with a C++ runtime.

For compiling the aplication you need autoconf, make and a recent C++ compiler. This application has compiled fine with g++ 4.6, g++ 4.8 and clang++ 3.3.

Installation from source
------------------------

* Clone the repository

* From the root of the application run
```script
touch NEWS AUTHORS
autoreconf -i
./configure --prefix=/usr/local # Install mjwm to /usr/local
make
cd src && sudo make install # Navigate to src to avoid installing test binaries
```

Usage
-----

``` script
Usage: mjwm [options]

Optional arguments:
    -o, --output-file [FILE]           Outfile file [Default: ./automenu]
    -s, --input-directory [DIRECTORY]  Directory to scan for '.desktop' files [Default: /usr/share/applications/]
    -a, --append-png                   Add '.png' to icon filenames
    -h, --help                         Show this help
    -v, --version                      Show version information
```

Examples
--------

Generate menu entries for files inside /etc/xdg
``` script
mjwm -s /etc/xdg/ # The trailing slash is required
```

Create menu entries with icons having a "png" extension
``` script
mjwm -a
```

Running tests
-------------

* Clone the repository

* From the root of the application run
``` script
touch NEWS AUTHORS
autoreconf -i
./configure --prefix=/usr/local
make
./test/test_runner.sh
```

_mjwm uses [http://sourceforge.net/projects/qunit](QUnit) for unit tests. QUnit is released under public domain._

Contributing
------------

* Fork the project.
* Make your feature addition or bug fix.
* Add tests for it. This is important so I don't break it in a future version unintentionally.
* Commit, but do not mess with the VERSION. If you want to have your own version, that is fine but bump the version in a commit by itself in another branch so I can ignore it when I pull.
* Send me a pull request.

License
-------

This tool is released under the GPLv3 license. Please refer to COPYING for license information.
