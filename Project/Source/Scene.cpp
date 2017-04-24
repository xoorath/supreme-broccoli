#include <Include/Scene.h>

#include <Include/Component.h>
#include <Include/Entity.h>
#include <Include/Log.h>
#include <Include/Macros.h>
#include <Include/Renderer.h>
#include <Include/Types.h>


#ifdef GAME_WINDOWS
#include <windows.h>
namespace XO {
struct FrameTimer {
    double Frequency = 0.0;
    int64 LastFrameTime = 0;

    FrameTimer() {
        LARGE_INTEGER li;
        xoFatalIf(!QueryPerformanceFrequency(&li), "Could not setup the frame timer!");

        Frequency = double(li.QuadPart);

        QueryPerformanceCounter(&li);
        LastFrameTime = li.QuadPart;
    }

    float operator () () {
        LARGE_INTEGER li;
        int64 frameTime;
        QueryPerformanceCounter(&li);
        frameTime = li.QuadPart;
        double delta = (double)(frameTime - LastFrameTime) / Frequency;
        LastFrameTime = frameTime;
        return (float)delta;
    }

};
}
#endif

namespace XO {

class SceneImpl {
public:
    Array<Entity*> EntitiesAtRoot;
    Array<Component*> UpdateList;
    Array<Component*> RenderList;
    Renderer* ActiveRenderer;
    FrameTimer Timer;


    void AddEntity(class Entity* entity) {
        EntitiesAtRoot.push_back(entity);
        entity->Initialize();
    }

    void RemoveEntity(class Entity* entity) {
        auto found = std::find(EntitiesAtRoot.begin(), EntitiesAtRoot.end(), entity);
        xoErrIf(found == EntitiesAtRoot.end(), "Entity was not found.");
        EntitiesAtRoot.erase(found);
    }

    void OnComponentAdded(class Entity* owner, class Component* component) {
        xoUnused(owner);
        auto flags = component->GetComponentFlags();
        if (flags & ComponentFlags::Init) {
            component->Init();
        }
        if (flags & ComponentFlags::InitRender) {
            component->InitRender(*ActiveRenderer);
        }
        if (flags & ComponentFlags::Update) {
            UpdateList.push_back(component);
        }
        if (flags & ComponentFlags::Render) {
            RenderList.push_back(component);
        }
    }

    void OnComponentRemoved(class Entity* owner, class Component* component) {
        xoUnused(owner);
    }

    void OnChildAdded(class Entity* parent, class Entity* child) {

    }

    void OnChildRemoved(class Entity* parent, class Entity* child) {

    }

    void Init(Renderer* renderer) {
        ActiveRenderer = renderer;

        LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
        LARGE_INTEGER Frequency;

        QueryPerformanceFrequency(&Frequency);
        QueryPerformanceCounter(&StartingTime);

        // Activity to be timed

        QueryPerformanceCounter(&EndingTime);
        ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;


        //
        // We now have the elapsed number of ticks, along with the
        // number of ticks-per-second. We use these values
        // to convert to the number of elapsed microseconds.
        // To guard against loss-of-precision, we convert
        // to microseconds *before* dividing by ticks-per-second.
        //

        ElapsedMicroseconds.QuadPart *= 1000000;
        ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    }

    float Tick() {
        float dt = Timer();
        Update(dt);
        Render(dt);
        return dt;
    }

private:
    void Update(float dt) {
        for (auto* comp : UpdateList) {
            comp->Update(dt);
        }
    }

    void Render(float dt) {
        for (auto* comp : RenderList) {
            comp->Render(*ActiveRenderer, dt);
        }
    }

};


Scene* Scene::CurrentScene = nullptr;

Scene* Scene::GetCurrentScene() {
    return CurrentScene;

}

void Scene::AddEntity(class Entity* entity) {
    Impl->AddEntity(entity);
}

void Scene::RemoveEntity(class Entity* entity) {
    Impl->RemoveEntity(entity);
}

void Scene::Init(Renderer* renderer) {
    Impl->Init(renderer);
}

float Scene::Tick() {
    return Impl->Tick();
}

Scene::Scene() {
    Impl = new SceneImpl();
    xoErrIf(CurrentScene != nullptr, "The current scene should be unassigned before creating a new one.");
    CurrentScene = this;
}

Scene::~Scene() {
    delete Impl;
    if (CurrentScene == this) {
        CurrentScene = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////// EntityImpl API
void Scene::OnComponentAdded(class Entity* owner, class Component* component) {
    Impl->OnComponentAdded(owner, component);
}

void Scene::OnComponentRemoved(class Entity* owner, class Component* component) {
    Impl->OnComponentRemoved(owner, component);
}

void Scene::OnChildAdded(class Entity* parent, class Entity* child) {
    Impl->OnChildAdded(parent, child);
}

void Scene::OnChildRemoved(class Entity* parent, class Entity* child) {
    Impl->OnChildRemoved(parent, child);
}
////////////////////////////////////////////////////////////////////////// End EntityImpl API

}