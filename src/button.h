#ifndef BUTTON_H
#define BUTTON_H

#include "config.h"

#include <stdbool.h>

void button_init(const button_config_t *config);
void button_update(void);
bool button_is_pressed(void);

#endif
