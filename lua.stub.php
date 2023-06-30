<?php

/**
 * @generate-class-entries
 * @undocumentable
 */

function lua_open(): int {}

function lua_global_put(int $handle, string $key, mixed $value): void {}

function lua_load_file(int $handle, string $file): bool {}

function lua_load_string(int $handle, string $file): bool {}

function lua_run(int $handle): bool {}

function lua_close(int $handle): void {}
