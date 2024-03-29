# Changelog

-   2021-11-20 Major improves on `StringSlice`, `sp<T>`
		
    - StringSlice become the major player, instead of NcString.
    - Add `join`, `format`, etc in StringSlice.
    - Disallow implict conversion from `sp<T>` to raw pointer. Use `get()` exclusively.
    - Disallow implict conversion from raw pointer to `sp<T>`. Use `retainAsSp()`.
    - Add `NcObject::alloc<T>()` to create object. Disallow using of `new`.

-   2021-11-20 Avoid using "new" with NcObject::alloc().
-   2021-11-20 Avoid using "new" with NcObject::alloc().
-   2021-03-29 Add NC_ASSERT()
-   2021-03-24 Add `Some` and `noValue`.
-   2021-03-24 Add NcFile & NcData.
-   2021-03-05 Fix bug in copying of wp.
-   2021-03-05 Fix bug in global strings.
-   2021-02-28 Add wp, equivalent of std::weak_ptr.
-   2021-02-18 Add Thread::sleep(). Make TimeTick a class. 
-   2021-02-18 Change the implementation of `sp<NcArray>`'s operator[].
-   2021-02-18 Add `synchroized()` and `ManualResetEvent`.
-   2021-02-17 Add NcArray::findWithCondition(), indexOfObject(), indexOfObjectWithCondition()
-   2021-02-17 Update Visual Studio Visualizer.
-   2021-02-16 Add NcString::format(). StringSlice::tokenize().
-   2021-02-16 Add log system. Add StackOrHeapAllocator.
-   2021-02-16 Add StringSlice::countSlice()
-   2021-02-16 "hello"\_s will create a StringSlice. Add startsWith() and endsWith().
-   2021-02-16 Add Math_hashU32(), Math_hashU64() and Math_hashSizeT()
-   2021-02-16 Support string literal. "hello"\_str will create a NcString object.
-   2021-02-16 Optimize NcString. Use a single allocation to create the string.
-   2021-02-16 Add subsliceFrom(), subsliceInRrange(), splitWithLimit(), replaceInRange() in StringSlice & NcString.
-   2021-02-15 Add NcObject::isKindOf()
