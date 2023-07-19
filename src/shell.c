#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

# define CMD_BUFSIZE 1024
# define TOK_BUFSIZE 64
# define TOK_DELIM   " \t\r\n\a"

int cd_sh   (char **args);
int help_sh (char **args);
int exit_sh (char **args);

char *builtin_cmds_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_cmds_func[]) (char **) = {
    &cd_sh,
    &help_sh,
    &exit_sh
};

char *readsh (void) {
    int  buffer_size = CMD_BUFSIZE;
    int  pos         = 0;
    char *buffer     = malloc (sizeof (char) * buffer_size);
    int  c;          // 'char' stored in 'int' just to handle EOF (as it's int, not char)
                     //  EOF : end-of-file or ^D

    if (!buffer) {
        fprintf (stderr, "sh: buffer allocation error\n");
        exit (EXIT_FAILURE);
    }

    while (1) {
        /* READ  */
        c = getchar ();

        /* STORE */
        if (c == EOF || c == '\n') {
            buffer[pos] = '\0';
            return buffer;
        } else {
            buffer[pos] = c;
        } pos++;

        /* REALLOCATE */
        if (pos >= buffer_size) {
            buffer_size += CMD_BUFSIZE;
            buffer = realloc (buffer, buffer_size);
            if (!buffer) {
                fprintf (stderr, "sh: buffer reallocation error\n");
                exit (EXIT_FAILURE);
            }
        }
    }
}

char **parsesh (char *line) {
    int buffer_size = TOK_BUFSIZE;
    int pos = 0;
    char **tokens = malloc (sizeof (char*) * buffer_size);
    char *token;

    if (!tokens) {
        fprintf (stderr, "sh: tokens allocation error\n");
        exit (EXIT_FAILURE);
    }

    /* READ */
    token = strtok (line, TOK_DELIM);

    while (token != NULL) {
        /* STORE */
        tokens[pos] = token;
        pos++;

        /* REALLOCATE */
        if (pos >= buffer_size) {
            buffer_size += TOK_BUFSIZE;
            tokens = realloc (tokens, buffer_size * sizeof (char*));
            if (!tokens) {
                fprintf (stderr, "sh: tokens reallocation error\n");
                exit (EXIT_FAILURE);
            }
        }

        /* READ */
        token = strtok (NULL, TOK_DELIM);
    }
    tokens[pos] = NULL;
    return tokens;
}

int coresh (char **args) {
    pid_t pid;
    // int status;

    pid = fork ();
    if (pid == 0) {
        /* CHILD */ 
        /* 
            execvp :
                - takes the name of the program to be executed as its first argument
                - takes an array of strings as its second argument
                - the array of strings must be terminated by a NULL pointer
                - if it returns (-1), it must have failed
         */
        if (execvp (args[0], args) == -1) {
            perror ("sh: execvp error");
            exit (EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror ("sh: fork error");
    } else {
        /* PARENT */ 
        wait(NULL);
    }

    return 1;
}

int cd_sh (char **args) {
    if (args[1] == NULL) {
        fprintf (stderr, "sh: expected arguments");
    } else {
        if (chdir (args[1]) != 0) {
            perror ("sh: could not change the directory");
        }
    }
    return 1;
}

int help_sh (char **args) {
    printf ("/*************************BASH SHELL*************************/\n");
    printf (" *              Developed by  : Haseeb Hijazi Khan          * \n");
    printf (" *              Language used : C                           * \n");
    printf (" *              Submitted to  : Dr. Janib ul Bashir         * \n");
    printf (" *              Course        : Operating Systems           * \n");
    printf ("/************************************************************/\n");
    printf (">> This shell handles all the bash commands.\n");
    printf (">> 'exit' to exit the shell.\n");
    printf (">> 'cd' to change the directory.\n");
    printf (">> 'man' to get information on other programs.\n");
}

int exit_sh (char **args) {
    return 0;
}

int execsh (char **args) {
    // empty command
    if (args[0] == NULL) {
        return 1;
    }

    // check for builtin commands
    int num_builtins = sizeof (builtin_cmds_str) / sizeof (char*);
    for (int i=0; i<num_builtins; i++) {
        if (strcmp (args[0], builtin_cmds_str[i]) == 0) {
            return (*builtin_cmds_func[i]) (args);
        }
    }

    // execute other bash commands
    return coresh (args);
}

void prompt (void) {
    char cwd[PATH_MAX];
    printf ("%s> ", getcwd (cwd, sizeof (cwd)));
}

void loopsh (void) {
    char  *line;
    char **args;
    int  status;

    do {
        prompt ();

        line   = readsh ();
        args   = parsesh (line);
        status = execsh (args);

        /* CLEANUP */
        free (line);
        free (args);
    } while (status);
}

int main () {
    loopsh ();

    return EXIT_SUCCESS;
}