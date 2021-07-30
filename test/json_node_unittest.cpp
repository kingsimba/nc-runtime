#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/json_node.h"

TEST(JsonSettingLoaderTest, basic)
{
    auto root = JsonNode::instanceWithContentsOfFile("test_data/config.json");
    ASSERT_TRUE(root.hasValue());

    auto name = root["user.name"]->asString();
    ASSERT_TRUE(name.hasValue());
    EXPECT_STREQ(name.value(), "Alexander the Great");

    auto userNode = root->nodeForKey("user");
    auto age = userNode["age"]->asInt();
    ASSERT_TRUE(age.hasValue());
    EXPECT_EQ(age.value(), 32);
    auto wrongType = userNode->asFloat();
    EXPECT_FALSE(wrongType.hasValue());

    auto titles = root["user.titles"]->asArray();
    ASSERT_TRUE(titles.hasValue());
    EXPECT_EQ(titles->arraySize(), 5);
    EXPECT_STREQ(titles[0]->asString().value(), "King of Macedon");

    EXPECT_FALSE(root["nonexist"]->asString().hasValue());
    EXPECT_FALSE(root["user.nonexist"]->asString().hasValue());
    EXPECT_STREQ(root["user.nonexist"]->asString().Or("simba"), "simba");
}

TEST(JsonSettingLoaderTest, nonexistFile)
{
    auto root = JsonNode::instanceWithContentsOfFile("non-exist.json");
    EXPECT_FALSE(root.hasValue());
    auto v = root["lua.main"]->asString();
    ASSERT_FALSE(v.hasValue());
}

static const char s_configJson[] =
    "{\"user\":{\"name\":\"Alexander the Great\",\"titles\":[\"King of Macedon\",\"Hegemon of Hellenic "
    "League\",\"Pharaoh of Egypt\",\"King of Persia\",\"Lord of Asia\"],\"age\":32}}";

TEST(JsonSettingLoaderTest, dumps)
{
    auto root = JsonNode::instanceWithContentsOfFile("test_data/config.json");
    ASSERT_TRUE(root.hasValue());

    auto json = root->dumpsJson(JSON_COMPACT);
    EXPECT_STREQ(json.value(), s_configJson);
}
