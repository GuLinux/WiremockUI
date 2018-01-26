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
cmake .. -DCMAKE_PREFIX_PATH=/usr/local/opt/qt5
```
**note** your Qt installation path may vary.


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

#### Install on OSX using brew

There is a brew formula available in the main repository.
This hasn't been submitted yet to the main brew repositories, but you can install easly the application with the following command, after cloning the repository:
```
brew install --build-from-source --HEAD ./support/brew/wiremock-ui.rb
```

