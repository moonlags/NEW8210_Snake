# NEW8210 Snake Game

A classic snake game with ability to play with a bot with 3 different difficulty levels.
This game is using directfb library to render graphics and read key presses.

https://github.com/user-attachments/assets/d4f3858e-7b60-4ef7-aa9b-575a4192c356

## Features

- Full classic snake game with ability to loop through screen edges
- Ability to play with bot that has 3 difficulty levels
- Score counting system
- Snake collision

## Getting started

### Prerequisites

- `gcc` from supplied toolchain
- (Optional) `make`

### Running

The program has multiple steps for running it:
- Installing the toolchain
- Compiling the program
- Transfering executable, font and `.desktop` file to the terminal
- Running the program

1. Installing the toolchain supllied in `./static/sdk-new8210.rar`
```bash
go run ./cmd/app
```

2. Compiling the program
```bash
go run ./cmd/app
```

3. Transfering executable, font and `.desktop` file to the terminal
```bash
go run ./cmd/app
```

4. Running the program
```bash
go run ./cmd/app
```

## Project structure

```
NEW8210_Snake
|
├── bin/            # compiled executable
├── include/        # NEW8210 header files
├── lib/            # NEW8210 library files
├── src/            # program code
├── static/         # fonts, sdk
├── build.sh        # build script
└── README.md
```
