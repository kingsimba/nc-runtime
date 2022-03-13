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
 *   EXPECT_TRUE(o != nullptr);
 *
 *   auto name = o["user.name"].asString();
 *   ASSERT_TRUE(name.hasValue());
 *   EXPECT_STREQ(name.value(), "Alexander The Great");
 *
 *   auto age = o["user.age"].asInt();
 *   ASSERT_TRUE(age.hasValue());
 *   EXPECT_EQ(age.value(), 38);
 *
 *   EXPECT_STREQ(o["user.nonexist"].asString().Or("Tony"), "Tony");
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
    static JsonNode makeWithContentsOfFile(const StringSlice& filename);
    static JsonNode makeWithCString(const char* buffer);
    static JsonNode makeWithStringSlice(const StringSlice& r);

    JsonNode() = default;
    JsonNode(std::nullptr_t){};
    JsonNode(JsonNode&& r) noexcept
    {
        m_root = r.m_root;
        r.m_root = nullptr;
    }
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
    static JsonNode array()
    {
        JsonNode o;
        o.m_root = json_array();
        return o;
    }
    static JsonNode nullObject()
    {
        JsonNode o;
        o.m_root = json_null();
        return o;
    }

    //////////////////////////////////////////////////////////////////////////
    // Operations
    void add(const char* key, const JsonNode& node);
    bool remove(const char* key);
    JsonNode nodeForKey(const char* key);
    JsonNode operator[](const char* key);

    void append(const JsonNode& node);
    JsonNode operator[](int index);

    //////////////////////////////////////////////////////////////////////////
    // Value
    Some<int> asInt();
    Some<i64> asInt64();
    Some<u32> asU32();
    Some<const char*> asString();
    Some<float> asFloat();
    Some<bool> asBool();
    JsonNode asArray();
    int arraySize();

    forceinline bool isTrue() { return json_is_true(m_root); }
    forceinline bool isFalse() { return json_is_false(m_root); }
    forceinline bool isNull() { return json_is_null(m_root); }

    forceinline bool operator!=(std::nullptr_t) const { return m_root != nullptr; }
    forceinline bool operator==(std::nullptr_t) const { return m_root == nullptr; }

    //////////////////////////////////////////////////////////////////////////
    // Other

    /**
     * Returns the JSON representation as a string.
     * @remarks
     *     Only object and array can be dumped.
     * @return
     *     Return empty string on error.
     */
    StringSlice dump(size_t flags = 0);

    /**
     * Returns the JSON representation as a string.
     *
     * @remarks
     *     Only object and array can be dumped.
     * @return
     *     Return `nullptr` on error.
     */
    sp<NcString> dumpAsString(size_t flags = 0);

    void operator=(const JsonNode& r)
    {
        json_decref(m_root);
        m_root = json_incref(r.m_root);
    }

private:
    JsonNode(json_t* root) : m_root(json_incref(root)) {}
    json_t* _nodeForKey(const char* key);

private:
    json_t* m_root = nullptr;
};
