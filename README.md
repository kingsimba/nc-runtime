[![Actions Status](https://github.com/kingsimba/nc-runtime/workflows/CMake/badge.svg)](https://github.com/kingsimba/nc-runtime/actions)

# NC-Runtime <!-- omit in toc -->

> **WARNING:** A work in progress

- [Contents](#contents)
  - [Smart Pointer](#smart-pointer)
    - [Rules of using `sp`.](#rules-of-using-sp)
  - [String Class](#string-class)
  - [Foundation Classes: NcObject, NcArray, NcString, etc](#foundation-classes-ncobject-ncarray-ncstring-etc)
  - [StackOrHeapAllocator](#stackorheapallocator)
  - [ManualResetEvent](#manualresetevent)
  - [Many more good stuffs](#many-more-good-stuffs)
- [Design Notes](#design-notes)
  - [std::shared_ptr vs self-implemented smart pointer.](#stdshared_ptr-vs-self-implemented-smart-pointer)
    - [The internal of shared_ptr](#the-internal-of-shared_ptr)
    - [Try to use shared_ptr](#try-to-use-shared_ptr)
    - [Things I like & dislike](#things-i-like--dislike)
    - [Allow implicit conversion](#allow-implicit-conversion)
    - [Makes all memory in one piece with `sp`](#makes-all-memory-in-one-piece-with-sp)
  - [`auto` keyword](#auto-keyword)
  - [String literals](#string-literals)
  - [Log System](#log-system)
  - [Visual Studio Visualizer](#visual-studio-visualizer)
  - [Implement operator [] for `sp<NcArray>`](#implement-operator--for-spncarray)
  - [Closure(Lambda Functions)](#closurelambda-functions)

# Contents

In my humble opinion, unlike other modern programming languages, C++ lacks some important infrastructures.
Everyone have to invent their own wheel. And so am I.

This project contains the methodology & wheels which I found useful.
They may not be the best ideas. But I hope you may find some pieces useful.

## Smart Pointer

`std::shared_ptr` gives a good direction in automating memory management.
But I'd like to offer a slightly different approach.

The smart pointer `sp` and `wp` works in many ways like `std::shared_ptr` and `std::weak_ptr`.

```cpp
// get() and use_count() works the same as std::shared_ptr
sp<NcData> data = NcData::allocWithBytes(buffer, len);
EXPECT_EQ(data.get()->length(), data->length());   // overload operator '->'
EXPECT_EQ(data.use_count(), 1);

wp<NcString> w(str);	// strong/raw to weak
auto s = w.lock(); 		// weak to strong
```

But they also bear important differences. Such as: The raw pointer and smart pointer can be freely converted to each other.

```cpp
sp<NcString> p1 = NcString::allocWithCString("hello");
NcString* raw = p1.get();  // sp -> raw
sp<NcString> p2 = retain(raw);   // raw -> sp
```

It makes calling raw-pointer functions easier:

```cpp
void myFunction(NcString* str) // raw pointer

sp<NcString> s = NcString::allocWithCString("abc");
myFunction(s); // implicitly conversion to raw pointer

auto arr = NcArray<NcString>::allocWithObjects("123"_str, "234"_str, NULL);
for(int i = 0; i < arr->Length(); i++) {
   NcString* str = arr[i]; // implicitly convert to raw pointer
}
```

And it allows to retake ownership from raw pointer.

```cpp
void SomeObject::setName(NcString* name) {
   m_name = retain(name);  // retake ownership from raw pointer
}
```

### Rules of using `sp`.

- There are only two ways of using pointer: `Widget*` or `sp<Widget>`. There is not `const` variant.
- Use raw pointer as much as possible(local variables, parameters etc), unless you want to take the ownership.
- If you want to take ownership, make it explicit by calling `retain()`.

```cpp
class SomeClass
{
 public:
   SomeClass() {
      sp<NcString> name = NcString::allocWithCString("hello"); // object creation, use `sp`
      m_name = sp;   // transfer ownership, use `sp`
   }

   void setName(NcString* name) { // function parameter: use raw pointer
      m_name = retain(name);  // explicitly take ownership
   }

 private:
   void processing() {
      NcString* name = m_name; // local variable: use raw pointer
      useName(name);
   }

   void useName(NcString* name) {   // function parameter: use raw pointer
   }

 private:
   sp<NcString> m_name;
}
```

## String Class

I want a string class which is both intuitive and efficient.

`StringSlices` is always on stack. It only contains a `char*` and a `length`. There is no heap memory allocation. So it's very fast.

```cpp
StringSlice slice = "hello---world"_s;
vector<StringSlice> pieces = slice.split("---");
EXPECT_TRUE(pieces[0].equals("hello"));
EXPECT_TRUE(pieces[1].equals("world"));
```

`NcString` is on heap. There is one `malloc()` for each string.

```cpp
auto str = NcString::allocWithCString("hello---world");
// splitting a string will only create StringSlices. So it's very fast.
auto slices = str.split("---");
EXPECT_EQ(str->retainCount(), 3); // because each slice holds a reference to the string
EXPECT_TRUE(slices[0].equals("hello"));
EXPECT_TRUE(slices[1].equals("world"));
```

Both `StringSlice` and `NcString` have rich set of functions, such as:

```cpp
"freeman"_s.startsWith("free");
"freeman"_s.endsWith("man");
EXPECT_EQ("internationalization"_s.countSlice("tion"), 2);
auto str = NcString::format("%s shall come", "The Day"); // The Day shall come
```

## Foundation Classes: NcObject, NcArray, NcString, etc

I want some foundation classes such as `NcObject`, `NcArray`, `NcString`. They enable things like:

- Type identification

  ```cpp
  sp<NcObject> box = MyBox::alloc();
  EXPECT_TRUE(box->isKindOf<MyBox>());
  EXPECT_TRUE(box->isKindOf<NcObject>());
  EXPECT_FALSE(box->isKindOf<NcString>());
  ```

- All objects can be converted in string.

  ```cpp
  auto str = o->toString();
  ```

- C style format can be extended to support any NcObject.

  ```cpp
  auto arr = NcArray<NcString>::alloc();
  arr->addObject("Earth"_str);
  arr->addObject("Mars"_str);
  arr[1] = "Jupiter"_str;
  EXPECT_EQ(NcString::format("Hello %@", arr), "Hello [\"Earth\", \"Jupiter\"]");
  ```

## StackOrHeapAllocator

In some functions, stack is enough for most cases. But occasionally, larger memory is required.

For example, in NcLog_write(), `char message[2048]' is very, very likely to be enough, but we can't count on it. StackOrHeapAllocator is invented exactly for situations like that.

```cpp
TEST(Stdlib, stackOrHeapAllocator) {
   u8* stack = (u8*)alloca(1024);
   StackOrHeapAllocator allocator(stack, 1024);
   EXPECT_EQ(allocator.allocArray<u8>(512) - stack, 0);
   EXPECT_EQ(allocator.allocArray<u8>(512) - stack, 512);
   EXPECT_GT(allocator.allocArray<u8>(1) - stack, 4096); // stack used up, so it's on heap

   // no leak
}
```

All allocated memory will be freed when `allocator` goes out of scope.

## ManualResetEvent

It simulate Win32 API `SetEvent()`. With C++11's `condition_variable` it's much easier.
I learned it from https://stackoverflow.com/questions/1501111/boost-equivalent-of-manualresetevent

```cpp
ManualResetEvent e;

std::thread t([&] {
   Thread::sleep(100);
   e.set();
});

// must wait until reset
TimeTick start = TimeTick::now();
e.wait();
TimeTick duration = TimeTick::now() - start;
EXPECT_GE(duration.ms(), 100);

t.join();

// not reset(). So wait on it will not block
start = TimeTick::now();
e.wait();
duration = TimeTick::now() - start;
EXPECT_LT(duration.ms(), 1);

// reset() again. Wait will block
e.reset();
start = TimeTick::now();
EXPECT_FALSE(e.waitWithTimeout(10));
duration = TimeTick::now() - start;
EXPECT_GE(duration.ms(), 10);
```

## Many more good stuffs

- Log System
- NcCache

# Design Notes

To be honest, I fall behind in terms of modern C++. Because I held a biased and negative attitude towards it and didn't spend much time to learn it. And in the New Years Days of 2021, I decide to rediscover modern C++. And see how things can be different if I use C++ 14.

## std::shared_ptr vs self-implemented smart pointer.

### The internal of shared_ptr

Before discussing the pros-and-cons of both implementations, I dive into the internal of `std::shared_ptr`

At first, I think `std::shared_ptr` will have to make one additional malloc() for the control block.
Until I read from https://www.nextptr.com/tutorial/ta1358374985/shared_ptr-basics-and-internals-with-examples:

> We mentioned above that the control block could either contain a pointer to
> the managed object or the object itself.
> The control block is dynamically allocated. Constructing the managed object in-place
> within the control block can avoid the two separate memory allocations for the object
> and the control block, resulting in an uncomplicated control block and better performance.
> The std::make_shared is a preferred way to construct a shared_ptr because it builds the
> managed object within the control block.

Later, I also learned there is `std::enabled_shared_from_this`. It can put the control block into the base object.

### Try to use shared_ptr

So I think `std::shared_ptr` might be good enough for my need. And write something out of it:

```cpp
class NcObject : public std::enable_shared_from_this<NcObject> {
};

class Time : public NcObject {
public:
  // I like allocWith() over constructors. Because it allows overloading
  static shared_ptr<Time> allocWithSeconds(int sec) { return std::make_shared<Time>(sec * 1000); }
  static shared_ptr<Time> allocWithMilliseconds(int ms) { return std::make_shared<Time>(ms); }

  // Problem: Constructors must be public. Because std::make_shared needs it.
  Time(int ms) : m_milliseconds(ms) {};

  sp<Time> self() {
     return static_pointer_case<Time>(shared_from_this());
  }

private:
  int m_milliseconds;
};

TEST(NcObject, basic) {
  auto t = Time::allocWithSeconds(10);

  // cast to base
  shared_ptr<NcObject> base = t;
  EXPECT_EQ(t.use_count(), 2);

  // cast to derived
  auto t2 = std::static_pointer_cast<Time>(base);
  EXPECT_EQ(t.use_count(), 3);
  EXPECT_TRUE(t.get() == t2.get()); // underlying the same object

  auto t3 = t->self();
  EXPECT_EQ(t.use_count(), 4);
}
```

`Time::self()` makes an important point to implement something like
`auto pieces = "hello-world"_str->split("-")`.
Each piece contains a strong reference to the original string.

### Things I like & dislike

Like:

1. Strong/weak. It makes delegate design pattern more robust.
2. Can put control block along with the controlled object.

Dislike:

1. Constructors must be public. Some user may call delete or create the object on stack.
2. Convert from raw pointer back to `shared_ptr` is inconvenient.
3. Must call `.get()` to get raw pointer.

I shall explain myself in the following sections.

### Allow implicit conversion

`std::shared_ptr` avoids implicit conversion, for a [good reason](https://www.informit.com/articles/article.aspx?p=31529&seqNum=7). You have to call `.get()` to get a raw pointer.

But I think, for this reason(inconveniency to convert to raw pointer), it tends to be abused by unexperienced programmers.
It pops up everywhere. And there are too many ways of using it:

```cpp
void f(widget* );
void f(widget& );
void f(shared_ptr<widget> );
void f(const shared_ptr<widget>& );
```

About the correct rules for using `shared_ptr`, I agree with [Sutter’s Mill](https://herbsutter.com/2013/06/05/gotw-91-solution-smart-pointer-parameters/) and [acel](https://stackoverflow.com/questions/3310737/should-we-pass-a-shared-ptr-by-reference-or-by-value)

> - Guideline: Don’t pass a smart pointer as a function parameter unless you want to use or manipulate the smart pointer itself, such as to share or transfer ownership.
> - Guideline: Express that a function will store and share ownership of a heap object using a by-value shared_ptr parameter.
> - Guideline: Use a non-const shared_ptr& parameter only to modify the shared_ptr. Use a const shared_ptr& as a parameter only if you’re not sure whether or not you’ll take a copy and share ownership; otherwise use widget\* instead (or if not nullable, a widget&).

So I derived my [rules of using `sp`](#rules-of-using-sp). I think it's more error-proof.

### Makes all memory in one piece with `sp`

With self-implemented control-block in NcObject. I can do some deep optimization, like appending the text directly after the NcString object.
Thus creating the string with a single allocation.
See `allocWithCString::allocWithCString()`

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
   > auto slices : vector<StringSlice> = "hello---world"_s.split("---");
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
sp<NcString> operator""_str(const char* literalStr, size_t len);
StringSlice operator""_s(const char* literalStr, size_t len);
```

This enables creating of literal NcString object or StringSlice.

```cpp
sp<NcString> str = "hello world"_str;
StringSlice slice = "hello world"_s;
```

NcString object created in this way is considered "static".
It has `retainCount()` at INT_MAX.
Calling retain() or release() on it has no effect.
With a global string manager, it's possible to make sure only one NcString object is created for each string literal.

```cpp
TEST(NcString, literal) {
  sp<NcString> s1, s2;
  for (int i = 0; i < 2; i++) {
    sp<NcString> s = "hello world"_str;
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

## Log System

There are often too many aims for a log system:

1. filter with level
2. filter with tags
3. generated message should contains file, line number, functions name
4. support writing to debugger/console/file/remote server
5. support packaging if file is too large

I think the last two requirements are too task-dependent.
(For example, if you are writing server programs, you may want to collect logs with Kubernetes
and then send to Elasticsearch)
So I shall write a log system which only support the first 3 requirements.
By default, the log messages will be written to Visual Studio Debugger(Windows) and
console(Windows & Linux).
But the default behavior can be overridden with NcLog_setCallback().

## Visual Studio Visualizer

`nc-runtime.natvis` is used to improve the debugging experience in Visual Studio.

Visualizer are included for `NcArray`, `sp`, `NcString`, `StringSlice` etc

![](vs_visualizer/visualizer.png)

## Implement operator [] for `sp<NcArray>`

`sp<NcArray>` should work just like ordinary array:

```cpp
auto v = NcArray<NcString>::alloc();
v->addObject("hello"_str);
auto& s = v[0]; // use []
```

To support than:

1. In order to detect element type, a `NcObject::ArrayElement` and `NcArray::ArrayElement` are added.

   NcObject::ArrayElement is unnecessary. There might be a better solution?

   ```cpp
   class NcObject {
   public:
      using ArrayElement = NcObject;
   }

   template <typename T>
   class NcArray : public NcObject {
   public:
      using ArrayElement = T;
   }
   ```

2. Implement operator [] in `sp`

   ```cpp
   template <typename T>
   class sp {
     /**
     * For accessing array element(If it's an array)
     */
     sp<T::ArrayElement>& operator[](int index) {
       return m_ptr->objectAtIndex(index]);
     }
   }
   ```

## Closure(Lambda Functions)

`NcArray::findWithCondition()` uses lambda expression.
See https://www.cprogramming.com/c++11/c++11-lambda-closures.html.

```cpp
auto v = NcArray<NcString>::alloc();
v->addObject(NcString::allocWithCString("hello"));
v->addObject(NcString::allocWithCString("world"));

auto startWord = "w"_s;
auto obj = v->findWithCondition([&](NcString* v) {
   return v->startsWith(startWord);
});
```

The capture syntax is more convenient than the old `userData`:

```cpp
typedef bool(*ArrayFinder)(T* obj, void* userData);
sp<T> findWithCondition(Array finder, void* userData);
```
