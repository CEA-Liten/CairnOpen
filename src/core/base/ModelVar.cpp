#include "ModelVar.h"
#include "GlobalSettings.h"
#include "MilpData.h"


ModelVar::ModelVar(const std::string& a_Name, t_unit a_Unit, const std::string& a_Description)
    : m_Name(a_Name),
    m_Description(a_Description)
{
    m_Unit.set_Value(a_Unit);
}

ModelVar::~ModelVar()
{
}

std::string ModelVar::getUnit() const
{
    return m_Unit.get_Value();
}

void ModelVar::setUnit(t_unit a_Unit)
{
    m_Unit.set_Value(a_Unit);
}


/*****************************************************************************************************/
ControlVar::ControlVar(const std::string& aName,
    double* ap_Value,
    const std::string& a_Description,
    double* ap_DefaultValue, bool a_isMPC)
    : ModelVar(aName, "", a_Description)
{
    m_IsMPC = a_isMPC;
    m_Value = ap_Value;
    m_DefaultValue = ap_DefaultValue;
    p_Hist = nullptr;
}

ControlVar::ControlVar(const std::string& aName,
    std::vector<double>* ap_Hist,
    const std::string& a_Description,
    double* ap_DefaultValue, bool a_isMPC)
    : ModelVar(aName, "", a_Description)
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

void ControlVar::subscribeMPC(const std::string& a_CompName, t_mapExchange& a_Import, size_t a_npdtTot)
{
    if (m_IsMPC) {
        std::string exName = a_CompName + "." + m_Prefix + m_Name;
        // TODO: vérif init coeff A et B
        p_ZEVariable = new ZEVariables(exName, &m_Unit, m_Name, "1", "0", true);
        a_Import[exName] = p_ZEVariable;
        std::vector<double>& vZEHist = *p_ZEVariable->ptrVariable();
        vZEHist.resize(a_npdtTot, 0.0);       
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


void ControlVar::set_Values(const std::string& a_ControlMode,
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
            std::vector<double>& vZEHist = *p_ZEVariable->ptrOutVariable();
            size_t vSize = std::min((size_t)npdtPast + 1, vZEHist.size());
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

std::vector<double> ControlVar::getValues()
{
    if (p_Hist) {
        return (*p_Hist);
    }
    else {
        return m_Hist;
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
ModelIO::ModelIO(const std::string& aName, t_flag a_IsUsed, t_unit a_Unit, const std::string& aDescription)
    : ModelVar(aName, a_Unit, aDescription)
{
    m_Type = EIOModelType::eMIPUndefined;
    m_IsUsed.set_Value(a_IsUsed);
}

ModelIO::ModelIO(const std::string& aName,
    MIPModeler::MIPExpression* aPtr,
    t_flag a_IsUsed, t_unit a_Unit, 
    const std::string& aDescription)
    : ModelIO(aName, a_IsUsed, a_Unit, aDescription)
{
    m_Type = EIOModelType::eMIPExpression;
    p_Expr = aPtr;
}

ModelIO::ModelIO(const std::string& aName,
    MIPModeler::MIPExpression1D* aPtr,
    t_flag a_IsUsed, t_unit a_Unit, 
    const std::string& aDescription)
    : ModelIO(aName, a_IsUsed, a_Unit, aDescription)
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


