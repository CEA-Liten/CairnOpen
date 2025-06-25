#pragma once
#include "OrObject.h"

class OrRoot :
    public OrObject
{
public:
	OrRoot();
	virtual ~OrRoot(void);

	//Chargement d'un fichier de configuration
	virtual bool Load(const std::string& a_FileName);


	const std::string& get_AppName();
	const std::string& get_FileName();

protected:
	std::string m_AppName;		// Nom de l'application
	std::string m_FileName;
	
	// Chargement
	bool m_Loading; // Chargement en cours
	virtual bool LoadGroups(const json& ap_MainSection);

	void Add(const std::string& a_Name, OrObject* ap_Group);
};

