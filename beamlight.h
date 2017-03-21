
#ifndef BEAMLIGHT_H
#define BEAMLIGHT_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

class BeamLight : public LogicComponent
{
    URHO3D_OBJECT(BeamLight, LogicComponent);
public:
    BeamLight(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);

private:
    Node* beamNode_;
    Node* flareNode_;

    Light* light_;

    SharedPtr<Material> beamMaterial_;
    SharedPtr<Material> flareMaterial_;
};

#endif // BEAMLIGHT_H
