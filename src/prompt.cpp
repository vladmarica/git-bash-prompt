#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <sstream>
#include <clocale>

#include <fcntl.h>
#include <io.h>

#include "colors.h"

#define GIT_STATUS_COMMAND "git status --porcelain=v1 -b";
#define PREFIX_NO_COMMITS "## No commits yet on ";

const char* STATUS_EQUAL = "\xe2\x89\xa1"; // ≡
const wchar_t STATUS_AHEAD = '↑';
const wchar_t STATUS_BEHIND = '↓';

typedef struct {
    std::string branch_name;
    int commits_behind = 1;
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
}

void parse_status_header(std::string line, git_repo_status* const status_ptr) {
    size_t pos = line.find(PREFIX_NO_COMMITS);
    if (pos != std::string::npos) {
        status_ptr->branch_name = line.substr(PREFIX_NO_COMMITS.size());
    }
    else {
        std::vector<std::string> tokens = split_string(line, ' ');
        status_ptr->branch_name = tokens[1];
        pos = status_ptr->branch_name.find("...");

        if (pos != std::string::npos) {
            status_ptr->branch_name = status_ptr->branch_name.substr(0, pos);
        }
    }
}

int main(void) {
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

    while ((chars_read = getline(&line_data_ptr, &line_length, fp)) != -1) {
        *(line_data_ptr + chars_read - 1) = '\0';

        if (line_num == 0) {
            line = "## master...origin/master [ahead 1, behind 1]";
            parse_status_header(line.c_str(), &status);
        }
        else {
            code_buffer[0] = line_data_ptr[0];
            code_buffer[1] = line_data_ptr[1];
            parse_status_line(code_buffer, &status);
        }

        line_num++;
    }

    std::cout << YELLOW << "[" << BLUE << status.branch_name;
    
    if (status.commits_ahead > 0)
        //std::cout << GREEN << " " << STATUS_AHEAD << std::to_string(status.commits_ahead);
    if (status.commits_behind > 0)
        //std::cout << GREEN << " " << STATUS_AHEAD << std::to_string(status.commits_behind);

    printf("%s", STATUS_EQUAL);
    std::cout << YELLOW << "]" << RESET;
    printf("%s", STATUS_EQUAL);

    fclose(fp);
    return 0;
}
