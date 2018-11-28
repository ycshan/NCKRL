# NCKRL

**NCKRL**, negative triple confidence for knowledge representation learning, is an extended model of TransE and enhanced version of CKRL. The concept of negative triple confidence can generate high quality negative triplets to some extent, which were used to support model training for detecting noises in knowledge graph, while learning robust knowledge representations simultaneously.

## Data

- FB15K

  | Dataset | #rel  | #ent   | #train  | #valid | #test  |
  | ------- | ----- | ------ | ------- | ------ | ------ |
  | FB15K   | 1,345 | 14,951 | 483,142 | 50,000 | 59,071 |

  Different ratios of negatives on FB15K.

  | Dataset      | FB15K-N1 | FB15K-N2 | FB15K-N3 |
  | ------------ | -------- | -------- | -------- |
  | #neg triples | 46,408   | 93,782   | 187,925  |

- WN18

  | Dataset | #rel | #ent   | #train  | #valid | #test |
  | ------- | ---- | ------ | ------- | ------ | ----- |
  | WN18    | 18   | 40,943 | 141,442 | 5,000  | 5,000 |

  Different ratios of negatives on WN18. Following the practice of generating noises on FB15K, we construct WN18-N1, WN18-N2 and WN18-N3.

  | Dataset      | WN18-N1 | WN18-N2 | WN18-N3 |
  | ------------ | ------- | ------- | ------- |
  | #neg triples | 14,144  | 28,289  | 56,577  |

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

