#ifndef MYGAME_STATICMODEL_H
#define MYGAME_STATICMODEL_H
#include "AbstractModel.h"


class StaticModel : public AbstractModel {
public:
    using AbstractModel::AbstractModel;
    bool IsAnimated() const override { return false; }
};








#endif //MYGAME_STATICMODEL_H