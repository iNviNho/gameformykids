

#ifndef GRASSES_H
#define GRASSES_H
#include "EntitiesHolder.h"


class Grasses {
private:
    EntitiesHolder grasses;
    void prepare();
public:
    explicit Grasses(const EntitiesHolder& grasses) : grasses(grasses) {
        this->prepare();
    }
    [[nodiscard]] const EntitiesHolder& GetEntities() const {return grasses;}
};



#endif //GRASSES_H
