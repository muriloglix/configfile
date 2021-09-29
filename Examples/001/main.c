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
 * File:   main.c
 * Author: Murilo Morais Marques <muriloglix@gmail.com>
 *
 * Created on September 28, 2021, 9:57 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "libconfigfile.h"

void print_all(configfile *config_to_print) {
    configfile *next = config_to_print;

    while (next != NULL) {
        printf("%s (%zd bytes) = %s (%zd bytes)\n", next->module_name, next->module_name_length, next->module_value, next->module_value_length);
        next = next->next;
    }
}

int main() {
    configfile *global_config;
    global_config = configfile_init("test.conf");

    if (global_config == NULL) {
        printf("Error (%d): %s\n", errno, strerror(errno));
        return (EXIT_FAILURE);
    }

    print_all(global_config);

    configfile_kill(global_config);

    return (EXIT_SUCCESS);
}

