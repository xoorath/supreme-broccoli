#pragma once

#include <Include/Assets/Texture2D.h>
#include <Include/Macros.h>
#include <Include/Types.h>
#include <Include/Subscription.h>

namespace XO {

enum VertexDescriptionFlags {
    Position    = 1,
    Normal      = 2,
    UV          = 4,
    Color       = 8,
};

struct SubMesh {
/*  Example:
    (assumes each vertex has a position, normal and UV)
                                               B______C
                                               /\    /
                                              /  \  /
                                             /    \/
                                            A``````D
*/

    // TODO: replace texture with a material.
    Texture2D       Texture;

    uint32          NumTriangles = 0;       // 6    (two triangles)
    uint32          VertexBufferSize = 0;   // Size in bytes.
    uint32          IndexBufferSize = 0;    // Size in bytes.

    float32*        VertexBuffer = nullptr; // [ Apx, Apy, Apz, Anx, Any, Anz, Atu, Atv, 
                                            //   Bpx, Bpy, Bpz, Bnx, Bny, Bnz, Btu, Btv,
                                            //   Cpx, Cpy, Cpz, Cnx, Cny, Cnz, Ctu, Ctv,
                                            //   Dpx, Dpy, Dpz, Dnx, Dny, Dnz, Dtu, Dtv ]

    uint32*         IndexBuffer = nullptr;  // [ 0, 1, 3, 3, 1, 2 ]


    VertexDescriptionFlags VertexDesc       // in the example we assume (Position|Normal|UV)
        = VertexDescriptionFlags::Position;
};

struct ModelData {
    class Model*    Owner = nullptr;
    Array<SubMesh>  Meshes;
};

class Model {
public:
    Model();
    ~Model();

    void Load(String path);
    void Unload();

    bool GetIsValid() const;

    // Called during the last loading stage.
    // The provided mesh data is only valid for the duration of the callback.
    Subscription1<ModelData&> OnLoaded;

private:
    xoPimpl(ModelImpl, Impl, 4);
};

}