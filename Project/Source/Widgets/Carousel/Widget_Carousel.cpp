#include <Include/Widgets/Carousel/Widget_Carousel.h>

#include <Include/Assets/AssetManager.h>
#include <Include/Assets/Config.h>
#include <Include/Entity.h>
#include <Include/Component_Model.h>
#include <Include/Log.h>

#include <ThirdParty/xo-math/xo-math.h>

namespace XO {
namespace UI {

class Widget_CarouselImpl {
public:

    Widget_CarouselImpl(Widget_Carousel* carousel)
        : Carousel(carousel)
        , CubeComponent("Models/Cube/cube.ini") {
    }

    void Init(XO::Entity* owner) {
        Carousel->GetString("ContentConfig", ContentConfig, Required);
        Carousel->GetFloat("CameraOffset", CameraOffset, Required, -3.0f);
        Carousel->GetInt("NumEntries", NumEntries, Required);
        
        ContentConfig = AssetManager::AssetsRoot() + String("/") + ContentConfig;
        Config InnerContentConfig(ContentConfig.c_str());
        xoFatalIf(!InnerContentConfig.IsValid(), "Couldn't load the inner content config for carousel widget: " << ContentConfig);;

        char buffer[32];
        for (int32 i = 0; i < NumEntries; ++i) {
            memset(buffer, 0, sizeof(buffer));
            itoa(i, buffer, 10);
            String entryName = "Entry";
            entryName += buffer;

            const char* modelName = InnerContentConfig.Get(entryName.c_str(), "Model", "UNKNOWN");
            xoLog("Loaded model: " << entryName << " name :" << modelName);
        }

        CubeEntity.AddComponent(&CubeComponent);
        CubeEntity.SetPosition(Vector3(0.0f, 0.0f, CameraOffset));
        owner->AddChild(&CubeEntity);
    }

    void Update(float dt) {

    }

private:
    XO::Entity CubeEntity;
    XO::Component_Model CubeComponent;

    Widget_Carousel* Carousel;

    // Data Driven
    String ContentConfig;
    float32 CameraOffset;
    int32 NumEntries;


};

void Widget_Carousel::Init(XO::Entity* owner) {
    Impl->Init(owner);
}

void Widget_Carousel::Update(float dt) {
    Impl->Update(dt);
}


Widget_Carousel::Widget_Carousel(class IDataProvider* rootDataProvider) 
: Super(rootDataProvider) {
    Impl = new Widget_CarouselImpl(this);
}

Widget_Carousel::~Widget_Carousel() {
    delete Impl;
}


}
}