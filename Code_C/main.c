#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/wait.h>

// Function prototypes
void handle_main_menu();
void handle_key_option();
void handle_encrypt_decrypt();
void handle_crack_code();
void handle_help();
void execute_program(char *program, char *args[]);
void list_files(WINDOW *win, const char *folder, char *selected_file);
void list_keys_from_file(WINDOW *win, const char *file_path, char *selected_key);
void prompt_input(WINDOW *win, const char *prompt, char *input);
void choose_key(WINDOW *win, const char *key_file, char *selected_key);
void choose_method(WINDOW *win, char *selected_method);
void execute_sym_crypt(const char *input_file, const char *output_file, const char *key, const char *method);
void execute_crack_code(const char *input_file, const char *output_file, const char *key_length, const char *dictionary);

int main() {
    // Start ncurses mode
    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    handle_main_menu();

    // End ncurses mode
    endwin();
    return 0;
}

void handle_main_menu() {
    char *choices[] = {"help", "key_option", "encrypt/decrypt", "crack_code", "exit"};
    int n_choices = 5;
    int highlight = 1, choice = 0, c;
    WINDOW *menu_win = newwin(15, 50, (LINES - 15) / 2, (COLS - 50) / 2);
    keypad(menu_win, TRUE);  // Enable arrow keys for the menu window
    box(menu_win, 0, 0);
    refresh();

    while (1) {
        werase(menu_win);
        box(menu_win, 0, 0);
        mvwprintw(menu_win, 1, 2, "Welcome to Crypto_Sym! Choose an option:");

        for (int i = 0; i < n_choices; i++) {
            if (highlight == i + 1)
                wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, 3 + i, 4, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        }
        wrefresh(menu_win);

        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_choices;
                else
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight == n_choices)
                    highlight = 1;
                else
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            default:
                break;
        }

        if (choice != 0) {
            switch (choice) {
                case 1: handle_help(); break;
                case 2: handle_key_option(); break;
                case 3: handle_encrypt_decrypt(); break;
                case 4: handle_crack_code(); break;
                case 5:
                    werase(menu_win);
                    mvwprintw(menu_win, 2, 2, "Exiting program...");
                    box(menu_win, 0, 0);
                    wrefresh(menu_win);
                    getch();
                    delwin(menu_win);
                    return;
            }
            choice = 0;
        }
    }
}


void handle_key_option() {
    char *choices[] = {"gen-key", "del-key", "list-key", "return"};
    int n_choices = 4;
    int highlight = 1, choice = 0, c;

    WINDOW *menu_win = newwin(15, 50, (LINES - 15) / 2, (COLS - 50) / 2);
    keypad(menu_win, TRUE);  // Enable arrow keys for the menu window
    box(menu_win, 0, 0);
    refresh();

    while (1) {
        werase(menu_win);
        box(menu_win, 0, 0);
        mvwprintw(menu_win, 1, 2, "Key Options:");
        for (int i = 0; i < n_choices; i++) {
            if (highlight == i + 1)
                wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, 3 + i, 4, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        }
        wrefresh(menu_win);

        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_choices;
                else
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight == n_choices)
                    highlight = 1;
                else
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            case KEY_BACKSPACE:  // Backspace key
                delwin(menu_win);
                return;  // Return to the main menu
            default:
                break;
        }

        if (choice != 0) {
            if (choice == 4) { // Return to the main menu
                delwin(menu_win);
                return;
            }

            char input[256] = {0};  // Buffer for user input
            char selected_key[256] = {0};  // Buffer for selected key

            if (choice == 1) { // gen-key
                prompt_input(menu_win, "Enter key length:", input);
                if (strlen(input) == 0) {
                    handle_main_menu();
                }

                char *args[] = {"./src/Key/key", "-g", input, NULL};
                execute_program("./src/Key/key", args);

            } else if (choice == 2) { // del-key
                list_keys_from_file(menu_win, "Util/Key/keys.txt", selected_key);
                if (strlen(selected_key) == 0) {
                    handle_main_menu();
                }

                char *args[] = {"./src/Key/key", "-d", selected_key, NULL};
                execute_program("./src/Key/key", args);

            } else if (choice == 3) { // list-key
                list_keys_from_file(menu_win, "Util/Key/used_keys.txt", NULL);
                handle_main_menu();
            }

            choice = 0;
        }
    }
}


void handle_encrypt_decrypt() {
    char *choices[] = {"encrypt", "decrypt", "return"};
    int n_choices = 3;
    int highlight = 1, choice = 0, c;

    WINDOW *menu_win = newwin(15, 50, (LINES - 15) / 2, (COLS - 50) / 2);
    keypad(menu_win, TRUE);  // Enable arrow keys for the menu window
    box(menu_win, 0, 0);
    refresh();

    while (1) {
        werase(menu_win);
        box(menu_win, 0, 0);
        mvwprintw(menu_win, 1, 2, "Encrypt/Decrypt Options:");
        for (int i = 0; i < n_choices; i++) {
            if (highlight == i + 1)
                wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, 3 + i, 4, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        }
        wrefresh(menu_win);

        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_choices;
                else
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight == n_choices)
                    highlight = 1;
                else
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            case KEY_BACKSPACE:  // Backspace key
                delwin(menu_win);
                return;  // Return to main menu
            default:
                break;
        }

        if (choice != 0) {
            if (choice == 3) { // Return
                delwin(menu_win);
                return;
            }

            char input_file[150] = {0}, output_file[150] = {0}, key[100] = {0}, method[150] = {0};
            char temp_output_file[256];
            if (choice == 1) {
                list_files(menu_win, "/home/hiraichi/work/Projet_Group_14/Code_C/Datas/test_file", input_file);
                if (strlen(input_file) == 0) {
                    handle_main_menu();
                }
                snprintf(temp_output_file, sizeof(temp_output_file), "/home/hiraichi/work/Projet_Group_14/Code_C/Datas/test_file/%s", input_file);
                strncpy(input_file, temp_output_file, sizeof(input_file));

                prompt_input(menu_win, "Enter output filename:", output_file);
                if (strlen(output_file) == 0) {
                    handle_main_menu();
                }

                snprintf(temp_output_file, sizeof(temp_output_file), "/home/hiraichi/work/Projet_Group_14/Code_C/Datas/encrypt_file/%s", output_file);
                strncpy(output_file, temp_output_file, sizeof(output_file));

            } else if (choice == 2) {
                list_files(menu_win, "/home/hiraichi/work/Projet_Group_14/Code_C/Datas/encrypt_file", input_file);
                if (strlen(input_file) == 0) {
                    handle_main_menu();
                }
                snprintf(temp_output_file, sizeof(temp_output_file), "/home/hiraichi/work/Projet_Group_14/Code_C/Datas/encrypt_file/%s", input_file);
                strncpy(input_file, temp_output_file, sizeof(input_file));

                prompt_input(menu_win, "Enter output filename:", output_file);
                if (strlen(output_file) == 0) {
                    handle_main_menu();
                }

                snprintf(temp_output_file, sizeof(temp_output_file), "/home/hiraichi/work/Projet_Group_14/Code_C/Datas/decrypt_file/%s", output_file);
                strncpy(output_file, temp_output_file, sizeof(output_file));
            }

            choose_key(menu_win, "/home/hiraichi/work/Projet_Group_14/Code_C/Util/Key/keys.txt", key);
            if (strlen(key) == 0) {
                handle_main_menu();
            }

            choose_method(menu_win, method);
            if (strlen(method) == 0) {
                handle_main_menu();
            }
            if (strcmp(method, "cbc") == 0) {
                if (choice == 1) {
                    strcpy(method, "cbc-crypt");
                } else {
                    strcpy(method, "cbc-uncrypt");
                }
            }

            execute_sym_crypt(input_file, output_file, key, method);
            choice = 0;
        }
    }
}
void handle_crack_code() {
    char input_file[150] = {0};
    char output_file[150] = {0};
    char key_length[10] = {0};
    char dictionary[150] = {0};

    WINDOW *win = newwin(15, 50, (LINES - 15) / 2, (COLS - 50) / 2);
    keypad(win, TRUE);  // Enable arrow keys for the menu window
    box(win, 0, 0);
    refresh();

    char temp_output_file[256];
    list_files(win, "/home/hiraichi/work/Projet_Group_14/Code_C/Datas/encrypt_file", input_file);
    if (strlen(input_file) == 0) {
        handle_main_menu();
    }
    snprintf(temp_output_file, sizeof(temp_output_file), "/home/hiraichi/work/Projet_Group_14/Code_C/Datas/encrypt_file/%s", input_file);
    strncpy(input_file, temp_output_file, sizeof(input_file));

    prompt_input(win, "Enter output filename:", output_file);
    if (strlen(output_file) == 0) {
        handle_main_menu();
    }
    snprintf(temp_output_file, sizeof(temp_output_file), "/home/hiraichi/work/Projet_Group_14/Code_C/Datas/crack_file/%s", output_file);
    strncpy(output_file, temp_output_file, sizeof(output_file));
    prompt_input(win, "Enter key length:", key_length);
    if (strlen(key_length) == 0) {
        handle_main_menu();
    }
    list_files(win, "/home/hiraichi/work/Projet_Group_14/Code_C/Util/Dictionary", dictionary);
    if (strlen(dictionary) == 0) {
        handle_main_menu();
    }
    snprintf(temp_output_file, sizeof(temp_output_file), "/home/hiraichi/work/Projet_Group_14/Code_C/Util/Dictionary/%s", dictionary);
    strncpy(dictionary, temp_output_file, sizeof(dictionary));
   
    execute_crack_code(input_file, output_file, key_length, dictionary);

    werase(win);
    mvwprintw(win, 2, 2, "Operation completed. Press any key to return.");
    box(win, 0, 0);
    wrefresh(win);
    wgetch(win);
    delwin(win);
    
}


void handle_help() {
    endwin();
    system("less README.md");
    refresh();
}
void execute_program(char *program, char *args[]) {
    endwin(); // Exit ncurses mode before executing the program
    if (execvp(program, args) == -1) {
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
}
void list_files(WINDOW *win, const char *folder, char *selected_file) {
    struct dirent *entry;
    DIR *dir = opendir(folder);
    if (!dir) {
        werase(win);
        mvwprintw(win, 2, 2, "Cannot open directory: %s", folder);
        box(win, 0, 0);
        wrefresh(win);
        wgetch(win);
        return;
    }

    char *files[100];
    int count = 0;

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') { // Ignore hidden files
            files[count] = strdup(entry->d_name);  // Dynamically allocate memory for filename
            count++;
        }
    }
    closedir(dir);

    // Initialize selection variables
    int highlight = 1, choice = 0, c;
    while (1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "Select a file:");
        for (int i = 0; i < count; i++) {
            if (highlight == i + 1)
                wattron(win, A_REVERSE);
            mvwprintw(win, 3 + i, 4, "%s", files[i]);
            wattroff(win, A_REVERSE);
        }
        wrefresh(win);

        // Handle input
        c = wgetch(win);
        switch (c) {
            case KEY_UP:  // Navigate up
                if (highlight == 1)
                    highlight = count;
                else
                    highlight--;
                break;
            case KEY_DOWN:  // Navigate down
                if (highlight == count)
                    highlight = 1;
                else
                    highlight++;
                break;
            case 10:  // Enter key
                choice = highlight;  // File is selected
                break;
            case KEY_BACKSPACE:  // Backspace (as returned by your terminal)
                delwin(win);
                return;  // Return without modifying `selected_file`
            default:
                break;
        }

        if (choice != 0) {
            // Copy the selected file name to the output parameter
            strcpy(selected_file, files[choice - 1]);
            break;  // Exit the loop after selection
        }
    }

    // Free allocated memory for filenames
    for (int i = 0; i < count; i++) {
        free(files[i]);
    }
}


void prompt_input(WINDOW *win, const char *prompt, char *input) {
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 2, 2, "%s", prompt);
    echo();
    wrefresh(win);
    wgetstr(win, input);
    noecho();
}

void choose_key(WINDOW *win, const char *key_file, char *selected_key) {
    FILE *fp = fopen(key_file, "r");
    if (!fp) {
        werase(win);
        mvwprintw(win, 2, 2, "Cannot open key file: %s", key_file);
        box(win, 0, 0);
        wrefresh(win);
        wgetch(win);
        return;
    }

    char *keys[100];
    int count = 0;
    char buffer[256];
    while (fscanf(fp, "%s", buffer) != EOF) {
        keys[count] = strdup(buffer);
        count++;
    }
    fclose(fp);

    int highlight = 1, choice = 0, c;
    while (1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "Select a key:");
        for (int i = 0; i < count; i++) {
            if (highlight == i + 1)
                wattron(win, A_REVERSE);
            mvwprintw(win, 3 + i, 4, "%s", keys[i]);
            wattroff(win, A_REVERSE);
        }
        wrefresh(win);

        c = wgetch(win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = count;
                else
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight == count)
                    highlight = 1;
                else
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            case KEY_BACKSPACE: // Backspace key
                delwin(win);
                return;
            default:
                break;
        }

        if (choice != 0) {
            strcpy(selected_key, keys[choice - 1]);
            break;
        }
    }

    for (int i = 0; i < count; i++)
        free(keys[i]);
}
void choose_method(WINDOW *win, char *selected_method) {
    char *methods[] = {"cbc", "mask", "xor"};
    int n_methods = 3;
    int highlight = 1, choice = 0, c;

    while (1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "Select a method:");
        for (int i = 0; i < n_methods; i++) {
            if (highlight == i + 1)
                wattron(win, A_REVERSE);
            mvwprintw(win, 3 + i, 4, "%s", methods[i]);
            wattroff(win, A_REVERSE);
        }
        wrefresh(win);

        c = wgetch(win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_methods;
                else
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight == n_methods)
                    highlight = 1;
                else
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            case KEY_BACKSPACE: // Backspace key
                delwin(win);
                return;
            default:
                break;
        }

        if (choice != 0) {
            strcpy(selected_method, methods[choice - 1]);
            break;
        }
    }
}

void execute_sym_crypt( const char *input_file, const char *output_file, const char *key, const char *method) {
    char *args[10];
    int arg_index = 0;

    args[arg_index++] = "./src/Partie_1/sym_crypt";
    args[arg_index++] = "-i";
    args[arg_index++] = (char *)input_file;
    args[arg_index++] = "-o";
    args[arg_index++] = (char *)output_file;
    args[arg_index++] = "-k";
    args[arg_index++] = (char *)key;
    args[arg_index++] = "-m";
    args[arg_index++] = (char *)method;

    if ((strcmp(method, "cbc-crypt") == 0)||(strcmp(method, "cbc-uncrypt") == 0)) {
        args[arg_index++] = "-v";
        args[arg_index++] = "/home/hiraichi/work/Projet_Group_14/Code_C/Util/Config/iv.txt";
    }

    args[arg_index] = NULL;

    endwin(); // Exit ncurses mode before executing the program
    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL); // Wait for the process to finish

    char *used_key_args[] = {
        "./src/Key/key",
        "-u",
        (char *)key,
        NULL
    };
    pid = fork();
    if (pid == 0) {
        execvp(used_key_args[0], used_key_args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL); // Wait for the process to finish
}
void execute_crack_code(const char *input_file, const char *output_file, const char *key_length, const char *dictionary) {
    char *break_args[] = {
        "./src/Partie_3/break_code",
        "-i", (char *)input_file,
        "-m", "all",
        "-k", (char *)key_length,
        "-d", (char *)dictionary,
        NULL
    };

    // Execute break_code
    endwin(); // Exit ncurses mode before executing the program
    pid_t pid = fork();
    if (pid == 0) {
        execvp(break_args[0], break_args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL); // Wait for the process to finish

    // Execute sym_crypt
    char *sym_crypt_args[] = {
        "./src/Partie_1/sym_crypt",
        "-i", (char *)input_file,
        "-o", (char *)output_file,
        "-f", "/home/hiraichi/work/Projet_Group_14/Code_C/Util/Key/crack_key.txt",
        "-m", "xor",
        NULL
    };

    pid = fork();
    if (pid == 0) {
        execvp(sym_crypt_args[0], sym_crypt_args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL); // Wait for the process to finish
}

void list_keys_from_file(WINDOW *win, const char *file_path, char *selected_key) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        werase(win);
        mvwprintw(win, 2, 2, "Failed to open file: %s", file_path);
        box(win, 0, 0);
        wrefresh(win);
        wgetch(win);
        return;
    }

    // Read keys from the file
    char *keys[100];
    int count = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline character
        keys[count] = strdup(buffer);
        count++;
    }
    fclose(file);

    int highlight = 1, choice = 0, c;
    while (1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "Select a key:");
        for (int i = 0; i < count; i++) {
            if (highlight == i + 1)
                wattron(win, A_REVERSE);
            mvwprintw(win, 3 + i, 4, "%s", keys[i]);
            wattroff(win, A_REVERSE);
        }
        wrefresh(win);

        c = wgetch(win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = count;
                else
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight == count)
                    highlight = 1;
                else
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            case KEY_BACKSPACE: // Backspace key
                delwin(win);
                return;
            default:
                break;
        }

        if (choice != 0) {
            if (selected_key != NULL) {
                strncpy(selected_key, keys[choice - 1], 256);
            }
            break;
        }
    }

    // Free allocated memory for keys
    for (int i = 0; i < count; i++) {
        free(keys[i]);
    }
}

