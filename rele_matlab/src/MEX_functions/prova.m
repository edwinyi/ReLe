clear all;
pathToReLe = '/home/matteo/Projects/github/ReLe/rele/';
mexCmd = ['mex -v -g GCC=''/usr/local/bin/gcc4.7'' CXX=''/usr/local/bin/g++4.7'' ' ...
    ' -DARMA_DONT_USE_CXX11' ...
    ' -I' pathToReLe 'include/rele/core' ...
                    ' -I' pathToReLe 'include/rele/policy' ...
                    ' -I' pathToReLe 'include/rele/utils' ...
                    ' -I' pathToReLe 'include/rele/generators' ...
                    ' -I' pathToReLe 'include/rele/algorithms' ...
                    ' -I' pathToReLe 'include/rele/envirorments' ...
                    ' -I' pathToReLe 'include/rele/approximators' ...
                    ' -I' pathToReLe 'include/rele/statistics' ...
                    ' -I' pathToReLe 'include/rele/IRL' ...
' /home/matteo/Projects/github/ReLe/rele-build/librele.a ' ...
' -largeArrayDims' ...
' -DARMA_BLAS_LONG' ...
' -lmwlapack -lmwblas -larmadillo' ...
' collectSamples.cpp'];
clc;
eval(mexCmd);


%%
clc
mexCmd = ['mex -v -g GCC=''/usr/local/bin/gcc4.7'' CXX=''/usr/local/bin/g++4.7'' ' ...
    ' -DARMA_DONT_USE_CXX11' ...
    ' -largeArrayDims' ...
' -DARMA_BLAS_LONG' ...
' -lmwlapack -lmwblas -larmadillo' ...
' testCaseArma.cpp'];
eval(mexCmd)

% testCaseArma