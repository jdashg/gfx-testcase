#pragma once

#include <cstdint>

void dump_program_log(const char* prefix, uint32_t prog);
void dump_shader_log(const char* prefix, uint32_t shader);
uint32_t make_program(const char* vs_src, const char* fs_src);
