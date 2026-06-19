#pragma once

#include "GANGOD-GEOMETRY.h"
#include "GANGOD-RENDER.h"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <string>
#include <array>

#define REGISTER_FIELD(Class, FieldName, Type) \
    fields.push_back({#FieldName, Type, Class});
namespace enggan{

    class Object;
    class Collider;
    static int AspectRatio = 800 / 600;

    enum FieldType{
        Gfloat,
        Gint,
        Gstring,
        GVec2,
        GVec3
    };

    struct FieldInfo{
        std::string NAME;
        FieldType field;
        void* point;
    };

    class Basic {
        private:
        std::string uniqueId;
        const std::string Gstr = "1ta23G4N1C0D31dG3NNG0du3R4t_iOn_P4eDI9t";

        std::string GenerateID(){
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<> dist(0, Gstr.size() - 1);

            for(size_t i = 0; i < 35; ++i){
                uniqueId += Gstr[dist(generator)];
            }
            return uniqueId;
        }
        public:
        std::vector<FieldInfo> fields;
        std::string classname;
        virtual void FixedUpdate(float dt) {};
        virtual void Update() = 0;
        virtual void EditorUpdate() {};
        virtual void Kill() = 0;

        bool operator ==(Basic& operand){
            GenerateID();
            operand.GenerateID();
            if(uniqueId == operand.uniqueId){
                return true;
            }
            return false;
        }

        bool operator !=(Basic& operand){
            std::cout << GenerateID() << std::endl;
            operand.GenerateID();
            if(uniqueId != operand.uniqueId){
                return true;
            }
            return false;
        }
    };

    class Component : public Basic{
    protected:
        Object* p;

    public:
        using Basic::operator!=;
        using Basic::operator==;
        virtual ~Component() {};
        Component(Object* parent);
        Component();
        void addParent(Object* parent){
            p = parent;
        }
        void Update() override = 0;
        void Kill() override = 0;
    };

    
    class Render : public Component{
        private:
        std::string sprPath;
        std::string shdrPath1;
        std::string shdrPath2;

    public:
        Sprite sprite;
        
        Render();
        void Update() override;
        void OnRender(unsigned int ID);
        void Kill() override;
    };

    class Collider : public Component{
        protected:
        virtual std::vector<float> Project(glm::vec2 axis) = 0;
        public:
        using Basic::operator!=;
        using Basic::operator==;
        Collider();
        void Update();
        void Kill();
        bool DoesCollide(std::function<void(glm::vec2 d, Object* objCallb)> callback);
    };

    class BoxCollider : public Collider{
        public:
        glm::vec2 size;
        glm::vec2 offset;
        BoxCollider();
        std::vector<float> Project(glm::vec2 axis) override;
    };

    class Body : public Component{
        private:
        glm::vec2 kineticEnergy;
        const float g = 9.81f;
        void CollideCallback(glm::vec2 d, Object* objCallb);
        public:
        glm::vec2 velocity;
        glm::vec2 resist = { 0.8f, 0.05f };
        float mass = 2;
        Body();
        void FixedUpdate(float dt) override;
        void Update();
        void Kill();
    };

    class Camera : public Component{
        public:
        glm::mat4 view;
        glm::mat4 projection;
        Camera();
        void Update() override;
        void Kill() override;
    };

    class Object : public Basic{
        
    private:
        Object* parent = nullptr;
    //vecs
    //arrays
        std::vector<Object*> children = {};
        std::vector<Component*> cmps = {};
    //parent
        //Object* parent;
    public:
        std::string name;

        glm::mat4 model;

        glm::vec3 pos = {0, 0, 0};
        glm::vec3 size;
//construct
        ~Object();
        Object();

        //basic
        void Update() override;
        void FixedUpdate(float dt) override;
        void Kill() override;
        //vectors
        //components
        template<class T> T* GetComponent();
        template<class T> void AddComponent();
        template<class T> void KillComponent();
        template<class T> bool TryComponent(T* result);
        std::vector<Component*> GetAllComponents();
    };

    class ObjMgr{
        private:
        ObjMgr() {}
        ObjMgr(ObjMgr& o) = delete;
        ObjMgr& operator = (const ObjMgr&) = delete;
        static std::vector<std::shared_ptr<Object>> o;
        public: 
        static std::shared_ptr<Object> Create();
        static ObjMgr& GetOne();
        static void Update();
        static void FixUpd(float dt);
        std::vector<std::shared_ptr<Object>> GetAllObjects(); 
        template <class T> T* GetWithType();
        template <class T> std::vector<T*> GetAllWithType();
    };

    class ScrMgr{
        private:
        ScrMgr() {}
        ScrMgr(ScrMgr& o) = delete;
        ScrMgr& operator =(ScrMgr& o) = delete;

        public:
        static float height;
        static float width;
        static void UpdateWindowCallback(GLFWwindow* wnd, int w, int h);
    };
}