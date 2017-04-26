#include <Include/Widgets/Carousel/Widget_Carousel.h>

#include <Include/Assets/AssetManager.h>
#include <Include/Assets/Config.h>
#include <Include/Entity.h>
#include <Include/Component_Model.h>
#include <Include/Log.h>
#include <Include/TextHelpers.h>
#include <Include/Widgets/Carousel/CarouselNodeProperties.h>
#include <Include/Widgets/Carousel/SubWidget_CarouselNode.h>

#include <ThirdParty/xo-math/xo-math.h>

#include <sstream>

namespace XO {
namespace UI {

struct CarouselDisplayProperties {
    float32 CameraYOffset;
    float32 CameraZOffset;

    int32 CarouselNodesOnScreen;
    float32 CarouselNode_Scale;
    float32 CarouselNode_ScaleFalloff;
    float32 CarouselNode_Alpha;
    float32 CarouselNode_AlphaFalloff;
    float32 CarouselNode_XOffset;
    float32 CarouselNode_XOffsetFalloff;
    float32 CarouselNode_ZOffset;
    float32 CarouselNode_ZOffsetFalloff;

    int32 GridNxN;
    float32 GridNode_Color[4];
    float32 GridNode_SelectedColor[4];
    float32 GridNode_Scale;
    float32 GridNode_Gap;
};

struct CarouselDataProperties {
    String ContentConfig;
    int32 NumEntries;
};

class Widget_CarouselImpl {
public:

    Widget_CarouselImpl(Widget_Carousel* carousel)
        : Owner(carousel)
        , CubeComponent("Models/Cube/cube.ini") {
    }

    ~Widget_CarouselImpl() {
        for (auto node : Nodes) {
            delete node;
        }
    }

    void Init(XO::Entity* owner) {

        float32 White[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float32 Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

        Owner->GetFloat("CameraYOffset", DisplayProperties.CameraYOffset, Optional, -1.0f);
        Owner->GetFloat("CameraZOffset", DisplayProperties.CameraZOffset, Optional, -3.0f);

        Owner->GetInt("CarouselNodesOnScreen", DisplayProperties.CarouselNodesOnScreen, Optional, 5);
        Owner->GetFloat("CarouselNode_Scale", DisplayProperties.CarouselNode_Scale, Optional, 1.0f);
        Owner->GetFloat("CarouselNode_ScaleFalloff", DisplayProperties.CarouselNode_ScaleFalloff, Optional, 0.9f);
        Owner->GetFloat("CarouselNode_Alpha", DisplayProperties.CarouselNode_Alpha, Optional, 1.0f);
        Owner->GetFloat("CarouselNode_AlphaFalloff", DisplayProperties.CarouselNode_AlphaFalloff, Optional, 0.8f);
        Owner->GetFloat("CarouselNode_XOffset", DisplayProperties.CarouselNode_XOffset, Optional, 1.0f);
        Owner->GetFloat("CarouselNode_XOffsetFalloff", DisplayProperties.CarouselNode_XOffsetFalloff, Optional, 0.5f);
        Owner->GetFloat("CarouselNode_ZOffset", DisplayProperties.CarouselNode_ZOffset, Optional, 1.0f);
        Owner->GetFloat("CarouselNode_ZOffsetFalloff", DisplayProperties.CarouselNode_ZOffsetFalloff, Optional, 0.5f);

        Owner->GetInt("GridNxN", DisplayProperties.GridNxN, Optional, 6);
        Owner->GetColor("GridNode_Color", DisplayProperties.GridNode_Color, Optional, White);
        Owner->GetColor("GridNode_SelectedColor", DisplayProperties.GridNode_SelectedColor, Optional, Red);
        Owner->GetFloat("GridNode_Scale", DisplayProperties.GridNode_Scale, Optional, 0.1666f);
        Owner->GetFloat("GridNode_Gap", DisplayProperties.GridNode_Gap, Optional, 0.1f);

        Owner->GetString("ContentConfig", DataProperties.ContentConfig, Required);
        Owner->GetInt("NumEntries", DataProperties.NumEntries, Required);

        
        Nodes.resize(DataProperties.NumEntries, nullptr);

        Config ContentConfig(AssetManager::RelativetoAssetsRoot(DataProperties.ContentConfig));
        for (int32 i = 0; i < DataProperties.NumEntries; ++i) {
            std::stringstream ss;
            ss << "Entry" << i;
            Nodes[i] = new UI::SubWidget_CarouselNode();
            CreateSubWidget(Nodes[i], Owner->GetOwner(), ContentConfig, ss.str());
        }

        owner->SetRotation(Vector3(-30.0f * Deg2Rad, 0.0f, 0.0f));
        owner->SetPosition(Vector3(0, DisplayProperties.CameraYOffset, DisplayProperties.CameraZOffset));

    }

    void UpdateNodeTransform(SubWidget_CarouselNode* node, float32 scrollPosition) {
        Entity& entity = node->GetWrapperEntity();

        float32 xstep = DisplayProperties.CarouselNode_XOffset;
        float32 x = 0.0f;
        for (float32 ix = 0.0f; ix < scrollPosition; ix += 1.0f) {
            x += xstep;
            xstep *= DisplayProperties.CarouselNode_XOffsetFalloff;
        }

        float32 zstep = DisplayProperties.CarouselNode_ZOffset;
        float32 z = 0.0f;
        for (float32 iz = 0.0f; iz < scrollPosition; iz += 1.0f) {
            z += zstep;
            zstep *= DisplayProperties.CarouselNode_ZOffsetFalloff;
        }

        float32 sstep = DisplayProperties.CarouselNode_Scale;
        float32 s = sstep;
        for (float32 is = 0.0f; is < scrollPosition; is += 1.0f) {
            s = sstep;
            sstep *= DisplayProperties.CarouselNode_ScaleFalloff;
        }

        float32 sign = scrollPosition >= 0.0f ? 1.0f : -1.0f;

        x *= sign;

        entity.SetPosition(Vector3(x, 0.0f, z));
        entity.SetScale(s);
        float32 rgba[4];
        node->GetRGBA(rgba);
        rgba[3] = s*s*s;
        node->SetRGBA(rgba);

    }

    void Update(float32 dt) {
        uint32 sideNodes = (DisplayProperties.CarouselNodesOnScreen - 1) / 2;
        for (uint32 i = 0; i < sideNodes+1; ++i) {
            UpdateNodeTransform(Nodes[i], (float)i);
        }

        
        for (uint32 i = Nodes.size()-1, j = 0; i > Nodes.size()- sideNodes; --i, ++j) {
            UpdateNodeTransform(Nodes[i], -(float)j);
        }
    }

private:
    void CreateSubWidget(SubWidget_CarouselNode* node, XO::Entity* owner, Config& config, String configGroup) {
        Owner->AddSubwidget(node);
        CarouselNodeProperties properties;
        const char* modelPath = config.Get(configGroup.c_str(), "Model", nullptr);
        xoFatalIf(modelPath == nullptr, "Carousel model data is invalid for entry: " << configGroup);
        properties.ModelPath = modelPath;

        const char* colorText = config.Get(configGroup.c_str(), "Color", nullptr);
        xoFatalIf(colorText == nullptr, "Carousel model data is invalid for entry: " << configGroup);
        

        xoFatalIf(!TryConvertStringToRGBA(String(colorText), properties.Color), "Couldn't parse color from carousel model data for entry: " << configGroup);

        node->Setup(properties);
    }

    Widget_Carousel* Owner;

    Array<SubWidget_CarouselNode*> Nodes;

    XO::Entity CubeEntity;
    XO::Component_Model CubeComponent;

    // Data Driven
    CarouselDisplayProperties DisplayProperties;
    CarouselDataProperties DataProperties;
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