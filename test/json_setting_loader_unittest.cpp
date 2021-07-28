#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/json_setting_loader.h"

TEST(JsonSettingLoaderTest, basic)
{
    JsonSettingLoader o;
    ASSERT_TRUE(o.loadFile("test_data/config.json"));
    auto root = o.root();

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
    JsonSettingLoader o;
    EXPECT_FALSE(o.loadFile("non-exist.json"));
    auto root = o.root();
    auto v = root["lua.main"].value().asString();
    ASSERT_FALSE(v.hasValue());
}

static const char s_configJson[] = "{\"user\":{\"name\":\"Alexander the Great\",\"titles\":[\"King of Macedon\",\"Hegemon of Hellenic League\",\"Pharaoh of Egypt\",\"King of Persia\",\"Lord of Asia\"],\"age\":32}}";

TEST(JsonSettingLoaderTest, dumps)
{
    JsonSettingLoader o;
    ASSERT_TRUE(o.loadFile("test_data/config.json"));
    auto root = o.root();

    auto json = root.dumpsJson(JSON_COMPACT);
    EXPECT_STREQ(json.value(), s_configJson);
}
