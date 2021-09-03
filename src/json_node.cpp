#include "stdafx_nc_runtime.h"
#include "nc_runtime/json_node.h"
#include "nc_runtime/nc_string.h"

Some<JsonNode> JsonNode::instanceWithContentsOfFile(const char* file)
{
    json_error_t err;
    json_t* root = json_load_file(file, 0, &err);
    if (root == nullptr)
    {
        NC_LOG_ERROR("Failed to load JsonConfig %s.", file);
        return noValue;
    }

    auto node = JsonNode(root);
    json_decref(root);

    return node;
}

Some<JsonNode> JsonNode::instanceWithCString(const char* buffer)
{
    json_error_t err;
    json_t* root = json_loads(buffer, 0, &err);
    if (root == nullptr)
    {
        NC_LOG_ERROR("Failed to load JsonConfig from buffer.");
        return noValue;
    }

    auto node = JsonNode(root);
    json_decref(root);

    return node;
}

Some<JsonNode> JsonNode::nodeForKey(const char* key)
{
    auto node = _nodeForKey(key);
    if (node == nullptr)
        return JsonNode();

    return JsonNode(node);
}

json_t* JsonNode::_nodeForKey(const char* key)
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

Some<JsonNode> JsonNode::operator[](const char* key)
{
    return nodeForKey(key);
}

Some<JsonNode> JsonNode::operator[](int index)
{
    if (json_is_array(m_root))
        return JsonNode(json_array_get(m_root, index));

    return JsonNode();
}

Some<int> JsonNode::asInt()
{
    if (json_is_integer(m_root))
        return (int)json_integer_value(m_root);
    else
        return noValue;
}

Some<i64> JsonNode::asInt64()
{
    if (json_is_integer(m_root))
        return json_integer_value(m_root);
    else
        return noValue;
}

Some<u32> JsonNode::asU32()
{
    if (json_is_integer(m_root))
        return (u32)json_integer_value(m_root);
    else
        return noValue;
}

Some<const char*> JsonNode::asString()
{
    if (json_is_string(m_root))
        return json_string_value(m_root);
    else
        return noValue;
}

Some<float> JsonNode::asFloat()
{
    if (json_is_real(m_root))
        return (float)json_real_value(m_root);
    else
        return noValue;
}

Some<bool> JsonNode::asBool()
{
    if (json_is_boolean(m_root))
        return json_boolean_value(m_root);
    else
        return noValue;
}

Some<JsonNode> JsonNode::asArray()
{
    if (json_is_array(m_root))
        return JsonNode(m_root);
    else
        return noValue;
}

int JsonNode::arraySize()
{
    if (json_is_array(m_root))
        return (int)json_array_size(m_root);

    return 0;
}

sp<NcString> JsonNode::dumpAsString(size_t flags /*= 0*/)
{
    char* buffer = json_dumps(m_root, flags);
    auto str = NcString::allocWithCString(buffer);
    free(buffer);
    return str;
}
