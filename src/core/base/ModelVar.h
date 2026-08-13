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
    ModelVar(const std::string& a_Name, t_flag a_IsUsed = t_flag{}, 
        t_unit a_Unit = t_unit{}, const std::string& a_Description = {});
    virtual ~ModelVar() noexcept = default;

    const std::string& getName() const { return m_Name; };
    const std::string& getDescription() const { return m_Description; };

    virtual std::string getUnit() const { return m_Unit.get_Value(); };
    virtual const UnitParam* pUnitParam() const { return &m_Unit; }; /* dynamically unit e.g. to pass to the corresponding ZEVariables */

    virtual bool IsUsed() const { return m_IsUsed.get_Value(); };
    virtual const FlagParam* pIsUsed() const { return &m_IsUsed; };  /* dynamically isUsed e.g. to pass to the corresponding ZEVariables */

protected:
    std::string m_Name;
    std::string m_Description;

    UnitParam m_Unit;
    FlagParam m_IsUsed;
};

/*****************************************************************************************************/
// RH, MPC 
class ControlVar : public ModelVar
{
public:
    ControlVar(const std::string& aName,
        double* ap_Value,
        const std::string& a_Description = "", 
        double* ap_DefaultValue = nullptr,
        bool a_isMPC = true
    );
    ControlVar(const std::string& aName,
        std::vector<double>* ap_Hist,
        const std::string& a_Description = "",
        double* ap_DefaultValue = nullptr,
        bool a_isMPC = true
    );
    ~ControlVar();

    void subscribeMPC(const std::string& a_CompName, t_mapExchange& a_Import, size_t a_npdtTot);
  
    void set_Values(const std::string& a_ControlMode,
        const InputParam::t_mapParams& a_Params,
        const class MilpData& a_MilpData,
        bool a_FirstInit
    );

    double get_DefaultValue();
    std::vector<double> getValues();

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
    ModelIO(const std::string& aName, t_flag a_IsUsed = t_flag{},
        t_unit a_Unit = t_unit{}, const std::string& aDescription = {});

    ModelIO(const std::string& aName, MIPModeler::MIPExpression* aPtr, 
        t_flag a_IsUsed = t_flag{}, t_unit a_Unit = t_unit{}, 
        const std::string& aDescription = {});

    ModelIO(const std::string& aName, MIPModeler::MIPExpression1D* aPtr, 
        t_flag a_IsUsed = t_flag{}, t_unit a_Unit = t_unit{}, 
        const std::string& aDescription = {});

    const EIOModelType& getType() const { return m_Type; };
    const t_pExpr& getPtr() const { return p_Expr; };
    bool isPExpr() const;

    std::size_t size() const;
    const t_value& evaluate(const double* ap_solution);
    const t_value& getValue() const { return m_evaluateExpr; };
    void close();

protected:
    EIOModelType m_Type;
    t_pExpr p_Expr;
    t_value m_evaluateExpr;
};


#endif