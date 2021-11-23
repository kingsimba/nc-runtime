#pragma once

#include "jansson/jansson.h"
#include "nc_runtime/nc_string.h"
#include "nc_runtime/nc_types.h"

/**
 * auto o = JsonNode::instanceWithCString(R"(
 *   {
 *       "user": {
 *           "name": "Alexander The Great",
 *           "age": 38
 *      },
 *       "document": {
 *       }
 *   })");
 *   EXPECT_TRUE(o.hasValue());
 *
 *   auto name = o["user.name"]->asString();
 *   ASSERT_TRUE(name.hasValue());
 *   EXPECT_STREQ(name.value(), "Alexander The Great");
 *
 *   auto age = o["user.age"]->asInt();
 *   ASSERT_TRUE(age.hasValue());
 *   EXPECT_EQ(age.value(), 38);
 *
 *   EXPECT_STREQ(o["user.nonexist"]->asString().Or("Tony"), "Tony");
 *
 *   // Delete
 *   o->remove("user.age");
 *   o->remove("document");
 *   auto str = o->dump();
 *   EXPECT_EQ(str, R"({"user": {"name": "Alexander The Great"}})");
 */
class JsonNode
{
public:
    static Some<JsonNode> makeWithContentsOfFile(const char* file);
    static Some<JsonNode> makeWithCString(const char* buffer);
    static Some<JsonNode> makeWithStringSlice(const StringSlice& r);

    NC_DEPRECATED static Some<JsonNode> instanceWithContentsOfFile(const char* file); // please use makeWithXxx()
    NC_DEPRECATED static Some<JsonNode> instanceWithCString(const char* buffer);      // please use makeWithXxx()

    JsonNode() = default;
    JsonNode(json_t* root) : m_root(json_incref(root)) {}
    JsonNode(const JsonNode& r) { m_root = json_incref(r.m_root); }
    ~JsonNode() { json_decref(m_root); }

    //////////////////////////////////////////////////////////////////////////
    // Object creation
    static JsonNode integer(i64 v)
    {
        JsonNode o;
        o.m_root = json_integer(v);
        return o;
    }
    static JsonNode real(double v)
    {
        JsonNode o;
        o.m_root = json_real(v);
        return o;
    }
    static JsonNode string(const StringSlice& v)
    {
        JsonNode o;
        o.m_root = json_stringn(v.internalBytes(), v.length());
        return o;
    }
    static JsonNode boolean(bool v)
    {
        JsonNode o;
        o.m_root = json_boolean(v);
        return o;
    }
    static JsonNode object()
    {
        JsonNode o;
        o.m_root = json_object();
        return o;
    }

    //////////////////////////////////////////////////////////////////////////
    // Operations
    void add(const char* key, const JsonNode& node);
    bool remove(const char* key);
    Some<JsonNode> nodeForKey(const char* key);
    Some<JsonNode> operator[](const char* key);
    Some<JsonNode> operator[](int index);

    //////////////////////////////////////////////////////////////////////////
    // Value
    Some<int> asInt();
    Some<i64> asInt64();
    Some<u32> asU32();
    Some<const char*> asString();
    Some<float> asFloat();

    Some<bool> asBool();
    forceinline bool isTrue()
    {
        auto v = asBool();
        return v.hasValue() && v.value();
    }
    forceinline bool isFalse()
    {
        Some<bool> v = asBool();
        return v.hasValue() && !v.value();
    }

    Some<JsonNode> asArray();
    int arraySize();

    //////////////////////////////////////////////////////////////////////////
    // Other
    json_t* rawNode() { return m_root; }

    /**
     * Returns the JSON representation of json as a string, or NULL on error.
     * The return value must be freed by the caller using free().
     */
    StringSlice dump(size_t flags = 0);
    sp<NcString> dumpAsString(size_t flags = 0);

    void operator=(const JsonNode& r)
    {
        json_decref(m_root);
        m_root = json_incref(r.m_root);
    }

private:
    json_t* _nodeForKey(const char* key);

private:
    json_t* m_root = nullptr;
};

template <>
class Some<JsonNode>
{
public:
    forceinline Some(const NoValueType& v) : m_hasValue(false) { UNUSED_VAR(v); }
    forceinline Some(const JsonNode& v) : m_hasValue(true), m_value(v) {}
    forceinline Some(const JsonNode&& v) : m_hasValue(true), m_value(std::move(v)) {}

    forceinline bool hasValue() { return m_hasValue; }

    forceinline const JsonNode& Or(const JsonNode& r) { return m_hasValue ? m_value : r; }

    JsonNode* operator->() { return &m_value; }

    Some<JsonNode> operator[](const char* key) { return m_value[key]; }
    Some<JsonNode> operator[](int index) { return m_value[index]; }

protected:
    bool m_hasValue;
    JsonNode m_value;
};
