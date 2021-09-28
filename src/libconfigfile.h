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
 * File:   libconfigfile.h
 * Author: Murilo Morais Marques <muriloglix@gmail.com>
 *
 * Created on September 3, 2021, 1:18 PM
 */

#ifndef LIBCONFIGFILE_H
#define LIBCONFIGFILE_H

typedef struct _configfile configfile;

struct _configfile {
    char *module_name;
    char *module_value;
    size_t module_name_length;
    size_t module_value_length;
    configfile *next;
};

/**
 * Function to initialize and run configuration file analysis.
 * @param filename String containing the name of the configuration file to perform the structure analysis and assembly.
 * @return Returns a structure containing modules and their values according to the configuration file defined in filename.
 */
configfile *configfile_init(const char *filename);

/**
 * Searches for a module defined by module_name and returns a structure for the module found, if not, returns NULL.
 * @param search_struct Structure where the search will be performed.
 * @param module_name String to search for.
 * @return Returns a pointer to the found structure or NULL if not found.
 */
configfile *configfile_get(configfile *search_struct, const char *module_name);

/**
 * Frees any and all memory allocated in the configfile type structure.
 * @param config_struct_to_kill Structure to be freed from memory.
 */
void configfile_kill(configfile *config_struct_to_kill);

#endif /* LIBCONFIGFILE_H */
