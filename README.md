#LightMd Markdown Editor

LightMd Markdown Editor is a free and open source editor for Markdown with lot of features. It has been tested on Linux as well as Windows. In near future, support for Mac OS will be added. 

#### Current Version 
Version Number: [1.0-1](https://github.com/bhavyanshu/LightMd_Editor/releases/tag/v1.0.1)   
Dated : 09-11-2014    
     
Developer Ver: [1.0-2](https://github.com/bhavyanshu/LightMd_Editor/archive/master.zip)    

#### Download & Install

View the [releases](https://github.com/bhavyanshu/LightMd_Editor/releases) for information on how to install.

##Features in current version

* Markdown formatting with keyboard shortcuts
* Syntax Highlight for Markdown
* Customizable for comfort
* Focus Mode and Full Screen Mode for distraction free typing
* Mutli-theme support

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

This will build the application if you have all the build dependencies. Come outside the *src* folder, you will see *build* & *bin* folders. In the *bin* folder, there is an executable file for the main application.

**For those building on windows**  

Get the latest Qt Creator. Download [source](https://github.com/bhavyanshu/LightMd_Editor/archive/master.zip). Go to *src* directory and open *src.pro* file in Qt Creator. Configure and run the project. It will build and run automatically.

Contact me at bhavyanshu@codershangout.org or visit https://bhavyanshu.me

###Support Libraries

* [PEG Markdown Highlight](http://hasseg.org/peg-markdown-highlight/)
* [QtFindReplaceDialog](http://qtfindreplace.sourceforge.net/)
