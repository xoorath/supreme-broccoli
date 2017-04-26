#include <Include/Assets/Model.h>

#include <Include/Assets/AssetManager.h>
#include <Include/Assets/Config.h>
#include <Include/Assets/Shader.h>
#include <Include/Assets/Texture2D.h>
#include <Include/Log.h>

#include <ThirdParty/tinyobjloader/tiny_obj_loader.h>
#include <ThirdParty/xo-math/xo-math.h>

#define ensure(cnd, msg) if(!(cnd)) { xoFatal(msg); __debugbreak(); return; }

namespace XO {

class ModelImpl {
public:
    Model* Owner;

    ModelImpl(Model* owner, String path) 
        : Owner(owner) {
        Config modelConfig(AssetManager::RelativetoAssetsRoot(path));

        const char* modelType = modelConfig.Get("model", "type", nullptr);
        xoFatalIf(modelType == nullptr, "Malformed model file.");

        if (strcmp(modelType, "obj") == 0)
        {
            String objFilename(modelConfig.Get("obj", "obj", nullptr));
            xoFatalIf(objFilename.length() <= 0, "malformed model file (obj section)");

            LoadOBJModel(AssetManager::RelativetoAssetsRoot(objFilename));
        }
        else {
            xoErr("Only obj models are supported at this time.");
        }
    }

    ~ModelImpl() {

    }

private:

    void LoadOBJModel(String objPath) {
        tinyobj::attrib_t attrib;
        Array<tinyobj::shape_t> shapes;
        Array<tinyobj::material_t> materials;
        String err;

        String materialPath;
        auto lastSlash = objPath.find_last_of('/');
        materialPath = objPath.substr(0, lastSlash+1);

        //
        // KNOWN LIMITATION !!!
        //
        // Here we're ignoring all faces not using the first material on that face.
        // We're also ignoring models made up of multiple objects.
        //
        // I just didn't feel like shooting myself in the foot and designing models to 
        // assume a single mesh, so I made the renderer API (OnLoaded) take data including sub meshes.
        //

        if (tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objPath.c_str(), materialPath.c_str())) {
            
            ensure(shapes.size() == 1 && materials.size() == 1, "OBJ loading failed. Only models with one mesh and one material are supported at this time.");

            auto& shape = shapes[0];
            auto& mesh = shape.mesh;
            auto& mat = materials[0];

            ModelData data;
            data.Owner = Owner;
            data.Meshes.resize(1);
            auto& submesh = data.Meshes[0];

            // some low number for the demo needs.
            constexpr uint32 MAX_VERTS = 4096;
            static uint32 UintBuffer[MAX_VERTS];
            static float32 FloatBuffer[MAX_VERTS];


            xoFatalIf(mesh.indices.size() > MAX_VERTS, "Mesh excedes max size.");
            xoFatalIf(mesh.indices.size() * (3 + 2 + 3) > MAX_VERTS, "Mesh excedes max size.");

            submesh.IndexBuffer = UintBuffer;
            submesh.VertexBuffer = FloatBuffer;

            // Currently only this configuration is supported.
            submesh.VertexDesc = (VertexDescriptionFlags)
                ((uint8)VertexDescriptionFlags::Position | 
                 (uint8)VertexDescriptionFlags::Normal | 
                 (uint8)VertexDescriptionFlags::UV);

            submesh.Texture.Load(mat.diffuse_texname);

            ensure(submesh.Texture.GetIsValid(), "Failed to load texture for mesh.");

            // Go over each triangle.
            uint32 currentIndex = 0;
            uint32 currentVert = 0;

            for (uint32 f = 0; f < mesh.indices.size() / 3; f++) {
                tinyobj::index_t idx0 = mesh.indices[3 * f + 0];
                tinyobj::index_t idx1 = mesh.indices[3 * f + 1];
                tinyobj::index_t idx2 = mesh.indices[3 * f + 2];

                float32 tc[3][2];
                if (attrib.texcoords.size() > 0) {
                    ensure(attrib.texcoords.size() > 2 * (uint32)idx0.texcoord_index + 1, "Index out of range for texture coord.");
                    ensure(attrib.texcoords.size() > 2 * (uint32)idx1.texcoord_index + 1, "Index out of range for texture coord.");
                    ensure(attrib.texcoords.size() > 2 * (uint32)idx2.texcoord_index + 1, "Index out of range for texture coord.");
                    tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
                    tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
                    tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
                    tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
                    tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
                    tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
                }
                else {
                    memset(tc, 0, (3 * 2 * sizeof(float32)));
                }

                float32 v[3][3];
                for (uint32 k = 0; k < 3; k++) {
                    uint32 f0 = (uint32)idx0.vertex_index;
                    uint32 f1 = (uint32)idx1.vertex_index;
                    uint32 f2 = (uint32)idx2.vertex_index;
                    ensure(f0 >= 0, "Index for vert is out of range.");
                    ensure(f1 >= 0, "Index for vert is out of range.");
                    ensure(f2 >= 0, "Index for vert is out of range.");

                    v[0][k] = attrib.vertices[3 * f0 + k];
                    v[1][k] = attrib.vertices[3 * f1 + k];
                    v[2][k] = attrib.vertices[3 * f2 + k];
                }

                float32 n[3][3];
                if (attrib.normals.size() > 0) {
                    uint32 f0 = idx0.normal_index;
                    uint32 f1 = idx1.normal_index;
                    uint32 f2 = idx2.normal_index;
                    ensure(f0 >= 0, "Index for normal is out of range.");
                    ensure(f1 >= 0, "Index for normal is out of range.");
                    ensure(f2 >= 0, "Index for normal is out of range.");
                    for (uint32 k = 0; k < 3; k++) {
                        n[0][k] = attrib.normals[3 * f0 + k];
                        n[1][k] = attrib.normals[3 * f1 + k];
                        n[2][k] = attrib.normals[3 * f2 + k];
                    }
                }
                else {
                    Vector3 a(v[0][0], v[1][0], v[2][0]);
                    Vector3 b(v[0][1], v[1][1], v[2][1]);
                    Vector3 c(v[0][2], v[1][2], v[2][2]);
                    Vector3 ab = a - b;
                    Vector3 bc = b - c;
                    Vector3 norm;
                    ab.Normalize();
                    bc.Normalize();
                    Vector3::Cross(ab, bc, norm);
                    for (uint32 k = 0; k < 3; k++) {
                        n[0][k] = norm[0];
                        n[1][k] = norm[1];
                        n[2][k] = norm[2];
                    }
                }

                for (int k = 0; k < 3; k++) {
                    // the reservation of this memory should be complete.
                    // however we should probably resize and simply index+assign.
                    submesh.VertexBuffer[currentVert++] = v[k][0];
                    submesh.VertexBuffer[currentVert++] = v[k][1];
                    submesh.VertexBuffer[currentVert++] = v[k][2];
                    submesh.VertexBuffer[currentVert++] = n[k][0];
                    submesh.VertexBuffer[currentVert++] = n[k][1];
                    submesh.VertexBuffer[currentVert++] = n[k][2];
                    submesh.VertexBuffer[currentVert++] = tc[k][0];
                    submesh.VertexBuffer[currentVert++] = tc[k][1];
                    submesh.IndexBuffer[currentIndex] = currentIndex++;
                }

            }
            submesh.VertexBufferSize = sizeof(float) * currentVert;
            submesh.IndexBufferSize = sizeof(uint32) * currentIndex;

            submesh.NumTriangles = currentIndex / 3;
            Owner->OnLoaded.Execute(data);
        }
        else {
            xoErr("Failed to load OBJ Model: " << objPath << "\ntinyobjloader:\n" << err);
        }
        //obj::Model model = obj::loadModelFromFile(objPath);

    }
};


Model::Model() 
: Impl(nullptr) {
}

Model::~Model() {
    Unload();
}

void Model::Load(String path) {
    xoErrIf(Impl != nullptr, "A model is being loaded twice. This will leak.");
    Impl = new ModelImpl(this, path);
}

void Model::Unload() {
    if (Impl)
    {
        delete Impl;
        Impl = nullptr;
    }
}

bool Model::GetIsValid() const {
    return true;
}


}