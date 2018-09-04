// Simple Git prompt for Bash, inspired by posh-git. Requires a terminal with UTF-8 support.

#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <unistd.h>

#define GIT_STATUS_COMMAND  "git status --porcelain=v1 -b 2>&1"
#define PREFIX_NO_COMMITS   "## No commits yet on "

#define STATUS_EQUAL        "\xe2\x89\xa1" // ≡
#define STATUS_AHEAD        "\xe2\x86\x91" // ↑
#define STATUS_BEHIND       "\xe2\x86\x93" // ↓

#define COLOR_RESET         "\e[0m"
#define COLOR_RED           "\e[31m"
#define COLOR_GREEN         "\e[32m"
#define COLOR_YELLOW        "\e[33m"
#define COLOR_BLUE          "\e[34m"

typedef struct {
    std::string branch_name;
    int commits_behind = 0;
    int commits_ahead = 0;

    bool show_index = false;
    uint16_t index_added_files = 0;
    uint16_t index_modified_files = 0;
    uint16_t index_removed_files = 0;

    bool show_working = false;
    uint16_t working_added_files = 0;
    uint16_t working_modified_files = 0;
    uint16_t working_removed_files = 0;
} git_repo_status;

std::vector<std::string> split_string(const std::string& target, char delimiter) {
    std::vector<std::string> result;
    std::string token;
    std::istringstream stream(target);

    while (std::getline(stream, token, delimiter)) {
        result.push_back(token);
    }

    return result;
}

void parse_status_line(const char* code, git_repo_status* const status_ptr) {
    if (strcmp(code, "??") == 0) {
        status_ptr->show_working = true;
        status_ptr->working_added_files++;
    }
    else {
        // Working directory changes
        switch (code[1]) {
            case 'A': status_ptr->working_added_files++; status_ptr->show_working = true; break;
            case 'M': status_ptr->working_modified_files++; status_ptr->show_working = true; break;
            case 'D': status_ptr->working_removed_files++; status_ptr->show_working = true; break;
        }

        switch (code[0]) {
            case 'A': status_ptr->index_added_files++; status_ptr->show_index = true; break;
            case 'M': status_ptr->index_modified_files++; status_ptr->show_index = true; break;
            case 'D': status_ptr->index_removed_files++; status_ptr->show_index = true; break;
        }
    }
}

void parse_status_header(std::string line, git_repo_status* const status_ptr) {
    size_t pos = line.find(PREFIX_NO_COMMITS);
    if (pos != std::string::npos) {
        status_ptr->branch_name = line.substr(strlen(PREFIX_NO_COMMITS));
    }
    else {
        std::vector<std::string> tokens = split_string(line, ' ');
        status_ptr->branch_name = tokens[1];
        pos = status_ptr->branch_name.find("...");

        if (pos != std::string::npos) {
            status_ptr->branch_name = status_ptr->branch_name.substr(0, pos);
        }
        
        if (tokens.size() == 4) { // either ahead or behind remote, not both
            int amount = atoi(tokens[3].substr(0, tokens[3].size() - 1).c_str());
            if (tokens[2].find("ahead") != std::string::npos) {
                status_ptr->commits_ahead = amount;
            }
            else {
                status_ptr->commits_behind = amount;
            }
        }
        else if (tokens.size() == 6) { // both ahead and behind upstream
            status_ptr->commits_ahead = atoi(tokens[3].substr(0, tokens[3].size() - 1).c_str());
            status_ptr->commits_behind = atoi(tokens[5].substr(0, tokens[5].size() - 1).c_str());
        }
    }
}

int main(void) {
    // Exit if we are not running in a TTY
    if (!isatty(fileno(stdin))) {
        return -1;
    }

    FILE* fp = popen(GIT_STATUS_COMMAND, "r");
    if (fp == NULL) {
        return 1;
    }

    git_repo_status status;

    size_t line_length = 256;
    std::string line(line_length, 0);
    char* line_data_ptr = line.data();

    ssize_t chars_read = 0;
    int line_num = 0;
    char code_buffer[3] = { 0 };

    // Parse the output of the 'git status' command
    while ((chars_read = getline(&line_data_ptr, &line_length, fp)) != -1) {
        *(line_data_ptr + chars_read - 1) = '\0';

        if (line_num == 0) {
            parse_status_header(line.c_str(), &status);
        }
        else {
            code_buffer[0] = line_data_ptr[0];
            code_buffer[1] = line_data_ptr[1];
            parse_status_line(code_buffer, &status);
        }

        line_num++;
    }
    
    if (WEXITSTATUS(pclose(fp)) != 0) {
        return 1;
    }

    // Print out the prompt
    printf(COLOR_YELLOW "[" COLOR_BLUE "%s", status.branch_name.c_str());
    
    if (status.commits_ahead > 0)
        printf(COLOR_BLUE " " STATUS_AHEAD "%d", status.commits_ahead);
   
    if (status.commits_behind > 0)
        printf(COLOR_BLUE " " STATUS_BEHIND "%d", status.commits_behind);
    
    if (status.commits_ahead == 0 && status.commits_behind == 0)
        printf(COLOR_BLUE " " STATUS_EQUAL);

    if (status.show_index)
        printf(COLOR_GREEN " +%d ~%d -%d", status.index_added_files, status.index_modified_files, status.index_removed_files);
    
    if (status.show_working) {
        if (status.show_index)
            printf(COLOR_YELLOW " |");
        printf(COLOR_RED " +%d ~%d -%d", status.working_added_files, status.working_modified_files, status.working_removed_files);
    }

    printf(COLOR_YELLOW "]" COLOR_RESET);
    
    return 0;
}
