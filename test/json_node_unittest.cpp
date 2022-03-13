#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/json_node.h"

TEST(JsonNodeTest, example)
{
    auto o = JsonNode::makeWithCString(R"(
    {
        "user": {
            "name": "Alexander The Great",
            "age": 38
        },
        "document": {
        }
    })");
    EXPECT_TRUE(o != nullptr);

    // chained access
    auto name = o["user"]["name"].asString();
    ASSERT_TRUE(name.hasValue());
    EXPECT_STREQ(name.value(), "Alexander The Great");
    name = o["user"]["age"].asString();
    ASSERT_FALSE(name.hasValue()); // type error
    name = o["user"]["name"]["non-exist"]["non-exist"].asString();
    ASSERT_FALSE(name.hasValue()); // chain doesn't exist

    // chained access with '.'
    name = o["user.name"].asString();
    ASSERT_TRUE(name.hasValue());
    EXPECT_STREQ(name.value(), "Alexander The Great");

    auto age = o["user.age"].asInt();
    ASSERT_TRUE(age.hasValue());
    EXPECT_EQ(age.value(), 38);

    EXPECT_STREQ(o["user.nonexist"].asString().Or("Tony"), "Tony");

    // Delete
    o.remove("user.age");
    o.remove("document");
    auto str = o.dumpAsString();
    EXPECT_EQ(str, R"({"user": {"name": "Alexander The Great"}})"_s);

    // Add
    o["user"].add("age", JsonNode::integer(22));
    str = o.dumpAsString();
    EXPECT_EQ(str, R"({"user": {"name": "Alexander The Great", "age": 22}})"_s);
}
TEST(JsonNodeTest, basic)
{
    auto root = JsonNode::makeWithContentsOfFile("test_data/config.json");
    ASSERT_TRUE(root != nullptr);

    auto name = root["user.name"].asString();
    ASSERT_TRUE(name.hasValue());
    EXPECT_STREQ(name.value(), "Alexander the Great");

    auto userNode = root.nodeForKey("user");
    auto age = userNode["age"].asInt();
    ASSERT_TRUE(age.hasValue());
    EXPECT_EQ(age.value(), 32);
    auto ageFloat = userNode["age"].asFloat();
    ASSERT_TRUE(ageFloat.hasValue());
    EXPECT_FLOAT_EQ(ageFloat.value(), 32.0f);
    auto wrongType = userNode.asFloat();
    EXPECT_FALSE(wrongType.hasValue());

    auto height = userNode["height"].asFloat();
    EXPECT_FLOAT_EQ(height.value(), 1.8f);
    auto heightInt = userNode["height"].asInt();
    EXPECT_FALSE(heightInt.hasValue());

    auto titles = root["user.titles"].asArray();
    ASSERT_TRUE(titles != nullptr);
    EXPECT_EQ(titles.arraySize(), 5);
    EXPECT_STREQ(titles[0].asString().value(), "King of Macedon");

    EXPECT_FALSE(root["nonexist"].asString().hasValue());
    EXPECT_FALSE(root["user.nonexist"].asString().hasValue());
    EXPECT_STREQ(root["user.nonexist"].asString().Or("simba"), "simba");

    EXPECT_TRUE(root["user.forward"].isTrue());
    EXPECT_FALSE(root["user.nonexist"].isTrue());
    EXPECT_FALSE(root["user.nonexist"].isFalse());
    EXPECT_FALSE(root["user.age"].isTrue());
    EXPECT_FALSE(root["user.age"].isFalse());

    EXPECT_TRUE(JsonNode::nullObject() != nullptr);
    EXPECT_TRUE(JsonNode::nullObject().isNull());
}

TEST(JsonNodeTest, nonexistFile)
{
    auto root = JsonNode::makeWithContentsOfFile("non-exist.json");
    EXPECT_TRUE(root == nullptr);
    auto v = root["lua.main"].asString();
    ASSERT_FALSE(v.hasValue());
}

static const StringSlice s_configJson =
    "{\"user\":{\"name\":\"Alexander the Great\",\"titles\":[\"King of Macedon\",\"Hegemon of Hellenic "
    "League\",\"Pharaoh of Egypt\",\"King of Persia\",\"Lord of Asia\"],\"age\":32,\"forward\":true,\"height\":1.8}}"_s;

TEST(JsonNodeTest, dumps)
{
    auto root = JsonNode::makeWithContentsOfFile("test_data/config.json");
    ASSERT_TRUE(root != nullptr);

    auto json = root.dumpAsString(JSON_COMPACT);
    EXPECT_EQ(json, s_configJson);

    JsonNode n;
    StringSlice s = n.dump();
    EXPECT_TRUE(s.length() == 0);
}

TEST(JsonNodeTest, array)
{
    auto json = JsonNode::array();
    EXPECT_EQ(json.dump(), "[]");
    EXPECT_EQ(json.arraySize(), 0);

    json.append(JsonNode::integer(1));
    EXPECT_EQ(json.arraySize(), 1);
    json.append(JsonNode::string("hello"));
    EXPECT_EQ(json.arraySize(), 2);
    json.append(JsonNode::boolean(true));
    EXPECT_EQ(json.arraySize(), 3);

    EXPECT_EQ(json[0].asInt().value(), 1);
    EXPECT_EQ(strcmp(json[1].asString().value(), "hello"), 0);
    EXPECT_EQ(json[2].asBool().value(), true);
}
