# AlgoHelper

This is the project I use to write algorithm.

This project does not contain a bunch of off-the-shelf algorithm code, but some useful functions and debug macros.

## Features
- Show your output in a different color.

- Show the time your algorithm cost.

- Debug macros `D`. Just use it like printf, it will only output text locally with another dolor.

- Show a red `'$'` when there's no `'\n'` at the end of output.

- Supports both C and Cpp code (as long as your code is C-compatible).

- ...

## Restrictions
- Only Works under Windows, **With MSVC Compiler** (typically Visual Studio use this)

- Currently `Release` Mode won't work correctly because compiler will optimize out some essential code ...

## How to use
Just clone or download this repo, open the project in Visual Studio and open the `main.c` under project `Algorithm`, writing your code in the `main` function as usual.
Then click run. (Ensure that AlgoHelperFramework is the project you start)

## TODO
- Enable you to run multiple testcase automatically and show the compare result.

- Support checking algorithm's fault against a correct algorithm with random-generated data. (对拍)

- 咕咕咕
