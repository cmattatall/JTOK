# JTOK
JSON C Library to be used for embedded systems. Loosely based on JSMN

# Building

Typically you will add this entire repository as a subdirectory into a cmake project, and use add_subdirectory before importing linking against the cmake library targets.
However, this can be built standalone and manually linked with the typical cmake usage: `cmake -S . -B build; cmake --build build`

# Usage 

If you've used the JSMN API before, the usage is identical. The only difference is the function "namespace" is jtok, rather than jsmn.
e.g. `jtok_parse` rather than `jsmn_parse`

For documentation on the JSMN API, please visit the API section for JSMN link in https://www.json.org/json-en.html
