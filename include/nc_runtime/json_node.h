#pragma once

#include "jansson/jansson.h"
#include "nc_runtime/nc_string.h"
#include "nc_runtime/nc_types.h"

/**
 * JsonSettingLoader o;
 * o.loadFile("test_data/config.json");
 * auto root = o.root();
 * auto v = root.stringValueForKey("user.name");
 * ASSERT_TRUE(v.hasValue());
 * EXPECT_STREQ(v.value(), "Alaxender The Great");
 * EXPECT_STREQ(root.stringValueForKey("user.nonexist").Or("Tony"), "Tony");
 */
class JsonNode
{
public:
    static Some<JsonNode> instanceWithContentsOfFile(const char* file);
    static Some<JsonNode> instanceWithCString(const char* buffer);

    JsonNode() {}
    JsonNode(json_t* root) : m_root(json_incref(root)) {}
    JsonNode(const JsonNode& r) { m_root = json_incref(r.m_root); }
    ~JsonNode() { json_decref(m_root); }

    Some<JsonNode> nodeForKey(const char* key);
    Some<JsonNode> operator[](const char* key);
    Some<JsonNode> operator[](int index);

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

    json_t* rawNode() { return m_root; }

    /**
     * Returns the JSON representation of json as a string, or NULL on error.
     * The return value must be freed by the caller using free().
     */
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
