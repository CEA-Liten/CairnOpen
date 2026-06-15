#include "CairnObject.h"

CairnObject::CairnObject(CairnObject* ap_Parent, const std::string& a_Name)
{
	m_Name = a_Name;
	if (ap_Parent) {
		p_Parent = ap_Parent;
		p_Parent->addChild(this);
	}
}

CairnObject::~CairnObject()
{
	if (p_Parent) {		
		p_Parent->removeChild(this);
	}
}


void CairnObject::setObjectName(const std::string& a_Name)
{
	m_Name = a_Name;
}

const std::string& CairnObject::objectName() const
{
	return m_Name;
}

void CairnObject::setObjectType(const std::string& a_Type)
{
	m_Type = a_Type;
}

const std::string& CairnObject::objectType() const
{
	return m_Type;
}

const std::vector<CairnObject*>& CairnObject::children() const
{
	return m_children;
}

std::vector<std::string> CairnObject::childrenNames() const
{
	std::vector<std::string> vRet;
	for (auto& vChild : m_children) {
		vRet.push_back(vChild->objectName());
	}
	return vRet;
}

void CairnObject::addChild(CairnObject* ap_Child)
{	
	if (ap_Child) {
		std::vector<CairnObject*>::iterator vIter = std::find(m_children.begin(), m_children.end(), ap_Child);
		if (vIter == m_children.end()) {
			m_children.push_back(ap_Child);
		}
	}	
}

void CairnObject::removeChild(CairnObject* ap_Child)
{
	if (ap_Child) {
		std::vector<CairnObject*>::iterator vIter = std::find(m_children.begin(), m_children.end(), ap_Child);
		if (vIter != m_children.end()) {
			m_children.erase(vIter);
		}
	}
}

CairnObject* CairnObject::findChild(const std::string& a_Name, const std::string& a_Type)
{
	for (auto& vChild : m_children) {
		if (vChild->objectName() == a_Name) {
			if (a_Type != "") {
				if (a_Type == m_Type)
					return vChild;
			}
			else
				return vChild;
		}
	}
	return nullptr;
}
