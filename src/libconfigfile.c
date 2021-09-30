/*
 * Copyright (C) 2021 Murilo Morais Marques <muriloglix@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * File:   libconfigfile.c
 * Author: Murilo Morais Marques <muriloglix@gmail.com>
 *
 * Created on September 3, 2021, 1:18 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "libconfigfile.h"

//#ifdef DISABLE_PRINT_MACROS
//#define PRINTF_WARNING(FORMAT, ...) ;
//#define PRINTF_ERROR(FORMAT, ...) ;
//#define PRINTF_ERRNO_ERROR() ;
//#define PRINTF_ERRNO_WARNING() ;
//#else
//#define PRINTF_WARNING(FORMAT, ...) fprintf(stderr, "%s() %s:%d. Warning" FORMAT, __func__, __FILE__, __LINE__, ##__VA_ARGS__)
//#define PRINTF_ERROR(FORMAT, ...) fprintf(stderr, "%s() %s:%d. Error" FORMAT, __func__, __FILE__, __LINE__, ##__VA_ARGS__)
//#define PRINTF_ERRNO_ERROR() if (errno) PRINTF_ERROR("(%d): %s\n", errno, strerror(errno))
//#define PRINTF_ERRNO_WARNING() if (errno) PRINTF_WARNING("(%d): %s\n", errno, strerror(errno))
//#endif

/**
 * Rearranges the string and removes trailing whitespace.
 * @param string String to be modified.
 * @return If successful, it returns the number of bytes in the string, if not, it returns zero.
 */
size_t configfile_trim_and_move(char *string) {
    size_t string_length, left_size, right_size;

    if (string == NULL) {
        return 0;
    }

    string_length = strlen(string);

    if (string_length == 0) {
        return 0;
    }

    left_size = 0;
    right_size = 0;

    for (left_size = 0; !isalnum(string[left_size]) && left_size < string_length; left_size++);
    if (left_size == string_length) {
        return 0;
    }

    for (right_size = 0; !isalnum(string[string_length - (right_size + 1)]) && right_size < string_length; right_size++);

    string_length = string_length - left_size - right_size;

    if (left_size > 0) {
        memmove(string, &string[left_size], string_length);
        memset(&string[string_length + 1], ' ', left_size - 1);
    }

    string[string_length] = '\0';

    return string_length;
}

/**
 * Allocates and populates a new structure of type configfile.
 * @param module_name Parameter name, cannot be NULL.
 * @param module_value Parameter value, cannot be NULL.
 * @return Returns a configfile structure allocated with malloc(3), on failure returns NULL. errno is set according to malloc(3).
 */
configfile *configfile_new(char *module_name, char *module_value) {
    size_t name_length, value_length;
    configfile *allocated_configfile;

    if (module_name == NULL || module_value == NULL) {
        return NULL;
    }

    name_length = strlen(module_name);
    value_length = strlen(module_value);

    allocated_configfile = malloc(sizeof (configfile));

    if (allocated_configfile == NULL) {
        //        PRINTF_ERRNO_ERROR();
        return NULL;
    }

    allocated_configfile->module_name = module_name;
    allocated_configfile->module_name_length = name_length;
    allocated_configfile->module_value = module_value;
    allocated_configfile->module_value_length = value_length;
    allocated_configfile->next = NULL;

    return allocated_configfile;
}

configfile *configfile_get(configfile *search_struct, const char *module_name) {
    size_t module_name_length;
    configfile *next;

    if (module_name == NULL) {
        return NULL;
    }

    next = search_struct;
    module_name_length = strlen(module_name);

    while (next != NULL) {
        if (next->module_name_length == module_name_length) {
            if (strncmp(next->module_name, module_name, module_name_length) == 0) {
                return next;
            }
        }
        next = next->next;
    }

    return NULL;
}

void configfile_kill(configfile *config_struct_to_kill) {
    configfile *next = config_struct_to_kill;

    while (next != NULL) {
        config_struct_to_kill = next;
        next = next->next;
        if (config_struct_to_kill->module_name != NULL) {
            free(config_struct_to_kill->module_name);
            config_struct_to_kill->module_name = NULL;
            config_struct_to_kill->module_name_length = 0;
        }
        if (config_struct_to_kill->module_value != NULL) {
            config_struct_to_kill->module_value = NULL;
            config_struct_to_kill->module_value_length = 0;
        }
        free(config_struct_to_kill);
    }
    config_struct_to_kill = NULL;
}

/**
 * Executes and analyzes all lines of the file, removing white spaces both in the module and in the value.
 * @param open_file FILE pointer with an open file with read permissions.
 * @return Returns a structure containing modules and their values according to the configuration file defined in filename.
 */
configfile *configfile_run(FILE *open_file) {
    char *line_contents, *delimiter;
    char *module_name, *module_value;
    size_t n, line_number, line_length, module_name_length, module_value_length;
    configfile *configfile_struct_return, *new_configfile, **configfile_struct_next;
    int errno_backup;

    if (open_file == NULL) {
        return NULL;
    }

    line_contents = NULL;
    n = 0;
    line_number = 0;
    configfile_struct_return = NULL;

    errno_backup = errno;
    errno = 0;

    while (getline(&line_contents, &n, open_file) > 0 && !errno) {
        line_length = strlen(line_contents);
        line_number++;

        module_name = line_contents;
        delimiter = strchr(module_name, '=');

repeat_delimiter:
        if (delimiter != NULL) {
            delimiter[0] = '\0';
            module_name_length = configfile_trim_and_move(module_name);

            if (!module_name_length) {
                delimiter = NULL;
                goto repeat_delimiter;
            }

            delimiter[0] = ' ';
            module_value = &module_name[module_name_length + 1];
            module_value_length = configfile_trim_and_move(module_value);

            if (!module_value_length) {
                delimiter = NULL;
                goto repeat_delimiter;
            }

            if ((module_name_length + module_value_length + 2) != line_length) {
                line_contents = realloc(line_contents, module_name_length + module_value_length + 2);
                module_name = line_contents;
                module_value = &module_name[module_name_length + 1];
            }

            new_configfile = configfile_new(module_name, module_value);

            if (new_configfile == NULL) {
                delimiter = NULL;
                goto repeat_delimiter;
            }

            if (configfile_struct_return == NULL) {
                configfile_struct_return = new_configfile;
            } else {
                *configfile_struct_next = new_configfile;
            }

            configfile_struct_next = &new_configfile->next;
        } else {
            free(line_contents);
        }


        line_contents = NULL;
        n = 0;
    }

    module_name = NULL;
    module_value = NULL;

    if (errno) {
        goto error_00;
    }

    if (line_contents != NULL) {
        free(line_contents);
        line_contents = NULL;
    }

    errno = errno_backup;
    return configfile_struct_return;

error_00:
    configfile_kill(configfile_struct_return);

    if (line_contents != NULL) {
        free(line_contents);
        line_contents = NULL;
    }
    return NULL;
}

configfile *configfile_init(const char *filename) {
    FILE *open_file;
    configfile *return_config_struct;

    if (filename == NULL)
        return NULL;

    open_file = fopen(filename, "r");
    if (open_file == NULL) {
        return NULL;
    }

    return_config_struct = configfile_run(open_file);

    fclose(open_file);
    open_file = NULL;

    return return_config_struct;
}