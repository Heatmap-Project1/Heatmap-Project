#include <raylib.h>
#include <iostream>
#include <raymath.h>
#include <cfloat>
using namespace std;
struct TriangleHit
{
    bool hit = false;
    int triangle = -1;
    float distance = FLT_MAX;
    Vector3 point;
};
Vector3 Barycentric(Vector3 p, Vector3 a, Vector3 b, Vector3 c)
{
    Vector3 v0 = Vector3Subtract(b, a);
    Vector3 v1 = Vector3Subtract(c, a);
    Vector3 v2 = Vector3Subtract(p, a);

    float d00 = Vector3DotProduct(v0, v0);
    float d01 = Vector3DotProduct(v0, v1);
    float d11 = Vector3DotProduct(v1, v1);
    float d20 = Vector3DotProduct(v2, v0);
    float d21 = Vector3DotProduct(v2, v1);

    float denom = d00*d11 - d01*d01;

    float v = (d11*d20 - d01*d21)/denom;
    float w = (d00*d21 - d01*d20)/denom;
    float u = 1.0f - v - w;

    return {u,v,w};
}

int main(){
    cout<< RAYLIB_VERSION<<endl;
    const int screenWidth = 1100;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Raylib Earth Simulation");

    // Define the camera to look into our 3D world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 0.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    
    // Load Earth texture
    Image img = LoadImage("resources/Equirectangular-projection new.png");
    ImageFlipVertical(&img);
    Texture2D bg= LoadTexture("resources/images.png");

    Texture2D earthTexture = LoadTextureFromImage(img);
    UnloadImage(img);
    Image continentMask=LoadImage("resources/World_location_map.png");
    ImageFlipVertical(&continentMask);
    Texture2D maskTexture = LoadTextureFromImage(continentMask);
    
    Shader hoverShader = LoadShader(0, "resources/hover.fs");
    int maskLoc = GetShaderLocation(hoverShader, "maskTexture");
    int colorLoc = GetShaderLocation(hoverShader, "hoveredColor");
    SetShaderValueTexture(hoverShader, maskLoc, maskTexture);
    
    

    // Create the sphere model (uncomment if applying to Mesh directly)
    Model earthModel = LoadModelFromMesh(GenMeshSphere(2.0f, 32, 32));
    earthModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = earthTexture;
    earthModel.materials[0].shader = hoverShader;
    

    Vector3 earthPos = { 0.0f, 0.0f, 0.0f };
    SetTargetFPS(60);
    Quaternion textureFix =
        QuaternionFromAxisAngle((Vector3){1,0,0}, -PI/2);
    Quaternion orientation = QuaternionIdentity();
    float rad=1.0f;
    
    while (!WindowShouldClose()){ 
        float wheel=GetMouseWheelMove();
        wheel=wheel*0.05f;
        rad=rad+wheel;
        
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            Vector2 delta = GetMouseDelta();
            Quaternion qYaw =
                QuaternionFromAxisAngle(
                    (Vector3){0,1,0},
                    delta.x * 0.005f
                );

            Quaternion qPitch =
                QuaternionFromAxisAngle(
                    (Vector3){1,0,0},
                    delta.y * 0.005f
                );

            orientation = QuaternionMultiply(qYaw, orientation);
            orientation = QuaternionMultiply(qPitch, orientation);

            orientation = QuaternionNormalize(orientation);
            
        }
        Vector3 localUp = {0, 1, 0};
        Vector3 spinAxis = Vector3RotateByQuaternion(localUp, orientation);
        float spinSpeed = 0.05f; // radians/sec

        Quaternion spin =
            QuaternionFromAxisAngle(
                spinAxis,   
                spinSpeed * GetFrameTime()
            );

        orientation = QuaternionMultiply(spin, orientation);
        orientation = QuaternionNormalize(orientation);
        Quaternion finalOrientation =
            QuaternionMultiply(orientation,textureFix);
            
        earthModel.transform = QuaternionToMatrix(finalOrientation);
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        TriangleHit best;
        Mesh mesh = earthModel.meshes[0];
        for(int i=0;i<mesh.triangleCount;i++)
        {
            int base = i*3;

            Vector3 a =
            {
                mesh.vertices[(base+0)*3],
                mesh.vertices[(base+0)*3+1],
                mesh.vertices[(base+0)*3+2]
            };

            Vector3 b =
            {
                mesh.vertices[(base+1)*3],
                mesh.vertices[(base+1)*3+1],
                mesh.vertices[(base+1)*3+2]
            };

            Vector3 c =
            {
                mesh.vertices[(base+2)*3],
                mesh.vertices[(base+2)*3+1],
                mesh.vertices[(base+2)*3+2]
            };

            a = Vector3Transform(a, earthModel.transform);
            b = Vector3Transform(b, earthModel.transform);
            c = Vector3Transform(c, earthModel.transform);

            RayCollision rc = GetRayCollisionTriangle(ray,a,b,c);

            if(rc.hit && rc.distance < best.distance)
            {
                best.hit = true;
                best.distance = rc.distance;
                best.point = rc.point;
                best.triangle = i;
            }
        }
        if(best.hit)
        {
            Mesh mesh = earthModel.meshes[0];

            int base = best.triangle * 3;

            Vector3 a =
            {
                mesh.vertices[(base+0)*3],
                mesh.vertices[(base+0)*3+1],
                mesh.vertices[(base+0)*3+2]
            };

            Vector3 b =
            {
                mesh.vertices[(base+1)*3],
                mesh.vertices[(base+1)*3+1],
                mesh.vertices[(base+1)*3+2]
            };

            Vector3 c =
            {
                mesh.vertices[(base+2)*3],
                mesh.vertices[(base+2)*3+1],
                mesh.vertices[(base+2)*3+2]
            };

            a = Vector3Transform(a, earthModel.transform);
            b = Vector3Transform(b, earthModel.transform);
            c = Vector3Transform(c, earthModel.transform);

            Vector3 bary = Barycentric(best.point, a, b, c);

            Vector2 uv0 =
            {
                mesh.texcoords[(base+0)*2],
                mesh.texcoords[(base+0)*2+1]
            };

            Vector2 uv1 =
            {
                mesh.texcoords[(base+1)*2],
                mesh.texcoords[(base+1)*2+1]
            };

            Vector2 uv2 =
            {
                mesh.texcoords[(base+2)*2],
                mesh.texcoords[(base+2)*2+1]
            };

            Vector2 uv;

            uv.x =
                bary.x*uv0.x +
                bary.y*uv1.x +
                bary.z*uv2.x;

            uv.y =
                bary.x*uv0.y +
                bary.y*uv1.y +
                bary.z*uv2.y;

            int px = (int)(uv.x * continentMask.width);
            int py = (int)(uv.y * continentMask.height);

            px = Clamp(px, 0, continentMask.width - 1);
            py = Clamp(py, 0, continentMask.height - 1);

            Color colo = GetImageColor(continentMask, px, py);

            float color[3] =
            {
                (float)colo.r,
                (float)colo.g,
                (float)colo.b
            };

            SetShaderValue(
                hoverShader,
                colorLoc,
                color,
                SHADER_UNIFORM_VEC3
            );
        }
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexturePro(
                bg,
                (Rectangle){0, 0, (float)bg.width, (float)bg.height},
                (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                (Vector2){0, 0},0.0f,
                WHITE
            );
            
            BeginMode3D(camera);
                
                DrawModel(earthModel, earthPos, rad, WHITE);
                SetTextureWrap(maskTexture, TEXTURE_WRAP_CLAMP);    
                DrawGrid(10, 10.0f);
                
            EndMode3D();

        EndDrawing();
    }

    // Unload textures and models
    UnloadTexture(earthTexture);
    UnloadModel(earthModel);
    UnloadTexture(bg);
    UnloadImage(continentMask);
    
    CloseWindow();
    return 0;
}

