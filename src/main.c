﻿




#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

void DrawAxes(float s);
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - models loading");





    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){0.0f, 1.0f, 2.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };     // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type
    Vector3 camDirection = { 0,0,-1 };
    //Vector ortogonal a la direccion de la camara
    Vector3 camDirectionRight = { 1,0,0 };
    //variables de control de camara
    float camSpeed = 1;


   // SearchandSetResourceDir("resources");

    Model model = LoadModel("resources/BOLSO.glb");                 // Load model
    Texture2D texture = LoadTexture("BOLSO.mtl"); // Load model texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;            // Set map diffuse texture

    if (model.meshes == NULL) {
        printf("ERROR CARGANDO EL MODELO");
        return 0;
    }


    Vector3 position = { 0.0f, 0.0f, 0.0f };                    // Set model position

    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);   // Set model bounds

    // NOTE: bounds are calculated from the original size of the model,
    // if model is scaled on drawing, bounds must be also scaled

    bool selected = false;          // Selected object flag

    DisableCursor();                // Limit cursor to relative movement inside the window

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------




    float modelRotAngle = 0;
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        camDirectionRight = Vector3Normalize(Vector3CrossProduct(camDirection, camera.up));
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        if (IsKeyPressed(KEY_R))
        {
            camera.position = (Vector3){ 0,0,0 };
        }
        if (IsKeyDown(KEY_W))
        {
            camera.position = Vector3Add(camera.position, Vector3Scale(camDirection, camSpeed * GetFrameTime()));
        }
        if (IsKeyDown(KEY_S))
        {
            camera.position = Vector3Add(camera.position, Vector3Scale(camDirection, -camSpeed * GetFrameTime()));
        }
        if (IsKeyDown(KEY_A))
        {
            camera.position = Vector3Add(camera.position, Vector3Scale(camDirectionRight, -camSpeed * GetFrameTime()));
        }
        if (IsKeyDown(KEY_D))
        {
            camera.position = Vector3Add(camera.position, Vector3Scale(camDirectionRight, camSpeed * GetFrameTime()));
        }
        if (IsKeyDown(KEY_Q))
        {
            camera.position.y += 1.0f * GetFrameTime();
        }
        if (IsKeyDown(KEY_E))
        {
            camera.position.y += -1.0f * GetFrameTime();
        }

        Vector2 mousedelta = GetMouseDelta();
       // Matrix Mrot = MatrixRotateXYZ((Vector3) { mousedelta.y * -0.005f, mousedelta.x * -0.005f, 0 });
       //camDirection = Vector3Transform(camDirection, Mrot);  ///rotar la funcion de la camara en cuestion de la matriz
        float pitch = -mousedelta.y * 0.1 * GetFrameTime();
        camera.target = Vector3Add(camera.position, camDirection);
        camDirection = Vector3RotateByAxisAngle(camDirection, camDirectionRight, pitch);

      
        float yaw = -mousedelta.x * 0.1 * GetFrameTime();
        //camera.target = Vector3Add(cam.position, camDirection);
        camDirection = Vector3RotateByAxisAngle(camDirection, camera.up, yaw);
        camera.target = Vector3Add(camera.position, camDirection);

        
      
        // Load new models/textures on drag&drop
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (droppedFiles.count == 1) // Only support one file dropped
            {
                if (IsFileExtension(droppedFiles.paths[0], ".obj") ||
                    IsFileExtension(droppedFiles.paths[0], ".gltf") ||
                    IsFileExtension(droppedFiles.paths[0], ".glb") ||
                    IsFileExtension(droppedFiles.paths[0], ".vox") ||
                    IsFileExtension(droppedFiles.paths[0], ".iqm") ||
                    IsFileExtension(droppedFiles.paths[0], ".m3d"))       // Model file formats supported
                {
                    UnloadModel(model);                         // Unload previous model
                    model = LoadModel(droppedFiles.paths[0]);   // Load new model
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set current map diffuse texture

                    bounds = GetMeshBoundingBox(model.meshes[0]);

                    // TODO: Move camera position from target enough distance to visualize model properly
                }
                else if (IsFileExtension(droppedFiles.paths[0], ".png"))  // Texture file formats supported
                {
                    // Unload current model texture and load new one
                    UnloadTexture(texture);
                    texture = LoadTexture(droppedFiles.paths[0]);
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
                }
            }

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }

        // Select model on mouse click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            // Check collision between ray and box
            if (GetRayCollisionBox(GetScreenToWorldRay(GetMousePosition(), camera), bounds).hit) selected = !selected;
            else selected = false;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);




        //Dibujar el modelo transformado
       rlPushMatrix();
       
        modelRotAngle += 45 * GetFrameTime();
        rlRotatef(modelRotAngle, 0, 1, 0);  //donde este el 1 es el eje por el que rotara   x,z,y
        DrawModel(model, position, 1.0f, WHITE);        // Draw 3d model with texture
        DrawAxes(30.0f);
       
        
        
       rlPopMatrix();

        DrawGrid(20, 10.0f);         // Draw a grid
        DrawAxes(2.0f);

        if (selected) DrawBoundingBox(bounds, GREEN);   // Draw selection box

        EndMode3D();

        DrawText("Drag & drop model to load mesh/texture.", 10, GetScreenHeight() - 20, 10, DARKGRAY);
        if (selected) DrawText("MODEL SELECTED", GetScreenWidth() - 110, 10, 10, GREEN);

        DrawText("(c) Castle 3D model by Alberto Cano", screenWidth - 200, screenHeight - 20, 10, GRAY);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);     // Unload texture
    UnloadModel(model);         // Unload model

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


void DrawAxes(float scale)
{
    float punta = scale / 10;
    float s = scale;
    rlBegin(RL_LINES);
    //eje X
    rlColor4ub(255, 0, 0, 255);
    rlVertex3f(0, 0, 0);
    rlVertex3f(s, 0, 0);

    rlVertex3f(s, 0, 0);
    rlVertex3f(s - punta, 0, punta);

    rlVertex3f(s, 0, 0);
    rlVertex3f(s - punta, 0, -punta);
    //eje Y
    rlColor4ub(0, 255, 0, 255);
    rlVertex3f(0, 0, 0);
    rlVertex3f(0, s, 0);

    rlVertex3f(0, s, 0);
    rlVertex3f(0, s - punta, punta);

    rlVertex3f(0, s, 0);
    rlVertex3f(0, s - punta, -punta);
    //eje Z
    rlColor4ub(0, 0, 255, 255);
    rlVertex3f(0, 0, 0);
    rlVertex3f(0, 0, s);

    rlVertex3f(0, 0, s);
    rlVertex3f(punta, 0, s - punta);

    rlVertex3f(0, 0, s);
    rlVertex3f(-punta, 0, s - punta);
    rlEnd();
}

