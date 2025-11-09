#pragma once

// Activates DEBUG configuration inside our ECS
#define DEBUG

// Defining NDEBUG when DEBUG is not defined.
#ifndef DEBUG
#ifndef NDEBUG

#define NDEBUG // (NDEBUG turns off asserts)

#endif // !NDEBUG
#endif // DEBUG