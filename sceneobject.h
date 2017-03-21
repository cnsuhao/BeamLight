
#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

class SceneObject : public LogicComponent
{
    URHO3D_OBJECT(SceneObject, Object);
public:
    SceneObject(Context* context);

    virtual void Set(Vector3 position);
    virtual void OnNodeSet(Node *node);

    Vector3 GetWorldPosition() const;
protected:
    Vector<SharedPtr<SoundSource> > sampleSources_;
    float randomizer_;

    void Disable();

    void PlaySample(Sound *sample, float gain = 0.3f);
};

#endif // SCENEOBJECT_H

