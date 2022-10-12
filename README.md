# 2022ECS150_Project1
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
 of `>`and add space in the both side of `>`. This function handles special 
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
of `<` and add space in the both side of `>`. This function handles 
special case **xx< xxx**, **xx< xx**,**xx< xx** and transform them to
**xx > xx**.

## Sources
[GNU Library](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Running-a-Command) 












