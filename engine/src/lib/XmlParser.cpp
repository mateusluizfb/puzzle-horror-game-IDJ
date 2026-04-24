#include "XmlParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

static std::string trim(const std::string& s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) return "";
  size_t end = s.find_last_not_of(" \t\r\n");
  return s.substr(start, end - start + 1);
}

static void skipWhitespace(const std::string& src, size_t& pos) {
  while (pos < src.size() && std::isspace((unsigned char)src[pos])) ++pos;
}

// Parse key="value" pairs from an attribute string.
static std::map<std::string, std::string> parseAttributes(const std::string& s) {
  std::map<std::string, std::string> attrs;
  size_t i = 0;

  while (i < s.size()) {
    while (i < s.size() && std::isspace((unsigned char)s[i])) ++i;
    if (i >= s.size()) break;

    // Read attribute name
    size_t keyStart = i;
    while (i < s.size() && s[i] != '=' && !std::isspace((unsigned char)s[i])) ++i;
    std::string key = s.substr(keyStart, i - keyStart);
    if (key.empty()) break;

    // Skip whitespace and '='
    while (i < s.size() && (std::isspace((unsigned char)s[i]) || s[i] == '=')) ++i;

    // Read quoted value
    if (i >= s.size()) break;
    char quote = s[i];
    if (quote != '"' && quote != '\'') break;
    ++i;
    size_t valStart = i;
    while (i < s.size() && s[i] != quote) ++i;
    std::string val = s.substr(valStart, i - valStart);
    if (i < s.size()) ++i; // skip closing quote

    attrs[key] = val;
  }

  return attrs;
}

// Parse a single element starting at src[pos] == '<'.
// Returns the populated XmlNode and advances pos past the closing '>'.
static XmlNode parseElement(const std::string& src, size_t& pos);

static XmlNode parseElement(const std::string& src, size_t& pos) {
  XmlNode node;

  // Skip '<'
  ++pos;

  // Processing instruction: <?...?>
  if (pos < src.size() && src[pos] == '?') {
    while (pos + 1 < src.size() && !(src[pos] == '?' && src[pos + 1] == '>')) ++pos;
    pos += 2;
    return node; // empty tag → caller skips it
  }

  // Comment: <!--...-->
  if (pos + 2 < src.size() && src[pos] == '!' && src[pos + 1] == '-' && src[pos + 2] == '-') {
    pos += 3;
    while (pos + 2 < src.size() &&
           !(src[pos] == '-' && src[pos + 1] == '-' && src[pos + 2] == '>')) ++pos;
    pos += 3;
    return node;
  }

  // DOCTYPE or CDATA declarations
  if (pos < src.size() && src[pos] == '!') {
    while (pos < src.size() && src[pos] != '>') ++pos;
    ++pos;
    return node;
  }

  // Read tag name
  size_t tagStart = pos;
  while (pos < src.size() && src[pos] != '>' && src[pos] != '/' &&
         !std::isspace((unsigned char)src[pos])) {
    ++pos;
  }
  node.tag = src.substr(tagStart, pos - tagStart);

  // Read attribute string up to '>' (may end with '/' for self-closing)
  size_t attrStart = pos;
  while (pos < src.size() && src[pos] != '>') ++pos;

  bool selfClosing = (pos > 0 && src[pos - 1] == '/');
  std::string attrStr = src.substr(attrStart, pos - attrStart - (selfClosing ? 1 : 0));
  node.attributes = parseAttributes(attrStr);

  if (pos < src.size()) ++pos; // skip '>'

  if (selfClosing || node.tag.empty()) {
    return node;
  }

  // Parse children and text content
  while (pos < src.size()) {
    skipWhitespace(src, pos);
    if (pos >= src.size()) break;

    if (src[pos] == '<') {
      // Closing tag?
      if (pos + 1 < src.size() && src[pos + 1] == '/') {
        while (pos < src.size() && src[pos] != '>') ++pos;
        ++pos;
        break;
      }
      // Child element
      XmlNode child = parseElement(src, pos);
      if (!child.tag.empty()) {
        node.children.push_back(child);
      }
    } else {
      // Text content
      size_t textStart = pos;
      while (pos < src.size() && src[pos] != '<') ++pos;
      node.text += src.substr(textStart, pos - textStart);
    }
  }

  node.text = trim(node.text);
  return node;
}

// ---------------------------------------------------------------------------
// XmlNode methods
// ---------------------------------------------------------------------------

std::string XmlNode::getAttribute(const std::string& name,
                                  const std::string& defaultValue) const {
  auto it = attributes.find(name);
  return (it != attributes.end()) ? it->second : defaultValue;
}

const XmlNode* XmlNode::findChild(const std::string& childTag) const {
  for (const auto& child : children) {
    if (child.tag == childTag) return &child;
  }
  return nullptr;
}

std::vector<const XmlNode*> XmlNode::findChildren(const std::string& childTag) const {
  std::vector<const XmlNode*> result;
  for (const auto& child : children) {
    if (child.tag == childTag) result.push_back(&child);
  }
  return result;
}

// ---------------------------------------------------------------------------
// XmlParser methods
// ---------------------------------------------------------------------------

XmlNode XmlParser::ParseFile(const std::string& filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    throw std::runtime_error("XmlParser::ParseFile: Cannot open file: " + filename);
  }
  std::stringstream ss;
  ss << f.rdbuf();
  return ParseString(ss.str());
}

XmlNode XmlParser::ParseString(const std::string& content) {
  size_t pos = 0;

  while (pos < content.size()) {
    skipWhitespace(content, pos);
    if (pos >= content.size()) break;

    if (content[pos] == '<') {
      XmlNode node = parseElement(content, pos);
      if (!node.tag.empty()) {
        return node;
      }
    } else {
      ++pos;
    }
  }

  return XmlNode{};
}
