API Code Conventions
====================
When writing user-facing code follow the [Arduino Style Guide](https://www.arduino.cc/en/Reference/StyleGuide)

Implementation Code Conventions
===============================
When writing code that is "hidden" from the end user (such as the actual *implementation* of Serial.println) please follow these guidelines:

White space:
* Expand tabs to 4 spaces.
* Don't leave trailing whitespace at the end of a line.
* For control blocks (if, for, while), put 1 space between the keyword and the opening parenthesis.
* Put 1 space after a comma, and 1 space around operators.

Braces:
* Use braces for all blocks, even no-line and single-line pieces of code.
* Put opening braces on the end of the line it belongs to, not on a new line.
* For else-statements, put the else on the same line as the previous closing brace.

Header files:
* Header files should be protected from multiple inclusion with #if directives. See an existing header for naming convention.

Names:
* Use underscore_case, not camelCase for all names.
* Use CAPS_WITH_UNDERSCORE for enums and macros.
* When defining a type use underscore_case and put '_t' after it.

Integer types:
* Preference for using the <stdint.h> types 
* Use size_t for things that count bytes / sizes of objects.

Comments:
* Be concise and only write comments for things that are not obvious.
* Use // prefix, NOT /* ... */. No extra fluff.


Examples
========

Braces, spaces, names and comments:
```
#define TO_ADD (123)

// This function will always recurse indefinitely and is only used to show
// coding style
int foo_function(int x, int some_value) {
    if (x < some_value) {
        foo(some_value, x);
    } else {
        foo(x + TO_ADD, some_value - 1);
    }

    for (int my_counter = 0; my_counter < x; my_counter++) {
    }
}
```

Type declarations:
```
typedef struct _my_struct_t {
    int member;
    void *data;
} my_struct_t;
```
