#include "stdafx_nc_runtime.h"
#include "nc_runtime/json_node.h"
#include "nc_runtime/nc_string.h"
#include "nc_runtime/nc_data.h"

Some<JsonNode> JsonNode::makeWithContentsOfFile(const StringSlice& file)
{
    auto data = NcData::allocWithContentsOfFile(file);
    if (data == nullptr)
        return noValue;

    return makeWithStringSlice(StringSlice::makeEphemeralWithBytes((char*)data->bytes(), data->length()));
}

Some<JsonNode> JsonNode::makeWithCString(const char* buffer)
{
    auto slice = StringSlice::makeEphemeral(buffer);
    return makeWithStringSlice(slice);
}

Some<JsonNode> JsonNode::makeWithStringSlice(const StringSlice& r)
{
    json_error_t err;
    json_t* root = json_loadb(r.internalBytes(), r.length(), 0, &err);
    if (root == nullptr)
    {
        NC_LOG_ERROR("Failed to load JsonConfig from buffer.");
        return noValue;
    }

    auto node = JsonNode(root);
    json_decref(root);

    return node;
}

NC_DEPRECATED Some<JsonNode> JsonNode::instanceWithContentsOfFile(const char* file)
{
    return makeWithContentsOfFile(file);
}

NC_DEPRECATED Some<JsonNode> JsonNode::instanceWithCString(const char* buffer)
{
    return makeWithCString(buffer);
}

void JsonNode::add(const char* key, const JsonNode& node)
{
    json_object_set(m_root, key, node.m_root);
}

bool JsonNode::remove(const char* key)
{
    if (strchr(key, '.') == NULL)
    {
        return json_object_del(m_root, key) != 0;
    }
    else
    {
        char keyCopy[512];
        strcpy(keyCopy, key);
        char* context = keyCopy;
        char* token;
        json_t* node = m_root;
        json_t* parent = NULL;
        char* lastNodeName = NULL;
        while (node != nullptr && (token = nc_strtok(NULL, ".", &context)) != NULL)
        {
            parent = node;
            lastNodeName = token;
            node = json_object_get(node, token);
        }

        return parent != NULL && json_object_del(parent, lastNodeName) != 0;
    }
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
    if (strchr(key, '.') == NULL)
    {
        return json_object_get(m_root, key);
    }
    else
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
    if (json_is_number(m_root))
        return (float)json_number_value(m_root);
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

StringSlice JsonNode::dump(size_t flags /*= 0*/)
{
    char* buffer = json_dumps(m_root, flags);
    auto str = NcString::allocByTakingBytes(buffer, strlen(buffer))->toSlice();
    return str;
}

sp<NcString> JsonNode::dumpAsString(size_t flags /*= 0*/)
{
    auto slice = dump(flags);
    return NcString::allocWithSlice(slice);
}
