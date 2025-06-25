#include "OrRoot.h"

OrRoot::OrRoot()
    : OrObject(nullptr)
{
    m_AppName = "";
    m_Loading = false;
}

OrRoot::~OrRoot(void)
{
}

bool OrRoot::Load(const std::string& a_FileName)
{
	bool vRet = false;

	// Chargement du fichier de configuration
	json input;
	std::ifstream file(a_FileName);
	if (file.is_open()) {
		// chargement 
		try
		{
			file >> input;
			if (input.contains("name")) {
				if (input["name"] == m_AppName) {
					m_Loading = true;
					vRet = LoadGroups(input);
					if (vRet) {
						m_FileName = a_FileName;
					}
					m_Loading = false;
				}
			}
		}
		catch (const std::exception&e)
		{
			std::cout << e.what();
		}		
	}
	return vRet;
}

const std::string& OrRoot::get_AppName()
{
    return m_AppName;
}

const std::string& OrRoot::get_FileName()
{
	return m_FileName;
}

bool OrRoot::LoadGroups(const json& ap_MainSection)
{
	bool vRet = true;
	for (iterator vIter = begin(); vIter != end(); vIter++) {
		if (*vIter) {
			if (ap_MainSection.contains((*vIter)->get_Name())) {
				vRet &= (*vIter)->Load(ap_MainSection[(*vIter)->get_Name()]);
			}
		}
	}
	return vRet;
}

void OrRoot::Add(const std::string& a_Name, OrObject* ap_Group)
{
	OrObject::Add(ap_Group);
	int vKey = ap_Group->get_Key();
	size_t vIndex = size() - 1;
	m_mapElem.insert(t_mapKey::value_type(a_Name, vIndex));
	m_mapKeyElem.insert(t_mapKeyIndex::value_type(vKey, vIndex));
	ap_Group->Init(a_Name, vKey);
}
