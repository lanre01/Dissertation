## Results for Cache Misses
### Pop 
| DS    | L1 cache miss (%) |
--------|------------------ |
| heap  |  0.21% |
| beap  |  6.43% | 
| bst   |  4.27% |


## Push 
| DS    | L1 cache miss (%) | 
|-------|-------------------|
| heap  |  1.04% | 
| beap  |  2.44% | 
| bst   |  3.67% |


## Results for Branch Misses
### Pop 
| DS    | L1 cache miss (%) |
|-------|-------------------|
| heap  |  0.68% |
| beap  |  3.39% |
| bst   |  1.77% |


### Push 
| DS    | L1 cache miss (%)|
|-----|--------------------|
| heap  |  2.42% |
| beap  |  1.58% |
| bst   |  1.10% |


## Detailed Cache Misses for Heap and Beap 
### Push
| DS   | 256  | 512  | 1024 | 2048 | 4096 | 8192 | 16384 |
|------|------|------|------|------|------|------|-------|
| Beap | 0%   | 0%   | 0.01%| 0.02%| 0.07%| 0.18%| 1.21% |
| Heap | 0.0% | 0.0% | 0.02%| 0.02%| 0.45%| 2.33%| 2.79% |


### Pop
| DS   | 256  | 512   | 1024  | 2048  | 4096  | 8192  | 16384 |
|------|------|-------|-------|-------|-------|-------|-------|
| Beap | 0%   | 0%    | 0.01  | 0.01  | 0.03  | 0.07  | 4.05  | 
| Heap | 0.0% | 0.01% | 0.01% | 0.04% | 0.17% | 0.31% | 0.32% |


fowly - c std library re-implementation from facebook

memory 
d-ary heaps comparisons
sacrifice memory to make mapping faster