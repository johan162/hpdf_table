# Building the library

## The short version; TL; DR
*For the long version see* @ref building-from-source "Building from source"

If the necessary @ref pre-req "pre-requisites" are fulfilled the distributed tar-ball can be built with:

```shell
$ tar xzf libhpdftbl-<version>.tar.gz
$ cd libhpdftbl-<version>
$ ./configure && make
```
If any libraries are missing the `configure` process will discover this and tell what needs to be installed.

@note By calling `./configure -h` a list of possible options on how the library should be compiled and installed
will be shown.

To verify the build run

```shell
$ make check
```
If everything works you should see a *Success!* message.
After checking that tha library passes the unit-tests it can be installed with

```shell
$ make install
```

If successfully this will install the library in the `/usr/local` subtree. 
It will build and install both a static and dynamic version of the library.


## Pre-requisites {#pre-req}

@note OSX Package manager: We recommend using `brew` as the package manager for OSX.

There are two external libraries required to build the library:

1. **libhpdf** - The Haru PDF library. On OSX this is most easily installed by using the `brew` OSX package manager. 
The library is available as `libharu` as of this writing the latest version is `libharu-2.3.0`  

2. **iconv** - The character encoding conversion library. On OSX > 11.x this is included by default once you 
have `xcode` command line tools installed which is basically a pre-requisite required for all development on OSX.  
*(On ancient versions of OSX this was not the case.)*

### Different versions of iconv on OSX

Unfortunately there are two different (and incompatible) versions of `libiconv` readily available for OSX. One library 
that uses the prefix "`iconv_*`" and the other "`libiconv_*`" on its exported functions. Compiling `libhpdftbl` 
requires the first of these which is the prevalent version and the default on both OSX and Linux.

This is almost exclusively an issue for those that actively develop on OSX and may have over time installed 
multiple versions of libraries and as such are aware of these challenges.

### OSX native libiconv

After installing `xcode` command line tools on OSXit is safe to assume that a library called `/usr/lib/iconv.dylib` 
is available. 

However, if one tries to list this library in `/usr/lib` there will not be a `libiconv.dylib`. 
Still, if the code is linked with `-liconv` it will work as expected. How come?   

The reason is the way OSX handles different library versions for different OSX SDKs. 
Since `xcode` supports developing for different OSX versions the SDK would need to include a complete setup of all  `*.dylib` of the right version for each included version of the SDK. To reduce diskspace all dynamic libraries are rolled-up in a dynamic link shared cache for each SDK version. The tool chain (e.g. `clang`) have been augmented to be aware of this. Hence, there is no need to have libraries in `/usr/lib`. 
Instead, OSX from v11 and onwards uses the concept of *stub libraries* with suffix `*.tbd` for each supported SDK version (tbd stands for "text based description"). 
They are small text files with meta information about the library used by the tool-chain.

For example for SDK 12.3 the stub for libiconv can be found at

```
/Library/Developer/CommandLineTools/SDKs/MacOSX12.3.sdk/usr/lib/libiconv.tbd
```

and the corresponding include header is located at

```
/Library/Developer/CommandLineTools/SDKs/MacOSX12.3.sdk/usr/include/iconv.h
```

### OSX GNU port of libiconv

If you have happened to install `libiconv` via the MacPorts you are out of luck and need to change. 
MacPorts uses the GNU version which uses the prefix "`libiconv_*`" for its exported function and is not 
compatible since the table library assumes the naming convention of the standard OSX version (after v11)

### Troubleshooting OSX `libiconv`

If the build complains about `libiconv` the following steps could be take to try to track down the 
problem:

1. Find out all installed versions of `libiconv` on your machine  

       $> find / -iregex '.*/libiconv.*' 2> /dev/null
   
   The "`2> /dev/null`" makes sure you don't get a lot of noise with "permission denied"

2. Find out the SDK path that is actively used  

       $> xcrun --show-sdk-path

3. Check you `PATH` variable  

       $> echo $PATH



## Building the library from source {#building-from-source}

There are two levels of rebuilding the library that we will discuss

1. Using a build environment to rebuild the library (i.e. building from the supplied tar-ball)

2. Rebuilding from a cloned repo and rebuild the build environment from scratch. As a 
principle no generated files are stored in the repo.

### Rebuilding using av existing build environment

Rebuilding the library using a pre-configured build environment requires `gcc` ( or `clang`) and `make`
together with the standard C/C++ libraries to build the library.

The library source with suitable build-environment is distributed as a tar-ball

1. `libhpdftbl-x.y.z.tar.gz`

This tar-ball includes a build environment constructed with the GNU autotools. 
This means that after downloading the tar-ball you can rebuild the library as so:

```shell
$ tar xzf libhpdftbl-x.y.z.tar.gz
$ cd libhpdf-x.y.z
$ ./configure && make
... (output from the configuration and build omitted) ...
```

and then (optionally) install the library with

```shell
$ make install
```

By default, the library will install under the `/usr/local` but that can be adjusted by using the `--prefix` 
parameter to `configure`. For example

```shell
$ tar xzf libhpdftbl-1.0.0.tar.gz
$ cd libhpdf-1.0.0
$ ./configure --prefix=/usr && make
... (output from the configuration and build omitted) ...
```

Please refer to `configure -h` for other possible configurations. As a shortcut two
utility scripts are included that give some extra `CFLAGS` flags to either compile the
library for production use `./scripts/stdbld.sh` or for debugging `./scripts/dbgbld.sh`
(See @ref lib-debug "Some notes on Debugging")


### Rebuilding from a cloned repo

@note This is for experienced developers!

The repo does not include any of the generated files as the tar-ball does. 
This means that the following build tools
needs to be installed in order to fully rebuild from a cloned repo.

1. A complete set of GNU compiler chain (or on OSX clang)
2. [GNU autotools](https://www.wikiwand.com/en/GNU_Autotools) (autoconf, automake, libtool)
3. [Doxygen](https://www.doxygen.nl/manual/index.html) in order to rebuild the documentation. 

Since it is completely out of the scope to describe the intricacies
of the GNU autotools we will only show what to do assuming this tool chain have already been installed.

To simplify the bootstrapping necessary to create a full autotools environment from the cloned
repo a utility script that does this is  provided in the form of `./scripts/bootstrap.sh`. After cloning the 
repo run (from the `libhpdftbl` top directory)

```shell
$ ./scripts/bootstrap.sh
```

This script will now run `autoreconf`, `automake`, `glibtoolize` as needed in order to create a full build environment. 
It will also run `configure` and if everything works as expected the last lines you will see (on OSX) will be

```
...
config.status: executing libtool commands
configure: --------------------------------------------------------------------------------
configure: INSTALLATION SUMMARY:
configure:   - Build configured for OSX.
configure:   - Can rebuild HTML docs with Doxygen.
configure:   - Can also create PDF docs (have LaTeX).
configure:   - Installing to /usr/local
configure: --------------------------------------------------------------------------------
```

and then to compile the library with

```shell
$ make
```

The simplest way to verify that everything works is to run the built-in unit/integration tests 

```shell
$ make check
Info: PASS: tut_ex01
Info: PASS: tut_ex02
<omitted ...>
Info: PASS: tut_ex20
Info: =================================
Info: SUCCESS! 20/20 tests passed.
Info: =================================
```

To install the library use

```shell
$> make install
```

This will install headers and library under "`/usr/local`" (unless the prefix was changed when running the `configure`)

## Miscellaneous 

### Some notes on Compiling for debugging {#lib-debug}

Since the library builds with `libtool` and this tool will generate a wrapper shell script for each example to
load the, not yet installed, library it also means this "executable" shell script cannot directly be used to debug with
for example `gdb`. 

The solution for this is to configure the library to only build static libraries which are directly linked
with the example binaries and as such can be debugged as usual. It is also a good idea to disable
optimization during debugging to make the source better follow the execution while 
stepping through the code. This configuration is done with:

```shell
$> ./configure --disable-shared CFLAGS="-O0 -ggdb"
```

After this all the examples will be statically linked and can be debugged as usual

An alternative way (as recommended in the 
[libtool manual](https://www.gnu.org/software/libtool/manual/html_node/Debugging-executables.html))
is to launch the debugger with:

```shell
$> libtool --mode=execute gdb <example program>
```

This will run the `gdb` debugger from command line. For debugging from within a IDE
(like Netbeans, Clion, etc.) use the static library method.

As a convenience a script is provided to handle the debug build configuration `scripts/dbgbld.sh`


### Some notes on updating the documentation

By design the documentation is not updated by the default make target in order minimize the build time during
development. To rebuild the *html* documentation build the target

```shell
$> make html
```

and to rebuild the *PDF* version build (assuming you have LaTeX installed)

```shell
$> make pdf
```

The resulting documentations are stored under `docs/out/html` and `docs/out/latex/refman.pdf`

@warning There is a shell script `scripts/docupload.sh.in` that the author (i.e. me!) uses to upload the
HTML and PDF documentation to the GitHub pages of the author. For obvious reason this script will 
not work for anyone else since it requires write access to the doc repo (through an SSL certificate).


### Some notes on Windows build

The source files are suitable augmented to also compile on MS Windows with selective defines. However, 
this is on a best effort basis since I have no longer access to a Windows system to verify the workings. 


### Some notes on using C or C++ to build

The source files are also suitable augmented to compile on both a C and a C++ compiler. However, 
the default build environment is set up for a pure C library build. To add a configuration switch for 
this would be the sensible way to handle this. This is not done and again, is left as an exercise 
for the reader.




