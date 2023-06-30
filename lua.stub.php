<?php

/**
 * @generate-class-entries
 * @undocumentable
 */

function test1(): void {}

function test2(string $str = ""): string {}

function lua_open(): int {}

function lua_load(int $handle, string $file): bool {}

function lua_run(int $handle): bool {}

function lua_close(int $handle): bool {}
