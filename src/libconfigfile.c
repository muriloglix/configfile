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
 * Removes whitespace before and after string, frees the old string and allocates a new one via malloc(3) to store the new string.
 * <p>
 * First it tries to trim (through sscanf(3)) and allocate memory in a temporary variable, if everything works then it frees what is in *string, sets the new address allocated inside *string and returns the amount of bytes allocated.
 * <p>
 * If something fails during sscanf(3) it will return 0 (zero) and nothing will be changed in the string variable and errno will be set according to sscanf(3).
 * @param string Variable used to serve as source and destination. It must receive the address of the pointer to be modified.
 * @return Returns the number of bytes allocated to hold the new string. Return the size_t type.
 */
size_t configfile_trim(char **string) {
    char *trimmed_string;
    int errno_backup;

    if (string == NULL || *string == NULL) {
        return 0;
    }

    trimmed_string = NULL;

    errno_backup = errno;
    errno = 0;

    if (sscanf(*string, "%ms", &trimmed_string) < 0) {
        //        PRINTF_ERRNO_ERROR();
        return 0;
    }

    free(*string);
    *string = trimmed_string;
    errno = errno_backup;

    return strlen(trimmed_string);
}

/**
 * Allocates and populates a new structure of type configfile.
 * @param module_name Parameter name, cannot be NULL.
 * @param module_value Parameter value, can be NULL.
 * @return Returns a configfile structure allocated with malloc(3), on failure returns NULL and sets errno according to the problem.
 */
configfile *configfile_new(char *module_name, char *module_value) {
    char *name, *value;
    size_t name_length, value_length;
    configfile *allocated_configfile;

    if (module_name == NULL) {
        return NULL;
    }

    name = module_name;
    value = module_value;

    name_length = configfile_trim(&name);

    if (!name_length) {
        return NULL;
    }

    if (value == NULL) {
        value_length = 0;
    } else {
        value_length = strlen(value);
    }

    allocated_configfile = malloc(sizeof (configfile));

    if (allocated_configfile == NULL) {
        //        PRINTF_ERRNO_ERROR();

        free(name);
        name = NULL;
        name_length = 0;

        if (value != NULL) {
            free(value);
            value = NULL;
            value_length = 0;
        }

        return NULL;
    }

    allocated_configfile->module_name = name;
    allocated_configfile->module_name_length = name_length;
    allocated_configfile->module_value = value;
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
            free(config_struct_to_kill->module_value);
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
    char *line_contents;
    size_t n, line_number;
    char *module_name, *module_value;
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
        line_number++;

        module_name = NULL;
        module_value = NULL;

        if (sscanf(line_contents, "%m[^=]=%ms", &module_name, &module_value) < 0) {
            goto error_00;
        }

        if (module_name == NULL) {
            //            PRINTF_WARNING(": Ignoring malformed line(%ld): %s\n", line_number, line_contents);
            continue;
        }

        new_configfile = configfile_new(module_name, module_value);

        if (new_configfile == NULL) {
            //            PRINTF_WARNING(": Ignoring malformed line(%ld): %s\n", line_number, line_contents);
            continue;
        }

        if (configfile_struct_return == NULL) {
            configfile_struct_return = new_configfile;
        } else {
            *configfile_struct_next = new_configfile;
        }

        configfile_struct_next = &new_configfile->next;
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
    if (module_name != NULL) {
        free(module_name);
        module_name = NULL;
    }

    if (module_value != NULL) {
        free(module_value);
        module_value = NULL;
    }

    //    PRINTF_ERRNO_ERROR();
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
        //        PRINTF_ERRNO_ERROR();
        return NULL;
    }

    return_config_struct = configfile_run(open_file);

    if (fclose(open_file)) {
        //        PRINTF_ERRNO_ERROR();
    }

    open_file = NULL;

    return return_config_struct;
}