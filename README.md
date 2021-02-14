# NC-Runtime

This is a proof-of-concept project. It's used to demonstrate some ideas such as:

1. String classes can be created and are both more efficient and intuitive than std::string.

   ```cpp
   auto s = StringSlice("You should name a 分隔符 variable with the same care 分隔符 as you are "
                       "naming your first born child.");
   auto pieces = s.split("分隔符 ");
   EXPECT_TRUE(pieces[0].equals("You should name a "));
   EXPECT_TRUE(pieces[1].equals("variable with the same care "));
   EXPECT_TRUE(pieces[2].equals("as you are naming your first born child."));
   ```

2. Some foundation classes such as `NcObject`, `NcArray`, `NcString`. They enable things like:

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
