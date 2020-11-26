# AST Builder

## Project description

This program is developed as a part of ISP RAS course.  
This program can build AST from mathematical expression of real numbers (even negative) with parentheses and simple operators (`+`, `-`, `*`, `/`) 
and visualize it using graphviz.

![MISSING AST SAMPLE HERE](https://raw.githubusercontent.com/viafanasyev/ast-builder/master/samples/simple-graph.png)

### Structure

* src/ : Main project
    * ast.h, ast.cpp : Definition and implementation of AST node, AST building and visualization functions
    * tokenizer.h, tokenizer.cpp : Definition and implementation of tokens and tokenizer functions
    * main.cpp : Entry point for the program.

* test/ : Tests and testing library
    * testlib.h, testlib.cpp : Library for testing with assertions and helper macros.
    * tokenizer_tests.cpp : Tests for tokenizer functions
    * main.cpp : Entry point for tests. Just runs all tests.

*samples/ : Samples of graphs

* doc/ : doxygen documentation

* Doxyfile : doxygen config file

### Run

#### AST Builder

To run main program execute next commands in terminal:
```shell script
cmake . && make
./ast-builder "2 - 3 - (-4 * 5) + ----5"
```

#### Tests

To run tests execute next commands in terminal:
```shell script
cmake . && make
./tests
```

### Documentation

Doxygen is used to create documentation. You can watch it by opening `doc/html/index.html` in browser.  

### OS

Program is developed under Ubuntu 20.04.1 LTS.