Build
------

Type "make" in command line to build 

Testing
-------
cat <input file> |./encryptUtil -n #threads -k <keyfile> ><output file>

I have tested the below scenarios.

1.Basic test input/out put validation as provided in the problem.
3. input Limit check: Tested input with invalud file and invalid thread count
2. Basic command line input + ve and -ve test cases by omitting command line parameters.
3.Data integrity check of an executable file by XOR transforming multiple times to confirm the file is created is of same.
4.Data Integrity check of an executable file by XOR transforming and transforming back to original file and compare.
5. Big file test: Tested test #4 with a 2Gb file and confirmed the output 
6.File size check: Tested with file size smaller than key size.
7.All above tests were repeated with single threads and multiple threads (n = 3, 5 ,15) and confirmed all threads are getting executed and output is same.
8. Thread synchronizationtest: Added random delays in thread execution to simulate thread delays and comfirmed the output as expected.

