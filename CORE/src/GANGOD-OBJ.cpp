#include "../headers/GANGOD-OBJ.h"
using namespace enggan;

std::vector<std::shared_ptr<Object>> ObjMgr::o = {};
float ScrMgr::height;
float ScrMgr::width;

Component::Component(Object* parent){
    p = parent;
}

//Render
Render::Render()
: sprite(Image2D("../sprites/default.png"), Shader("../shaders/mainShader.vert", "../shaders/mainShader.frag"))
{
    classname = "Render";
    REGISTER_FIELD(&sprPath, "sprite path", FieldType::Gstring);
    REGISTER_FIELD(&shdrPath1, "vertex shader path", FieldType::Gstring);
    REGISTER_FIELD(&shdrPath2, "fragment shader path", FieldType::Gstring); 
}

void Render::Update(){
    sprite.image = Image2D(sprPath);
    sprite.shader = Shader(shdrPath1.c_str(), shdrPath2.c_str());
    sprite.Draw([this](unsigned int ID) { this->OnRender(ID); });
}

void Render::OnRender(unsigned int ID){
    int uni_asp = glGetUniformLocation(ID, "model");
    int uni_cam = glGetUniformLocation(ID, "view");
    int uni_pro = glGetUniformLocation(ID, "proj");
    auto cam = ObjMgr::GetOne().GetWithType<Camera>();
    if(cam){
        glUniformMatrix4fv(uni_cam, 1, GL_FALSE, &cam->view[0][0]);
        glUniformMatrix4fv(uni_pro, 1, GL_FALSE, &cam->projection[0][0]);
    }
    glUniformMatrix4fv(uni_asp, 1, GL_FALSE, &p->model[0][0]);
}

void Render::Kill()
{
}

Camera::Camera(){
    classname = "Camera";
    projection = glm::ortho(-4.0f,4.0f, -4.0f, 4.0f, -100.0f, 100.0f);
}

void Camera::Update(){
    view = glm::lookAt(p->pos, p->pos + glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Kill(){}

void Collider::Update(){
    //CheckCollision({});
}
void Collider::Kill(){

}

Collider::Collider(){

}

bool Collider::DoesCollide(std::function<void(glm::vec2 d, Object* objCallb)> callback){
    glm::vec2 axis[] = {
        { 0, 1 },
        { 1, 0 },
        { 0, -1 },
        { -1, 0 }
    };
    // 0 is MIN
    // 1 is MAX

   
    std::vector<Collider*> objs = ObjMgr::GetOne().GetAllWithType<Collider>();
    for (const auto& o : objs){
        if(o == this) { continue; }
        // why tf it don't wanna work?
        // ...
         glm::vec2 minvecToPush = glm::vec2(0);
        float minDiff = __FLT_MAX__;
        float overlap1 = 0;
        float overlap2 = 0;
        bool found = false;
        for (const glm::vec2& vec : axis){
            float diff1 = Project(vec)[1] - o->Project(vec)[0]; // maxA - minB
            float diff2 = o->Project(vec)[1] - Project(vec)[0]; // maxB - minA
            if(diff1 <= 0 || diff2 <= 0) {
                // found one axis that we dont collide
                
                //callback(glm::vec2(0), o->p);
                found = false;
                break;
            }
            found = true;
            float preminDiff = std::min(diff1, diff2);
            if(preminDiff < minDiff){
                overlap1 = diff1;
                overlap2 = diff2;
                minDiff = preminDiff;
                minvecToPush = vec;
            }
        }
        if(found){
            if(overlap1 < overlap2){
                callback(minvecToPush, o->p);
            }
            else{
                callback(-minvecToPush, o->p);
            }
            
            return true;
        }
    }
    // oh jeez
    return false;
}

BoxCollider::BoxCollider() : size(glm::vec2(.5f)){
    classname = "Box Collider";
    REGISTER_FIELD(&offset, "Offset", FieldType::GVec2);
    REGISTER_FIELD(&size, "Collider size", FieldType::GVec2);
}

std::vector<float> BoxCollider::Project(glm::vec2 axis){
    // go around axis
    float min = __FLT_MAX__;
    float max = -__FLT_MAX__;
    std::vector<float> ret;
    glm::vec2 verts[4] = {
        { size.x + p->pos.x + offset.x, size.y + p->pos.y + offset.y },
        { size.x + p->pos.x + offset.x, p->pos.y - size.y + offset.y},
        { p->pos.x - size.x + offset.x, size.y + p->pos.y + offset.y},
        { p->pos.x - size.x+ offset.x, p->pos.y - size.y+ offset.y },
    };

    for (const auto& v : verts){
        float d = glm::dot(v, axis);
        if(d < min){
            min = d;
        }
        if(d > max){
            max = d;
        }
    }

    ret.push_back(min);
    ret.push_back(max);

    return ret;
}

// Phys
Body::Body(){
    classname = "Body";
    velocity = glm::vec2(0);
    kineticEnergy = glm::vec2(mass * g);
}

void Body::CollideCallback(glm::vec2 d, Object* objCallb){
    //velocity += glm::vec3(d.x * g, d.y * g, 0.0f);
    if(d.x != 0 || d.y != 0){
        if(objCallb->GetComponent<enggan::Body>()){
            // law the m1V1 = m2V2 => V1 = m2V2/m1
            Body* refb = objCallb->GetComponent<enggan::Body>();
            velocity.y = ((refb->mass * refb->velocity.y) / mass) * d.y * resist.y;
            velocity.x = ((refb->mass * refb->velocity.x) / mass) * d.x  * resist.x;
        }

        float x = velocity.x * resist.x;
        float y = -d.y * std::sqrt((kineticEnergy.y * 2) / mass) * resist.y;
        velocity.x = x;
        velocity.y = y;
    }
    
}

void Body::FixedUpdate(float dt){
    velocity.y -= g*dt;
    kineticEnergy = { (mass * std::max(velocity.x * velocity.x, velocity.x * -velocity.x)) / 2, 
        (mass * std::max(velocity.y * velocity.y, velocity.y * -velocity.y)) / 2 };

    Collider* c = p->GetComponent<Collider>();
    if(c){
        c->DoesCollide([this](glm::vec2 d, Object* objCallb) { CollideCallback(d, objCallb); });
    }
    if(std::max(velocity.x, -velocity.x) <= g*dt * 0.01f){
        velocity.x = 0;
    }

    if(std::max(velocity.y, -velocity.y) <= g*dt * 0.01f){
        velocity.y = 0;
    }
    p->pos += glm::vec3(velocity * dt, 0);
}

void Body::Update(){

}

void Body::Kill(){

}
// object
Object::Object() : pos(glm::vec3(0)), size(glm::vec3(1)){
    classname = "Object";
    REGISTER_FIELD(&pos, "Position", FieldType::GVec3);
    REGISTER_FIELD(&size, "Size", FieldType::GVec3);
}

void enggan::Object::Update()
{
    model = glm::scale(glm::translate(glm::mat4(1), pos), size);
    for(const auto& c : cmps){
        c->Update();
    }
}
void enggan::Object::Kill()
{
}
template <class T>
T *Object::GetComponent()
{
    for(const auto& c : cmps){
        T* cast = dynamic_cast<T*>(c);

        if(cast){
            return cast;
        }
    }
    return nullptr;
}

template<class T> void Object::AddComponent(){
    T* c = new T();
    c->addParent(this);
    cmps.push_back(c);
}

template<class T> void Object::KillComponent(){
    for(auto it = cmps.begin(); it != cmps.end(); ++it){
        T* cast = dynamic_cast<T*>(*it);
        
        if(cast){
            cast->Kill();
            cmps.erase(it);
            return;
        }
    }
}

template<class T> bool Object::TryComponent(T* result){
    for(const auto& c : cmps){
        T* cast = dynamic_cast<T*>(c);

        if(cast){
            result = cast;
            return false;
        }
    }
    result = nullptr;
    return false;
}

void Object::FixedUpdate(float dt){
    for(const auto& c : cmps){
        c->FixedUpdate(dt);
    }
}

std::vector<Component*> Object::GetAllComponents(){
    return cmps;
}

template Component* Object::GetComponent<Component>();

//render
template Render* Object::GetComponent<Render>();

template void Object::AddComponent<Render>();

template void Object::KillComponent<Render>();

template bool Object::TryComponent<Render>(Render* result);
//camera
template Camera* Object::GetComponent<Camera>();

template void Object::AddComponent<Camera>();

template void Object::KillComponent<Camera>();

template bool Object::TryComponent<Camera>(Camera* result);
//colliders
//collider
template Collider* Object::GetComponent<Collider>();
template void Object::KillComponent<Collider>();
template bool Object::TryComponent<Collider>(Collider* result);
//box
template BoxCollider* Object::GetComponent<BoxCollider>();
template void Object::AddComponent<BoxCollider>();
template void Object::KillComponent<BoxCollider>();
template bool Object::TryComponent<BoxCollider>(BoxCollider* result);
//body
template Body* Object::GetComponent<Body>();
template void Object::AddComponent<Body>();
template void Object::KillComponent<Body>();
template bool Object::TryComponent<Body>(Body* result);
Object::~Object(){
}
enggan::Component::Component()
{
}

ObjMgr& ObjMgr::GetOne(){
    static ObjMgr ins;
    return ins;
}

std::vector<std::shared_ptr<Object>> ObjMgr::GetAllObjects(){
    return o;
}

void ObjMgr::Update(){
    for(const auto& e : o){
        e->Update();
    }
}
void ObjMgr::FixUpd(float dt){
    for(const auto& e : o){
        e->FixedUpdate(dt);
    }
}

std::shared_ptr<Object> ObjMgr::Create(){
    std::shared_ptr<Object> p = std::make_shared<Object>();
    o.push_back(p);
    return p;
}

template <class T> T* ObjMgr::GetWithType()
{
    for(std::shared_ptr<Object> e : GetOne().GetAllObjects()){
        T* res = e->GetComponent<T>();
        if(res){
            return res;
        }
    }
    return nullptr;
}

template <class T> std::vector<T*> ObjMgr::GetAllWithType()
{
    std::vector<T*> result;
    for(std::shared_ptr<Object> e : GetOne().GetAllObjects()){
        T* res = e->GetComponent<T>();
        if(res){
            result.push_back(res);
        }
    }
    return result;
}

template Render* ObjMgr::GetWithType<Render>();
template Camera* ObjMgr::GetWithType<Camera>();

void ScrMgr::UpdateWindowCallback(GLFWwindow* wnd, int h, int w){
    width = (float)w;
    height = (float)h;
    glViewport(0, 0, width, height);
    AspectRatio = width / height;
    Camera* c = ObjMgr::GetOne().GetWithType<Camera>();
    if(c){
        c->projection = glm::ortho(width / 200, -width / 200, -height / 200, height / 200, -100.0f, 100.0f);
    }
}