# Logical Scheme Interpreter

A visual programming environment and flowchart interpreter written in C++ using SFML. It enables users to graphically build flowcharts, execute them while tracking the output, and generate formatted C++ code from the visual graph.

## Key Features

* Interactive Visual Canvas: Drag-and-drop block placement, dynamic sizing based on text length, anchor-based connections, and right-click deletion for blocks and arrows.
* Expression Parsing: Custom implementation of the Shunting-Yard algorithm (infix to postfix conversion) supporting arithmetic operations (`+`, `-`, `*`, `/`, `%`) and conditional operators (`<`, `<=`, `>`, `>=`, `==`, `!=`).
* Embedded Runtime Console: Dual-pane interface (Input/Output) rendering inside the SFML window with scroll support.
* C++ Code Generation: Automatic graph traversal that translates visual logic into structured C++ code, complete with variable declarations, `if/else` branches, `while` loops, and proper indentation.
* Project Persistence: Custom text-based serializer for saving and loading project states, including block positions, contents, and directional connections.

## Architecture and Design

The codebase relies on Object-Oriented Programming (OOP) principles to handle rendering, user input, graph traversal, and memory safety.

### Core Class Hierarchy

All visual nodes inherit from a common abstract base class, `Block`:

```
                    ┌───────────┐
                    │   Block   │
                    └─────┬─────┘
                          │
  ┌───────────┬───────────┼───────────┬───────────┬───────────┐
  │           │           │           │           │           │
startBlock actionBlock decisionBlock inputBlock outputBlock stopBlock
```

* `Block`: Defines common properties (positioning, drawing interface, bounding boxes, text handling, dynamic anchor positioning).
* `startBlock` / `stopBlock`: Define flow entry and termination points.
* `actionBlock`: Handles variable assignments and mathematical operations (e.g., `x = a + b`).
* `decisionBlock`: Evaluates boolean expressions for branching execution (`if/else`) or loops (`while`).
* `inputBlock` / `outputBlock`: Handle user input (`cin`) and console output (`cout`).

### Connections and Memory Management

* Anchors (`anchor`): Embedded contact points on each block (Top, Right, Bottom, Left) that act as connection origins or targets.
* Arrows (`Arrow`): Managed objects that calculate segmented polylines between connected anchors, handling arrow-head rotation and click-distance detection for removal.
* Smart Pointers: Block instances (`std::unique_ptr<Block>`) and arrow connections (`std::unique_ptr<Arrow>`) are stored in standard vectors, ensuring automatic resource cleanup without manual memory leaks.

### Execution and Parsing

1. Expression Parsing: Raw strings in action or decision blocks are split into tokens, checked for declared variables, converted to postfix notation using an operator stack, and evaluated using a operand stack.
2. Code Generation: Traversal starts at `startBlock` and recursively follows `connectedToTruth` and `connectedToFalse` pointers. Loop structures (`while`) are automatically detected when a decision block's left anchor connects back into a preceding control path.

## Controls and Usage

* Place Block: Click and drag any shape from the top toolbar into the canvas area below the header line.
* Connect Blocks: Click a valid source anchor (red circle), then click a target anchor on another block to create an arrow.
* Edit Text: Double-click inside an Action, Decision, Input, or Output block to type. Press `Enter` or `Escape` to commit changes.
* Delete Element: Right-click directly on any block or arrow path to remove it.
* Run Program: Click `RUN` in the bottom HUD. If input is requested, type into the prompt in the right-side console and press `Enter`.
* Save / Load: Click `Save` to write the current canvas to a file path, or select `Open` from the main menu to restore a saved project.
