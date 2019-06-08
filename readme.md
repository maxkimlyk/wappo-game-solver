# What is it?

This is bruteforcer for solving [Wappo Game](https://play.google.com/store/apps/details?id=com.pinkzero.wappogame). It might be useful if you stuck in certain level.

# How to build?

First, you need Cmake and Boost to be installed.

```
wappo_game_solver$ mkdir build && cd build
wappo_game_solver/build$ cmake ..
wappo_game_solver/build$ make
```

# How to use?

Create map description file (for example, `my_map.map`) formated like `example.map` or `example2.map` and run solver by command:
```
wappo_game_solver my_map.map
```
