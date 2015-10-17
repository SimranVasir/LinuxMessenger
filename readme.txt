the program takes 3 command lines receiving port # localhost sending port #
For example:
./stalk 80 localhost 90

The other person running the program will reverse the port numbers
For exmaple:
./stalk 90 localhost 80

then communication can begin

When one user types '!" on the terminal his program will terminate. 
The other user's program will continue to run (but won't be able to communicate anymore) until that user also types '!'. 

I used the beej's Guide and Posix threads guide as an outline therefore some of my code might be similar to the code on those two guides.