# NCKRL

**NCKRL**, negative triple confidence for knowledge representation learning, is an extended model of TransE and enhanced version of CKRL. The concept of negative triple confidence can generate high quality negative triplets to some extent, which were used to support model training for detecting noises in knowledge graph, while learning robust knowledge representations simultaneously.

## Data

- FB15K
- WN18

## Compile

```
g++ -std=c++11 -o nckrl main.cpp Train.cpp Test.cpp
```

or

just type make in the folder ./

## Run

for training and testing just type:

```
./nckrl

command line options:
-e: training epochs as: -e number, default is set to 1000
-d: dimension of entities and relations as: -d number, default is set to 50
-r: margin as: -r number, default is set to 1
-l: learning rate as: -l decimal, default is set to 0.001
-m: method as: -m [bern|unif], default is set to bern
-s: data set as: -s [FB15K|WN18], default is set to FB15K
-n: noise rate as: -n [10|20|40], default is set to 10%
-c: number of negative candidates as: -c number, default is set to 20
```

