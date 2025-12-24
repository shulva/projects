```shell
.
├── assignments # documents and starter codes of programming assigments
│   ├── GapBuffer
│   ├── HashMap
│   └── KDTree
├── README.md
```

# Programing Assignments
Programming assignments are the most important and valuable part of this course, which guide you to create STL-compliant container classes from scratch. The implementation of these classes refers to most C++ grammars and features introduced in the lecture. By completing these assignments, you will better understand and master what taught in this class and improve your C++ programming skills.

This repo has collected programming assignments released from three different semesters of this course, including HashMap, KDTree and GapBuffer. Each assignment is accompanied by comprehensive materials, including documents, starter codes, benchmarks, build scripts, and solutions.

- [GapBuffer](./assignments/GapBuffer/): sequential container optimized for fast inserttion and deletion at any position;
- [KDTree](./assignments/KDTree/): k-dimension binary search tree for efficient storage and query of multi-dimension data;
- [HashMap](./assignments/HashMap): associative container similiar to the [unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) provided in STL;

The original versions of these programming assignments were built based on [QtCreator](https://www.qt.io/product/development-tools), which requires tedious and cumbersome process to setup development environment for self-learners. This repo has refactored the build and test process of each assignment using [CMake](https://cmake.org/) and [GoogleTest](https://github.com/google/googletest) for the convenience of development on Linux machines. And you only need to ensure that your Linux machine has installed CMake and C++ compiler before getting started with these assignments. To setup development environemnt on Ubuntu systems, you may refer to following commands:

```shell
sudo apt-get update
sudo apt-get install build-essential libssl-dev cmake -y
```

# Related Resources
All contents in this repo are sourced from online resources inclduing:
- CS106L Winter 2018: https://web.stanford.edu/class/archive/cs/cs106l/cs106l.1184/assignments.html
- CS106L Winter 2020: https://web.stanford.edu/class/archive/cs/cs106l/cs106l.1204/index.html
- CS106L Spring 2022: https://web.stanford.edu/class/archive/cs/cs106l/cs106l.1226/
- CS106L Spring 2023: https://web.stanford.edu/class/cs106l/
- Other GitHub Repos: https://github.com/PKUFlyingPig/CS106L
