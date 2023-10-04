
# Coding Standards

## Contents

- [Denomination](#denomination)
- [Formatting](#formatting)
- [Functions](#functions)
- [Typedefs, enum, union](#typedefs-enum-union)
- [Classes](#classes)
- [Headers](#headers)
- [Templates](#templates)
- [Macros and Pre-processors](#macros-and-pre-processors)
- [Comments](#comments)
- [Files](#files)
- [Makefile](#makefile)

## Denomination

Naming convention by type.

- Class names should be UpperCamelCase (aka PascalCase)
- Struct: ??
- A typedef’s name must start by t_
- A union’s name must start by u
- An enum’s name must start by e_
- A global’s name must start by g_
- Function names should be camelCase
- Variables should be snake_case
- Private class attributes and member functions should be followed_by_an_underscore_
- Files and directories names can only contain lowercases, digits and ’_’ (snake_case)

## Formatting

- Indentations : 
- Function Length : 
- Column Length : no limit
- Space Between Functions : 
- Empty lines : must be empty
- order for param(const name& ); function return -> std::string& func( void )




## Functions

- A function that doesn’t take arguments must be explicitly prototyped with the
word "void" as the argument.


## Typedefs, enum, union



## Classes



## Headers

- Header files must be protected from double inclusions. If the file is ft_foo.h, its bystander macro is FT_FOO_H.
- use forward declarations when possible




## Templates



## Macros and Pre-processors

- macros used project wide in separate header

## Comments



## Files



## Makefile


