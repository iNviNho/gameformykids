

#ifndef GRASSES_H
#define GRASSES_H
#include "EntitiesHolder.h"


class Grasses {
private:
    EntitiesHolder grasses;
    void prepare();
public:
    explicit Grasses(EntitiesHolder&& grasses) : grasses(std::move(grasses)) {
        this->prepare();
    }
    const EntitiesHolder& GetEntities() const {return grasses;}
};



#endif //GRASSES_H
