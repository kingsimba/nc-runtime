#pragma once

#include "jansson/jansson.h"
#include "nc_runtime/nc_types.h"

class JsonSettingNode
{
public:
    JsonSettingNode() {}
    JsonSettingNode(json_t* root) : m_root(json_incref(root)) {}
    JsonSettingNode(const JsonSettingNode& r) { m_root = json_incref(r.m_root); }
    ~JsonSettingNode() { json_decref(m_root); }

    Some<JsonSettingNode> nodeForKey(const char* key);

    Some<int> intValueForKey(const char* key);
    Some<i64> int64ValueForKey(const char* key);
    Some<const char*> stringValueForKey(const char* key);
    Some<u32> u32ValueForKey(const char* key);
    Some<float> floatValueForKey(const char* key);

    int arraySize() { return (int)json_array_size(m_root); }
    JsonSettingNode arrayElementAtIndex(int i) { return JsonSettingNode(json_array_get(m_root, i)); }

    void operator=(const JsonSettingNode& r)
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

    JsonSettingNode& root() { return m_root; }

private:
    JsonSettingNode m_root;
};
