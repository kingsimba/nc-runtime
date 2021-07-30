#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/json_setting_loader.h"

TEST(JsonSettingLoaderTest, basic)
{
    auto someRoot = JsonNode::instanceWithContentsOfFile("test_data/config.json");
    ASSERT_TRUE(someRoot.hasValue());

    auto root = someRoot.value();

    auto name = root["user.name"].value().asString();
    ASSERT_TRUE(name.hasValue());
    EXPECT_STREQ(name.value(), "Alexander the Great");

    auto userNode = root.nodeForKey("user");
    auto age = userNode.value()["age"].value().asInt();
    ASSERT_TRUE(age.hasValue());
    EXPECT_EQ(age.value(), 32);

    auto wrongType = userNode.value().asFloat();
    EXPECT_FALSE(wrongType.hasValue());

    auto titles = root["user.titles"].value().asArray();
    ASSERT_TRUE(titles.hasValue());
    EXPECT_EQ(titles.value().arraySize(), 5);
    EXPECT_STREQ(titles.value()[0].value().asString().value(), "King of Macedon");

    EXPECT_FALSE(root["nonexist"].value().asString().hasValue());
    EXPECT_FALSE(root["user.nonexist"].value().asString().hasValue());
    EXPECT_STREQ(root["user.nonexist"].value().asString().Or("simba"), "simba");
}

TEST(JsonSettingLoaderTest, nonexistFile)
{
    auto someRoot = JsonNode::instanceWithContentsOfFile("non-exist.json");
    EXPECT_FALSE(someRoot.hasValue());
    auto root = someRoot.value();
    auto v = root["lua.main"].value().asString();
    ASSERT_FALSE(v.hasValue());
}

static const char s_configJson[] =
    "{\"user\":{\"name\":\"Alexander the Great\",\"titles\":[\"King of Macedon\",\"Hegemon of Hellenic "
    "League\",\"Pharaoh of Egypt\",\"King of Persia\",\"Lord of Asia\"],\"age\":32}}";

TEST(JsonSettingLoaderTest, dumps)
{
    auto someRoot = JsonNode::instanceWithContentsOfFile("test_data/config.json");
    ASSERT_TRUE(someRoot.hasValue());
    auto root = someRoot.value();

    auto json = root.dumpsJson(JSON_COMPACT);
    EXPECT_STREQ(json.value(), s_configJson);
}
