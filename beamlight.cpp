
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
    light_->SetColor(LucKey::RandomColor(1.0f));

    StaticModel* lightModel{ node_->CreateComponent<StaticModel>() };
    lightModel->SetModel(CACHE->GetResource<Model>("Models/Lamp.mdl"));
    SharedPtr<Material> lightMaterial_{ CACHE->GetResource<Material>("Materials/Light.xml")->Clone() };
    lightMaterial_->SetShaderParameter("MatDiffColor", light_->GetColor());
    lightModel->SetMaterial(0, lightMaterial_);

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

}

void BeamLight::Update(float timeStep)
{
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




