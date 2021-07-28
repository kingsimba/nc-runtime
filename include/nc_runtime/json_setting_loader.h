#pragma once

#include "jansson/jansson.h"
#include "nc_runtime/nc_types.h"

class JsonNode
{
public:
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

    Some<JsonNode> asArray();
    int arraySize();

    /**
     * Returns the JSON representation of json as a string, or NULL on error.
     * The return value must be freed by the caller using free().
     */
    Some<char*> dumpsJson(size_t flags = 0);

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

/**
 * JsonSettingLoader o;
 * o.loadFile("test_data/config.json");
 * auto root = o.root();
 * auto v = root.stringValueForKey("user.name");
 * ASSERT_TRUE(v.hasValue());
 * EXPECT_STREQ(v.value(), "Alaxender The Great");
 * EXPECT_STREQ(root.stringValueForKey("user.nonexist").Or("Tony"), "Tony");
 */
class JsonSettingLoader
{
public:
    JsonSettingLoader();
    ~JsonSettingLoader();

    bool loadFile(const char* file);
    bool loadFromBuffer(const char* buffer);

    JsonNode& root() { return m_root; }

private:
    JsonNode m_root;
};
