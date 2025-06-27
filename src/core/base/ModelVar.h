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
    ModelVar(const QString& a_Name = "", const QString& a_Unit = "", const QString& a_Currency = "");
    ModelVar(const QString& a_Name = "", const QString* pa_Unit = nullptr, const QString& a_Currency = "");

    const QString& getName() const { return m_Name; };
    QString getUnit() const;
    const QString* getPtrUnit() const { return p_Unit; };
    void setPtrUnit(const QString* p_Unit) { p_Unit = p_Unit; }

private:
    QString m_Unit;
    const QString* p_Unit;

protected:
    QString m_Name;
    QString m_Currency; //TODO: make currency dynamic!
};

// RH, MPC 
class ControlVar : public ModelVar
{
public:
    ControlVar(const QString& aName,
        double* ap_Value,
        double* ap_DefaultValue = nullptr,
        bool a_isMPC = true
    );
    ControlVar(const QString& aName,
        std::vector<double>* ap_Hist,
        double* ap_DefaultValue = nullptr,
        bool a_isMPC = true
    );
    ~ControlVar();

    void subscribeMPC(const QString& a_CompName, t_mapExchange& a_Import);
  
    void set_Values(const QString& a_ControlMode,
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
    QString m_Prefix{ "MPC" }; // MPC par défaut   
    class ZEVariables* p_ZEVariable{ nullptr };
};

class ModelIO : public ModelVar
{
public:
    ModelIO(const QString& aName = "",
        const QString& a_Unit = "", const QString& aCurrency = ""
    );
    ModelIO(const QString& aName,
        MIPModeler::MIPExpression* aPtr,
        const QString& a_Unit = "", const QString& aCurrency = ""
    );
    ModelIO(const QString& aName,
        MIPModeler::MIPExpression1D* aPtr,
        const QString& a_Unit = "", const QString& aCurrency = ""
    );

    ModelIO(const QString& aName = "",
        const QString* a_Unit = nullptr, const QString& aCurrency = ""
    );
    ModelIO(const QString& aName,
        MIPModeler::MIPExpression* aPtr,
        const QString* a_Unit = nullptr, const QString& aCurrency = ""
    );
    ModelIO(const QString& aName,
        MIPModeler::MIPExpression1D* aPtr,
        const QString* a_Unit = nullptr, const QString& aCurrency = ""
    );

    const EIOModelType& getType() const { return m_Type; };
    const t_pExpr& getPtr() const { return p_Expr; };
    bool isPExpr() const;

    size_t  size();
    const t_value& evaluate(const double* ap_solution);
    const t_value& getValue() const { return m_evaluateExpr; };
    void close();

protected:
    EIOModelType m_Type;
    t_pExpr p_Expr;
    t_value m_evaluateExpr;
};


#endif