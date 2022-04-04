# Project 1
  - [Project Description](https://github.com/giannhskp/Software-Development-for-Algorithmic-Problems_Project-1/blob/master/Project1_2021.pdf)
  - [Explanation of Implementation Details ](https://github.com/giannhskp/Software-Development-for-Algorithmic-Problems_Project-1/blob/master/README.txt)

## Development Team
  - Ioannis Kapetangeorgis ([giannhskp](https://github.com/giannhskp))
  - Dimitrios Sitaras ([sitaras](https://github.com/Sitaras))

## Part 1

Given a input dataset with vectors and a query dataset, for every query vector find:
  - The true nearest neighbor of the input dataset.
  - The aproximate nearest neighbor of the input dataset.
  - The aproximate N nearest neighbors of the input dataset.
  - All vectors inside a given range R. (approximate search)

In order to find the aproximate nearest neighbor(s) we use:
  - [Locality-sensitive hashing (LSH)](https://en.wikipedia.org/wiki/Locality-sensitive_hashing)
  - Projection to Hypercube

## Part 2

Vector Clustering.

The initialization of the clusters is done using kMeans++.

The assignment to each cluster can be performed by each of the following:
  - Lloyds assignment.
  - LSH reverse assignment using Range search.
  - Hypercube reverse assignment using Range search.
