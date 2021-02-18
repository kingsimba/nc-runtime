# Changelog

- 2021-02-18 Add Thread::sleep(). Make TimeTick a class. 
- 2021-02-18 Change the implementation of `sp<NcArray>`'s operator[].
- 2021-02-18 Add `synchroized()` and `ManualResetEvent`.
- 2021-02-17 Add NcArray::findWithCondition(), indexOfObject(), indexOfObjectWithCondition()
- 2021-02-17 Update Visual Studio Visualizer.
- 2021-02-16 Add NcString::format(). StringSlice::tokenize().
- 2021-02-16 Add log system. Add StackOrHeapAllocator.
- 2021-02-16 Add StringSlice::countSlice()
- 2021-02-16 "hello"\_s will create a StringSlice. Add startsWith() and endsWith().
- 2021-02-16 Add Math_hashU32(), Math_hashU64() and Math_hashSizeT()
- 2021-02-16 Support string literal. "hello"\_str will create a NcString object.
- 2021-02-16 Optimize NcString. Use a single allocation to create the string.
- 2021-02-16 Add subsliceFrom(), subsliceInRrange(), splitWithLimit(), replaceInRange() in StringSlice & NcString.
- 2021-02-15 Add NcObject::isKindOf()
