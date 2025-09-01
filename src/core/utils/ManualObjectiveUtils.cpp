
#include "TechnicalSubModel.h"

bool CAIRNCORESHARED_EXPORT isBlended(SubModel* ap_Model) 
{
    if (ap_Model) {
        TechnicalSubModel* vTechnicalSubModel = (TechnicalSubModel*)ap_Model;
        return (vTechnicalSubModel->ObjectiveType() == "Blended");
    }
    return false;
}
