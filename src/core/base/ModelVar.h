#ifndef EXCHANGEVAR_H
#define EXCHANGEVAR_H

#include <Eigen/SparseCore>
#include <Eigen/Dense>
#include "MIPModeler.h"
#include "CairnAPI.h"
#include "InputParam.h"
#include "ZEVariables.h"

enum EIOModelType {
    eMIPUndefined = -1,
    eMIPExpression = 0,
    eMIPExpression1D
};
typedef std::variant< MIPModeler::MIPExpression*, MIPModeler::MIPExpression1D*> t_pExpr;

class ModelVar {
public:
    ModelVar(const std::string& a_Name = "", t_unit a_Unit = "");
    ~ModelVar();

    const std::string& getName() const { return m_Name; };

    std::string getUnit() const;
    void setUnit(t_unit a_Unit);

    const UnitParam* pUnitParam() const { return &m_Unit; }; /* Used to dynamically pass the unit e.g. from ModelIO to the corresponding ZEVariables */

protected:
    std::string m_Name;
    UnitParam m_Unit;
};

/*****************************************************************************************************/
// RH, MPC 
class ControlVar : public ModelVar
{
public:
    ControlVar(const std::string& aName,
        double* ap_Value,
        double* ap_DefaultValue = nullptr,
        bool a_isMPC = true
    );
    ControlVar(const std::string& aName,
        std::vector<double>* ap_Hist,
        double* ap_DefaultValue = nullptr,
        bool a_isMPC = true
    );
    ~ControlVar();

    void subscribeMPC(const std::string& a_CompName, t_mapExchange& a_Import);
  
    void set_Values(const std::string& a_ControlMode,
        const InputParam::t_mapParams& a_Params,
        const class MilpData& a_MilpData,
        bool a_FirstInit
    );

    double get_DefaultValue();

    void ComputeValue(int aNpdtPast);

protected:
    std::vector<double>* p_Hist;
    std::vector<double> m_Hist;
    double* m_Value;
    double* m_DefaultValue;

    void set_DefaultValues(int nptPast);
    double get_Value(size_t i);
    void set_Value(size_t i, double a_Value);
    void resize(size_t a_Size);

    // Spécifique MPC
    bool m_IsMPC{ true };
    std::string m_Prefix{ "MPC" }; // MPC par défaut   
    class ZEVariables* p_ZEVariable{ nullptr };
};

/*****************************************************************************************************/

class ModelIO : public ModelVar
{
public:
    ModelIO(const std::string& aName = "", t_flag a_IsUsed = true, t_unit a_Unit = "");
    ModelIO(const std::string& aName, MIPModeler::MIPExpression* aPtr, 
        t_flag a_IsUsed = true, t_unit a_Unit = "");
    ModelIO(const std::string& aName, MIPModeler::MIPExpression1D* aPtr, 
        t_flag a_IsUsed = true, t_unit a_Unit = "");

    const EIOModelType& getType() const { return m_Type; };
    bool IsUsed() { return m_IsUsed.get_Value(); };
    const t_pExpr& getPtr() const { return p_Expr; };
    bool isPExpr() const;

    size_t  size();
    const t_value& evaluate(const double* ap_solution);
    const t_value& getValue() const { return m_evaluateExpr; };
    void close();

protected:
    EIOModelType m_Type;
    FlagParam m_IsUsed;
    t_pExpr p_Expr;
    t_value m_evaluateExpr;
};


#endif