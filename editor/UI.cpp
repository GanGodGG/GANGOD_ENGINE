#include "UI.h"
using namespace enggan;

std::shared_ptr<enggan::Object> Editor::current = nullptr;
std::vector<std::shared_ptr<enggan::Object>> Editor::All = 
 enggan::ObjMgr::GetOne().GetAllObjects();

void Editor::DrawHierarchy(){
    // -------- Creating Hierarchy --------
    ImGui::Begin("Hierarchy");
    // on click, create new gameobject
    if(ImGui::Button("Create new obj...")){
        std::shared_ptr<Object> newobj = ObjMgr::Create();
        newobj->AddComponent<Render>();
        newobj->AddComponent<BoxCollider>();
        newobj->name = "Ingameobj -> " + std::to_string(All.size());
        All = enggan::ObjMgr::GetOne().GetAllObjects();
    }
    ImGui::Separator();
    for(size_t i = 0; i < All.size(); ++i){
        std::shared_ptr<Object> o = All[i];
        bool isSelect = (current == o);
        if(o->name.size() > 0){
            if(ImGui::Selectable(o->name.c_str(), isSelect)){
                current = o;
                std::cout << current->pos.x << std::endl;
            }
        }
    }
    ImGui::End();
}

void DrawField(FieldInfo field){
    switch(field.field){
        case FieldType::GVec3:{
            float vec[3] = { static_cast<float*>(field.point)[0], 
                static_cast<float*>(field.point)[1],
                static_cast<float*>(field.point)[2] };
            if(ImGui::InputFloat3(field.NAME.c_str(), vec)){
                static_cast<float*>(field.point)[0] = vec[0];
                static_cast<float*>(field.point)[1] = vec[1];
                static_cast<float*>(field.point)[2] = vec[2];
            }
        }
        break;
        case FieldType::GVec2:{
            float vec[2] = { static_cast<float*>(field.point)[0], static_cast<float*>(field.point)[1] };
            if(ImGui::InputFloat2(field.NAME.c_str(), vec)){
                static_cast<float*>(field.point)[0] = vec[0];
                static_cast<float*>(field.point)[1] = vec[1];
            }
        }
        break;

        case FieldType::Gfloat:
            ImGui::InputFloat(field.NAME.c_str(), static_cast<float*>(field.point));
        break;

        case FieldType::Gstring:
            char buf[128];
            std::string* name = static_cast<std::string*>(field.point);
            strncpy(buf, name->c_str(), sizeof(buf));
            if(ImGui::InputText(field.NAME.c_str(), buf, sizeof(buf))){
                field.point = buf;
            }
        break;
    }
}

void Editor::DrawInspector(){
    ImGui::Begin("Inspector");

    if(current == nullptr){
        ImGui::Text("Select object!");
        ImGui::End();
        return;
    }

    char NameBuffer[128];

    strncpy(NameBuffer,current->name.c_str(),sizeof(NameBuffer));
    if(ImGui::InputText("NAME IT: ", NameBuffer, sizeof(NameBuffer))){
        current->name = NameBuffer;
    }

    ImGui::Separator();
    if(ImGui::CollapsingHeader("Object basic properties", ImGuiTreeNodeFlags_DefaultOpen)){
            ImGui::Indent();
            for(auto& f : current->fields){
                DrawField(f);
            }
            ImGui::Unindent();
            ImGui::Spacing();
    }
    for(int i = 0; i < current->GetAllComponents().size(); ++i){
        Component* comp = current->GetAllComponents()[i];
        std::string head = comp->classname;
        //head += std::to_string(i);
        //if(head.find("class ") == 0) head.erase(0, 6);
        ImGui::PushID(i);
        if(ImGui::CollapsingHeader(head.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){
            ImGui::Indent();
            for(auto& f : comp->fields){
                DrawField(f);
            }
            ImGui::Unindent();
            ImGui::Spacing();
        }
        ImGui::PopID();
    }
    
    ImGui::End();
}

void Editor::DrawScene(){
    ImGui::Begin("Scene");
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 min = ImGui::GetItemRectMin();
    for(auto& obj : All){
        ImVec2 pos = ImVec2(min.x + obj->pos.x, min.y + obj->pos.y);
        ImTextureRef texture_ref = ImTextureRef((ImTextureID)(intptr_t)obj->GetComponent<Render>()->sprite.image.texid);
        dl->AddImage
        (
            texture_ref,
            pos,
            ImVec2((pos.x + obj->size.x * obj->GetComponent<Render>()->sprite.image.x / 10), 
            (pos.y + obj->size.y * obj->GetComponent<Render>()->sprite.image.y / 10))
        );
        if(current == obj){
            dl->AddRect
            (
                pos,
                ImVec2((pos.x + obj->size.x * obj->GetComponent<Render>()->sprite.image.x / 10), 
            (pos.y + obj->size.y * obj->GetComponent<Render>()->sprite.image.y / 10)),
                IM_COL32(255, 0, 0, 255)
            );
            if(current->GetComponent<BoxCollider>()){
                BoxCollider* bc = current->GetComponent<BoxCollider>();
                //std::cout<<"Have collider!" <<std::endl;
                dl->AddRect
                (
                    pos,
                    ImVec2((pos.x + bc->offset.x + obj->size.x * (bc->size.x / 2) * obj->GetComponent<Render>()->sprite.image.x / 5), 
                    (pos.y + bc->offset.y + obj->size.y * (bc->size.y / 2)  * obj->GetComponent<Render>()->sprite.image.y / 5)),
                    IM_COL32(0, 255, 0, 255)
                );  
            }
        }
        
    }
    ImGui::End();
}

void Editor::BUILD(){
    for(int p = 0; p < All.size(); ++p){
        nlohmann::json SceneInfo;
        auto o = All[p];
        SceneInfo[o->name]["name"] = o->name;
        // position
        SceneInfo[o->name]["position"]["x"] = o->pos.x;
        SceneInfo[o->name]["position"]["y"] = o->pos.y;
        SceneInfo[o->name]["position"]["z"] = o->pos.z;
        // position

        for(int i = 0; i < o->GetAllComponents().size(); ++i){
            auto j = o->GetAllComponents()[i];
            for(const auto& field : j->fields){
                auto inf = SceneInfo[o->name]["Components"][j->classname][field.NAME];
                switch(field.field){
                    case FieldType::GVec3:{
                    float vec[3] = { static_cast<float*>(field.point)[0], 
                    static_cast<float*>(field.point)[1],
                    static_cast<float*>(field.point)[2] };

                    inf["x"] = vec[0];
                    inf["y"] = vec[1];
                    inf["z"] = vec[2];
                }
                break;
            case FieldType::GVec2:{
                glm::vec2 v = *static_cast<glm::vec2*>(field.point);
                inf["x"] = v.x;
                inf["y"] = v.y;                
            }
            break;

                case FieldType::Gfloat:
                    //ImGui::InputFloat(field.NAME.c_str(), static_cast<float*>(field.point));
                    inf = *(static_cast<float*>(field.point));
                break;

                case FieldType::Gstring:
                    std::string* name = static_cast<std::string*>(field.point);
                    inf = name->c_str();
                    break;
                }
            }
        }

        std::ofstream file;
        file.open("scene");
        file << SceneInfo.dump(4);
        file.close();
    }
}