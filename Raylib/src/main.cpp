#include <raylib.h>
#include <raymath.h>


int main(){
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
    Image img = LoadImage("resources/earth_diffuse.png");
    ImageFlipVertical(&img);

    Texture2D earthTexture = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Create the sphere model (uncomment if applying to Mesh directly)
    Model earthModel = LoadModelFromMesh(GenMeshSphere(2.0f, 32, 32));
    earthModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = earthTexture;

    Vector3 earthPos = { 0.0f, 0.0f, 0.0f };
    SetTargetFPS(60);
    Quaternion textureFix =
        QuaternionFromAxisAngle((Vector3){1,0,0}, -PI/2);
    Quaternion orientation = QuaternionIdentity();
    
    while (!WindowShouldClose()){ 
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
        float spinSpeed = 0.5f; // radians/sec

        Quaternion spin =
            QuaternionFromAxisAngle(
                spinAxis,
                spinSpeed * GetFrameTime()
            );

        orientation = QuaternionMultiply(spin, orientation);
        orientation = QuaternionNormalize(orientation);
        Quaternion finalOrientation =
            QuaternionMultiply(orientation, textureFix);

        earthModel.transform = QuaternionToMatrix(finalOrientation);

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(camera);
                DrawModel(earthModel, earthPos, 1.0f, WHITE);
                DrawGrid(10, 10.0f);
                
            EndMode3D();

        EndDrawing();
    }

    // Unload textures and models
    UnloadTexture(earthTexture);
    UnloadModel(earthModel);
    CloseWindow();
    return 0;
}

