# PE

## --- ABOUT ---

All c and c++ (and assembly if present) files from ./src will be compiled, along side the main.cpp file
    in ./compiled directory and later linked into the final executable
    at ./out/{TYPE}/{EXE_NAME}

---

## --- VARIABLES ---

- EXE_NAME: name of the executable
- TYPE:     debug or release mode

---

## --- COMPILE ---

- compilers:  `gcc`, `g++`
- linkers:    `ld`
- assemblers: `nasm`
- tools:      `make`

---

## --- COMMANDS ---

- `make clean`:      remove all .o files from ./compiled and ./out/{TYPE}
- `make make_dirs`:  create the ./compiled and ./out/{TYPE} directories and ./docs
- `make interfaces`: generate the interface files for the project, if any(
    .h files will be copied to ./src/interfaces,
    from ./src/** where .h file == dirname
)
- `make clean_copyright`: remove all copyright comments from the source files
- `make clean_interfaces`: remove all .h files from ./src/interfaces
- `make debug`:       make_dirs and make interfaces and compile the project in debug mode
- `make run`:         make debug and run the created executable(if compiled successfully)
- `make release`:     make_dirs and make interfaces and compile the project in release mode
- `make release_run`: make release and run the created executable(if compiled successfully)
- `make docs`:        generate the documentation for the project using doxygen in ./docs
- `make copyright`:   if "LICENSE.md" present append copy of the content of file to the end of every .cpp and .c and .hpp and .h files in `/* */`(mutli-line comments)
- `make help`:        print all possible commands

---

## --- PREDEFINED MACROS ---

- MAIN_PROJECT_VERSION:  version of the project
- MAIN_PROJECT_NAME:     name of the project
- MAIN_PROJECT_AUTHOR:   author of the project

---

## --- FILES ---

- `LICENSE.md`: file containing the license/copyright of the project
- `main.cpp`:   main file of the project
- `main.c`:     main file of the project
- `shared.hpp`: header file containing all the shared code between all the files
- `brief.txt`:  brief description of the project

---

## --- DIRECTORIES ---

- `./compiled`:          directory containing all the compiled files
- `./docs`:              directory containing the documentation
- `./out`:               directory containing the executables
- `./scripts`:           directory containing all the scripts
- `./src`:               directory containing all the source and header files
- `./src/interfaces`:    directory containing all the interface files
- `./external`:          directory containing all the external libraries
- `./external/libs`:     directory containing all the external dynamic libraries (`.dll`, `.so`)
- `./external/includes`: directory containing all the external includes (`.h`, `.hpp`)
- `./external/static`:   directory containing all the external static libraries (`.a`, `.lib`)

---

## --- VERSIONS OF LANGUAGES ---

- `c`:   `99`
- `c++`: `17`  

---

## --- VERSIONS OF TOOLS ---

- `gcc`:    `15.1.1`
- `g++`:    `15.1.1`
- `nasm`:   `2.16.03`

---

## --- DOCUMENTATION ---

### code should be self documenting, if not, please document it

using doxygen comments like:

```c
/*
 * @brief FUNCTION_NAME.
 * @details DESCRIPTION.
 * @param PARAMETER.
 * @return RETURN.
*/
```

#### or single line doxygen comments like

```c
/// This calculates <something>...
```

##### or normal comments like

```c
// This calculates <something>...
```

### Do not use the following

```c
/* This is if statement. */
if (...) {
    // ...
}
```

#### or like this

```c
// This is loop
for (...) {
    // ...
}
```

---
