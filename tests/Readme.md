
# Testing in Cairn


Cairn Open is tested by three types of tests:

- Models, PrivateTests/Models: very simple tests to focus on each component and test its options. 
- apicpp, apipython: test of C++ and Python API commands
- PrivateTests/advanced_scripts: Test of external scripts
- Integration: Test of study that we want to be able to reproduce in the future.
- PrivateTests/Pegase: tests that use co-simulation features of Pegase.

Adding tests in Cairn:
1. Each new developpment should be associated to a new test.
1. If a development is solving a bug, the new test has to be failing in the previous Cairn version and success in the new. Ideally, the test is made on the basis on the case given by the user in the bug tracker.
1. The tests have to be as simple as possible: in term of memory and time of computation:
    * store only a part of the results usefull for the test,
    * shorten the time horizon.


How to use Cairn Test:
1. Develop in your own branch.
1. Add a test for your development. 
1. Merge with the current integration branch into your branch.
1. Test your branch via Jenkins
1. **Only** if the test succeeds: do a pull request or merge into integration.

What should I do if some tests fail ?
1. Check and solve the tests by order of complexity:
    1. Models, PrivateTests/Models
    1. apicpp, apipython
    1. PrivateTests/advanced_scripts
    1. Integration
1. If a test has very small changes, it is possible to update it.
1. Don't hesitate to contact the development team. 

Models
---------

Models folder contains minimal studies to test components, and their options.

To model, add a folder with the following files:
- [study].json
- [study]_dataseries.csv

and if necessary a file for sampling (to test some options of the model):
- sampling.csv


The following tests will be conducted:

1. Launch the study
2. Compare the PLAN and HIST file. Write the difference in the file. The differences are summed up in the file report_testing.
3. Compare the dataseries results file. If they are different, print the graphs of differences in the folder [study]_NRT.
4. Compare the lp files on short time horizon (10 timesteps). 
5. Lauch sensitivity and compare a selection of KPIs in the results. 


Add/Modify a test
--------------------



Generate the reference results by using the python script:
```
python generate_refs
```

Commit the study files, and the refs. 




