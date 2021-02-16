> **WARNING:** A work in progress

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

# Design Notes

To be honest, I fall behind in terms of modern C++. Because I held a biased and negative attitude towards it and didn't spend much time to learn it.

And in the New Years Days of 2021, I decide to rediscover modern C++. And see how things can be different if I use C++ 14. There must be some good parts in it.

## std::shared_ptr vs self-implemented smart pointer.

Historically, I always do RC in a base class. And I want to try out `std::shared_ptr` this time.

The most important implementation difference I noticed is that:

- For std::shared_ptr, the control block(contains RC, weak_ptrs) is put outside of the object.
- For most self-implemented RC, the control block is in the base object.

At first, I think `std::shared_ptr` will have to make one additional malloc() for the control block.
Until I read from https://www.nextptr.com/tutorial/ta1358374985/shared_ptr-basics-and-internals-with-examples:

> We mentioned above that the control block could either contain a pointer to
> the managed object or the object itself.
> The control block is dynamically allocated. Constructing the managed object in-place
> within the control block can avoid the two separate memory allocations for the object
> and the control block, resulting in an uncomplicated control block and better performance.
> The std::make_shared is a preferred way to construct a shared_ptr because it builds the
> managed object within the control block.

So I think `std::shared_ptr` might be good. And write NcObject, NcString, NcArray out of it:

```cpp
class MyBox : public NcObject {
public:
  static sp<MyBox> alloc() { return std::make_shared<MyBox>(); }

  MyBox(){};   // must be public. Because std::make_shared needs it.
};

TEST(NcObject, cast) {
  auto box = MyBox::alloc();

  // cast to base
  sp<NcObject> base = box;
  EXPECT_EQ(box.use_count(), 2);

  // cast to derived
  auto box2 = std::static_pointer_cast<MyBox>(base);
  EXPECT_EQ(box.use_count(), 3);
  EXPECT_TRUE(box.get() == box2.get());
}
```

So far so good. Until I try to write something like this:

```cpp
TEST(NcString, split) {
  vector<StringSlice> slices = NcString::allocWithCString("hello---world")->split("---");
  auto s = NcString::allocByJoiningSlices(slices, " ");
  EXPECT_STREQ(s->cstr(), "hello world");
  EXPECT_EQ(s->length(), 11);
}
```

First, `NcString::allocWithCString()` creates `shared_ptr<NcString>`.
Then `NcString::split()` split it into slices. Each slice should hold a RC to the original string.
So in split() the RC must be increased.

This is impossible with `std::shared_ptr`. You can't convert a raw pointer(`this`) back into a smart pointer.

And there is another slight inconveniency:

```cpp
static int _calculateStringLength(NcString* str) { return str->length(); }

TEST(NcString, basic) {
  sp<NcString> s = NcString::allocWithCString("hello");
  EXPECT_STREQ(s->cstr(), "hello");
  // must call .get() to convert to ordinary pointer.
  EXPECT_EQ(_calculateStringLength(s.get()), 5);
}
```

You have to call `.get()` to get a raw pointer. There is no implicit conversion, though for a good reason https://www.informit.com/articles/article.aspx?p=31529&seqNum=7.

Third, with self-implemented RC in NcObject. I can append the text directly after the object.
Thus creating the string with a single allocation.

```cpp
class NcString
{
public:
   static NcString* allocWithCString(const char* str) {
      size_t len = strlen(str) + 1;
      NcString* mem = (NcString*)malloc(sizeof(NcString) + len);
      char* strBuffer = (char*)(mem+1);
      new(mem) NcString(); // call inplacement new
      memcpy(strBuffer, str, len);
      ...
   }
}
```

For conclusion, after analysis, I still give self-implemented RC a go. Based on reasons:

1. `std::shared_ptr` implements the control block outside of the object. So you can't convert a raw pointer back into a smart pointer.
2. `std::shared_ptr` doesn't allow manual RC. I think it might be useful in some low-level classes.
3. I want complete control over how the RC is implemented and can make in-depth optimization.

## `auto` keyword

The `auto` keyword makes writing code a breeze. Especially so for rarely used types or templated types.

```cpp
auto iter = str->iter(); // iter is of type StringCharacterIterator.
auto pieces = str->split(); // pieces is of type std::vector<StringSlice>
```

But at the cost of making it harder to read or navigate:

1. With VS2019, you have to hover the cursor on top of the variable to see its type.

   > **NOTE:** Android Studio inserts gray text to mark the type
   > behind the variable name, which is very nice.
   >
   > ```cpp
   > auto slices : vector<StringSlice> = StringSlice("hello---world").split("---");
   > ```

2. You can't easily ctrl+click to jump to type definition.
3. It creates room for mistakes when using basic types.

   ```cpp
   auto a = 3.0;
   // b is double, because a is accidentally created as double.
   // But I might think b is an integer.
   auto b = a + 3;
   ```

But overall, I think it's worthwhile to use `auto`. Especially for complex types. But for basic types, I prefer to make the type explicit.

## String literals

C++11 support customized suffix for string literals.

```cpp
sp<NcString> operator""_s(const char* literalStr, size_t len);
```

This enables creating of literal NcString object.

```cpp
sp<NcString> s = "hello world"_s;
```

It has `retainCount()` at INT_MAX.
Calling retain() or release() on it has no effect.
With a global string manager, it's possible to make sure only one NcString object is created for each string literal.

```cpp
TEST(NcString, literal) {
  sp<NcString> s1, s2;
  for (int i = 0; i < 2; i++) {
    sp<NcString> s = "hello world"_s;
    if (i == 0)
      s1 = s;
    else
      s2 = s;
  }

  // s1 is exactly the same as s2, because of the literal string manager.
  EXPECT_EQ(s1.get(), s2.get());

  auto s3 = "hello world"_s;
  // for s1 == s3, it must be compiled with /GF(enable string pool) for Visual Studio
  EXPECT_EQ(s1.get(), s3.get());
}
```
