
#include "beamlight.h"



void BeamLight::RegisterObject(Context* context)
{
    context->RegisterFactory<BeamLight>();
}

BeamLight::BeamLight(Context* context) : LogicComponent(context)
{
}

void BeamLight::OnNodeSet(Node* node)
{ if(!node) return;

    light_ = node_->CreateComponent<Light>();
    light_->SetLightType(LIGHT_SPOT);

    StaticModel* lampModel{ node_->CreateComponent<StaticModel>() };
    lampModel->SetModel(CACHE->GetResource<Model>("Models/Lamp.mdl"));
    lampMaterial_ = CACHE->GetResource<Material>("Materials/Lamp.xml")->Clone();
    lampModel->SetMaterial(0, lampMaterial_);

    beamNode_ = node_->CreateChild("Beam");
    beamNode_->Translate(Vector3::FORWARD * 0.82f);
    beamNode_->SetScale(Vector3(1.0f, 1.0f, 1.8f));

    StaticModel* beamModel{ beamNode_->CreateComponent<StaticModel>() };
    beamModel->SetModel(CACHE->GetResource<Model>("Models/Plane.mdl"));
    beamMaterial_ = CACHE->GetResource<Material>("Materials/Beam.xml")->Clone();
    beamModel->SetMaterial(beamMaterial_);

    flareNode_ = node_->CreateChild("Flare");
    flareNode_->Rotate(Quaternion(90.0f, Vector3::RIGHT));
    StaticModel* flareModel{ flareNode_->CreateComponent<StaticModel>() };
    flareModel->SetModel(CACHE->GetResource<Model>("Models/Plane.mdl"));
    flareMaterial_ = CACHE->GetResource<Material>("Materials/Flare.xml")->Clone();
    flareModel->SetMaterial(flareMaterial_);

//    SetColor(LucKey::RandomColor(1.0f));
}

void BeamLight::Update(float timeStep)
{
    Vector3 euler{ node_->GetWorldRotation().EulerAngles() };

    float shift{ 0.23f + 0.05f * euler.x_ };
    SetColor(Color(MC->Sine(0.1f, 0.0f, 1.0f, shift),
                   MC->Sine(0.2f, 0.0f, 1.0f, shift),
                   MC->Sine(0.3f, 0.0f, 1.0f, shift),
                   1.0f));
    light_->SetBrightness(euler.x_ != -90.0f ? Pow(MC->Sine(0.125f, 0.0f, 1.0f, euler.y_ / 90.0f), 0.75f)
                                            : 1.0f);

    Vector3 camDeltaPos{ node_->GetWorldPosition() - MC->GetCameraPosition() };

    node_->RotateAround(Vector3::ZERO, Quaternion(timeStep * 42.0f, Vector3::UP), TS_WORLD);
    beamNode_->LookAt(node_->GetWorldPosition() + node_->GetDirection(), camDeltaPos);
    float normAngle{ 1.0f - node_->GetDirection().Angle(camDeltaPos) / 90.0f };
    Color lightColor{ light_->GetColor() * light_->IsEnabled() * light_->GetBrightness() };
    Color beamColor{ lightColor * Clamp(Pow(1.1f - Abs(normAngle) - 0.16f, 0.9f), 0.0f, 1.0f) };
    Color flareColor{ lightColor * Clamp(Pow(0.5f - normAngle, 4.0f) - 0.23f, 0.0f, 1.0f) };
    beamMaterial_->SetShaderParameter("MatDiffColor", beamColor);
    flareMaterial_->SetShaderParameter("MatDiffColor", flareColor);
    flareNode_->SetScale(normAngle);
}

void BeamLight::SetColor(Color color)
{
    light_->SetColor(color);
    lampMaterial_->SetShaderParameter("MatDiffColor", light_->GetColor() * light_->IsEnabled() * light_->GetBrightness());
}




