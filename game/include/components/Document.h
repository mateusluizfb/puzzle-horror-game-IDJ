#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <memory>
#include <string>
#include "Component.h"
#include "DialogueBox.h"

class GameObject;
class Text;

class Document : public Component {
private:
  std::string documentText;
  DialogueBox::PortraitMode portraitMode = DialogueBox::PortraitMode::STILL;
  Text* promptText = nullptr;
  bool isTouching = false;
  bool dialogueOpen = false;
  std::weak_ptr<GameObject> dialogueObject;

public:
  Document(GameObject& associated,
           const std::string& documentText,
           DialogueBox::PortraitMode portraitMode = DialogueBox::PortraitMode::STILL);

  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject& other) override;
};

#endif