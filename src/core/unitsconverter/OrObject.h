// *********************************************************
//	 Orionde
//   JCh
// *********************************************************
//
#pragma once
#include "COrHeader.h"

class OrRoot;
class OrParam;

class OrObject
{
public:
	typedef std::vector<OrObject*>::iterator iterator;
	typedef std::vector<OrObject*>::const_iterator const_iterator;
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	iterator find(const std::string& a_name);
	iterator find(const int& a_key);
	std::size_t size() const;
	OrObject* operator [] (std::size_t i);

	OrObject(long a_Key, OrObject* ap_Parent);
	OrObject(OrObject* ap_Parent);
	virtual ~OrObject(void);

	virtual const std::string& get_Name() const;
	const std::string& get_Kind() const;
	bool IsHardLink() const;

	//Retourne la clef.
	int get_Key();
	int get_Key(const std::string& a_Name);
	OrObject* get_ElemByKey(int a_Key);

	//Conversion d'une clef en 1 index;	
	virtual int Key2Index(int a_Key);

	//Ajoute un �l�ment (Name) au groupe sp�cifi� par ses clefs.
	virtual long Add(const std::string& a_Name, const std::string& a_Kind);
	virtual long Add(const std::string& a_Name, int a_Key, const std::string& a_Kind);
	long Add(const std::vector<int>& a_Keys, const std::string& a_Name, const std::string& a_Kind = "");
	virtual void Init(const std::string& a_Name, int a_Key);
	virtual void Init();

	//Suprime un �l�ment .	
	virtual bool Remove(int a_Key);
	void Clear();	// Supprime tous les �l�ments

	// Sauvegarde les param�tres
	virtual bool CanSave();
	void EnableSave(bool a_Enable);
	bool Save(ojson &ap_Group);
	virtual bool SaveProperties(ojson &ap_Group);
	// Charge les param�tres
	bool Load(const json& ap_Group);
	virtual bool LoadProperties(const json& ap_Group);
	virtual void InitProperties();

	virtual void AddLink(OrObject* ap_Ref);		// Ajoute une 'Ref' et 'WhoRefMe' � ap_Ref
	void Linked(OrObject* ap_Ref);				// Ajoute un 'WhoRefMe'
	void ReleaseLink(OrObject* ap_Ref);			// Supprime une 'Ref' et 'WhoRefMe' � ap_Ref
	void ReleaseLinked(OrObject* ap_Ref);		// Supprime un 'WhoRefMe'

	OrObject* get_Ref(const std::string& a_Name = ""); // retourne la r�f�rence nomm�e ou la premi�re si non vide
	OrObject* get_Ref(const int& a_Key);
	const std::vector<OrObject*> &get_Refs();

	OrObject *get_Parent();

protected:
	int m_Key;				//Clef (ID) du objet.		
	std::string m_Name;		//Nom du objet
	std::string m_Kind;		//Type du objet 
	bool m_HardLink;		// Vrai: l'�l�ment et les �l�ments qui lui font r�f�rence ont la m�me dur�e de vie
	bool m_Modified;		// Flag indiquant si l'a session'objet a �t� modifi� depuis son chargement
	bool m_ContLoad;		// Flag indiquant que l'on continue le chargement
	bool m_CanSave;			// Flag indiquant que l'objet peut �tre sauv�
	std::string m_ElemsName;
	OrObject* p_Parent;				// R�f�rence vers l'objet parent
	OrRoot* p_Root;				// R�f�rence vers l'objet 'Racine'

	t_mapKey m_mapElem;				// Correspondance Noms -> Index
	t_mapKeyIndex m_mapKeyElem;		// Correspondance Key -> Index

	std::vector<OrParam*> m_Params;	// Liste des param�tres du groupe
	t_mapKey m_mapParams;
	t_mapKey::iterator m_IterParams;
	int AddParam(const std::string& a_Name, OrParam* ap_Param);

	// R�f�rence avec d'autres objets
	std::vector<OrObject*> m_Refs; 
	std::vector<OrObject*> m_WhoRefsMe;	// Contient la liste des objets faisant r�f�rence � cet objet

	//---------------------------------------
	virtual bool Add();
	virtual bool Add(const std::string& a_Kind);
	void Add(OrObject* ap_Group);
	int CreateKey();
	OrRoot* get_Root();
	virtual long NoRef(long a_Key);	// Op�ration � effectuer en cas de r�f�rence nulle (Remove par d�faut)
	bool LoadArray(const json& ap_Group);

private:
	void Init(int a_Key, OrObject* p_Parent);
	// Les �l�ments du objet	
	std::vector<OrObject*>  m_Elems;

};

