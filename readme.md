# Unilevel

Unilevel is the **Universal 3D Level Design Toolkit**. It is intended as a general-purpose 3D level editing and mapping tool which can be readily integrated with games and game engines.

This project is currently under construction.

## Compiling Unilevel from source

First, ensure that you have [make](https://www.gnu.org/software/make/) and [Python 3](https://www.python.org/downloads/) installed.

With `make` and `python` binaries installed and available in your `PATH`, run this command in the Unilevel repository's root directory to automate downloading and building Unilevel's dependencies. (This may take a few minutes.)

```
python scripts/get_dependencies.py
```

Ensure that the [SConstruct](https://scons.org/doc/production/HTML/scons-user/ch01s02.html) build tool is available on your system:

```
python -m pip install scons
```

Once `scons` is installed and Unilevel's dependencies have been made available, run this command in the repository's root directory to build Unilevel in release mode:

```
scons
```

Unilevel can be built in one of several modes, using the `mode` argument to `scons`. When the argument is omitted, `mode=release` is used by default.

```
# Release mode: Default, same as omitting the argument.
scons mode=release
# Fast compilation mode: Do not optimize.
scons mode=fast
# Debug mode: Do not optimize, do include debug symbols.
scons mode=debug
```
