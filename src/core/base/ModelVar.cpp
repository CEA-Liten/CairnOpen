#include "ModelVar.h"
#include "GlobalSettings.h"
#include "MilpData.h"


ModelVar::ModelVar(const QString& a_Name, const QString& a_Unit, const QString& a_Currency)
    : m_Name(a_Name), m_Unit(a_Unit), p_Unit(nullptr), m_Currency(a_Currency)
{
}

ModelVar::ModelVar(const QString& a_Name, const QString* pa_Unit, const QString& a_Currency)
    : m_Name(a_Name), m_Unit(""), p_Unit(pa_Unit), m_Currency(a_Currency)
{
}

QString ModelVar::getUnit() const {
    QString unit = m_Unit;
    if (p_Unit) unit = *p_Unit;
    if (m_Currency != "") {
        return (m_Currency + "/" + unit);
    }
    return unit;
};

/*****************************************************************************************************/
ControlVar::ControlVar(const QString& aName,
    double* ap_Value,
    double* ap_DefaultValue, bool a_isMPC)
    : ModelVar(aName, "")
{
    m_IsMPC = a_isMPC;
    m_Value = ap_Value;
    m_DefaultValue = ap_DefaultValue;
    p_Hist = nullptr;
}

ControlVar::ControlVar(const QString& aName,
    std::vector<double>* ap_Hist,
    double* ap_DefaultValue, bool a_isMPC)
    : ModelVar(aName, "")
{
    m_IsMPC = a_isMPC;
    m_Value = nullptr;
    m_DefaultValue = ap_DefaultValue;
    p_Hist = ap_Hist;
}

ControlVar::~ControlVar()
{
    if (p_ZEVariable) delete p_ZEVariable;
}

void ControlVar::subscribeMPC(const QString& a_CompName, t_mapExchange& a_Import)
{
    if (m_IsMPC) {
        QString exName = a_CompName + "." + m_Prefix + m_Name;
        // TODO: vérif init coeff A et B
        p_ZEVariable = new ZEVariables(exName, getUnit(), m_Name, "1", "0", true);
        a_Import[exName] = p_ZEVariable;
    }
}

void ControlVar::resize(size_t a_Size)
{
    if (p_Hist) {
        if (a_Size > p_Hist->size()) p_Hist->resize(a_Size);
    }
    else {
        if (a_Size > m_Hist.size()) m_Hist.resize(a_Size);
    }
}


void ControlVar::set_Values(const QString& a_ControlMode,
    const InputParam::t_mapParams& a_Params,
    const class MilpData& a_MilpData,
    bool a_FirstInit
)
{
    bool vIsRH = (a_ControlMode == "RollingHorizon" || (a_ControlMode == "MPC" && !m_IsMPC));
    bool vIsMPC = (a_ControlMode == "MPC" && m_IsMPC);
    uint npdtPast = a_MilpData.npdtPast();
    uint npdt = a_MilpData.npdt();
    resize(npdtPast + npdt);

    if (vIsMPC) {
        // recopier les valeurs de la ZE: 
        if (p_ZEVariable) {
            QVector<float>& vZEHist = *p_ZEVariable->ptrOutVariable();
            size_t vSize = std::min((int)npdtPast + 1, vZEHist.size());
            for (uint i = 0;i < vSize;i++) {
                set_Value(i, vZEHist[i]);
            }
        }
    }
    else {
        if (a_FirstInit || !vIsRH) {
            set_DefaultValues(npdtPast);
        }
        else {
            InputParam::t_mapParams::const_iterator vIter = a_Params.find(m_Name);
            uint timeshift = a_MilpData.timeshift();
            for (uint i = 0;i < npdtPast - timeshift;i++) {
                set_Value(i, get_Value(i + timeshift));
            }
            for (uint i = npdtPast - timeshift;i < npdtPast + npdt - timeshift;i++) {
                set_Value(i, std::get<double>((*vIter->second)[i + timeshift]));
            }
        }
    }
}

void ControlVar::set_DefaultValues(int npdtPast)
{
    if (p_Hist) {
        if (npdtPast >= p_Hist->size()) p_Hist->resize(npdtPast + 1);
    }
    else {
        if (npdtPast >= m_Hist.size()) m_Hist.resize(npdtPast + 1);
    }

    double vDefaultValue = get_DefaultValue();
    for (uint i = 0;i < npdtPast + 1;i++) {
        set_Value(i, vDefaultValue);
    }
}

double ControlVar::get_DefaultValue()
{
    if (m_DefaultValue)
        return *m_DefaultValue;
    else
        return 0.0;
}

void ControlVar::ComputeValue(int aNpdtPast)
{
    if (m_Value) {
        if (aNpdtPast > 0 && aNpdtPast < m_Hist.size()) {
            *m_Value = m_Hist[aNpdtPast - 1];
        }
        else {
            *m_Value = get_DefaultValue();
        }
    }
}

double ControlVar::get_Value(size_t i)
{
    if (p_Hist) {
        return (*p_Hist)[i];
    }
    else {
        return m_Hist[i];
    }
}

void ControlVar::set_Value(size_t i, double a_Value)
{
    if (p_Hist) {
        (*p_Hist)[i] = a_Value;
    }
    else {
        m_Hist[i] = a_Value;
    }
}


/*****************************************************************************************************/
ModelIO::ModelIO(const QString& aName,
    const QString& a_Unit,
    const QString& aCurrency)
    : ModelVar(aName, a_Unit, aCurrency)
{
    m_Type = EIOModelType::eMIPUndefined;

}

ModelIO::ModelIO(const QString& aName,
    MIPModeler::MIPExpression* aPtr,
    const QString& a_Unit,
    const QString& aCurrency
)
    : ModelIO(aName, a_Unit, aCurrency)
{
    m_Type = EIOModelType::eMIPExpression;
    p_Expr = aPtr;
}

ModelIO::ModelIO(const QString& aName,
    MIPModeler::MIPExpression1D* aPtr,
    const QString& a_Unit,
    const QString& aCurrency
)
    : ModelIO(aName, a_Unit, aCurrency)
{
    m_Type = EIOModelType::eMIPExpression1D;
    p_Expr = aPtr;
}
//pointer
ModelIO::ModelIO(const QString& aName,
    const QString* a_Unit,
    const QString& aCurrency
)
    : ModelVar(aName, a_Unit, aCurrency)
{
    m_Type = EIOModelType::eMIPUndefined;

}

ModelIO::ModelIO(const QString& aName,
    MIPModeler::MIPExpression* aPtr,
    const QString* a_Unit,
    const QString& aCurrency
)
    : ModelIO(aName, a_Unit, aCurrency)
{
    m_Type = EIOModelType::eMIPExpression;
    p_Expr = aPtr;
}

ModelIO::ModelIO(const QString& aName,
    MIPModeler::MIPExpression1D* aPtr,
    const QString* a_Unit,
    const QString& aCurrency
)
    : ModelIO(aName, a_Unit, aCurrency)
{
    m_Type = EIOModelType::eMIPExpression1D;
    p_Expr = aPtr;
}

size_t ModelIO::size()
{
    size_t vRet = 0;
    switch (m_Type) {
    case eMIPExpression1D:
    {
        MIPModeler::MIPExpression1D* pExpr = (MIPModeler::MIPExpression1D*)(std::get<eMIPExpression1D>(p_Expr));
        if (pExpr)
            vRet = pExpr->size();
        break;
    }
    }
    return vRet;
}

const t_value& ModelIO::evaluate(const double* ap_solution)
{
    switch (m_Type) {
    case eMIPExpression:
    {
        MIPModeler::MIPExpression* pExpr = (MIPModeler::MIPExpression*)(std::get<eMIPExpression>(p_Expr));
        if (pExpr) {
            m_evaluateExpr = pExpr->evaluate(ap_solution);
        }
        break;
    }
    case eMIPExpression1D:
    {
        MIPModeler::MIPExpression1D* pExpr = (MIPModeler::MIPExpression1D*)(std::get<eMIPExpression1D>(p_Expr));
        if (pExpr) {
            std::vector<double> pValues;
            for (auto& vExpr : *pExpr) {
                pValues.push_back(vExpr.evaluate(ap_solution));
            }
            m_evaluateExpr = pValues;
        }
        break;
    }
    }
    return m_evaluateExpr;
}

void ModelIO::close()
{
    switch (m_Type) {
    case eMIPExpression:
    {
        MIPModeler::MIPExpression* pExpr = (MIPModeler::MIPExpression*)(std::get<eMIPExpression>(p_Expr));
        if (pExpr) {
            pExpr->close();
        }
        break;
    }
    case eMIPExpression1D:
    {
        MIPModeler::MIPExpression1D* pExpr = (MIPModeler::MIPExpression1D*)(std::get<eMIPExpression1D>(p_Expr));
        if (pExpr) {
            for (auto& vExpr : *pExpr) {
                vExpr.close();
            }
        }
        break;
    }
    }
}

bool ModelIO::isPExpr() const
{
    bool vRet = false;
    switch (m_Type) {
    case eMIPExpression:
        vRet = (std::get<eMIPExpression>(p_Expr) != nullptr);
        break;
    case eMIPExpression1D:
        vRet = (std::get<eMIPExpression1D>(p_Expr) != nullptr);
        break;
    default:
        vRet = false;
        break;
    }
    return vRet;
}


