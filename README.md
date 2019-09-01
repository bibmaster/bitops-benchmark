# bitops-benchmark

```
conan remote add conan-mpusz https://api.bintray.com/conan/mpusz/conan-mpusz
```

Results:
```
-----------------------------------------------------
Benchmark              Time           CPU Iterations
-----------------------------------------------------
fwd uniform            7 ns          7 ns  100333414
fwd uniform64         12 ns         12 ns   57298989
fwd 1-digit            1 ns          1 ns 1000000000
fwd 2-digit            1 ns          1 ns  691025616
fwd 3-digit            1 ns          1 ns  520841487
fwd 4-digit            2 ns          2 ns  426908556
fwd 5-digit            2 ns          2 ns  296015856
fwd 6-digit            3 ns          3 ns  262421490
fwd 7-digit            3 ns          3 ns  231584107
fwd 8-digit            3 ns          3 ns  212016439
fwd 9-digit            4 ns          4 ns  192513640
fwd 10-digit           4 ns          4 ns  172577879

clz uniform            2 ns          2 ns  398020833
clz uniform64          5 ns          5 ns  125844052
clz 1-digit            2 ns          2 ns  395530959
clz 2-digit            2 ns          2 ns  402423121
clz 3-digit            2 ns          2 ns  385764288
clz 4-digit            2 ns          2 ns  400147561
clz 5-digit            2 ns          2 ns  403882145
clz 6-digit            2 ns          2 ns  402692770
clz 7-digit            2 ns          2 ns  402990695
clz 8-digit            2 ns          2 ns  396600940
clz 9-digit            2 ns          2 ns  402515540
clz 10-digit           2 ns          2 ns  393240786

```