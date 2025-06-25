#include "ModelTS.h"
#include "ZEVariables.h"

ModelTS::ModelTS(const QString& aName, const QString& a_Unit)
    : ModelVar(aName, a_Unit, "")
{
    m_default = 1.0;
    m_min = std::nan("1");
    m_max = std::nan("1");
}


ModelTS::ModelTS(const QString& aName,
    const QString& a_Unit, InputParam::ModelParam* ap_Variable)
    : ModelTS(aName, a_Unit)
{
    if (ap_Variable) {
        p_Variable = ap_Variable;
        m_Comment = ap_Variable->getDescription();

        if (const double* pval = std::get_if<double>(&ap_Variable->getDefault())) {
            if (!std::isnan(*pval))
                m_default = *pval;
        }
        if (const double* pval = std::get_if<double>(&ap_Variable->getMin())) {
            m_min = *pval;
        }
        if (const double* pval = std::get_if<double>(&ap_Variable->getMax())) {
            m_max = *pval;
        }
    }   
}

ModelTS::~ModelTS()
{
    if (p_ZEVariable) delete p_ZEVariable;
}

void ModelTS::setName(const QString& a_Name)
{ 
    m_Name = a_Name;
    //update the name of the corresponding ZEVariable
    if (p_ZEVariable) {
        p_ZEVariable->setName(a_Name);
    }
};

void ModelTS::setDefault(double a_Value)
{
    m_default = a_Value;
}

void ModelTS::set_Values(uint aNpdtPast)
{
    // recopier les valeurs de la ZE: 
    // décalage nptPast 
    if (p_ZEVariable && p_Variable) {
        bool vFill = false;
        bool isBlocking = p_Variable->IsBlocking();
        QVector<float>& vZEHist = *p_ZEVariable->ptrOutVariable();
        if (p_Variable->getType() == eVectorDouble) {
            if (p_Variable->isPValue()) {
                size_t vDestSize = p_Variable->size();
                if (vDestSize > 0) {
                    try
                    {
                        p_Variable->copyValues(vZEHist, aNpdtPast);
                        vFill = true;
                    }
                    catch (const std::exception& e)
                    {
                        qCritical() << "ERROR fillVectorData: in SubModel, variable: " << m_Name << ", size of " << vDestSize;
                        qCritical() << e.what();                        
                    }
                }                
            }
        }
        if (!vFill) {
            if (isBlocking) {
                qCritical() << "ERROR fillVectorData: in SubModel, variable: " << m_Name;
            }
            else  {
                qWarning() << "Warning fillVectorData: in SubModel, variable: " << m_Name;                
            }
        }        
    }   
}

void ModelTS::set_Values(size_t a_npdtTot, double a_Value)
{
    if (a_npdtTot > 0) {
        if (p_ZEVariable) {
            QVector<float>& vZEHist = *p_ZEVariable->ptrOutVariable();
            size_t vSize = vZEHist.size();
            size_t vOffset = std::min((size_t)0, vSize - a_npdtTot);
            for (size_t i = vOffset; i < vSize; i++)
                vZEHist[i] = a_Value;
        }
        else if (p_Variable) {
            p_Variable->setValues(a_Value, a_npdtTot);
        }
    }
}

bool ModelTS::checkProfile()
{
    bool vRet = true;
    if (p_ZEVariable) {
        if (m_Name != "" && !std::isnan(m_min) && !std::isnan(m_max)) {
            qInfo() << "checking " << m_Name;
            QVector<float>& vZEHist = *p_ZEVariable->ptrOutVariable();
            size_t vSize = vZEHist.size();
            if (vSize) {
                float vMin = vZEHist[0], vMax = vZEHist[0];
                for (size_t i = 1;i < vSize;i++) {
                    if (vZEHist[i] > vMax)  vMax = vZEHist[i];
                    if (vZEHist[i] < vMin)  vMin = vZEHist[i];
                }
                if (vMax < m_min || vMax > m_max || vMin < m_min || vMin > m_max)
                {
                    qCritical() << "ERROR in profile " << m_Name
                        << " values should be in the range [" << m_min << ";" << m_max << "] "
                        << " instead of [" << vMin << ";" << vMax << "] ";
                    vRet = false;
                }
            }           
            qInfo() << "end checking " << m_Name;
        }
    }
    return vRet;
}

void ModelTS::subscribeTS(const QString& a_exName, t_mapExchange& a_Import, size_t a_npdtTot)
{
    p_ZEVariable = new ZEVariables(m_Name, getUnit(), m_Comment + " Profile ", "1", "0", true);
    a_Import[a_exName] = p_ZEVariable;
    QVector<float>& vZEHist = *p_ZEVariable->ptrVariable();
    vZEHist.resize(a_npdtTot);
    set_Values(a_npdtTot, m_default);    
}
