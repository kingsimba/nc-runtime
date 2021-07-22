#include "stdafx_nc_runtime.h"
#include "nc_runtime/json_setting_loader.h"

JsonSettingLoader::JsonSettingLoader()
{
}

JsonSettingLoader::~JsonSettingLoader()
{
}

bool JsonSettingLoader::loadFile(const char* file)
{
    json_error_t err;
    json_t* root = json_load_file(file, 0, &err);
    if (root == nullptr)
    {
        NC_LOG_ERROR("Failed to load JsonConfig %s", file);
        return false;
    }

    m_root = JsonSettingNode(root);
    json_decref(root);

    return true;
}

Some<JsonSettingNode> JsonSettingNode::nodeForKey(const char* key)
{
    auto node = _nodeForKey(key);
    if (node == nullptr)
        return noValue;

    return JsonSettingNode(node);
}

json_t* JsonSettingNode::_nodeForKey(const char* key)
{
    char keyCopy[512];
    strcpy(keyCopy, key);
    char* context = keyCopy;
    char* token;
    json_t* node = m_root;
    while (node != nullptr && (token = nc_strtok(NULL, ".", &context)) != NULL)
    {
        node = json_object_get(node, token);
    }
    return node;
}

Some<int> JsonSettingNode::intValueForKey(const char* key)
{
    auto node = _nodeForKey(key);
    if (!json_is_integer(node))
        return noValue;

    return (int)json_integer_value(node);
}

Some<i64> JsonSettingNode::int64ValueForKey(const char* key)
{
    auto node = _nodeForKey(key);
    if (!json_is_integer(node))
        return noValue;

    return json_integer_value(node);
}

Some<const char*> JsonSettingNode::stringValueForKey(const char* key)
{
    auto node = _nodeForKey(key);
    if (!json_is_string(node))
        return noValue;

    return json_string_value(node);
}

Some<u32> JsonSettingNode::u32ValueForKey(const char* key)
{
    auto node = _nodeForKey(key);
    if (!json_is_integer(node))
        return noValue;

    return (u32)json_integer_value(node);
}

Some<float> JsonSettingNode::floatValueForKey(const char* key)
{
    auto node = _nodeForKey(key);
    if (!json_is_real(node))
        return noValue;

    return (float)json_real_value(node);
}
