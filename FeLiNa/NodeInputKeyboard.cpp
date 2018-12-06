#include "NodeInputKeyboard.h"
#include "Application.h"
#include "ModuleInput.h"


NodeInputKeyboard::NodeInputKeyboard(int id, char key) : Node( id, "Input Event", {100,100}, 0, 1, NodeType::EventType)
{
	this->key = key;
	this->key_code = SDL_GetScancodeFromName(&key);
	subtype = NodeSubType::InputKeyboard;

}

bool NodeInputKeyboard::Update()
{
	returned_result = false;


	if (App->input->GetKey(key_code) == KEY_REPEAT)
	{
		returned_result = true;
	}


	return returned_result;
}

void NodeInputKeyboard::DrawNode()
{

	if (ImGui::InputText("Key react:", &key, 2))
	{
		key_code = SDL_GetScancodeFromName(&key);
	}

}