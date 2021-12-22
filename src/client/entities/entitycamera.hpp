#include "entitybase.hpp"

// TODO: this class should probably be a bit more meaningful and have a server-side counterpart

class CWorld;

class CEntityCamera : public CEntityBase
{
    DeclBaseClass(CEntityCamera, CEntityBase);

    public:
    CEntityCamera(entityId_t id, CWorld *pWorld) : BaseClass(id, pWorld) {};
    ~CEntityCamera() {};    
};