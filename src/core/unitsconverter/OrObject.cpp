// *********************************************************
//	 Orionde
//   JCh
// *********************************************************
//
#include "OrObject.h"
#include "OrJsonUtils.h"
#include "OrParam.h"

OrObject::OrObject(long a_Key, OrObject* ap_Parent)
{
    Init(a_Key, ap_Parent);
}

OrObject::OrObject(OrObject* ap_Parent)
{
    Init(-1, ap_Parent);
}

void OrObject::Init(int a_Key, OrObject* ap_Parent)
{
    p_Parent = ap_Parent;
    m_Key = a_Key;
    m_Name = "";
    m_Kind = "";
    m_ElemsName = "items";
    m_HardLink = false;
    m_Modified = false;
    m_ContLoad = false;
    m_CanSave = true;

    m_Elems.clear();
    m_mapElem.clear();
    m_mapKeyElem.clear();
    
    p_Root = get_Root();
}

OrObject::~OrObject(void)
{
    // Supression des références
    iterator vIter;
    for (vIter = m_Refs.begin(); vIter < m_Refs.end(); vIter++) {
       if (*vIter)
           (*vIter)->ReleaseLinked(this);
    }    
    m_Refs.clear();

    // Remise à zéro de la référence dans les éléments me faisant référence
    std::vector<OrObject*> vHardLinks;
    for (vIter = m_WhoRefsMe.begin(); vIter < m_WhoRefsMe.end(); vIter++) {
        if (*vIter) {
            if ((*vIter)->IsHardLink())
                vHardLinks.push_back(*vIter);
            else {
                (*vIter)->ReleaseLink(this);
            }
        }        
    }
    m_WhoRefsMe.clear();

    // Suppression des liens
    for (vIter = vHardLinks.begin(); vIter < vHardLinks.end(); vIter++) {
        if (*vIter) {
            OrObject* vParentLink = (*vIter)->get_Parent();
            vParentLink->Remove((*vIter)->get_Key());
            (*vIter) = nullptr;
        }
    }
    vHardLinks.clear();

    // Suppression des groupes fils
    for (vIter = begin(); vIter < end(); vIter++) {
        if (*vIter) {
            delete (*vIter);
            (*vIter) = nullptr;
        }
    }
    m_Elems.clear();
    m_mapElem.clear();
    m_mapKeyElem.clear();

    // Suppression des paramètres
    int n = m_Params.size();
    for (int i = 0; i < n; i++) {
        if (m_Params[i]) {
            delete m_Params[i];
            m_Params[i] = nullptr;
        }
    }
    m_Params.clear();
    m_mapParams.clear();
}

const std::string& OrObject::get_Name() const
{
    return m_Name;
}

const std::string& OrObject::get_Kind() const
{
    return m_Kind;
}

bool OrObject::IsHardLink() const
{
    return m_HardLink;
}

int OrObject::get_Key()
{
    return m_Key;
}

int OrObject::get_Key(const std::string& a_Name)
{
    int vRet = -1;
    OrObject::iterator vIter = find(a_Name);
    if (vIter != end()) {
        vRet = (*vIter)->get_Key();
    }
    return vRet;
}

OrObject* OrObject::get_ElemByKey(int a_Key)
{
    OrObject* vRet = nullptr;
    iterator vIter = find(a_Key);
    if (vIter != end()) {
        vRet = (*vIter);
    }
    return vRet;
}

int OrObject::Key2Index(int a_Key)
{
    int vRet = -1;
    t_mapKeyIndex::iterator vIterator = m_mapKeyElem.find(a_Key);
    if (vIterator != m_mapKeyElem.end()) {
        // La clef existe, on récupère l'index
        vRet = vIterator->second;
    }
    return vRet;
}

long OrObject::Add(const std::string& a_Name, const std::string& a_Kind)
{
    return Add(a_Name, -1, a_Kind);
}

long OrObject::Add(const std::string& a_Name, int a_Key, const std::string& a_Kind)
{
    m_ContLoad = false; // Arrêt en cas d'erreur
    // Vérifier que le nom n'existe pas déjà
    long vRet = get_Key(a_Name);
    if (vRet < 0) { // L'élément n'existe pas, on peut l'ajouter	
        // Ajout de l'élément à la fin de la liste des éléments		
        if (Add(a_Kind)) {
            // Ajout dans la map et du nom, initialisation de l'élément
            long vIndex = Key2Index(a_Key);
            if (a_Key < 0 || vIndex >= 0)
                vRet = CreateKey(); // clef n'existe pas ou déjà utilisée
            else
                vRet = a_Key;
            vIndex = size() - 1;
            m_mapElem.insert(t_mapKey::value_type(a_Name, vIndex));
            m_mapKeyElem.insert(t_mapKeyIndex::value_type(vRet, vIndex));
            m_Elems[vIndex]->Init(a_Name, vRet);
        }
    }
    return vRet;
}

long OrObject::Add(const std::vector<int>& a_Keys, const std::string& a_Name, const std::string& a_Kind)
{
    return 0;
}

int OrObject::AddParam(const std::string& a_Name, OrParam* ap_Param)
{
    m_mapParams.insert(t_mapKey::value_type(a_Name, m_Params.size()));
    m_Params.push_back(ap_Param);
    return m_Params.size() - 1;
}

bool OrObject::Add()
{
    m_Elems.push_back(new OrObject(this));
    return true;
}

bool OrObject::Add(const std::string& a_Kind)
{
    return Add();
}

void OrObject::Add(OrObject* ap_Group)
{
    m_Elems.push_back(ap_Group);
}

int OrObject::CreateKey()
{
    long vRet = m_Elems.size() - 1;
    while (Key2Index(vRet) != -1) vRet++;
    return vRet;
}

OrRoot* OrObject::get_Root()
{
    if (p_Parent)
        return p_Parent->get_Root();
    else
        return (OrRoot*)this;
}

long OrObject::NoRef(long a_Key)
{
    Remove(a_Key);
    return -1;
}

void OrObject::Init(const std::string& a_Name, int a_Key)
{
    m_Name = a_Name;
    m_Key = a_Key;
    Init();
}

void OrObject::Init()
{
}

bool OrObject::Remove(int a_Key)
{
    bool vRet = true;
    // Si l'élément existe
    long vIndex = Key2Index(a_Key);
    OrObject* vElem = m_Elems[vIndex];
    if (vElem) {
        // Supprimer l'élément de la map Nom->Index 
        // (le nom de l'élément pourra être réutilisé)		
        m_mapElem.erase(vElem->get_Name());

        // Supprimer l'élément de la map Clef->Index
        m_mapKeyElem.erase(vElem->get_Key());

        // Supprimer l'élément (son destructeur supprime tous ces sous-éléments)
        delete vElem;
        m_Elems[vIndex] = nullptr;
    }
    return vRet;
}

void OrObject::Clear()
{
    // Suppression des éléments fils
    size_t n = m_Elems.size();
    for (size_t i = 0; i < n; i++) {
        if (m_Elems[i])
            delete m_Elems[i];
    }
    m_Elems.clear();
    m_mapElem.clear();
    m_mapKeyElem.clear();
}

bool OrObject::CanSave()
{
    return m_CanSave;
}

void OrObject::EnableSave(bool a_Enable)
{
    m_CanSave = a_Enable;
}

bool OrObject::Save(ojson& ap_Group)
{
    return false;
}

bool OrObject::SaveProperties(ojson& ap_Group)
{
    return false;
}

bool OrObject::Load(const json& a_Group)
{
    bool vRet = LoadProperties(a_Group);
    // Boucle sur les éléments
    if (a_Group.contains(m_ElemsName)) {
        vRet &= LoadArray(a_Group[m_ElemsName]);        
    }
    else if (a_Group.is_array()) {
        vRet &= LoadArray(a_Group);       
    }    
    if (vRet) {
        InitProperties();
    }    
    return vRet;
}

bool OrObject::LoadArray(const json& a_Group)
{
    bool vRet = true;
    if (a_Group.is_array()) {
        for (const auto& j : a_Group) {
            std::string vName;
            if (orjson::from_json(j, "name", vName)) {
                std::string vKind = "";
                int vKey = -1, vIndex = -1;
                orjson::from_json(j, "kind", vKind);
                if (orjson::from_json(j, "key", vKey)) {
                    vIndex = Key2Index(Add(vName, vKey, vKind));
                }
                else {
                    vIndex = Key2Index(Add(vName, vKind));
                }
                if (vIndex >= 0 && vIndex < m_Elems.size()) {
                    if (m_Elems[vIndex]) {
                        if (!m_Elems[vIndex]->Load(j)) {
                            NoRef(m_Elems[vIndex]->get_Key());
                            vRet = m_ContLoad;
                        }
                        else if (!m_Elems[vIndex])
                            vRet = m_ContLoad;
                    }
                }
                else {
                    vRet = m_ContLoad;
                }
            }
        }
    }
    return vRet;
}
bool OrObject::LoadProperties(const json& ap_Group)
{
    t_mapKey::iterator vIter;
    for (vIter = m_mapParams.begin(); vIter != m_mapParams.end(); vIter++) {
        m_Params[vIter->second]->LoadProperties(ap_Group, vIter->first);
    }
    return true;
}

void OrObject::InitProperties()
{
}

void OrObject::AddLink(OrObject* ap_Ref)
{
    if (ap_Ref) {
        iterator vIter;
        for (vIter = m_Refs.begin(); vIter < m_Refs.end(); vIter++) {
            if (*vIter == ap_Ref) {
                // existe déjà
                break;
            }
        }
        if (vIter == m_Refs.end()) {
            // n'existe pas déjà, on l'ajoute à la liste des référence
            m_Refs.push_back(ap_Ref);

            // informe l'objet qu'il a été référencé
            ap_Ref->Linked(this);
        }
    }
}

void OrObject::Linked(OrObject* ap_Ref)
{
    if (ap_Ref) {
        iterator vIter;
        for (vIter = m_WhoRefsMe.begin(); vIter < m_WhoRefsMe.end(); vIter++) {
            if (*vIter == ap_Ref) {
                break;
            }
        }
        if (vIter == m_WhoRefsMe.end()) {
            m_WhoRefsMe.push_back(ap_Ref);
        }
    }
}

void OrObject::ReleaseLink(OrObject* ap_Ref)
{
    if (ap_Ref) {
        iterator vIter;
        for (vIter = m_Refs.begin(); vIter < m_Refs.end(); vIter++) {
            if (*vIter == ap_Ref) {
                *vIter = nullptr;
                ap_Ref->ReleaseLinked(this);
                break;
            }
        }
    }
}

void OrObject::ReleaseLinked(OrObject* ap_Ref)
{
    if (ap_Ref) {
        iterator vIter;
        for (vIter = m_WhoRefsMe.begin(); vIter < m_WhoRefsMe.end(); vIter++) {
            if (*vIter == ap_Ref) {
                *vIter = nullptr;
                break;
            }
        }
    }
}

OrObject* OrObject::get_Ref(const std::string& a_Name)
{
    if (a_Name == "") {
        iterator vIter;
        for (vIter = m_Refs.begin(); vIter < m_Refs.end(); vIter++) {
            if (*vIter != nullptr) {
                return *vIter;                
            }
        }
    }
    else {
        iterator vIter;
        for (vIter = m_Refs.begin(); vIter < m_Refs.end(); vIter++) {
            if (*vIter != nullptr) {
               if ((*vIter)->get_Name() == a_Name)
                   return *vIter;
            }
        }
    }
    return nullptr;
}

OrObject* OrObject::get_Ref(const int& a_Key)
{
    if (a_Key < 0) {
        iterator vIter;
        for (vIter = m_Refs.begin(); vIter < m_Refs.end(); vIter++) {
            if (*vIter != nullptr) {
                return *vIter;
            }
        }
    }
    else {
        iterator vIter;
        for (vIter = m_Refs.begin(); vIter < m_Refs.end(); vIter++) {
            if (*vIter != nullptr) {
                if ((*vIter)->get_Key() == a_Key)
                    return *vIter;
            }
        }
    }
    return nullptr;
}

const std::vector<OrObject*>& OrObject::get_Refs()
{
    return m_Refs;
}

OrObject* OrObject::get_Parent()
{
    return p_Parent;
}

OrObject::iterator OrObject::begin()
{
    return m_Elems.begin();
}

OrObject::iterator OrObject::end()
{
    return m_Elems.end();
}

OrObject::const_iterator OrObject::begin() const
{
    return  m_Elems.begin();
}

OrObject::const_iterator OrObject::end() const
{
    return m_Elems.end();
}

OrObject::iterator OrObject::find(const std::string& a_name)
{
    t_mapKey::iterator vIter = m_mapElem.find(a_name);
    if (vIter != m_mapElem.end()) {
        return m_Elems.begin() + vIter->second;
    }
    return m_Elems.end();
}

OrObject::iterator OrObject::find(const int& a_key)
{
    t_mapKeyIndex::iterator vIter = m_mapKeyElem.find(a_key);
    if (vIter != m_mapKeyElem.end()) {
        return m_Elems.begin() + vIter->second;
    }
    return m_Elems.end();
}

std::size_t OrObject::size() const
{
    return m_Elems.size();
}

OrObject* OrObject::operator[](std::size_t a_Index)
{
    OrObject* vRet = nullptr;
    if (a_Index >= 0 && a_Index < m_Elems.size()) {
        vRet = m_Elems[a_Index];
    }
    return vRet;
}

