#include "CairnAPIUtils.h"
#include "CairnCore.h"
#include "Cairn_Exception.h"


namespace CairnAPIUtils {
	
	t_list get_Possible_Model_Names()
	{
		// static list for the internal Cairn components
		t_list vRet;
		for (auto const& vItem : mModelsMap) {
			vRet.insert(vRet.end(), vItem.second.begin(), vItem.second.end());
		}

		// dynamics list for the external components (Private)
		ModelFactory vModelFactory;
		vModelFactory.findModels();
		QStringList vModels = vModelFactory.getModelList();
		for (auto& vModel : vModels) {
			vRet.push_back(vModel.toStdString());
		}
		return vRet;
	}

	t_list get_Possible_Component_Types() {
		t_list vRet;
		for (auto const& vItem : CairnAPIUtils::mModelsMap) {
			vRet.push_back(vItem.first);
		}
		return vRet;
	}

	std::string get_Component_Type(const std::string& a_Model) {
		for (auto const& vItem : mModelsMap) {
			if (std::find(vItem.second.begin(), vItem.second.end(), a_Model) != vItem.second.end()) {
				return vItem.first;
			}
		}
		for (auto const& vItem : mPrivateModelsMap) {
			if (std::find(vItem.second.begin(), vItem.second.end(), a_Model) != vItem.second.end()) {
				return vItem.first;
			}
		}
		return "Unknown";
	}

	t_list getParametersName(std::vector<InputParam*> a_Inputs, CairnAPI::ESettingsLimited a_setLimited)
	{
		t_list vRet;
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				QList<QString> vQList;
				vInput->getParameters(vQList, a_setLimited);
				for (auto& vParam : vQList) {
					vRet.push_back(vParam.toStdString());
				}
			}
		}
		return vRet;
	}

	std::string getParamValue(const t_value& a_Value)
	{
		std::string vRet = "NON_COMPATIBLE";
		if (std::holds_alternative<std::string>(a_Value)) {
			vRet = std::get<std::string>(a_Value);
		}
		/*else if (std::holds_alternative<bool>(a_Value)) {
			vRet = std::to_string(std::get<bool>(a_Value));
		}*/
		else if (std::holds_alternative<int>(a_Value)) {
			vRet = std::to_string(std::get<int>(a_Value));
		}
		else {
			try
			{
				vRet = std::to_string(std::get<double>(a_Value));
			}
			catch (const std::exception&)
			{
				// type non compatible
			}
		}
		return vRet;
	}

	std::vector<double> getParamVectorValue(const t_value& a_Value)
	{
		if (std::holds_alternative<std::vector<double>>(a_Value)) {
			return std::get<std::vector<double>>(a_Value);
		}
		return {};
	}

	t_value getParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name) {
		t_value vRet = "N/A";
		QString vQName = QString(a_Name.c_str());
		for (auto& vInput : a_Inputs) {
			if (vInput) {				
				if (vInput->getParameterValue(vQName, vRet))
					break;				
			}
		}
		return vRet;
	}

	void getParameters(std::vector<InputParam*> a_Inputs, t_dict& a_Params) {
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				QList<QString> vQList;
				vInput->getParameters(vQList);
				for (auto& vParamName : vQList) {
					// mettre les valeurs vide ?					
					t_value vRet;
					if (vInput->getParameterValue(vParamName, vRet))
						a_Params[vParamName.toStdString()] = vRet;
				}
			}
		}
	}

	bool setParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name, const t_value& a_Value) {
		bool vOk = true;
		if (a_Name != "id" && a_Name != "type") // ne pas changer le nom et le type!
		{
			QString vQName = QString(a_Name.c_str());
			QString vQValue = QString(CairnAPIUtils::getParamValue(a_Value).c_str());
			std::vector<double> vVectValue;
			if (vQValue == "NON_COMPATIBLE") {
				//try vector of double
				vVectValue = getParamVectorValue(a_Value);
			}
			bool vFind = false;
			for (auto& vInput : a_Inputs) {
				if (vInput) {
					if (vQValue != "NON_COMPATIBLE") {
						vFind = vInput->setParameterValue(vQName, vQValue);
					}
					else {
						vFind = vInput->setParameterValue(vQName, vVectValue);
					}
					if (vFind) break;
				}
			}
			vOk &= vFind;
		}
		return vOk;
	}

	bool setParameters(std::vector<InputParam*> a_Inputs, const t_dict& a_Params) {
		bool vOk = true;
		for (auto& vParam : a_Params) {
			QString vQValue = QString(CairnAPIUtils::getParamValue(vParam.second).c_str());
			QString vQName = QString(vParam.first.c_str());
			bool vFind = false;
			for (auto& vInput : a_Inputs) {
				if (vInput) {
					vFind = vInput->setParameterValue(vQName, vQValue);
					if (vFind) break;
				}
			}
			vOk &= vFind;
		}
		return vOk;
	}

	void setError(ECodeError a_Err, const std::string& a_msg) {
		if (a_Err != noError) {
			QString vErrMsg(a_msg.c_str());
			Cairn_Exception cairn_error;
			switch (a_Err) {
			case noCairn:
				cairn_error.setMessage("Cairn is not Initialized");
				break;
			case errInit:
				cairn_error.setMessage("Error when Initializing the problem");
				break;
			case errRead:
				cairn_error.setMessage("Failed to read model, " + vErrMsg);
				break;
			case errFile:
				cairn_error.setMessage("File does not exist, " + vErrMsg);
				break;
			case errWrite:
				cairn_error.setMessage("Failed to write file, " + vErrMsg);
				break;
			case errLink:
				cairn_error.setMessage("Bad link, " + vErrMsg);
				break;
			case errErase:
				cairn_error.setMessage("Cannot remove, " + vErrMsg);
				break;
			case errNotFound:
				cairn_error.setMessage("Not found: " + vErrMsg);
				break;
			case errParam:
				cairn_error.setMessage("Parameter not found!");
				break;
			case errAlreadyExist:
				cairn_error.setMessage("Already exist: " + vErrMsg);
				break;
			case errCreate:
				cairn_error.setMessage("Failed to create: " + vErrMsg);
				break;
			case errAdd:
				cairn_error.setMessage("Failed to add: " + vErrMsg);
				break;
			default:
				cairn_error.setMessage(vErrMsg);
				break;
			}
			throw std::range_error(cairn_error.message().toStdString());
		}
	}
}
