# Graph Visualizer

A simple and interactive Graph Visualizer built with C++ and Qt6. It reads graph data from standard input and renders it dynamically, allowing you to drag and drop nodes.

## How to use

Build and run the proyect, through the terminal enter the graph in the following format:

```
N M
u_1 v_1
u_2 v_2
...
u_M v_M
```

where N is the number of nodes, M the number of edges and there is a edge between nodes u_i and v_i for every 1 <= i <= M.

Now, if you want to change or add to the displayed algorithm, just modify the funcion "algorithmVisualize".

## Prerequisites

To build this project, you need C++17, CMake and Qt6.