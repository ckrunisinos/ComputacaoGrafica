#include "Mesh.h"

class Animal : public Mesh {
    public:
    void walkForwards();
    void walkBackwards();

    protected:
    float maxX, minX, minY, maxY;
};

