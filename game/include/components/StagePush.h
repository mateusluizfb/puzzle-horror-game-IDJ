#ifndef STAGEPUSH_H
#define STAGEPUSH_H

#include "Component.h"
#include <string>

class StagePush : public Component {
private:
  std::string targetStage;
  bool triggered;

public:
  StagePush(GameObject& associated, const std::string& stageName);
  
  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject& other) override;
};

#endif
