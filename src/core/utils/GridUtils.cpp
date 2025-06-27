
#include "GridSubModel.h"

bool CAIRNCORESHARED_EXPORT isInjection(SubModel* ap_Model)
{
    if (ap_Model) {
        GridSubModel* vGrid = (GridSubModel*)ap_Model;
        return (vGrid->getSens() < 0);
    }
    return false;
}

bool CAIRNCORESHARED_EXPORT isExtraction(SubModel* ap_Model)
{
    if (ap_Model) {
        GridSubModel* vGrid = (GridSubModel*)ap_Model;
        return (vGrid->getSens() >= 0);
    }
    return false;
}