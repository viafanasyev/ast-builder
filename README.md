# AST Builder

## Project description

This program is developed as a part of ISP RAS course.  
This program can build AST from mathematical expression of real numbers (even negative) and variables (variable name starts with letter and contain letters and digits) 
with parentheses and simple operators (`+`, `-`, `*`, `/`, `^`) 
and visualize it using graphviz. Also it can convert expressions into TeX/PDF format.

AST can be optimized with `--optimized` option. Currently supported optimizations:
* Unary plus operators removed;
* Double negation operators removed.

![MISSING AST SAMPLE HERE](https://raw.githubusercontent.com/viafanasyev/ast-builder/master/samples/simple-expression.png)
![MISSING TEX SAMPLE HERE](https://raw.githubusercontent.com/viafanasyev/ast-builder/master/samples/simple-expression.pdf.png)

NOTE: This program runs only on UNIX-like OS. Also `dot` and `pdflatex` should be installed.

### Structure

* src/ : Main project
    * ast.h, ast.cpp : Definition and implementation of AST node, AST building, visualization and TeX conversion functions
    * ast-optimizers.h, ast-optimizers.cpp : Definition and implementation of AST optimizers
    * tokenizer.h, tokenizer.cpp : Definition and implementation of tokens and tokenizer functions
    * main.cpp : Entry point for the program.

* test/ : Tests and testing library
    * testlib.h, testlib.cpp : Library for testing with assertions and helper macros.
    * tokenizer_tests.cpp : Tests for tokenizer functions
    * main.cpp : Entry point for tests. Just runs all tests.

* samples/ : Samples of graphs

* doc/ : doxygen documentation

* Doxyfile : doxygen config file

### Run

#### AST Builder

To run main program execute next commands in terminal:
```shell script
cmake . && make
./ast-builder "2 - 3 - (-4 * x) / y + ----5"
./ast-builder "2 - 3 - (-4 * x) / y + ----5" --optimized
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