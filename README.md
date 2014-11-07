#LightMd Markdown Editor

LightMd Markdown Editor is a free and open source editor for Markdown with lot of features. It has been tested on Linux as well as Windows. In near future, support for Mac OS will be added. 

#### Current Version 
Version Number : [1.0.0](https://github.com/bhavyanshu/LightMd_Editor/releases/tag/v1.0.0)
Dated : 04-11-2014

#### Download & Install

View the [releases](https://github.com/bhavyanshu/LightMd_Editor/releases) for information on how to install.

##Features in current version

* Markdown formatting with keyboard shortcuts
* Syntax Highlight for Markdown
* Customizable for comfort
* Focus Mode and Full Screen Mode for distraction free typing

View [changelog](https://github.com/bhavyanshu/LightMd_Editor/blob/master/debian/changelog) for more details.

To know what more features will be added, view [this](https://github.com/bhavyanshu/LightMd_Editor/labels/enhancement). You can also post a feature request here.

##License

This application is under GNU GPLv3. Please read the COPYING.txt file for further terms and conditions of the license.

>Copyright 2014 Bhavyanshu Parasher  
 This file is part of "LightMd Editor".  
 "LightMd Editor" is free software: you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the Free Software Foundation,
 either version 3 of the License, or (at your option) any later version.
 "LightMd Editor" is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License along with "LightMd Editor".
 If not, see http://www.gnu.org/licenses/.

##Contribute

To contribute, simply fork, clone, patch and send pull request.
I have built it using Qt Creator. The source code is written in C++ and Qt5. So either use Qt Creator to build the project or follow the instructions given below.

Use the following commands to build. Make sure you have g++, gcc and Qt5 libraries. 

    cd ./src
    qmake
    make -f Makefile

This will build the application if you have all the build dependencies. Come outside the src folder, you will see build & bin folder. In the bin folder, there is an executable file for the main application.

**For those building on windows**  
Get the [zip](https://github.com/bhavyanshu/LightMd_Editor/blob/master/src/win32/) from src/win32 folder. The source for windows will always be zipped because this directory structure is perfect for linux packaging and I don't want to ruin it. Just extract the zip and select the .pro file in it. Make sure you have Qt Creator installed. If at first you are unable to build it then just switch to release mode and it will just work somehow. It's weird I know. There is also an issue with mingw and windows hence I could not keep the windows directory structure similar to linux one. I am still using dlls for findreplace in case of windows. It would be great if you can help with this.

Contact me at bhavyanshu@codershangout.org or visit https://bhavyanshu.me

###Support Libraries

* [PEG Markdown Highlight](http://hasseg.org/peg-markdown-highlight/)
* [QtFindReplaceDialog](http://qtfindreplace.sourceforge.net/)
