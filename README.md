![XStatic]
===============================

## Latest version = v0.1

Windows (TBD) and Linux under heavy development
Ubuntu prebuilt under ubuntu-install/INSTALL.md



## How is it different from cscope, ctags, and CodeQuery? What are the advantages?

CodeQuery is quite good but is missing some of IDAPro's functionality.
XStatic attemps at integrating in some of IDAPro's capabilities along with
many RE/VR best practices in the form of new queries and enhanced workflow
(e.g. middle click on a symbol and it will do the search for you)

CodeQuery is an AWESOME starting point!  I'm taking it as a base and moving
toward a more precise and efficent workflow similar to that of IDAPro's.

Enhancements to CodeQuery in this inital release:
- Better layout with stubs for custom automated matching rather than one
  symbol at a time.
- Symbol ID shown in the view
- String QUERY!!!
- SymbolID query, useful when you want to un-alias something
- A navigation stack like navigating ... (lots more to improve)
- Middle button click takes you directly to the symbol rather than (1)
  highligh, click on the Paste button.
- Better handling of what the analyst would like to see!

Both cscope and ctags can do symbol lookup and identify functions, macros, classes and structs.

cscope is very C-centric, but is fuzzy enough to cover C++ and Java, but not very well for e.g. it doesn't understand destructors and class member instantiations. It can't provide relationships of class inheritance and membership. cscope can do "functions that call this function" and "functions called by this function". This is a very powerful feature that makes cscope stand out among comparable tools.

ctags does many languages well and understands destructors, member instantiations, and the relationships of class membership and inheritance. From ctags, we can find out "members and methods of this class", "class which owns this member or method", "parent of this class", "child of this class" etc. However, it doesn't do "functions that call this function" or "functions called by this function".

So both these tools have their pros and cons, but complement each other.

XStatic is a project that attempts to combine IDAPro workflow with CodeQuery's goals. The features available from both cscope and ctags, provide faster database access compared to cscope (because it uses sqlite) and provides a nice GUI tool as well. Due to this faster database access, fast auto-completion of search terms and multiple complex queries to perform visualization is possible.


XStatic TODO:
- Finish up the XRef To/From 
- Graph beyond one hop away
- More precise parsing!!!!!!!!!!!

In addition, [pycscope](https://github.com/portante/pycscope) is used to add support for Python, in place of cscope.

## What features does XStatic have?

* Combines the best of both cscope and ctags
* Faster due to the use of sqlite for the XStatic database
* Cross-platform GUI tool
* Fast auto-completion of search term
* Case-insensitive, partial keyword search - wildcard search supported * and ?
* Exact match search
* Filter search results by file path
* File viewer with syntax highlighting
* Ability to open viewed file in an external editor or IDE
* Visualization of function call graph and class inheritance based on search term
* Visualization graphs can be saved to PNG or Graphviz DOT files
* Lots of RE techniques!!!

## What does it cost? How is it licensed?

It's mostly freeware and free open source software.

Part of this  software is licensed under the [GNU GPL v3](http://www.gnu.org/licenses/gpl.html). See [LICENSE.md](LICENSE.md) or [LICENSE.txt](windows-install/LICENSE.txt).

Files under the `querylib` directory are licensed under the [MIT license](http://opensource.org/licenses/MIT). See [QueryLib README](querylib/README.txt). This is a library to query XStatic database files. This library is MIT-licensed, so that it may be used to create plugins for editors, IDEs and other software without license restrictions. It's only dependency is on sqlite3.


## How do I use it?

On Ubuntu: [HOWTO-UBUNTU](ubuntu-install/INSTALL.md)


## How do I contact the authors for support, issues, bug reports, fix patches, feature requests etc.?
huyxu@gmail.com

Email address:    
huyxvu@gmail.com

Website: [XStatic website](https://www.hxspace.com/xstatic) (under
construction)

GIT Repository:
https://github.com/hxvu/xstatic.git


## List of Contributors
HxView (huyxvu@gmail.com)
ruben2020   


## Credits
This is derived from the work of ruben2020 for the great starting point of
CodeQuery

We thank the people behind the following projects:    
[cscope](http://cscope.sourceforge.net/) - our database is derived from this   
[Exuberant ctags](http://ctags.sourceforge.net/)- our database is derived from this   
[pycscope](https://github.com/portante/pycscope) - our database (for Python) is derived from this    
[sqlite3](http://www.sqlite.org/) - our database is using this format    
[CMake](http://www.cmake.org/) - cross-platform build toolchain for XStatic    
[Qt open source](http://qt-project.org/) - GUI toolkit used to build XStatic    
[optlist](http://michael.dipperstein.com/optlist/index.html) - cqmakedb uses this to parse command line options   
[showgraph](http://code.google.com/p/showgraph/) - visualization done using this library    
[Axialis](http://www.axialis.com/iconworkshop) - free images for XStatic and this website

