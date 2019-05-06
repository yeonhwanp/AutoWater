#ifndef REQUESTS_H
#define REQUESTS_H

#include <Arduino.h>

uint8_t char_append(char* buff, char c, uint16_t buff_size);
void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial);

#endif
