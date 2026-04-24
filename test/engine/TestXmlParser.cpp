#include <gtest/gtest.h>
#include "XmlParser.h"

// ---------------------------------------------------------------------------
// ParseString — basic element
// ---------------------------------------------------------------------------

TEST(XmlParserTest, SimpleElement) {
  XmlNode root = XmlParser::ParseString("<root></root>");
  EXPECT_EQ(root.tag, "root");
  EXPECT_TRUE(root.children.empty());
  EXPECT_TRUE(root.text.empty());
}

TEST(XmlParserTest, SelfClosingElement) {
  XmlNode root = XmlParser::ParseString("<img src=\"tile.png\"/>");
  EXPECT_EQ(root.tag, "img");
  EXPECT_EQ(root.getAttribute("src"), "tile.png");
  EXPECT_TRUE(root.children.empty());
}

// ---------------------------------------------------------------------------
// Attributes
// ---------------------------------------------------------------------------

TEST(XmlParserTest, MultipleAttributes) {
  XmlNode root = XmlParser::ParseString("<map width=\"30\" height=\"20\" tilewidth=\"64\"/>");
  EXPECT_EQ(root.getAttribute("width"),     "30");
  EXPECT_EQ(root.getAttribute("height"),    "20");
  EXPECT_EQ(root.getAttribute("tilewidth"), "64");
}

TEST(XmlParserTest, MissingAttributeReturnsDefault) {
  XmlNode root = XmlParser::ParseString("<map width=\"10\"/>");
  EXPECT_EQ(root.getAttribute("height", "0"), "0");
  EXPECT_EQ(root.getAttribute("missing"),     "");
}

TEST(XmlParserTest, SingleQuotedAttribute) {
  XmlNode root = XmlParser::ParseString("<tag key='value'/>");
  EXPECT_EQ(root.getAttribute("key"), "value");
}

// ---------------------------------------------------------------------------
// Children
// ---------------------------------------------------------------------------

TEST(XmlParserTest, ChildElement) {
  XmlNode root = XmlParser::ParseString("<root><child/></root>");
  EXPECT_EQ(root.children.size(), 1u);
  EXPECT_EQ(root.children[0].tag, "child");
}

TEST(XmlParserTest, MultipleChildren) {
  XmlNode root = XmlParser::ParseString(
    "<root><a/><b/><c/></root>"
  );
  EXPECT_EQ(root.children.size(), 3u);
  EXPECT_EQ(root.children[0].tag, "a");
  EXPECT_EQ(root.children[1].tag, "b");
  EXPECT_EQ(root.children[2].tag, "c");
}

TEST(XmlParserTest, NestedChildren) {
  XmlNode root = XmlParser::ParseString(
    "<map><layer><data encoding=\"csv\">1,2,3</data></layer></map>"
  );
  EXPECT_EQ(root.tag, "map");
  const XmlNode* layer = root.findChild("layer");
  ASSERT_NE(layer, nullptr);
  const XmlNode* data = layer->findChild("data");
  ASSERT_NE(data, nullptr);
  EXPECT_EQ(data->getAttribute("encoding"), "csv");
  EXPECT_EQ(data->text, "1,2,3");
}

// ---------------------------------------------------------------------------
// findChild / findChildren
// ---------------------------------------------------------------------------

TEST(XmlParserTest, FindChildReturnsNull) {
  XmlNode root = XmlParser::ParseString("<root><a/></root>");
  EXPECT_EQ(root.findChild("missing"), nullptr);
}

TEST(XmlParserTest, FindChildrenMultipleMatches) {
  XmlNode root = XmlParser::ParseString(
    "<root><layer id=\"1\"/><layer id=\"2\"/><other/></root>"
  );
  auto layers = root.findChildren("layer");
  ASSERT_EQ(layers.size(), 2u);
  EXPECT_EQ(layers[0]->getAttribute("id"), "1");
  EXPECT_EQ(layers[1]->getAttribute("id"), "2");
}

TEST(XmlParserTest, FindChildrenNoMatch) {
  XmlNode root = XmlParser::ParseString("<root><a/></root>");
  EXPECT_TRUE(root.findChildren("missing").empty());
}

// ---------------------------------------------------------------------------
// Text content
// ---------------------------------------------------------------------------

TEST(XmlParserTest, TextContent) {
  XmlNode root = XmlParser::ParseString("<data>hello world</data>");
  EXPECT_EQ(root.text, "hello world");
}

TEST(XmlParserTest, TextContentTrimmed) {
  XmlNode root = XmlParser::ParseString("<data>  \n  trimmed  \n  </data>");
  EXPECT_EQ(root.text, "trimmed");
}

// ---------------------------------------------------------------------------
// Processing instruction and comments are skipped
// ---------------------------------------------------------------------------

TEST(XmlParserTest, SkipsProcessingInstruction) {
  XmlNode root = XmlParser::ParseString(
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?><root/>"
  );
  EXPECT_EQ(root.tag, "root");
}

TEST(XmlParserTest, SkipsComment) {
  XmlNode root = XmlParser::ParseString(
    "<!-- this is a comment --><root/>"
  );
  EXPECT_EQ(root.tag, "root");
}

// ---------------------------------------------------------------------------
// ParseFile
// ---------------------------------------------------------------------------

TEST(XmlParserTest, ParseFileThrowsOnMissingFile) {
  EXPECT_THROW(XmlParser::ParseFile("nonexistent_file.xml"), std::runtime_error);
}

TEST(XmlParserTest, ParseFileTmx) {
  XmlNode root = XmlParser::ParseFile("game/assets/map/tilemap.tmx");
  EXPECT_EQ(root.tag, "map");
  EXPECT_EQ(root.getAttribute("width"),  "30");
  EXPECT_EQ(root.getAttribute("height"), "20");

  const XmlNode* layer = root.findChild("layer");
  ASSERT_NE(layer, nullptr);

  const XmlNode* data = layer->findChild("data");
  ASSERT_NE(data, nullptr);
  EXPECT_EQ(data->getAttribute("encoding"), "csv");
  EXPECT_FALSE(data->text.empty());
}
