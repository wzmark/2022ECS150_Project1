# project 2 report 
## Simple Shell
### Summary
We accomplish this project by developing different phases in order. 
We first tries to run all simple commands the hard way, and then implement the 
outpur redirection, including parsing the output redirection from the command 
line and manipulate the stdout file descriptor before running the specified 
program,and in the end, implement piplines and adding some extra features.

### Simple commands the hard way
To running the shell, we first have the `ViewStart()` function as the main function
to print out shell message and get user input. The main data struct 
`SshellInput shell` will store in this function. This data struct shell will pass 
to different function for different purpose. In order to print the complete 
message based on the running result, we create another function called 
`PrintMessage()`, by reading success status of each command, this function
can use loop to form the status code of the completed message. Then we need a 
function to print error pessage, so we create a function called `ErrorHandler()`,
which is a swtich function that will be called when error occur and using 
`current` to determine error types. After we set those function to 
print different messages, we create an `ExitHandler()` function to detect whether
user input have keyword exit. If function detects return 1 and exist
in the ViewStart() function.

### Arguments
In order to maker our shell have the ability to handle command lines containing
programs and arguments, we first have the `SplitInput()` function to split 
the user input into command and argument into data struct. We create 
`listOfCommand`, which is a return value and will store struct of
commands and arguments, and we also create `numOfCommand` to store number of 
commands. After we successfully split the user input into ideal struct, we
create one execute argument function called `ExecuteDefinedCommand()`, which
executes the set up combo of command and arguments, and a helper function
 called `ExecuteCommand()`, to relocate the original argument and move comma
 argument, and call the `ExecuteBuildInCommand()`, which is a function that
 implement buildin commands in the next phase.

### Builtin Commands
We create a function called `ExecuteBuildInCommand()` to implement commands.By 
creating a pointer called `singleCommand`, which stands for commands need
to execute, including **pwd**, **cd** The functionwill check whether 
singleCommand contain these keywords. Then,  this function will use the if 
statement to determine which key word need to execute and return 1.


### Output Redirection
While we implement the output redirection, we first create the function called 
`RedirectionOutput()`, which will open file and store the data into file.
It contains a pointer called `singlecommand` that contains the command 
and argument. This function will relocate the position of the argument and add 
command in the argument at first, and then it will open the file and use pipe 
to set up `STDOUT`. After building this function, we create another function
called `RedirectionCommandHandler()` to handle special cases such as the symbol
 of `>`and add space in the both side of >. This function handles special 
case **xx>xxx**, **xx> xx** ,**xx >xx** and transform them to **xx > xx**.

### Pipeline Commands
After we implement the output redirection, we now are focusing on implementing 
pipelines. We create a function called `ExecutePipelineCommands()`, which will
apply the pipeline and will determine whether have buildinfunction
in the first place. Moreover, it sets up the pipeline and runs the pipeline
if only one command execute, and no need to setup and run pipeline, and the
parent need to wait all child process end.

### Extra Features
In the end, we need to add extra features to our shell. We first add several
commands such as **pushd**, **dirs**, **popd** to the function 
`ExecuteBuildInCommand()` we create in phase 3 in order to make a proper stack.
Furthermore, we create two inverse functions that handel with the inverse 
redirection. The first function is called `InverseRedirect()`, which helps 
open file and read the data from file. This function will relocate the position
of the argument and add command in the argument, after that, this function
will open the file and use pipe to read the data. The second function is 
called `InverseRedirectionCommandHandler()`, which will detect the symbol 
of **<** and add space in the both side of **>**. This function handles 
special case **xx< xxx**, **xx< xx**,**xx< xx** and transform them to
**xx > xx**.

## Testing Environment
### Commands Testing
To test whether commands successfully work on our shell, after we type`./sshell`, 
we first try `echo` to see whether it can print correct words, and then we try
the exit command to see if the shell will exit properly. Then we focus on 
testing builtin commands. We first usse command `pwd` to get the current 
working directory, and then type `cd..` to get to the parent directory and
type `pwd` again to see whether the directory has changed. 

### Output Redirection Testing
To test whether the `>` symbol works for our shell, we tries to test if the 
standard output redirection is indicated to a file. We echo words into
file and cat that file, to see if it prints the same words

### Piping Testing
To test whether the meta-character `|` works and allow multiple commands to 
be connected to each other and displays the correct exit value. We test two 
different pipeline cases. Given the first cases the correct filename and 
correct command, to see if it can print expected words and **exit value 1**,
and the second cases the wrong filename that doesn't exsists to check if it 
prints the **exit value 2**.

### Error Management Testing
To test whether our error massage match different error type, we test different
types of errors to see if they perfectly match each other. The first type of 
error is **parsing errors**, which the shell will only display an error message on
`stderr`, and it would not die. So we type lots of arguments in command line to
test **too many process arguments**, we type wrong command combination to test 
**missing command**, we run **echo >** to no file to test **no output file**, 
and we type wrong command direction to test **mislocated output redirection**. 
After we finishing testing the **parsing errors**, we now focus on
**launching errors**. These errors will not cause the shell to die but will
ask for a new input. We try `cd` a non-exsist file to check 
**cannot cd into directory** and we try type wrong command to check
**command not found**. 

### Extra Features Testing
To test whether we successfully build the stack, we use the same test method
as we test on **Command**, first type command **dirs** to create a stack, and 
then type command **pushd** to push a file into the stack. Then we type **pwd**
to see whether the file has been pushed. After we successfully push the file, we
type **dirs** again and type function **popd** to pop the file out of the stack. 
Then, we type **pwd** again to see if we successfully pop the file we create out 
of it. 

### Reference Program Testing
After we successfully test all the cases on csif and gets expected outputs, we 
now run the **shell_ref** to see whether our shell gets the exact same output
as the reference program. We type command **echo -e "echo date\ndate\ndate -u\
ndate-u\necho Hello world|grep Hello|wc -l\nexit\n" | /home/cs150jp/public/p1
/sshell_ref >& ref_output** to print the output of our shell, and type command
**echo -e "echo date\ndate\ndate -u\ndate-u\necho Hello world|grep Hello|wc -l
\nexit\n" | ./sshell >& your_output** to print the output of the reference 
shell. The command we type covers pipeline testing, command testing and output
redirection testing. Then we use `diff` to see whether there are some 
difference between two outputs.

### Note about the memory management
For this project, String and Struct has been malloc when program need these 
values. After each loop finished, memory for storing command and argument will 
be freed. Therefore, no memeory leak will occur in ideal way.





## Sources
[GNU Library](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Running-a-Command) 












