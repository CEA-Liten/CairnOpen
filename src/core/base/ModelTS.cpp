#include "ModelTS.h"
#include "ZEVariables.h"

ModelTS::ModelTS(const std::string& aName, const UnitParam* a_Unit, const std::string& aDescription)
    : ModelVar(aName, "", aDescription)
{
    m_default = 1.0;
    m_min = std::nan("1");
    m_max = std::nan("1");
    if(a_Unit) m_Unit = *a_Unit; /* Case of timeseries where the unit of corresponding ModelTS is a copy of that from ModelParam */
}

ModelTS::ModelTS(const std::string& aName, const UnitParam* a_Unit, ModelParam* ap_Variable)
    : ModelTS(aName, a_Unit)
{
    if (ap_Variable) {
        p_Variable = ap_Variable;
        m_Description = ap_Variable->getDescription();

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

void ModelTS::setName(const std::string& a_Name)
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

const std::vector<double>* ModelTS::get_Values(size_t aNpdtPast) const
{
    // Standard timeseries (type == eVectorDouble)
    if (p_Variable && p_Variable->getType() == EParamType::eVectorDouble)
    {
        const t_pvalue ptr = p_Variable->getPtr();
        if (auto* vec = std::get_if<eVectorDouble>(&ptr))
            return *vec;

        cWarning() << "ModelTS::get_Values() called on a vector parameter with non-defined values";
        return nullptr;
    }

    // Implicit timeseries (type == eString); e.g. an EnergyVector timeseries 
    if (p_ZEVariable)
    {
        const std::vector<double>* srcValue = p_ZEVariable->ptrOutVariable();
        if (!srcValue || srcValue->size() <= aNpdtPast)
            return nullptr;

        // Return cached result if still valid
        if (m_cachedSrc == srcValue &&
            m_cachedOffset == aNpdtPast &&
            m_cachedValues.size() == srcValue->size() - aNpdtPast)
        {
            return &m_cachedValues;
        }

        // Cache is invalid -> recompute
        m_cachedValues.resize(srcValue->size() - aNpdtPast);

        for (size_t i = 0; i < m_cachedValues.size(); ++i)
            m_cachedValues[i] = (*srcValue)[i + aNpdtPast];

        // Update cache metadata
        m_cachedSrc = srcValue;
        m_cachedOffset = aNpdtPast;

        return &m_cachedValues;
    }

    return nullptr;
}


void ModelTS::set_Values(uint aNpdtPast)
{
    if (p_Variable->getType() != eVectorDouble) {
        return;
    }

    // recopier les valeurs de la ZE: 
    // décalage nptPast 
    if (p_ZEVariable && p_Variable) {
        bool vFill = false;
        bool isBlocking = p_Variable->IsBlocking();
        std::vector<double>& vZEHist = *p_ZEVariable->ptrOutVariable();

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
                    cCritical() << "ModelTS: variable: " << m_Name << ", size of " << vDestSize;
                    cCritical() << e.what();                        
                }
            }                
        }

        if (!vFill) {
            if (isBlocking) {
                cCritical() << "ModelTS: mandatory variable " << m_Name << " has not been filled!";
            }
            else  {
                cWarning() << "ModelTS: optional variable: " << m_Name << " has not been filled!";
            }
        }        
    }   
}

void ModelTS::set_Values(size_t a_npdtTot, double a_Value)
{
    if (a_npdtTot > 0) {
        if (p_ZEVariable) {
            std::vector<double>& vZEHist = *p_ZEVariable->ptrOutVariable();
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
            cDebug() << "checking " << m_Name;
            std::vector<double>& vZEHist = *p_ZEVariable->ptrOutVariable();
            size_t vSize = vZEHist.size();
            if (vSize) {
                float vMin = vZEHist[0], vMax = vZEHist[0];
                for (size_t i = 1;i < vSize;i++) {
                    if (vZEHist[i] > vMax)  vMax = vZEHist[i];
                    if (vZEHist[i] < vMin)  vMin = vZEHist[i];
                }
                if (vMax < m_min || vMax > m_max || vMin < m_min || vMin > m_max)
                {
                    cCritical() << "ERROR in profile " << m_Name
                        << " values should be in the range [" << m_min << ";" << m_max << "] "
                        << " instead of [" << vMin << ";" << vMax << "] ";
                    vRet = false;
                }
            }           
            cDebug() << "end checking " << m_Name;
        }
    }
    return vRet;
}

void ModelTS::subscribeTS(const std::string& a_exName, t_mapExchange& a_Import, size_t a_npdtTot)
{
    p_ZEVariable = new ZEVariables(m_Name, &m_Unit, m_Description + " Profile ", "1", "0", false);
    a_Import[a_exName] = p_ZEVariable;
    std::vector<double>& vZEHist = *p_ZEVariable->ptrVariable();
    vZEHist.resize(a_npdtTot, 0.0);
    set_Values(a_npdtTot, m_default);    
}
