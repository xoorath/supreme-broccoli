#include <Include/Widgets/Carousel/Widget_Carousel.h>

#include <Include/Assets/AssetManager.h>
#include <Include/Assets/Config.h>
#include <Include/Component_Model.h>
#include <Include/Entity.h>
#include <Include/Input.h>
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
    struct NodeLocation {
        Vector3 Position;
        float32 Scale;
        float32 Alpha;
    };

    Array<NodeLocation> NodeLocations;
    float32 ScrollPosition = 0.0f;
    int32 CurrentlySelectedIndex = 0;
    int32 AutoScrollIndex = -1;

    bool IsManuallyScrolling = false;

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

        xoFatalIf(DataProperties.NumEntries <= 0, "No nodes were provided to the carousel. This isn't supported.");
        
        Nodes.resize(DataProperties.NumEntries, nullptr);

        CurrentlySelectedIndex = DataProperties.NumEntries / 2;

        Config ContentConfig(AssetManager::RelativetoAssetsRoot(DataProperties.ContentConfig));
        for (int32 i = 0; i < DataProperties.NumEntries; ++i) {
            std::stringstream ss;
            ss << "Entry" << i;
            Nodes[i] = new UI::SubWidget_CarouselNode();
            CreateSubWidget(Nodes[i], Owner->GetOwner(), ContentConfig, ss.str());
        }

        owner->SetRotation(Vector3(-30.0f * Deg2Rad, 0.0f, 0.0f));
        owner->SetPosition(Vector3(0, DisplayProperties.CameraYOffset, DisplayProperties.CameraZOffset));

        SetupNodeLocations();

        Input& input = Input::Get();
        input.SubscribeToMouseEvent(
            InputPriorities::Carousel_Navigation, 
            MouseSubscriptionType::MouseDrag,

            /*Consume?*/ [this](const class Vector2&) { 
                IsManuallyScrolling = true;
                AutoScrollIndex = -1;
                return InputConsumptionType::Consume; 
            },

            /*Update*/ [this](const class Vector2& start, const class Vector2& current, const class Vector2& deltaPos, float dt) {
                if (IsManuallyScrolling) {
                    ScrollPosition += (-deltaPos.x / 1280.0f)  * 3.0f;
                }
            },

            /*End*/ [this]() {
                IsManuallyScrolling = false;
            });

        input.SubscribeToKeyEvent(
            InputPriorities::Carousel_Navigation,
            KeySubscriptionType::Press,

            /*Consume?*/[](KeyboardKey key) {
                if (key == KeyboardKey::Left || key == KeyboardKey::Right) {
                    return InputConsumptionType::Consume;
                }
                return InputConsumptionType::DontConsume;
            },

            /*Update*/ [this](KeyboardKey key) {
                int32 scrollDir = key == KeyboardKey::Left ? -1 : 1;
                if (AutoScrollIndex == -1) {
                    AutoScrollIndex = CurrentlySelectedIndex + scrollDir;
                }
                else {
                    AutoScrollIndex += scrollDir;
                }
                if (AutoScrollIndex < 0) {
                    AutoScrollIndex = 0;
                }
                else if (AutoScrollIndex >= Nodes.size() - 1) {
                    AutoScrollIndex = Nodes.size() - 1;
                }
                IsManuallyScrolling = false;
            },

            /*End*/ [this]() {
            });

    }

    void SetupNodeLocations() {
        uint32 sideNodes = ((DisplayProperties.CarouselNodesOnScreen-1)/2)+1;
        // +1 is to allow an extra one of 0 alpha at the end.
        NodeLocations.resize(sideNodes +1);

        float32 xstep = DisplayProperties.CarouselNode_XOffset;
        float32 x = 0.0f;

        float32 zstep = DisplayProperties.CarouselNode_ZOffset;
        float32 z = 0.0f;

        float32 sstep = DisplayProperties.CarouselNode_Scale;
        float32 s = sstep;

        float32 astep = DisplayProperties.CarouselNode_Alpha;
        float32 a = astep;
        for (uint32 i = 0; i < NodeLocations.size(); ++i) {
            NodeLocations[i].Position.Set(x, 0.0f, z);
            x += xstep;
            xstep *= DisplayProperties.CarouselNode_XOffsetFalloff;
            z += zstep;
            zstep *= DisplayProperties.CarouselNode_ZOffsetFalloff;
            
            NodeLocations[i].Scale = s;
            s *= DisplayProperties.CarouselNode_ScaleFalloff;

            NodeLocations[i].Alpha = a;
            a *= DisplayProperties.CarouselNode_AlphaFalloff;
        }
        // this location is where off screen elements go.
        NodeLocations[NodeLocations.size() - 1].Alpha = 0.0f;
    }

    void GetLocationForNode(NodeLocation& outNodeLocation, float32 scrollPosition) {
        float32 absScrollPosition = Abs(scrollPosition);

        // Get indices of known scrolling positions, and the delta this node is between them.
        int32 scrollIndex = (int32)floor(absScrollPosition);
        int32 nextScrollIndex = scrollIndex + 1;
        float32 scrollDelta = absScrollPosition - (float32)scrollIndex;

        float32 sign;
        if (scrollPosition >= 0.0f) {
            sign = 1.0f;
        }
        else {
            sign = -1.0f;
            scrollDelta = scrollDelta;
        }
        
        // Assuming this node is on screen, interpolate between the positions then flip relevant signs.
        if (nextScrollIndex < (int32)NodeLocations.size()) {
            auto& prev = NodeLocations[scrollIndex];
            auto& next = NodeLocations[nextScrollIndex];

            Vector3::Lerp(prev.Position, next.Position, scrollDelta, outNodeLocation.Position);
            outNodeLocation.Position.x *= sign;

            outNodeLocation.Scale = Lerp(prev.Scale, next.Scale, scrollDelta);

            outNodeLocation.Alpha = Lerp(prev.Alpha, next.Alpha, scrollDelta);
        }
        else {
            // Make this node invisible if it's scrolled off screen.
            auto& last = NodeLocations[NodeLocations.size() - 1];
            outNodeLocation.Position = last.Position;
            outNodeLocation.Scale = last.Scale;
            outNodeLocation.Alpha = 0.0f;
        }
    }

    void Update(float32 dt) {
        float32 sideNodes = (float)(DisplayProperties.CarouselNodesOnScreen - 1) / 2;

        NodeLocation location;
        float32 rgba[4];
        
        ////////////////////////////////////////////////////////////////////////// Auto scroll based on keyboard/far-click

        if (AutoScrollIndex != -1) {
            if (AutoScrollIndex == CurrentlySelectedIndex) {
                AutoScrollIndex = -1;
            }
            else {
                if (AutoScrollIndex < CurrentlySelectedIndex) {
                    ScrollPosition += dt*7;
                }
                else {
                    ScrollPosition -= dt*7;
                }

            }
        }


        ////////////////////////////////////////////////////////////////////////// Select active index based on scroll position
        while (ScrollPosition > 0.5f) {
            ScrollPosition -= 1.0f;
            CurrentlySelectedIndex--;
        }

        while (ScrollPosition < -0.5f) {
            ScrollPosition += 1.0f;
            CurrentlySelectedIndex++;
        }

        if (CurrentlySelectedIndex < 0) {
            CurrentlySelectedIndex = 0;
        }

        if (CurrentlySelectedIndex >= (int32)Nodes.size()) {
            CurrentlySelectedIndex = (int32)Nodes.size()-1;
        }

        ////////////////////////////////////////////////////////////////////////// clamp scroll in place (with over-scroll spring)
        const float MaxOverScroll = 0.4f; // don't set higher than 0.5
        if (CurrentlySelectedIndex == 0 && ScrollPosition > MaxOverScroll) {
            ScrollPosition = MaxOverScroll;
        }
        if (CurrentlySelectedIndex == (int32)Nodes.size() - 1 && ScrollPosition < -MaxOverScroll) {
            ScrollPosition = -MaxOverScroll;
        }

        ////////////////////////////////////////////////////////////////////////// Smoothly return to active index.
        if (!IsManuallyScrolling && AutoScrollIndex == -1) {
            // TODO: data drive
            const float32 scrollSnapSpeed = 7.0f;
            ScrollPosition = Lerp(ScrollPosition, 0.0f, dt * scrollSnapSpeed);
        }

        ////////////////////////////////////////////////////////////////////////// Position all nodes based on the known scroll position.
        for (uint32 i = 0; i < Nodes.size(); ++i) {
            float32 indexRelativePosition = (float32)i - (float32)CurrentlySelectedIndex;
            GetLocationForNode(location, ScrollPosition + indexRelativePosition);
            
            Nodes[i]->GetRGBA(rgba);
            rgba[3] = location.Alpha;
            Nodes[i]->SetRGBA(rgba);

            auto& entity = Nodes[i]->GetWrapperEntity();
            entity.SetPosition(location.Position);
            entity.SetScale(location.Scale);
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