# Building the library

## The short version

### Compiling the tar ball

If you downloaded the tar-ball then it should be trivial to build and install if you have the necessary pre-requisites. Just download the tar-ball and do the standard spell:

```c
$ tar xzf libhpdftbl-1.0.0.tar.gz
$ cd libhpdf-1.0.0
$ ./configure && make
$ make install
```

If you miss any library the `configure` process will discover this and tell you what you need to install.
This would otherwise compile and install the library in `/usr/local` subtree. It wil build and install both a static and dynamic library.

@note By calling `./configure -h` a list of possible options on how the library should be compiled and installed
will be shown.

Depending on your system this might also be available as a pre-built package for you to install directly via perhaps `apt`on Linux or `brew` on OSX. 

### Compiling after cloning the git repo

The repo does not include any generated files as the tar-ball does. This means that the following build tools
needs to be setup in order to rebuild from a cloned repo. 

1. A complete set of GNU compiler chain (or on OSX clang)
2. An installation of the autotools (autoconf, automake, libtool)
3. An installation of Doxygen (to generate documentation)

If these three pre-requisites are installed then the build environment is bootstrapped by running

```shell
$ ./scripts/bootstrap.sh
```

and then continue to compile the library

```shell
$ make
```

and (optionally) install the library

```shell
$ make install
```


## Pre-requisites

@note OSX Package manager: We recommend using `brew` as the package manager for OSX.

There are two external libraries required to rebuild libhpdftbl and more importantly use the library with an actual application and these are:

1. **libhpdf** - The Haru PDF library. On OSX this is most easly installed by using the `brew` OSX package manager. The library is available as `libharu` as of this writing the latest version is `libharu-2.3.0`  

2. **iconv** - The character encoding conversion library. On OSX > 11.x this is included by default once you have `xcode` command line tools installed which is basically a pre-requisite required for all development on OSX.  
*(On really old versions of OSX this was not the case.)*

## Different versions of iconv on OSX

Unfortunately there are two main versions of `libiconv` readily available for OSX which are incompatible as one uses the prefix "`iconv_*`" and the other "`libiconv_*`" on its exported functions. Compiling `libhpdftbl` requires the first of these which is the prevelant version and the default on both OSX and Linux.

This is almost exclusivly an issue for those that actively develop on OSX and may have over time installed multiple versions of libraries and as such are aware of these challenges.

### OSX native libiconv
After installing `xcode` command line tools on OSX you can assume that a library called `/usr/lib/iconv.dylib` is available. However, if you actually try to list this library in `/usr/lib` you will not find it! Still, if you link your code with `-liconv` it will work as expected. How come?   

The reason is the way OSX handles different library versions for different OSX SDKs. Since `xcode` supports developing for different OSX versions the SDK would need to include a complete setup of all  `*.dylib` of the right version for each included version of the SDK. To reduce diskspace all dynamic librares are rolled-up in a dynamic link shared cache for each SDK version. The tool chain (e.g. `gcc`) have been augmented to be aware of this. Hence there is no need to have libraries in `/usr/lib`. Instead OSX from v11 and onwards uses the concept of *stub libraries* `*.tbd` (tbd stands for "text based description") which are much smaller text files with some meta information about the library used by the tool-chain.

For example for SDK 12.3 the stub for libiconv can be found at

```
/Library/Developer/CommandLineTools/SDKs/MacOSX12.3.sdk/usr/lib/libiconv.tbd
```

and the corresponding include header at

```
/Library/Developer/CommandLineTools/SDKs/MacOSX12.3.sdk/usr/include/iconv.h
```


### OSX GNU port of libiconv
If you have happened to install `libiconv` via the MacPorts you are out of luck and need to change. MacPorts uses the GNU version which uses the prefix "`libiconv_*`" for its exported function and is not compatible since the table library assumes the naming convention of the standard OSX version (after v11)

### Troubleshooting OSX `libiconv`

1. Find out all installed versions of `libiconv` on your installation  

       $> find / -iregex '.*/libiconv.*' 2> /dev/null
   
   The "`2> /dev/null`" makes sure you don't get a lot of noise "permission denied"

2. Find out the SDK path that is actively used  

       $> xcrun --show-sdk-path

3. Check you `PATH` variable  

       $> echo $PATH



## Building the library from source

There are two levels of rebuilding the library

1. Using a build environment to rebuild the library

2. Rebuilding from a cloned repo and rebuild the build environment

### Rebuilding using a existing build environment

Rebuilding the library using a pre-configured build environment only requires `gcc` and `make` together with the standard C/C++ libraries to be installed.

The library source with suitable build-environments are distributed as a tar-ball

1. libhpdf-src-x.y.z.tar.gz

This tar-ball include a build environment constructed with the GNU autotools. This means that after downloading the tar-ball you can rebuild the library as so:

```shell
$> ./configure && make
... (output from the configuration and build omitted) ...
```

@note: The git repo do not have a build environment setup.


### Rebuilding from the cloned repo

Rebuilding from the cloned repo requires the GNU autotools tool-chain to be installed. Since it is completely out of the scope 
to decribe the intricacies
of the GNU autotools we will only show what to do assuming this tool chain have been installed.

To simplify the potentially painful bootstrap of creating a full autotools environment a utility script that does this is 
provided in the form of "`scripts/bootstrap.sh`". After cloning the repo run (from the `libhpdftbl` directory)

```
./scripts/bootstrap.sh
```

This script will now run `autoreconf`, `automake`, `glibtoolize` as needed in order to create a full build environment. It will also run `configure` and if everything works as expected the last lines you will see (on OSX) will be

```shell
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

The final step you need to do is compile the library as so

```shell
$> make
```

The simplest way to verify that everything works is to execute one of the example programs (in the `examples/` directory) as so:

```shell
$> ./examples/example01
Stroking 5 examples.
Sending to file "/tmp/example01.pdf" ...
Done.
```

If you would like to install the library make the install target

```shell
$> make install
```

This will install headers and library under "`/usr/local`" (unless the prefix was changed when running the `configure`)


## Some notes on Windows build

The source files are suitable augmented to also compile on MS Windows with selective defines. However, since I have no access to a Windows system to verify the workings this is left as an exercise to the reader.

## Using C or C++ to build

The source files are also suitable augmented to compile on both a C and a C++ compiler. However, the default build environment is setup for a pure C library build. To add a configuration switch for this would be the sensible way to handle this. This is not done and again, is left as an exercise for the reader.




