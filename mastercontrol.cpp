
#include "beamlight.h"

#include "mastercontrol.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl* MasterControl::instance_ = NULL;

MasterControl* MasterControl::GetInstance()
{
    return MasterControl::instance_;
}

MasterControl::MasterControl(Context *context):
    Application(context)
{
    instance_ = this;
}

void MasterControl::Setup()
{
    SetRandomSeed(TIME->GetSystemTime());

    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("luckey", "logs")+"BeamLight.log";
    engineParameters_[EP_WINDOW_TITLE] = "BeamLight";
    engineParameters_[EP_WINDOW_ICON] = "icon.png";
    engineParameters_[EP_WORKER_THREADS] = false;
    engineParameters_[EP_RESOURCE_PATHS] = "Data;CoreData;Resources;";
}
void MasterControl::Start()
{
    BeamLight::RegisterObject(context_);

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();

    Zone* zone{ scene_->CreateComponent<Zone>() };
    zone->SetAmbientColor(Color::BLACK);

    //Create floor and roof
    Node* floorNode{ scene_->CreateChild("Floor") };
    floorNode->SetPosition(Vector3::DOWN * 2.3f);
    floorNode->SetScale(Vector3(100.0f, 0.1f, 100.0f));
    floorNode->CreateComponent<StaticModel>()->SetModel(CACHE->GetResource<Model>("Models/Box.mdl"));

    Node* roofNode{ scene_->CreateChild("Floor") };
    roofNode->SetPosition(Vector3::UP * 3.0f);
    roofNode->SetScale(Vector3(100.0f, 0.1f, 100.0f));
    roofNode->CreateComponent<StaticModel>()->SetModel(CACHE->GetResource<Model>("Models/Box.mdl"));

    //Create camera and viewport
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->SetPosition(Vector3::UP + Vector3::BACK * 10.0f);
    cameraNode_->LookAt(Vector3::DOWN * 0.5f);

    SharedPtr<Viewport> viewport{ new Viewport(context_, scene_, cameraNode_->CreateComponent<Camera>()) };

    RenderPath* effectRenderPath = viewport->GetRenderPath();
    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath->SetEnabled("FXAA3", true);
    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));
    effectRenderPath->SetShaderParameter("BloomHDRThreshold", 0.23f);
    effectRenderPath->SetShaderParameter("BloomHDRMix", Vector2(1.0f, 0.23f));
    effectRenderPath->SetEnabled("BloomHDR", true);

    RENDERER->SetViewport(0, viewport);

    //Create lights
    int rows{ 5 };
    int columns{ 5 };
    for (int j{0}; j < rows; ++j) {
        for (int i{0}; i < columns; ++i) {
            if (j == rows - 1 && i != columns -1)
                continue;

            Node* beamNode{ scene_->CreateChild("BeamLight") };
            beamNode->SetPosition(Vector3::FORWARD);
            beamNode->RotateAround(Vector3::ZERO, Quaternion(i * (360.0f / columns), Vector3::UP), TS_WORLD);
            beamNode->RotateAround(Vector3::ZERO, Quaternion(90.0f - (180.0f / rows) * (j + 1), beamNode->GetRight()), TS_WORLD);
            beamNode->CreateComponent<BeamLight>();
        }
    }

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(MasterControl, HandleSceneUpdate));
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}
void MasterControl::Exit()
{
    engine_->Exit();
}
void MasterControl::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    float timeStep{ eventData[SceneUpdate::P_TIMESTEP].GetFloat() };
    float flySpeed{ 5.0f };
    float delta{ timeStep * flySpeed };

    cameraNode_->Translate(Vector3(delta * (INPUT->GetKeyDown(KEY_D) - INPUT->GetKeyDown(KEY_A)),
                                   delta * (INPUT->GetKeyDown(KEY_Q) - INPUT->GetKeyDown(KEY_E)),
                                   delta * (INPUT->GetKeyDown(KEY_W) - INPUT->GetKeyDown(KEY_S))));

}

float MasterControl::Sine(const float freq, const float min, const float max, const float shift)
{
    float phase{SinePhase(freq, shift)};
    float add{0.5f * (min + max)};
    return LucKey::Sine(phase) * 0.5f * (max - min) + add;
}
float MasterControl::Cosine(const float freq, const float min, const float max, const float shift)
{
    return Sine(freq, min, max, shift + 0.25f);
}
float MasterControl::SinePhase(float freq, float shift)
{
    return M_PI * 2.0f * (freq * scene_->GetElapsedTime() + shift);
}
