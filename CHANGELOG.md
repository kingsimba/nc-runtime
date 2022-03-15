# Changelog

- 2022-03-15 Use template for Vector3 and Quaternion.
- 2022-03-15 Add `double` version of some functions - `nc_wrapToTwoPi()`, `Math_mod()`, `Math_turnAngle()`.
- 2022-03-13 Add operator `==` and `!=` for `Pose2`.
- 2022-03-13 Add `JsonNode::array()` and `JsonNode:append()`.
- 2022-03-01 Add `Math_rectPointDistance()`.
- 2022-02-28 Add `FixedCircularArray`.
- 2022-02-22 Add `ImageU8::allocWithData()`.
- 2022-01-19 Add `RectF::isNegtiveMinimum(), RectF::expand(float width, float height)`.
- 2022-01-13 Add `StableRate` for loop rate control.
- 2022-01-07 Add `TimeDuration`. Add a lot of operator overloads for `TimeDuration` and `TimeTick`.
- 2021-12-31 Add default constructor for `Some<T>`.
- 2021-12-10 Improve JsonNode. Remove `Some<JsonNode>` etc. |api-break|

  - Remove `Some<JsonNode>`. JsonNode itself can be compared with `nullptr`.
  - Add `JsonNode::nullObject()`. It is a valid json node and totally different from `nullptr`.
  - Fix bug in JsonNode::dump().

- 2021-12-09 Add `StringSlice::cstr()`.

  `cstr()` may make a copy of the memory if it's not '\0' ended.
  Deprecate StringSlice::toCString().

- 2021-12-08 RCSS-813 related:

  - Add `ImagePainter::line()`.
  - Add parameter `CustomInflationFunc func` to constructor of `CostmapInflator`.
  - Add `CostmapInflator::setSeed()`.

- 2021-12-06 Add Base64.
- 2021-11-25 Add StringSlice::makeWithContentsOfFile().
- 2021-11-25 Fix bug in StringSlice::trim().
- 2021-11-24 Add StringSlice::trim(), trimStart(), trimEnd().
- 2021-11-24 Support conversion between std::string and StringSlice.
- 2021-11-20 Major improves on `StringSlice`, `sp<T>`

  - StringSlice become the major player, instead of NcString.
  - Add `join`, `format`, etc in StringSlice.
  - Disallow implict conversion from raw pointer to `sp<T>`. Use `retainAsSp()`.

- 2021-11-15 Support JSON modification with`JsonNode::remove()`, `JsonNode::add()`, `JsonNode::object()`, `JsonNode::integer()`, `JsonNode::real()` etc.
- 2021-11-15 Improve `NcObject::alloc()`.
- 2021-11-08 Add `NC_DEPRECATED`
- 2021-11-05 Add `NcObject::alloc<T>()`. Avoid using of new/delete.
- 2021-11-05 Add StateMachine.
- 2021-10-26 Add `Math_segmentPointDistance()`.
- 2021-10-22 Fix a bug in `RectF::setAsNegativeMinimum()`.
- 2021-10-18 Add `Math_linePointDistance()` and `Math_segmentsIntersect()`.
- 2021-10-14 Add `Vector2i` and `Math_clipSegmentByRect()`.
- 2021-10-12 Add `Rect::setAsNegativeMinimum()`, `Rect::combinePoint()` and `Rect::combineRect()`.
- 2021-10-05 Add `Math_pointInPolygon()` and `NcPolyline::testPoint()`.
- 2021-09-28 Fixed bug in JsonNode::asFloat().
- 2021-09-26 Add `NcPolyline` to support polyline and polygon.
- 2021-09-15 Add `CostmapInflator::inflateInplace()`.
- 2021-09-13 Add NcString::join() and NcString::allocByJoiningStrings().
- 2021-09-03 Add Mutex, LockGuard. Use GUARDED_BY() to detect thread error with clang.
- 2021-09-03 Add StringSlice::equalsCaseInsensitive().
- 2021-09-03 Add JsonNode::asBool(), JsonNode::isTrue(), JsonNode::isFalse().
- 2021-08-18 Add nc_ntohl() nc_ntohs(), nc_flipU16(), nc_flipU32() for endian swapping.
- 2021-08-18 `ResetableEvent` supports auto-reset.
- 2021-08-04 Add `Vector3` and some common operations.
- 2021-07-14 Add `JsonSettingLoader`
- 2021-07-14 Add "nc_argparse.h"
- 2021-07-13 Add `Math_turnAngle()`, `Math_wrapTwoPi()`
- 2021-07-02 Add `nc_abs()`, `nc_min()`, `nc_max()`, `nc_clamp()`
- 2021-07-02 Add `Quaternion` and `QuaterionD`.
- 2021-06-30 Add `nc_isnan()`.
- 2021-06-30 Add `nc_radianToDegree()` and `nc_degreeToRadian()`.
- 2021-06-30 Add single & double precision PI. NC_PI, NC_PI_2, NC_PI_D, NC_PI_2_D etc.
- 2021-06-22 Add NcImage, NcImageU8 and CostmapInflator.
- 2021-06-22 Add `StandardFreer`, `nc_copyArray()`.
- 2021-03-29 Add NC_ASSERT()
- 2021-03-24 Add `Some` and `noValue`.
- 2021-03-24 Add NcFile & NcData.
- 2021-03-05 Fix bug in copying of wp.
- 2021-03-05 Fix bug in global strings.
- 2021-02-28 Add wp, equivalent of std::weak_ptr.
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
