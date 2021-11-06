# embedded-scpi-lite

A minimal(ly) SCPI-like parser-in-a-header-file for use on memory-constrained microcontrollers where other, more complete SCPI parser libraries would consume too many resources; it uses only those string processing functions required by ANSI-C.

Use is via a derived class which not only implements the language-specific handlers, but defines an array of command-to-handling-method mappings.
