# RHC2-ERHC2
Reduction through Homogeneous Clustering

Source codes in C++ for the implementation of the algorithms included in the experimental study of the paper titled "Reduction through homogeneous clustering: Variations for categorical data and fast data reduction"

**How to compile**

g++ cppfilename -o executablefilename

**How to run**

**condensing set construction:**

algorithm directory/training_file directory/condensing_file number_of_folds

example:

./RHC pd/data-tr pd/data-rhc-cs 5

**Classification:**

knn directory/training_file directory/testing_file number_of_folds number_of_nearest_neighbours

Example of conventinal k-NN (with k=1):

./knn pd/data-tr pd/data-ts 5 1

Example of RHC based kNN classifier

./knn pd/data-rhc-cs pd/data-ts 5 1
