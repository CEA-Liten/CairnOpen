#ifndef ZEVariables_H
#define ZEVariables_H
#include "CairnCore_global.h"
#include "InputParam.h"

/**
 * \details
* This component defines the variables to be exchanged with PEGASE simulation environment.
*/
class CAIRNCORESHARED_EXPORT ZEVariables: public QObject
{
    Q_OBJECT
public:
    ZEVariables(
        const QString& aName = "",
        const QString& aUnit = "",
        const QString& aDesc = "",        
        const QString& asCoeffExport = "1",
        const QString& asOffsetExport = "0",
        const bool& aIsMPC = false);
  

    QVector<float>* ptrVariable();     /** Access to Pointer to vector of IO float variable */
    QVector<float>* ptrOutVariable();
    QString Name() {return mName;}                       /** Access to Associated name of variable */
    QString Unit() {return mUnit;}                       /** Access to Associated unit of variable */
    QString Desc() {return mDesc;}                       /** Access to Associated description of variable */
    float initValue() {return minitValue;}                   /** Access to Associated initial value of variable */
    float CoeffExport() {return mCoeffExport;}                   /** Associated multiplicative factor for export */
    float CoeffOffset() {return mCoeffOffset;}                   /** Associated offset value for export */
    bool IsMPC() { return m_IsMPC; }

    void setName(const QString& a_Name) { mName = a_Name; }

    void setCoeffExport(float aCoeffExport) {mCoeffExport=aCoeffExport;}
    void setCoeffOffset(float aCoeffOffset) {mCoeffOffset=aCoeffOffset;}

    bool set_Values(class InputParam::ModelParam* a_Param,
        double aTimeStepOut, const std::vector<double>& aTimeStepsIn, uint aNpdtPast);
    bool update_PastValues(int nptPast, int timeShift);

    void IsExt(bool a_IsExt) { m_IsExt = a_IsExt; };
private:
    QString mName ;             /** Associated name of variable */
    QString mUnit ;             /** Associated unit of variable */
    QString mDesc ;             /** Associated description of variable */
    float minitValue ;              /** Associated initial value of variable */
    float mCoeffExport ;             /** Associated multiplicative factor for export */
    float mCoeffOffset ;             /** Associated offset factor for export */

    QVector<float> m_Values;
    QVector<float> m_OutValues;
    bool m_IsMPC{ false };
    bool m_IsExt{ false };
};

typedef QMap<QString, ZEVariables*> t_mapExchange;

#endif // ZEVariables_H
