# random-lines
Sample random lines from large files

Two c++ programs that randomly sample files in O(n) complexity (i.e., no sorting).
The caveat is that you have to precisely know how many lines are in file you want to
sample *a priori*. Still linear!

## Installation

Note: requires [boost](http://boost.org) libraries to compile.

```
  [jvierstra@test0 ~] git clone https://github.com/jvierstra/random-lines.git
  [jvierstra@test0 ~] cd random-lines
  [jvierstra@test0 ~] make install
```

## Usage

```
  [jvierstra@test0 ~] random-lines -?
  random-lines [options]
  output random lines

  Options:
    -?, --help                  display help and usage
    -v, --version               show version information
    -n, --num=1                 number of lines to return
    -N, --max=4294967295        total lines in the file
    -s, --seed=                 seed for random number generator
```

### `random-lines`

File is read in via STDIN outputs to STDOUT. You can set seed to reproduce result.
Also, note that there are ***NO*** spaces between argument flag and value.

```
  [jvierstra@test0 ~] seq 1 1000 | random-lines -n10 -N1000 -s1  
  33
  40
  692
  767
  831
  873
  930
  948
  978
  987
```

### `random-lines-pairs`

Same as above but outputs pairs of lines -- usefull for subsampling large SAM files

```

  [jvierstra@test0 ~] seq 1 1000 | random-lines-pairs -n10 -N1000 -s1
  33
  34
  39
  40
  691
  692
  763
  764
  829
  830
  869
  870
  927
  928
  959
  960
  975
  976
  985
  986
```

## Performance

This is FAST -- 1000 lines drawn from 10 million in <0.5sec

```
  [jvierstra@test0 ~] time seq 1 100000000 | random-lines -n10000 -N10000000 -s1 > /dev/null

  real	0m0.336s
  user	0m0.393s
  sys	0m0.071s
```  
