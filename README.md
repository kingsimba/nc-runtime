# NC-Runtime

This is a proof-of-concept project. It's used to demonstrate some ideas such as:

1. An efficient `StringSlice` which is way faster than STL string.
2. Some foundation classes such as `NcObject`, `NcArray`, `NcString`. They enable things like:

   - Objective-C style lock:

   ```cpp
   synchronized(m_array) {

   }
   ```

   - All objects can be converted into string.

   ```cpp
   NcObject* o;
   auto str = o->toString();
   ```

   - C style format can be extended to support any NcObject.

   ```cpp
   auto arr = NcArray<NcString>::alloc();
   arr->addObject(_S("Obj1"));
   arr->addObject(_S("Obj2"));
   EXPECT_STREQ(NcString::format("Hello %@", arr)->cstr(), "Hello [\"Obj1\", \"Obj2\"]");
   ```
