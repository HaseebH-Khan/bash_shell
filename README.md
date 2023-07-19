## Description
Bash Shell developed in C which features all unix commands.
Tokenization based on quotation as well.
Allows multiple commands using piping and redirections.
Handles signals and interrupts.

## Clone SSH
git clone .link goes here.

## Compile
```bash
gcc -o shell shell.c
```


## Run
```bash
./shell
```

## Workflow

### Reading Commands
In C, a fixed memory block is allocated for the command. If it exceeds, we reallocate it in a larger memory block (BUFSIZE extra char).
Tokenization uses same technique however with null-terminated array of pointers.

### Parsing Commands
- Used standard delimeters for strtok tokenizing function. 
- Quotation, backslashing, pipiing, redirect, gobling etc to be handled <====

### Builtins Provided
All builtin commands are in builtin_cmds_str.
`cd` cannot be simply executed by fork/exec as the directory change has to happen in the parent process itself & fork/exec only changes the state of child process. So, we use `chdir` system call.
