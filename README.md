# Wiremock UI

A simple frontend for Wiremock. It can show requests log, and add/edit mappings.

## Installation

Only compilation from source code is supported right now.

### Dependencies

 - Qt 5 (tested with 5.10.0)
 - CMake 3

#### Linux

Usually you'll be able to simply install dependencies from your package manager.
In ubuntu, that would be:
```
sudo apt install build-essential cmake qtbase5-dev
```

#### OSX

Perhaps the simplest way is to install dependencies using brew:
```
brew install cmake qt
```

### Compilation

Go to the sources directory:
```
cd WiremockUI
```

Create the build directory, and go inside it:
```
mkdir build
cd build
```

Run `cmake` to configure the build:
```
cmake .. -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.10.0_1
```
**note**: `/usr/local/Cellar/qt5/5.10.0_1` depends on the Qt version you just installed with brew, and might change over time.
Please do an `ls /usr/local/Cellar/qt5/` to find out your installed Qt version.

Finally run `make` to start compilation:
```
make
make install # optional
```

You can now start the application:
```
./src/WiremockUI
```

If you did run `make install`, the program will be available in the system path. just type `WiremockUI` in any terminal.
