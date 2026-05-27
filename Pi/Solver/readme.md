Makefile options:

1. make all 
    Default behaviour
    input: cube_string
    output: Classic solution using begginers algorithm
2. make all RUBICO=1 
    RUBICO flag changes the output
    input: cube_string 
    output: Rubico! ready solution 
3. make all TEST=1
    TEST flag changes the input 
    input: scramble algorithm 
    output: Classic solution using begginers algorithm
4. make all DEBUGALL=1
    DEBUGALL flag changes the output 
    intput: cube_string 
    output: Triggers all debug prints for all solve stages + final classic solution using begginers algorithm

All flags can be combined to change the desired input/output format 
