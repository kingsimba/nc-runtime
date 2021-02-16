# Changelog

* 2021-02-16 Add Math_hashU32(), Math_hashU64() and Math_hashSizeT()
* 2021-02-16 Support string literal. "hello"_s will create a NcString object.
* 2021-02-16 Optimize NcString. Use a single allocation to create the string.
* 2021-02-16 Add subsliceFrom(), subsliceInRrange(), splitWithLimit(), stringByReplacingStringInRange() in StringSlice & NcString.
* 2021-02-15 Add NcObject::isKindOf()