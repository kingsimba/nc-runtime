#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/json_node.h"

TEST(JsonSettingLoaderTest, example)
{
    auto o = JsonNode::instanceWithCString(R"(
    {
        "user": {
            "name": "Alexander The Great",
            "age": 38
        },
        "document": {
        }
    })");
    EXPECT_TRUE(o.hasValue());

    // chained access
    auto name = o["user"]["name"]->asString();
    ASSERT_TRUE(name.hasValue());
    EXPECT_STREQ(name.value(), "Alexander The Great");
    name = o["user"]["age"]->asString();
    ASSERT_FALSE(name.hasValue()); // type error
    name = o["user"]["name"]["non-exist"]["non-exist"]->asString();
    ASSERT_FALSE(name.hasValue()); // chain doesn't exist

    // chained access with '.'
    name = o["user.name"]->asString();
    ASSERT_TRUE(name.hasValue());
    EXPECT_STREQ(name.value(), "Alexander The Great");

    auto age = o["user.age"]->asInt();
    ASSERT_TRUE(age.hasValue());
    EXPECT_EQ(age.value(), 38);

    EXPECT_STREQ(o["user.nonexist"]->asString().Or("Tony"), "Tony");

    // Delete
    o->remove("user.age");
    o->remove("document");
    auto str = o->dumpAsString();
    EXPECT_STREQ(str->cstr(), R"({"user": {"name": "Alexander The Great"}})");

    // Add
    o["user"]->add("age", JsonNode::integer(22));
    str = o->dumpAsString();
    EXPECT_STREQ(str->cstr(), R"({"user": {"name": "Alexander The Great", "age": 22}})");
}
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
    auto ageFloat = userNode["age"]->asFloat();
    ASSERT_TRUE(ageFloat.hasValue());
    EXPECT_FLOAT_EQ(ageFloat.value(), 32.0f);
    auto wrongType = userNode->asFloat();
    EXPECT_FALSE(wrongType.hasValue());

    auto height = userNode["height"]->asFloat();
    EXPECT_FLOAT_EQ(height.value(), 1.8f);
    auto heightInt = userNode["height"]->asInt();
    EXPECT_FALSE(heightInt.hasValue());

    auto titles = root["user.titles"]->asArray();
    ASSERT_TRUE(titles.hasValue());
    EXPECT_EQ(titles->arraySize(), 5);
    EXPECT_STREQ(titles[0]->asString().value(), "King of Macedon");

    EXPECT_FALSE(root["nonexist"]->asString().hasValue());
    EXPECT_FALSE(root["user.nonexist"]->asString().hasValue());
    EXPECT_STREQ(root["user.nonexist"]->asString().Or("simba"), "simba");

    EXPECT_TRUE(root["user.forward"]->isTrue());
    EXPECT_FALSE(root["user.nonexist"]->isTrue());
    EXPECT_FALSE(root["user.nonexist"]->isFalse());
    EXPECT_FALSE(root["user.age"]->isTrue());
    EXPECT_FALSE(root["user.age"]->isFalse());
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
    "League\",\"Pharaoh of Egypt\",\"King of Persia\",\"Lord of Asia\"],\"age\":32,\"forward\":true,\"height\":1.8}}";

TEST(JsonSettingLoaderTest, dumps)
{
    auto root = JsonNode::instanceWithContentsOfFile("test_data/config.json");
    ASSERT_TRUE(root.hasValue());

    sp<NcString> json = root->dumpAsString(JSON_COMPACT);
    EXPECT_STREQ(json->cstr(), s_configJson);
}
