#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <memory>
#include <string>
#include "Component.h"

class GameObject;
class Text;

class Document : public Component {
private:
  std::string documentText;
  Text* promptText = nullptr;
  bool isTouching = false;
  bool dialogueOpen = false;
  std::weak_ptr<GameObject> dialogueObject;

public:
  Document(GameObject& associated, const std::string& documentText);

  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject& other) override;
};

#endif