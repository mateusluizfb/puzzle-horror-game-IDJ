#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>
#include <vector>
#include <map>

struct XmlNode {
  std::string tag;
  std::map<std::string, std::string> attributes;
  std::vector<XmlNode> children;
  std::string text;

  // Returns the attribute value or defaultValue if not found.
  std::string getAttribute(const std::string& name,
                           const std::string& defaultValue = "") const;

  // Returns a pointer to the first child with the given tag, or nullptr.
  const XmlNode* findChild(const std::string& childTag) const;

  // Returns all children with the given tag.
  std::vector<const XmlNode*> findChildren(const std::string& childTag) const;
};

class XmlParser {
public:
  // Parse an XML file and return the root element.
  static XmlNode ParseFile(const std::string& filename);

  // Parse an XML string and return the root element.
  static XmlNode ParseString(const std::string& content);
};

#endif
