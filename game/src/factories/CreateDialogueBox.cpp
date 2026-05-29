#include "CreateDialogueBox.h"
#include "DialogueBox.h"
#include "GameObject.h"
#include "Log.h"

GameObject* CreateDialogueBox(Rect box,
                              std::string fontFile,
                              int fontSize,
                              SDL_Color textColor,
                              std::string text,
                              std::vector<std::string> options)
{
    Log::info("CREATE_DIALOGUE_BOX - Creating dialogue game object");

    auto go = new GameObject();
    go->box = box;
    go->tag = "dialogue";

    DialogueBox* db = new DialogueBox(*go, go->box, fontFile, fontSize, textColor);
    db->SetText(text);
    if (!options.empty()) {
        db->SetOptions(options);
    }
    go->AddComponent(db);

    return go;
}
