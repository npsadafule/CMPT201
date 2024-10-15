#include "msgs.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define HISTORY_SIZE 10

char prev_directory[BUFFER_SIZE];

// History variables
char history[HISTORY_SIZE][BUFFER_SIZE];
int command_numbers[HISTORY_SIZE];
int total_commands = 0;

// Function to handle the shell prompt
void print_prompt() {
  char cwd[BUFFER_SIZE];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    write(STDERR_FILENO, FORMAT_MSG("shell", GETCWD_ERROR_MSG),
          strlen(FORMAT_MSG("shell", GETCWD_ERROR_MSG)));
  } else {
    write(STDOUT_FILENO, cwd, strlen(cwd));
    write(STDOUT_FILENO, "$ ", 2);
  }
}

// Function to add a command to history
void add_to_history(const char *command) {
  if (strlen(command) == 0)
    return; // Ignoring empty commands
  int index = total_commands % HISTORY_SIZE;
  strncpy(history[index], command, BUFFER_SIZE);
  command_numbers[index] = total_commands;
  total_commands++;
}

void display_history() {
  int start =
      (total_commands > HISTORY_SIZE) ? total_commands - HISTORY_SIZE : 0;
  char output[BUFFER_SIZE];

  for (int i = total_commands - 1; i >= start; i--) {
    int index = i % HISTORY_SIZE;
    snprintf(output, BUFFER_SIZE, "%d\t%s\n", command_numbers[index],
             history[index]);
    write(STDOUT_FILENO, output, strlen(output));
  }
}

// Function to clean up zombie processes
void clean_zombie_processes() {
  int status;
  pid_t pid;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    // Loop to reap all zombie processes
  }
}

// Signal handler for SIGINT (Ctrl+C)
void handle_sigint(int sig) {
  // Print help info
  write(STDOUT_FILENO, "exit: exit the shell\n", 21);
  write(STDOUT_FILENO, "pwd: print the current directory\n", 34);
  write(STDOUT_FILENO, "cd: change the current directory\n", 34);
  write(STDOUT_FILENO, "history: display the last 10 commands\n", 38);
  write(STDOUT_FILENO, "help: Displays help information on internal commands\n",
        53);
  write(STDOUT_FILENO, "$ ", 2);
  print_prompt(); // Redisplay prompt after Ctrl+C
}

// Function to handle internal commands
int handle_internal_command(char **args, const char *original_command) {
  // add_to_history(original_command);

  // Exit command
  if (strcmp(args[0], "exit") == 0) {
    if (args[1] != NULL) {
      write(STDERR_FILENO, FORMAT_MSG("exit", TMA_MSG),
            strlen(FORMAT_MSG("exit", TMA_MSG)));
    } else {
      exit(0);
    }
    return 1;
  }

  // pwd command
  if (strcmp(args[0], "pwd") == 0) {
    if (args[1] != NULL) {
      write(STDERR_FILENO, FORMAT_MSG("pwd", TMA_MSG),
            strlen(FORMAT_MSG("pwd", TMA_MSG)));
    } else {
      char cwd[BUFFER_SIZE];
      if (getcwd(cwd, sizeof(cwd)) == NULL) {
        write(STDERR_FILENO, FORMAT_MSG("pwd", GETCWD_ERROR_MSG),
              strlen(FORMAT_MSG("pwd", GETCWD_ERROR_MSG)));
      } else {
        write(STDOUT_FILENO, cwd, strlen(cwd));
        write(STDOUT_FILENO, "\n", 1);
      }
    }
    return 1;
  }

  // cd command
  if (strcmp(args[0], "cd") == 0) {
    if (args[1] == NULL || strcmp(args[1], "~") == 0) {
      const char *home = getenv("HOME");
      if (home == NULL) {
        struct passwd *pw = getpwuid(getuid());
        home = pw->pw_dir;
      }
      strcpy(prev_directory, getenv("PWD"));
      if (chdir(home) < 0) {
        write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
              strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
      }
    } else if (strcmp(args[1], "-") == 0) {
      char temp[BUFFER_SIZE];
      strcpy(temp, prev_directory);
      strcpy(prev_directory, getenv("PWD"));
      if (chdir(temp) < 0) {
        write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
              strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
      }
    } else if (args[2] != NULL) {
      write(STDERR_FILENO, FORMAT_MSG("cd", TMA_MSG),
            strlen(FORMAT_MSG("cd", TMA_MSG)));
    } else if (args[1][0] == '~') {
      const char *home = getenv("HOME");
      if (home == NULL) {
        struct passwd *pw = getpwuid(getuid());
        home = pw->pw_dir;
      }
      char path[BUFFER_SIZE];
      snprintf(path, BUFFER_SIZE, "%s%s", home, args[1] + 1);
      strcpy(prev_directory, getenv("PWD"));
      if (chdir(path) < 0) {
        write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
              strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
      }
    } else {
      strcpy(prev_directory, getenv("PWD"));
      if (chdir(args[1]) < 0) {
        write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
              strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
      }
    }
    return 1;
  }

  // Help command
  if (strcmp(args[0], "help") == 0) {
    if (args[1] == NULL) {
      write(STDOUT_FILENO, "exit: exit the shell\n", 21);
      write(STDOUT_FILENO, "pwd: print the current directory\n", 34);
      write(STDOUT_FILENO, "cd: change the current directory\n", 34);
      write(STDOUT_FILENO, "history: display the last 10 commands\n", 38);
      write(STDOUT_FILENO,
            "help: Displays help information on internal commands\n", 53);
    } else if (args[2] != NULL) {
      write(STDERR_FILENO, FORMAT_MSG("help", TMA_MSG),
            strlen(FORMAT_MSG("help", TMA_MSG)));
    } else {
      if (strcmp(args[1], "exit") == 0) {
        write(STDOUT_FILENO, "exit: exit the shell\n", 21);
      } else if (strcmp(args[1], "pwd") == 0) {
        write(STDOUT_FILENO, "pwd: print the current directory\n", 34);
      } else if (strcmp(args[1], "cd") == 0) {
        write(STDOUT_FILENO, "cd: change the current directory\n", 34);
      } else if (strcmp(args[1], "history") == 0) {
        write(STDOUT_FILENO, "history: display the last 10 commands\n", 38);
      } else if (strcmp(args[1], "help") == 0) {
        write(STDOUT_FILENO,
              "help: Displays help information on internal commands\n", 53);
      } else {
        char formatted_message[BUFFER_SIZE];
        snprintf(formatted_message, BUFFER_SIZE, "%s: %s\n", args[1],
                 EXTERN_HELP_MSG);
        write(STDOUT_FILENO, formatted_message, strlen(formatted_message));
      }
    }
    return 1;
  }

  // History command (display recent 10 commands)
  if (strcmp(args[0], "history") == 0) {
    display_history();
    return 1;
  }

  return 0; // Not an internal command
}

// Function to handle "!!" and "!n"
int handle_history_commands(char *buffer) {
  if (strcmp(buffer, "!!") == 0) {
    if (total_commands == 0) {
      write(STDERR_FILENO, FORMAT_MSG("history", HISTORY_NO_LAST_MSG),
            strlen(FORMAT_MSG("history", HISTORY_NO_LAST_MSG)));
      return -1;
    } else {
      int index = (total_commands - 1) % HISTORY_SIZE;
      char output[BUFFER_SIZE];
      snprintf(output, BUFFER_SIZE, "%s\n", history[index]);
      write(STDOUT_FILENO, output, strlen(output));
      snprintf(buffer, BUFFER_SIZE, "%s",
               history[index]); // copying the last command to buffer
      add_to_history(buffer);
      return 1;
    }
  } else if (buffer[0] == '!' && isdigit(buffer[1])) {
    int n = atoi(&buffer[1]);
    if (n >= total_commands || (n < total_commands - HISTORY_SIZE) || n < 0) {
      write(STDERR_FILENO, FORMAT_MSG("history", HISTORY_INVALID_MSG),
            strlen(FORMAT_MSG("history", HISTORY_INVALID_MSG)));
      return -1;
    } else {
      int index = n % HISTORY_SIZE;
      char output[BUFFER_SIZE];
      snprintf(output, BUFFER_SIZE, "%s\n", history[index]);
      write(STDOUT_FILENO, output, strlen(output));
      snprintf(buffer, BUFFER_SIZE, "%s", history[index]);
      add_to_history(buffer);
      return 1;
    }
  } else if (buffer[0] == '!') {
    write(STDERR_FILENO, FORMAT_MSG("history", HISTORY_INVALID_MSG),
          strlen(FORMAT_MSG("history", HISTORY_INVALID_MSG)));
    return -1;
  }
  return 0;
}

// Main function to run the shell
int main() {
  char buffer[BUFFER_SIZE];
  char original_buffer[BUFFER_SIZE];
  char command_to_add[BUFFER_SIZE];
  char command_for_parsing[BUFFER_SIZE];
  char *args[64];
  char *token, *saveptr;
  pid_t pid;
  int background;

  signal(SIGINT, handle_sigint);

  while (1) {
    print_prompt(); // Print the shell prompt

    // Read user input
    ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
      write(STDERR_FILENO, FORMAT_MSG("shell", READ_ERROR_MSG),
            strlen(FORMAT_MSG("shell", READ_ERROR_MSG)));
      continue;
    }

    // Null terminate the input string and remove the newline character
    buffer[bytes_read] = '\0';
    buffer[strcspn(buffer, "\n")] = '\0';

    // Save the original buffer before any modifications
    strncpy(original_buffer, buffer, BUFFER_SIZE);

    // Check for "!!" or "!n" commands
    int history_result = handle_history_commands(buffer);
    if (history_result == -1) {
      continue;
    }

    // Determine which command to add to history
    if (history_result == 1) {
      strncpy(command_to_add, buffer, BUFFER_SIZE);
    } else {
      strncpy(command_to_add, original_buffer, BUFFER_SIZE);
    }

    // Add command to history
    if (strcmp(original_buffer, "!!") != 0 && original_buffer[0] != '!') {
      add_to_history(command_to_add);
    }

    // copy command_to_add to command_for_parsing for tokenization
    strncpy(command_for_parsing, command_to_add, BUFFER_SIZE);

    // Parse the command and arguments
    int i = 0;
    background = 0;
    token = strtok_r(command_for_parsing, " ", &saveptr);
    while (token != NULL) {
      args[i++] = token;
      token = strtok_r(NULL, " ", &saveptr);
    }
    args[i] = NULL;

    if (i == 0) {
      continue; // If no command was entered, prompt again
    }

    // Check for internal commands
    if (handle_internal_command(args, original_buffer)) {
      continue; // Skip forking if it is an internal command
    }

    // Check if command should be run in the background
    if (strcmp(args[i - 1], "&") == 0) {
      background = 1;
      args[i - 1] = NULL;
      strcat(command_to_add, " &");
    }

    // Fork a child process to execute the command
    pid = fork();
    if (pid < 0) {
      write(STDERR_FILENO, FORMAT_MSG("shell", FORK_ERROR_MSG),
            strlen(FORMAT_MSG("shell", FORK_ERROR_MSG)));
    } else if (pid == 0) {
      // Child process
      if (execvp(args[0], args) < 0) {
        write(STDERR_FILENO, FORMAT_MSG("shell", EXEC_ERROR_MSG),
              strlen(FORMAT_MSG("shell", EXEC_ERROR_MSG)));
        exit(EXIT_FAILURE);
      }
    } else {
      // Parent process
      if (!background) {
        // Foreground: wait for the child process to finsh
        if (waitpid(pid, NULL, 0) < 0) {
          write(STDERR_FILENO, FORMAT_MSG("shell", WAIT_ERROR_MSG),
                strlen(FORMAT_MSG("shell", WAIT_ERROR_MSG)));
        }
      } else {
        // Background: do not wait for the child
        clean_zombie_processes();
      }
    }
  }

  return 0;
}
