# Equation solver
This code was written at summer school in MIPT by DED32

## Installation
you should install a gcc, clone this repository and run:
```bash
gcc main.c hash.c common.c parser.c -lm
```
## Usage
start a program, enter your login and password, enter a eq and get your solutions

## Flags
--help: show help
--quiet: disable dialog outputs
--oldenter: enable old enter (a b c)
--testin: parametr: [file.txt] unittests from file
--debugargs: debug argparse
--debugparse: debug parsing eq
--drawplot: draw a plot
--drawplotoffset: draw a plot by offsets
--signup: register a user
--randtests: parametr: [num_tests] generate random tests
--seed: parametr: [seed] set seed
--game: run a game to enter
